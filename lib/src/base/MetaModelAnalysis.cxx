//                                               -*- C++ -*-
/**
 *  @brief Class to define functional chaos
 *
 *  Copyright 2015-2016 EDF-Phimeca
 *
 *  This library is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
#include "otgui/MetaModelAnalysis.hxx"

#include "otgui/MetaModel.hxx"

#include "openturns/Uniform.hxx"

using namespace OT;

namespace OTGUI {

/* Default constructor */
MetaModelAnalysis::MetaModelAnalysis()
  : DesignOfExperimentAnalysis()
  , isDistributionComputed_(false)
  , leaveOneOutValidation_(false)
{
}


/* Constructor with parameters */
MetaModelAnalysis::MetaModelAnalysis(const String& name, const DesignOfExperiment& designOfExperiment)
  : DesignOfExperimentAnalysis(name, designOfExperiment)
  , isDistributionComputed_(false)
  , leaveOneOutValidation_(false)
{
  setInterestVariables(designOfExperiment_.getOutputSample().getDescription());
}


void MetaModelAnalysis::setDesignOfExperiment(const DesignOfExperiment& designOfExperiment)
{
  isDistributionComputed_ = false;
  DesignOfExperimentAnalysis::setDesignOfExperiment(designOfExperiment);
}


void MetaModelAnalysis::setInterestVariables(const Description& variablesNames)
{
  if (!variablesNames.getSize())
    throw InvalidDimensionException(HERE) << "The number of outputs to analyse must be superior to 0";

  const Description modelVariablesNames(designOfExperiment_.getOutputSample().getDescription());
  for (UnsignedInteger i=0; i<variablesNames.getSize(); ++i)
    if (!modelVariablesNames.contains(variablesNames[i]))
      throw InvalidArgumentException(HERE) << "The name " << variablesNames[i] << " does not match an output name of the model";

  AnalysisImplementation::setInterestVariables(variablesNames);
}


bool MetaModelAnalysis::isLeaveOneOutValidation() const
{
  return leaveOneOutValidation_;
}


void MetaModelAnalysis::setLeaveOneOutValidation(const bool validation)
{
  leaveOneOutValidation_ = validation;
}


NumericalSample MetaModelAnalysis::getEffectiveInputSample() const
{
  // if data model
  if (!designOfExperiment_.hasPhysicalModel())
    return designOfExperiment_.getInputSample();

  NumericalSample effectiveInputSample(designOfExperiment_.getInputSample());
  effectiveInputSample.setDescription(designOfExperiment_.getPhysicalModel().getInputNames());

  // if only deterministic inputs
  if (!designOfExperiment_.getPhysicalModel().hasStochasticInputs())
    return effectiveInputSample;

  // if the physical model has stochastic variables: we do not take into account the deterministic variables
  Indices inputIndices;
  for (UnsignedInteger i=0; i<designOfExperiment_.getPhysicalModel().getInputs().getSize(); ++i)
    if (designOfExperiment_.getPhysicalModel().getInputs()[i].isStochastic())
      inputIndices.add(i);

  return effectiveInputSample.getMarginal(inputIndices);
}


NumericalSample MetaModelAnalysis::getEffectiveOutputSample() const
{
  Indices outputIndices;
  for (UnsignedInteger i=0; i<getInterestVariables().getSize(); ++i)
  {
    bool outputFound = false;
    for (UnsignedInteger j=0; j<designOfExperiment_.getOutputSample().getDescription().getSize(); ++j)
    {
      if (designOfExperiment_.getOutputSample().getDescription()[j] == getInterestVariables()[i])
      {
        outputIndices.add(j);
        outputFound = true;
        break;
      }
    }
    if (!outputFound)
      throw InvalidArgumentException(HERE) << "The output to analyze "  << getInterestVariables()[i] <<" is not an output of the model " << designOfExperiment_.getOutputSample().getDescription();
  }

  NumericalSample effectiveOutputSample(designOfExperiment_.getOutputSample().getMarginal(outputIndices));
  effectiveOutputSample.setDescription(getInterestVariables());

  return effectiveOutputSample;
}


ComposedDistribution MetaModelAnalysis::getDistribution()
{
  if (!isDistributionComputed_)
  {
    // use the composed distribution of the physical model:
    // if the physical model has stochastic variables
    if (designOfExperiment_.hasPhysicalModel() && designOfExperiment_.getPhysicalModel().hasStochasticInputs())
    {
      distribution_ = designOfExperiment_.getPhysicalModel().getComposedDistribution();
    }
    // use Uniform distributions:
    // if the physical model has only deterministic variables
    // else if data model
    else
    {
      // get min/max inputSample
      const NumericalPoint min(designOfExperiment_.getInputSample().getMin());
      const NumericalPoint max(designOfExperiment_.getInputSample().getMax());

      // build Uniform
      ComposedDistribution::DistributionCollection distributionCollection;
      for (UnsignedInteger i=0; i<designOfExperiment_.getInputSample().getDimension(); ++i)
        distributionCollection.add(Uniform(min[i], max[i]));

      distribution_ = ComposedDistribution(distributionCollection);
    }
    isDistributionComputed_ = true;
  }
  return distribution_;
}


