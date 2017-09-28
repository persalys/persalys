//                                               -*- C++ -*-
/**
 *  @brief Class to define functional chaos
 *
 *  Copyright 2015-2017 EDF-Phimeca
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
#include "otgui/DesignOfExperimentEvaluation.hxx"

#include <openturns/Uniform.hxx>

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
  if (designOfExperiment_.getOutputSample().getSize())
    setInterestVariables(designOfExperiment_.getOutputSample().getDescription());
}


/* Constructor with parameters */
MetaModelAnalysis::MetaModelAnalysis(const String& name, const Analysis& analysis)
  : DesignOfExperimentAnalysis(name)
  , isDistributionComputed_(false)
  , leaveOneOutValidation_(false)
{
  SimulationAnalysis * analysis_ptr = dynamic_cast<SimulationAnalysis*>(analysis.getImplementation().get());

  if (!analysis_ptr)
  {
    throw InvalidArgumentException(HERE) << "The given analysis does not contain any design of experiments";
  }
  designOfExperiment_ = analysis_ptr->getDesignOfExperiment();
  setInterestVariables(analysis_ptr->getInterestVariables());
}


bool MetaModelAnalysis::isLeaveOneOutValidation() const
{
  return leaveOneOutValidation_;
}


void MetaModelAnalysis::setLeaveOneOutValidation(const bool validation)
{
  leaveOneOutValidation_ = validation;
}


Sample MetaModelAnalysis::getEffectiveInputSample() const
{
  // if data model
  if (!designOfExperiment_.hasPhysicalModel())
    return designOfExperiment_.getInputSample();

  // if only deterministic inputs
  if (!designOfExperiment_.getPhysicalModel().hasStochasticInputs() || !designOfExperiment_.getInputSample().getSize())
    return designOfExperiment_.getInputSample();

  // if the physical model has stochastic variables: we do not take into account the deterministic variables
  Indices inputIndices;
  for (UnsignedInteger i = 0; i < designOfExperiment_.getPhysicalModel().getInputs().getSize(); ++i)
    if (designOfExperiment_.getPhysicalModel().getInputs()[i].isStochastic())
      inputIndices.add(i);

  if (!inputIndices.check(designOfExperiment_.getInputSample().getDimension()))
    throw InvalidArgumentException(HERE) << "The design of experiments input sample dimension ("
                                         << designOfExperiment_.getInputSample().getDimension()
                                         << ") does not match the number of stochastic inputs in the physical model ("
                                         << inputIndices.getSize() << ")";

  return designOfExperiment_.getInputSample().getMarginal(inputIndices);
}


Sample MetaModelAnalysis::getEffectiveOutputSample() const
{
  if (!getInterestVariables().getSize())
    throw InvalidDimensionException(HERE) << "You have not defined output variable to be analysed. Set the list of interest variables.";

  const Description modelOutputsNames(designOfExperiment_.getOutputSample().getDescription());
  Indices outputIndices;
  for (UnsignedInteger i = 0; i < getInterestVariables().getSize(); ++i)
  {
    bool outputFound = false;
    for (UnsignedInteger j = 0; j < modelOutputsNames.getSize(); ++j)
    {
      if (modelOutputsNames[j] == getInterestVariables()[i])
      {
        outputIndices.add(j);
        outputFound = true;
        break;
      }
    }
    if (!outputFound)
      throw InvalidArgumentException(HERE) << "The output to analyze "  << getInterestVariables()[i]
                                           << " is not an output of the model " << modelOutputsNames
                                           << ". Check the list of interest variables.";
  }

  Sample effectiveOutputSample(designOfExperiment_.getOutputSample().getMarginal(outputIndices));
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
      const Point min(designOfExperiment_.getInputSample().getMin());
      const Point max(designOfExperiment_.getInputSample().getMax());

      // build Uniform
      ComposedDistribution::DistributionCollection distributionCollection;
      for (UnsignedInteger i = 0; i < designOfExperiment_.getInputSample().getDimension(); ++i)
        distributionCollection.add(Uniform(min[i], max[i]));

      distribution_ = ComposedDistribution(distributionCollection);
    }
    isDistributionComputed_ = true;
  }
  return distribution_;
}


