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
#include "otgui/FunctionalChaosAnalysis.hxx"

#include "otgui/MetaModel.hxx"

#include <openturns/LinearEnumerateFunction.hxx>
#include <openturns/FixedStrategy.hxx>
#include <openturns/LeastSquaresStrategy.hxx>
#include <openturns/BasisSequenceFactory.hxx>
#include <openturns/LeastSquaresMetaModelSelectionFactory.hxx>
#include <openturns/LegendreFactory.hxx>
#include <openturns/LARS.hxx>
#include <openturns/Uniform.hxx>
#include <openturns/StandardDistributionPolynomialFactory.hxx>
#include <openturns/FunctionalChaosRandomVector.hxx>
#include <openturns/PersistentObjectFactory.hxx>

using namespace OT;

namespace OTGUI {

CLASSNAMEINIT(FunctionalChaosAnalysis);

static Factory<FunctionalChaosAnalysis> Factory_FunctionalChaosAnalysis;

/* Default constructor */
FunctionalChaosAnalysis::FunctionalChaosAnalysis()
  : MetaModelAnalysis()
  , chaosDegree_(2)
  , sparseChaos_(false)
{
}


/* Constructor with parameters */
FunctionalChaosAnalysis::FunctionalChaosAnalysis(const String & name, const DesignOfExperiment & designOfExperiment)
  : MetaModelAnalysis(name, designOfExperiment)
  , chaosDegree_(2)
  , sparseChaos_(false)
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


void FunctionalChaosAnalysis::setDesignOfExperiment(const DesignOfExperiment& designOfExperiment)
{
  polynomialFamilyCollection_.clear();
  MetaModelAnalysis::setDesignOfExperiment(designOfExperiment);
}


OrthogonalProductPolynomialFactory::PolynomialFamilyCollection FunctionalChaosAnalysis::getPolynomialFamilyCollection()
{
  if (polynomialFamilyCollection_.isEmpty())
  {
    // distribution:
    ComposedDistribution distribution(getDistribution());

    // adaptiveStrategy
    for (UnsignedInteger i = 0; i < distribution.getDimension(); ++i)
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
  isRunning_ = true;
  try
  {
    // clear result
    initialize();
    result_ = FunctionalChaosAnalysisResult();

    // check
    if (designOfExperiment_.getInputSample().getSize()*designOfExperiment_.getOutputSample().getSize() == 0)
      throw InvalidArgumentException(HERE) << "The design of experiment must contains not empty input AND output samples";
    if (designOfExperiment_.getInputSample().getSize() != designOfExperiment_.getOutputSample().getSize())
      throw InvalidArgumentException(HERE) << "The input sample and the output sample must have the same size";
    if (!getInterestVariables().getSize())
      throw InvalidDimensionException(HERE) << "You have not defined output variable to be analysed. Set the list of interest variables.";

    // get effective samples
    const Sample effectiveInputSample(getEffectiveInputSample());
    const Sample effectiveOutputSample(getEffectiveOutputSample());

    // check chaos degree
    if (!getSparseChaos())
    {
      const UnsignedInteger inputDimension = effectiveInputSample.getDimension();
      const UnsignedInteger size = designOfExperiment_.getOutputSample().getSize();
      const UnsignedInteger minimumSize  = BinomialCoefficient(chaosDegree_ + inputDimension, chaosDegree_);
      if (size < minimumSize)
        throw InvalidArgumentException(HERE) << "Design of experiment size too small : "
                                            << size
                                            << ". It must be superior or equal to C(degree+nbInputs, degree) = "
                                            << minimumSize << ")\n";
    }

    // create FunctionalChaosAlgorithm and run it
    FunctionalChaosAlgorithm functionalChaos(buildFunctionalChaosAlgorithm(effectiveInputSample, effectiveOutputSample));
    functionalChaos.run();

    // set result_
    result_.outputSample_ = effectiveOutputSample;
    result_.functionalChaosResult_ = functionalChaos.getResult();

    // build metamodel
    Function metamodelFunction(result_.functionalChaosResult_.getMetaModel());
    Description variablesNames(effectiveInputSample.getDescription());
    variablesNames.add(effectiveOutputSample.getDescription());
    metamodelFunction.setDescription(variablesNames);
    buildMetaModel(result_, metamodelFunction);

    result_.metaModelOutputSample_ = metamodelFunction(effectiveInputSample);

    // post process
    try
    {
      postProcessFunctionalChaosResult(effectiveInputSample);
    }
    catch (std::exception & ex)
    {
      errorMessage_ = OSS() << "Impossible to compute Sobol indices and moments.\n" << ex.what() << "\nTry to increase the size of the design of experiment.";
    }

    // validation
    validateMetaModelResult(result_, effectiveInputSample);

    notify("analysisFinished");
  }
  catch (std::exception & ex)
  {
    errorMessage_ =  OSS() << errorMessage_ << ex.what();
    notify("analysisBadlyFinished");
  }
  isRunning_ = false;
}


Function FunctionalChaosAnalysis::runAlgo(const Sample& inputSample, const Sample& outputSample)
{
  FunctionalChaosAlgorithm functionalChaos(buildFunctionalChaosAlgorithm(inputSample, outputSample));
  functionalChaos.run();

  return functionalChaos.getResult().getMetaModel();
}


FunctionalChaosAlgorithm FunctionalChaosAnalysis::buildFunctionalChaosAlgorithm(const Sample& inputSample, const Sample& outputSample)
{
  const UnsignedInteger inputDimension = inputSample.getDimension();
  const EnumerateFunction phi(inputDimension);

  // adaptiveStrategy
  const OrthogonalProductPolynomialFactory multivariateBasis(getPolynomialFamilyCollection(), phi);
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


void FunctionalChaosAnalysis::postProcessFunctionalChaosResult(const Sample& inputSample)
{
  // check
  if (!result_.outputSample_.getSize())
    throw InvalidValueException(HERE) << "Problem during the creation of the metamodel: The outputSample is empty.\n";
  if (!result_.functionalChaosResult_.getCoefficients().getSize())
    throw InvalidValueException(HERE) << "Problem during the creation of the metamodel: The FunctionalChaosResult is empty.\n";

  // get dim
  const UnsignedInteger inputDimension = inputSample.getDimension();
  const UnsignedInteger outputDimension = result_.outputSample_.getDimension();

  // get FunctionalChaosRandomVector
  const FunctionalChaosRandomVector vector(result_.functionalChaosResult_);

  // mean
  result_.mean_ = vector.getMean();

  // variance - sobol indices
  Point variance(outputDimension);
  Sample firstOrderIndices(outputDimension, inputDimension);
  firstOrderIndices.setDescription(inputSample.getDescription());
  Sample totalIndices(outputDimension, inputDimension);

  for (UnsignedInteger i = 0; i < outputDimension; ++i)
  {
    variance[i] = vector.getCovariance()(i, i);

    for (UnsignedInteger j = 0; j < inputDimension; ++j)
    {
      firstOrderIndices[i][j] = vector.getSobolIndex(j, i);
      totalIndices[i][j] = vector.getSobolTotalIndex(j, i);
    }
  }
  result_.variance_ = variance;
  result_.sobolResult_ = SobolResult(firstOrderIndices, totalIndices, result_.outputSample_.getDescription());
}


FunctionalChaosAnalysisResult FunctionalChaosAnalysis::getResult() const
{
  return result_;
}


String FunctionalChaosAnalysis::getPythonScript() const
{
  OSS oss;
  oss << getName() << " = otguibase.FunctionalChaosAnalysis('" << getName() << "', " << getDesignOfExperiment().getName() << ")\n";
  if (getInterestVariables().getSize() < getDesignOfExperiment().getOutputSample().getDimension())
  {
    oss << "interestVariables = [";
    for (UnsignedInteger i = 0; i < getInterestVariables().getSize(); ++i)
    {
      oss << "'" << getInterestVariables()[i] << "'";
      if (i < getInterestVariables().getSize() - 1)
        oss << ", ";
    }
    oss << "]\n";
    oss << getName() << ".setInterestVariables(interestVariables)\n";
  }
  oss << getName() << ".setChaosDegree(" << getChaosDegree() << ")\n";
  oss << getName() << ".setSparseChaos(" << (getSparseChaos() ? "True" : "False") << ")\n";
  oss << getName() << ".setLeaveOneOutValidation(" << (isLeaveOneOutValidation() ? "True" : "False") << ")\n";

  return oss;
}


bool FunctionalChaosAnalysis::analysisLaunched() const
{
  return getResult().getMetaModelOutputSample().getSize();
}


/* String converter */
String FunctionalChaosAnalysis::__repr__() const
{
  OSS oss;
  oss << MetaModelAnalysis::__repr__()
      << " chaosDegree=" << getChaosDegree()
      << " sparseChaos=" << getSparseChaos();
  return oss;
}


/* Method save() stores the object through the StorageManager */
void FunctionalChaosAnalysis::save(Advocate& adv) const
{
  DesignOfExperimentAnalysis::save(adv);
  adv.saveAttribute("chaosDegree_", chaosDegree_);
  adv.saveAttribute("sparseChaos_", sparseChaos_);
  adv.saveAttribute("result_", result_);
}


/* Method load() reloads the object from the StorageManager */
void FunctionalChaosAnalysis::load(Advocate& adv)
{
  DesignOfExperimentAnalysis::load(adv);
  adv.loadAttribute("chaosDegree_", chaosDegree_);
  adv.loadAttribute("sparseChaos_", sparseChaos_);
  adv.loadAttribute("result_", result_);
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