void MetaModelAnalysis::buildMetaModel(MetaModelAnalysisResult& result, const NumericalMathFunction& function)
{
  MetaModel metaModel("MetaModel_" + getName() + "_", function);

  const Description inputsNames(function.getInputDescription());
  const Description outputsNames(function.getOutputDescription());

  if (designOfExperiment_.hasPhysicalModel())
  {
    // inputs
    for (UnsignedInteger i=0; i<inputsNames.getSize(); ++i)
      metaModel.addInput(designOfExperiment_.getPhysicalModel().getInputByName(inputsNames[i]));

    // outputs
    for (UnsignedInteger i=0; i<outputsNames.getSize(); ++i)
      metaModel.addOutput(designOfExperiment_.getPhysicalModel().getOutputByName(outputsNames[i]));
  }
  else
  {
    // inputs
    for (UnsignedInteger i=0; i<inputsNames.getSize(); ++i)
      metaModel.addInput(Input(inputsNames[i]));
    // outputs
    for (UnsignedInteger i=0; i<outputsNames.getSize(); ++i)
      metaModel.addOutput(Output(outputsNames[i]));
  }
  result.metaModel_ = metaModel;
}


void MetaModelAnalysis::computeError(const NumericalSample& metaOutSample, const NumericalSample& outSample, NumericalPoint& error, NumericalPoint& q2)
{
  // check
  if (!outSample.getSize())
    throw InvalidValueException(HERE) << "Problem during the validation of the metamodel: outputSample is empty.\n";
  if (!metaOutSample.getSize())
    throw InvalidValueException(HERE) << "Problem during the validation of the metamodel: metaOutSample is empty.\n";

  const UnsignedInteger size = outSample.getSize();
  const UnsignedInteger dimension = outSample.getDimension();
  const NumericalPoint variance(outSample.computeVariance());

  error = NumericalPoint(dimension);
  q2 = NumericalPoint(dimension);

  for (UnsignedInteger i=0; i<dimension; ++i)
  {
    // sum[ (ŷ_j/j - y_j)^2 ]
    double quadraticResidual = 0.;
    for (UnsignedInteger j=0; j<size; ++j)
    {
      const double diff = metaOutSample[j][i] - outSample[j][i];
      quadraticResidual += diff * diff;
    }
    // sqrt ( sum[ (ŷ_j/j - y_j)^2 ] )
    error[i] = sqrt(quadraticResidual) / size;
    // 1 - sum[ (ŷ_j/j - y_j)^2 ] / (n-1) / Var
    q2[i] = 1.0 - (quadraticResidual / (size - 1.0)) / variance[i];
  }
}


void MetaModelAnalysis::validateMetaModelResult(MetaModelAnalysisResult& result, const NumericalSample& inputSample)
{
  // validation: leave-one-out
  if (leaveOneOutValidation_)
  {
    NumericalSample outputSampleLOO(inputSample.getSize(), result.outputSample_.getDimension());

    for (UnsignedInteger i=0; i<inputSample.getSize(); ++i)
    {
      // remove input_i
      NumericalSample inLearnSample(inputSample);
      inLearnSample.erase(i);
      NumericalSample outLearnSample(result.outputSample_);
      outLearnSample.erase(i);

      // build and run OT::MetaModel Algorithm
      NumericalMathFunction function(runAlgo(inLearnSample, outLearnSample));

      // evaluate output at input_i
      NumericalPoint outputValuesForInput_i(function(inputSample[i]));

      // fill sample
      for (UnsignedInteger j=0; j<result.outputSample_.getDimension(); ++j)
        outputSampleLOO[i][j] = outputValuesForInput_i[j];
    }
    result.metaModelOutputSampleLOO_ = outputSampleLOO;

    // compute Q2
    computeError(result.metaModelOutputSampleLOO_, result.outputSample_, result.errorQ2LOO_, result.q2LOO_);
  }
}


/* Method save() stores the object through the StorageManager */
void MetaModelAnalysis::save(Advocate& adv) const
{
  DesignOfExperimentAnalysis::save(adv);
  adv.saveAttribute("leaveOneOutValidation_", leaveOneOutValidation_);
}


/* Method load() reloads the object from the StorageManager */
void MetaModelAnalysis::load(Advocate& adv)
{
  DesignOfExperimentAnalysis::load(adv);
  adv.loadAttribute("leaveOneOutValidation_", leaveOneOutValidation_);
}
}