void MetaModelAnalysis::buildMetaModel(MetaModelAnalysisResult& result, const Function& function)
{
  MetaModel metaModel("MetaModel_", function);

  const Description inputsNames(function.getInputDescription());
  const Description outputsNames(function.getOutputDescription());

  // inputs
  for (UnsignedInteger i = 0; i < inputsNames.getSize(); ++i)
  {
    if (designOfExperiment_.getPhysicalModel().getInputNames().contains(inputsNames[i]))
      metaModel.addInput(designOfExperiment_.getPhysicalModel().getInputByName(inputsNames[i]));
    else
      metaModel.addInput(Input(inputsNames[i]));
  }

  // outputs
  for (UnsignedInteger i = 0; i < outputsNames.getSize(); ++i)
  {
    if (designOfExperiment_.getPhysicalModel().getOutputNames().contains(outputsNames[i]))
    {
      Output output(designOfExperiment_.getPhysicalModel().getOutputByName(outputsNames[i]));
      if (!output.getDescription().empty())
        output.setDescription(output.getDescription() + " - Meta model generated by " + getName());
      else
        output.setDescription("Meta model generated by " + getName());
      output.setValue(0.);
      output.setHasBeenComputed(false);
      metaModel.addOutput(output);
    }
    else
    {
      metaModel.addOutput(Output(outputsNames[i], getName() + " metamodel"));
    }
  }

  // copula
  if (designOfExperiment_.getPhysicalModel().hasStochasticInputs())
    metaModel.setCopula(designOfExperiment_.getPhysicalModel().getCopula());

  result.metaModel_ = metaModel;
}


void MetaModelAnalysis::computeError(const Sample& metaOutSample, const Sample& outSample, Point& error, Point& q2)
{
  // check
  if (!outSample.getSize())
    throw InvalidValueException(HERE) << "Problem during the validation of the metamodel: outputSample is empty.\n";
  if (!metaOutSample.getSize())
    throw InvalidValueException(HERE) << "Problem during the validation of the metamodel: metaOutSample is empty.\n";

  const UnsignedInteger size = outSample.getSize();
  const UnsignedInteger dimension = outSample.getDimension();
  const Point variance(outSample.computeVariance());

  error = Point(dimension);
  q2 = Point(dimension);

  for (UnsignedInteger i = 0; i < dimension; ++i)
  {
    // sum[ (ŷ_j/j - y_j)^2 ]
    double quadraticResidual = 0.;
    for (UnsignedInteger j = 0; j < size; ++j)
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


void MetaModelAnalysis::validateMetaModelResult(MetaModelAnalysisResult& result, const Sample& inputSample)
{
  // validation: leave-one-out
  if (leaveOneOutValidation_)
  {
    // check
    if (!result.outputSample_.getSize())
      throw InvalidValueException(HERE) << "Problem during the creation of the metamodel: The outputSample is empty.\n";

    informationMessage_ = "Meta model has been created.\nThe validation is running.";
    notify("informationMessageUpdated");

    Sample outputSampleLOO(inputSample.getSize(), result.outputSample_.getDimension());

    for (UnsignedInteger i = 0; i < inputSample.getSize(); ++i)
    {
      if (stopRequested_)
      {
        leaveOneOutValidation_ = false;
        return;
      }

      progressValue_ = (int) (i * 100 / inputSample.getSize());
      notify("progressValueChanged");

      // remove input_i
      Sample inLearnSample(inputSample);
      inLearnSample.erase(i);
      Sample outLearnSample(result.outputSample_);
      outLearnSample.erase(i);

      // build and run OT::MetaModel Algorithm
      Function function(runAlgo(inLearnSample, outLearnSample));

      // evaluate output at input_i
      Point outputValuesForInput_i(function(inputSample[i]));

      // fill sample
      for (UnsignedInteger j = 0; j < result.outputSample_.getDimension(); ++j)
        outputSampleLOO[i][j] = outputValuesForInput_i[j];
    }
    result.metaModelOutputSampleLOO_ = outputSampleLOO;

    // compute Q2
    computeError(result.metaModelOutputSampleLOO_, result.outputSample_, result.errorQ2LOO_, result.q2LOO_);
  }
}


/* String converter */
String MetaModelAnalysis::__repr__() const
{
  OSS oss;
  oss << DesignOfExperimentAnalysis::__repr__()
      << " leaveOneOutValidation=" << isLeaveOneOutValidation();
  return oss;
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
