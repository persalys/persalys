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
#include "otgui/FunctionalChaosAnalysis.hxx"

#include "otgui/MetaModel.hxx"

#include "openturns/LinearEnumerateFunction.hxx"
#include "openturns/FixedStrategy.hxx"
#include "openturns/LeastSquaresStrategy.hxx"
#include "openturns/BasisSequenceFactory.hxx"
#include "openturns/LeastSquaresMetaModelSelectionFactory.hxx"
#include "openturns/LegendreFactory.hxx"
#include "openturns/LARS.hxx"
#include "openturns/Uniform.hxx"
#include "openturns/StandardDistributionPolynomialFactory.hxx"
#include "openturns/FunctionalChaosRandomVector.hxx"
#include "openturns/PersistentObjectFactory.hxx"

using namespace OT;

namespace OTGUI {

CLASSNAMEINIT(FunctionalChaosAnalysis);

static Factory<FunctionalChaosAnalysis> RegisteredFactory;

/* Default constructor */
FunctionalChaosAnalysis::FunctionalChaosAnalysis()
  : DesignOfExperimentAnalysis()
  , isDistributionComputed_(false)
  , chaosDegree_(2)
  , sparseChaos_(false)
  , leaveOneOutValidation_(false)
{
}


/* Constructor with parameters */
FunctionalChaosAnalysis::FunctionalChaosAnalysis(const String & name, const DesignOfExperiment & designOfExperiment)
  : DesignOfExperimentAnalysis(name, designOfExperiment)
  , isDistributionComputed_(false)
  , chaosDegree_(2)
  , sparseChaos_(false)
  , leaveOneOutValidation_(false)
{
}


/* Virtual constructor */
FunctionalChaosAnalysis* FunctionalChaosAnalysis::clone() const
{
  return new FunctionalChaosAnalysis(*this);
}


UnsignedInteger FunctionalChaosAnalysis::getChaosDegree() const
{
  return chaosDegree_;
}


void FunctionalChaosAnalysis::setDesignOfExperiment(const DesignOfExperiment& designOfExperiment)
{
  isDistributionComputed_ = false;
  polynomialFamilyCollection_.clear();
  DesignOfExperimentAnalysis::setDesignOfExperiment(designOfExperiment);
}


void FunctionalChaosAnalysis::setChaosDegree(const UnsignedInteger degree)
{
  chaosDegree_ = degree;
}


bool FunctionalChaosAnalysis::getSparseChaos() const
{
  return sparseChaos_;
}


void FunctionalChaosAnalysis::setSparseChaos(const bool sparse)
{
  sparseChaos_ = sparse;
}


bool FunctionalChaosAnalysis::isLeaveOneOutValidation() const
{
  return leaveOneOutValidation_;
}


void FunctionalChaosAnalysis::setLeaveOneOutValidation(const bool validation)
{
  leaveOneOutValidation_ = validation;
}


ComposedDistribution FunctionalChaosAnalysis::getDistribution()
{
  if (!isDistributionComputed_)
  {
    if (designOfExperiment_.hasPhysicalModel() && designOfExperiment_.getPhysicalModel().hasStochasticInputs())
    {
      distribution_ = designOfExperiment_.getPhysicalModel().getComposedDistribution();
    }
    else
    {
      ComposedDistribution::DistributionCollection distributionCollection;
      const NumericalPoint min(designOfExperiment_.getInputSample().getMin());
      const NumericalPoint max(designOfExperiment_.getInputSample().getMax());
      for (UnsignedInteger i=0; i<designOfExperiment_.getInputSample().getDimension(); ++i)
        distributionCollection.add(Uniform(min[i], max[i]));
      distribution_ = ComposedDistribution(distributionCollection);
    }
    isDistributionComputed_ = true;
  }
  return distribution_;
}


OrthogonalProductPolynomialFactory::PolynomialFamilyCollection FunctionalChaosAnalysis::getPolynomialFamilyCollection()
{
  if (polynomialFamilyCollection_.isEmpty())
  {
    // distribution:
    ComposedDistribution distribution(getDistribution());

    // adaptiveStrategy
    for (UnsignedInteger i=0; i<designOfExperiment_.getInputSample().getDimension(); ++i)
    {
      const Distribution marginal(distribution.getMarginal(i));
      const StandardDistributionPolynomialFactory factory(marginal);
      polynomialFamilyCollection_.add(factory);
    }
  }
  return polynomialFamilyCollection_;
}


void FunctionalChaosAnalysis::run()
{
  if (designOfExperiment_.getInputSample().getSize()*designOfExperiment_.getOutputSample().getSize() == 0)
    throw InvalidArgumentException(HERE) << "The design of experiment must contains not empty input AND output samples";
  if (!getOutputsToAnalyse().getSize())
    throw InvalidDimensionException(HERE) << "The number of outputs to analyse must be superior to 0";

  // clear result
  result_ = FunctionalChaosAnalysisResult();

  // get marginals
  Indices indices;
  for (UnsignedInteger i=0; i<getOutputsToAnalyse().getSize(); ++i)
  {
    bool outputFound = false;
    for (UnsignedInteger j=0; j<designOfExperiment_.getOutputSample().getDescription().getSize(); ++j)
    {
      if (designOfExperiment_.getOutputSample().getDescription()[j] == getOutputsToAnalyse()[i])
      {
        indices.add(j);
        outputFound = true;
        break;
      }
    }
    if (!outputFound)
      throw InvalidArgumentException(HERE) << "The output to analyse "  << getOutputsToAnalyse()[i] <<" is not an output of the model " << designOfExperiment_.getOutputSample().getDescription();
  }

  const UnsignedInteger inputDimension = designOfExperiment_.getInputSample().getDimension();
  const UnsignedInteger outputDimension = getOutputsToAnalyse().getSize();

  // check chaos degree
  if (!getSparseChaos())
  {
    const UnsignedInteger minimumSize  = BinomialCoefficient(chaosDegree_ + inputDimension, chaosDegree_);
    if (designOfExperiment_.getInputSample().getSize() < minimumSize)
      throw InvalidArgumentException(HERE) << "Design of experiment size too small : "
                                           << designOfExperiment_.getInputSample().getSize()
                                           << ". It must be superior or equal to C(degree+nbInputs, degree) = "
                                           << minimumSize << ")\n";
  }

  // create FunctionalChaosAlgorithm and run it
  FunctionalChaosAlgorithm functionalChaos(buildFunctionalChaosAlgorithm(designOfExperiment_.getInputSample(), designOfExperiment_.getOutputSample().getMarginal(indices)));
  functionalChaos.run();

  // set result_
  result_.outputSample_ = designOfExperiment_.getOutputSample().getMarginal(indices);
  // - ot result
  result_.functionalChaosResult_ = functionalChaos.getResult();
  postProcessFunctionalChaosResult();

  // validation: leave one out
  if (leaveOneOutValidation_)
  {
    NumericalSample outputSampleLOO(result_.outputSample_.getSize(), outputDimension);
    for (UnsignedInteger i=0; i<result_.outputSample_.getSize(); ++i)
    {
      NumericalSample inSample(designOfExperiment_.getInputSample());
      inSample.erase(i);
      NumericalSample outSample(result_.outputSample_);
      outSample.erase(i);
      // build and run FunctionalChaosAlgorithm
      FunctionalChaosAlgorithm functionalChaosLOO(buildFunctionalChaosAlgorithm(inSample, outSample));
      functionalChaosLOO.run();
      NumericalPoint outputValuesForInput_i(functionalChaosLOO.getResult().getMetaModel()(designOfExperiment_.getInputSample()[i]));
      for (UnsignedInteger j=0; j<outputDimension; ++j)
        outputSampleLOO[i][j] = outputValuesForInput_i[j];
    }
    result_.metaModelOutputSampleLOO_ = outputSampleLOO;
    computeErrorQ2LOO();
  }

  notify("fromFunctionalChaosMetaModelCreated");
  notify("analysisFinished");
}


FunctionalChaosAlgorithm FunctionalChaosAnalysis::buildFunctionalChaosAlgorithm(const NumericalSample& inputSample, const NumericalSample& outputSample)
{
  const UnsignedInteger inputDimension = inputSample.getDimension();

  // adaptiveStrategy
  const OrthogonalProductPolynomialFactory multivariateBasis(getPolynomialFamilyCollection(), inputDimension);
  const UnsignedInteger baseDim = LinearEnumerateFunction(inputDimension).getStrataCumulatedCardinal(chaosDegree_);
  const FixedStrategy adaptiveStrategy(multivariateBasis, baseDim);

  // projectionStrategy
  LeastSquaresStrategy projectionStrategy;
  if (sparseChaos_)
  {
    BasisSequenceFactory basisSequenceFactory = LARS();
    basisSequenceFactory.setMaximumRelativeConvergence(-1.0);
    projectionStrategy = LeastSquaresMetaModelSelectionFactory(basisSequenceFactory,
                                                               CorrectedLeaveOneOut());
  }

  // FunctionalChaosAlgorithm
  FunctionalChaosAlgorithm functionalChaos(inputSample,
                                           outputSample,
                                           getDistribution(),
                                           adaptiveStrategy,
                                           projectionStrategy);

  return functionalChaos;
}


void FunctionalChaosAnalysis::postProcessFunctionalChaosResult()
{
  if (!result_.outputSample_.getSize())
    throw InvalidValueException(HERE) << "Problem during the creation of the metamodel: The outputSample is empty.\n";
  if (!result_.functionalChaosResult_.getCoefficients().getSize())
    throw InvalidValueException(HERE) << "Problem during the creation of the metamodel: The FunctionalChaosResult is empty.\n";

  const UnsignedInteger inputDimension = designOfExperiment_.getInputSample().getDimension();
  const UnsignedInteger outputDimension = result_.outputSample_.getDimension();

  // - metamodel
  MetaModel metaModel("MetaModel_" + getName() + "_", result_.functionalChaosResult_.getMetaModel());
  if (designOfExperiment_.hasPhysicalModel())
  {
    metaModel.setInputs(designOfExperiment_.getPhysicalModel().getInputs());
    if (designOfExperiment_.getPhysicalModel().getOutputs().getSize() == outputDimension)
    {
      metaModel.setOutputs(designOfExperiment_.getPhysicalModel().getOutputs());
    }
    else
    {
      OutputCollection outputs;
      for (UnsignedInteger i=0; i<designOfExperiment_.getPhysicalModel().getOutputs().getSize(); ++i)
        if (getOutputsToAnalyse().contains(designOfExperiment_.getPhysicalModel().getOutputs()[i].getName()))
          outputs.add(designOfExperiment_.getPhysicalModel().getOutputs()[i]);
    }
  }
  else
  {
    InputCollection inputs;
    for (UnsignedInteger i=0; i<inputDimension; ++i)
      inputs.add(Input(designOfExperiment_.getInputSample().getDescription()[i]));
    OutputCollection outputs;
    for (UnsignedInteger i=0; i<outputDimension; ++i)
      outputs.add(Output(result_.outputSample_.getDescription()[i]));

    metaModel.setInputs(inputs);
    metaModel.setOutputs(outputs);
  }
  result_.metaModel_ = metaModel;

  // Post-process the results
  result_.metaModelOutputSample_ = result_.functionalChaosResult_.getMetaModel()(designOfExperiment_.getInputSample());

  FunctionalChaosRandomVector vector(result_.functionalChaosResult_);
  result_.mean_ = vector.getMean();

  NumericalPoint variance(outputDimension);
  NumericalSample firstOrderIndices(outputDimension, inputDimension);
  firstOrderIndices.setDescription(designOfExperiment_.getInputSample().getDescription());
  NumericalSample totalIndices(outputDimension, inputDimension);

  for (UnsignedInteger i=0; i<outputDimension; ++i)
  {
    variance[i] = vector.getCovariance()(i, i);

    for (UnsignedInteger j=0; j<inputDimension; ++j)
    {
      firstOrderIndices[i][j] = vector.getSobolIndex(j, i);
      totalIndices[i][j] = vector.getSobolTotalIndex(j, i);
    }
  }
  result_.variance_ = variance;
  result_.sobolResult_ = SobolResult(firstOrderIndices, totalIndices, result_.outputSample_.getDescription());
}


void FunctionalChaosAnalysis::computeErrorQ2LOO()
{
  if (!result_.outputSample_.getSize())
    throw InvalidValueException(HERE) << "Problem during the creation of the metamodel: The outputSample is empty.\n";
  if (result_.variance_.getSize() != result_.outputSample_.getDimension())
    throw InvalidValueException(HERE) << "Problem during the creation of the metamodel: The variance is not valid.\n";

  const UnsignedInteger outputDimension = result_.outputSample_.getDimension();
  const UnsignedInteger size = result_.outputSample_.getSize();

  result_.errorQ2LOO_ = NumericalPoint(outputDimension);
  result_.q2LOO_ = NumericalPoint(outputDimension);

  for (UnsignedInteger i=0; i<outputDimension; ++i)
  {
    double quadraticResidual = 0.;
    for (UnsignedInteger j=0; j<size; ++j)
    {
      const double diff = result_.metaModelOutputSampleLOO_[j][i] - result_.outputSample_[j][i];
      quadraticResidual += diff*diff;
    }
    result_.errorQ2LOO_[i] = sqrt(quadraticResidual) / size;
    result_.q2LOO_[i] = 1 - (quadraticResidual/(size-1)) / result_.variance_[i];
  }
}


FunctionalChaosAnalysisResult FunctionalChaosAnalysis::getResult() const
{
  return result_;
}


String FunctionalChaosAnalysis::getPythonScript() const
{
  OSS oss;
  oss << getName() << " = otguibase.FunctionalChaosAnalysis('" << getName() << "', " << getDesignOfExperiment().getName() << ")\n";
  if (getOutputsToAnalyse().getSize() < getDesignOfExperiment().getOutputSample().getDimension())
  {
    oss << "outputsToAnalyse = [";
    for (UnsignedInteger i=0; i<getOutputsToAnalyse().getSize(); ++i)
    {
      oss << "'" << getOutputsToAnalyse()[i] << "'";
      if (i < getOutputsToAnalyse().getSize()-1)
        oss << ", ";
    }
    oss << "]\n";
    oss << getName() << ".setOutputsToAnalyse(outputsToAnalyse)\n";
  }
  oss << getName() << ".setChaosDegree(" << getChaosDegree() << ")\n";
  oss << getName() << ".setSparseChaos(" << (getSparseChaos()? "True" : "False") << ")\n";
  oss << getName() << ".setLeaveOneOutValidation(" << (isLeaveOneOutValidation()? "True" : "False") << ")\n";

  return oss;
}


bool FunctionalChaosAnalysis::analysisLaunched() const
{
  return getResult().getSobolResult().getFirstOrderIndices().getSize();
}


/* String converter */
String FunctionalChaosAnalysis::__repr__() const
{
  OSS oss;
  oss << "class=" << GetClassName()
      << " name=" << getName()
      << " designOfExperiment=class=" << getDesignOfExperiment().GetClassName() << " name=" << getDesignOfExperiment().getName()
      << " chaosDegree=" << getChaosDegree()
      << " sparseChaos=" << getSparseChaos()
      << " leaveOneOutValidation=" << isLeaveOneOutValidation();
  return oss;
}


void FunctionalChaosAnalysis::save(Advocate& adv) const
{
  DesignOfExperimentAnalysis::save(adv);
  adv.saveAttribute("chaosDegree_", chaosDegree_);
  adv.saveAttribute("sparseChaos_", sparseChaos_);
  adv.saveAttribute("leaveOneOutValidation_", leaveOneOutValidation_);
  adv.saveAttribute("result_", result_);
}


void FunctionalChaosAnalysis::load(Advocate& adv)
{
  DesignOfExperimentAnalysis::load(adv);
  adv.loadAttribute("chaosDegree_", chaosDegree_);
  adv.loadAttribute("sparseChaos_", sparseChaos_);
  adv.loadAttribute("leaveOneOutValidation_", leaveOneOutValidation_);
  adv.loadAttribute("result_", result_);
  isDistributionComputed_ = false;
}


UnsignedInteger FunctionalChaosAnalysis::BinomialCoefficient(const UnsignedInteger n,
                                                             const UnsignedInteger k)
{
  if (k > n) return 0; // by convention
  UnsignedInteger value = 1;
  for (UnsignedInteger i = 0; i < std::min(k, n - k); ++ i)
  {
    value *= n - i;
    value /= i + 1;
  }
  return value;
}
}