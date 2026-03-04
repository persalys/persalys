//                                               -*- C++ -*-
/**
 *  @brief Base class to make a sensitivity analysis of a data model
 *
 *  Copyright 2015-2025 EDF-Phimeca
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
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "persalys/DataSensitivityAnalysis.hxx"

#include <openturns/RankSobolSensitivityAlgorithm.hxx>
#include <openturns/PersistentObjectFactory.hxx>
#include <openturns/HypothesisTest.hxx>
#include <openturns/CorrelationAnalysis.hxx>
#include <openturns/BootstrapExperiment.hxx>
#include <openturns/HSICEstimatorGlobalSensitivity.hxx>
#include <openturns/HSICUStat.hxx>
#include <openturns/HSICVStat.hxx>

using namespace OT;

namespace PERSALYS
{
CLASSNAMEINIT(DataSensitivityAnalysis)

const static Factory<DataSensitivityAnalysis> Factory_DataSensitivityAnalysis;

DataSensitivityAnalysis::DataSensitivityAnalysis(const String &name, 
  const DesignOfExperiment & design, 
  const unsigned char analysisType,
  const OT::Collection<OT::CovarianceModel> &covarianceModels,
  bool computeCovModelParameters)
  : DesignOfExperimentAnalysis(name, design)
  , type_(std::byte{analysisType})
  , covarianceModels_(covarianceModels)
  , computeCovModelParameters_(computeCovModelParameters)
{
  UnsignedInteger variablesNumber = design.getInputSample().getDimension() + design.getOutputSample().getDimension();
  if (type_.computeGlobalHSIC() && covarianceModels_.getSize() != variablesNumber)
    throw InvalidArgumentException(HERE) << "The number of covariance models (" << covarianceModels_.getSize() << ") must be equal to the total number of input and output variables (" << variablesNumber << ") when HSIC indices are computed.";
}

DataSensitivityAnalysis * DataSensitivityAnalysis::clone() const
{
  return new DataSensitivityAnalysis(*this);
}

void DataSensitivityAnalysis::setHSICParameters(bool computeAsymptoticPValues, bool computePermutationPValues, bool useUStatistic)
{
  computeAsymptoticPValues_   = computeAsymptoticPValues;
  computePermutationPValues_  = computePermutationPValues;
  useUStatistic_              = useUStatistic;
}

bool DataSensitivityAnalysis::canBeLaunched(String &errorMessage) const
{
  return DataSensitivityAnalysis::CanBeLaunched(errorMessage, designOfExperiment_);
}

bool DataSensitivityAnalysis::hasValidResult() const
{
  bool isMC = designOfExperiment_.getType() == DesignOfExperiment::Type::MC;
  bool hasValidResult = true;
  if (type_.computeRankSobol())
  {
    hasValidResult = hasValidResult && !result_.firstOrderSobolIndices_.isEmpty();
    if (isMC)
      hasValidResult = hasValidResult && !result_.firstOrderSobolIndicesInterval_.isEmpty();
  }
  if (type_.computeSRC())
  {
    hasValidResult = hasValidResult && result_.SRCIndices_.getSize();
    hasValidResult = hasValidResult && result_.signedSRCIndices_.getSize();
    if (isMC)    {
      hasValidResult = hasValidResult && !result_.SRCIndicesInterval_.isEmpty();
      hasValidResult = hasValidResult && !result_.signedSRCIndicesInterval_.isEmpty();
    }
  }
  if (type_.computeGlobalHSIC())
  {
    hasValidResult = hasValidResult && !result_.globalHSICIndices_.isEmpty();
    hasValidResult = hasValidResult && !result_.globalR2HSICIndices_.isEmpty();
    if (computeAsymptoticPValues_)
      hasValidResult = hasValidResult && !result_.globalPValuesAsymptotic_.isEmpty();
    if (computePermutationPValues_)
      hasValidResult = hasValidResult && !result_.globalPValuesPermutation_.isEmpty();
  }
  
  return hasValidResult;
}

const DataSensitivityAnalysisResult & DataSensitivityAnalysis::getResult() const
{
  return result_;
}

void DataSensitivityAnalysis::initialize()
{
  AnalysisImplementation::initialize();
  result_ = DataSensitivityAnalysisResult(designOfExperiment_);

  result_.analysisType_               = type_;
  result_.covarianceModels_           = covarianceModels_;
  result_.computeAsymptoticPValues_   = computeAsymptoticPValues_;
  result_.computePermutationPValues_  = computePermutationPValues_;
  result_.useUStatistic_              = useUStatistic_;
}

void DataSensitivityAnalysis::launch()
{
  if (result_.analysisType_.computeRankSobol())
    computeSobolIndices();
  if (result_.analysisType_.computeSRC())
    computeSRCIndices();
  if (result_.analysisType_.computeGlobalHSIC())
    computeGlobalHSICIndices();

  checkIndependance();
}

void DataSensitivityAnalysis::computeSobolIndices()
{
  const Sample outSample{designOfExperiment_.getOutputSample()};
  Sample marginalOutSample;
  const Sample inSample{designOfExperiment_.getInputSample()};
  const auto nbOutputs = outSample.getDimension();
  const UnsignedInteger n = inSample.getSize();
  RankSobolSensitivityAlgorithm algo;

  for(UnsignedInteger i = 0 ; i < nbOutputs; ++i)
  {
    marginalOutSample = outSample.getMarginal(i);
    algo.setDesign(inSample, marginalOutSample, n);
    result_.firstOrderSobolIndices_.add(algo.getFirstOrderIndices());
    result_.firstOrderSobolIndicesInterval_.add(algo.getFirstOrderIndicesInterval());
  }
}

/* adapted from SRCAnalysis::launch */
void DataSensitivityAnalysis::computeSRCIndices()
{
  const Sample inputSample{designOfExperiment_.getInputSample()};
  const Sample outputSample{designOfExperiment_.getOutputSample()};

  const UnsignedInteger nbOutputs = outputSample.getDimension();
  const UnsignedInteger nbInputs  = inputSample.getDimension();

  Sample indices{0, inputSample.getDimension()};
  Sample signedIndices{0, inputSample.getDimension()};
  Point r2(nbOutputs);

  for(UnsignedInteger i = 0 ; i < nbOutputs ; ++i)
  {
    const Point signedSRC{CorrelationAnalysis{inputSample, outputSample.getMarginal(i)}.computeSRC()};
    signedIndices.add(signedSRC);
    Point unscaledSRC;
    for(UnsignedInteger j = 0 ; j < nbInputs ; ++j)
    {
      const Scalar squaredSRC = signedSRC[j] * signedSRC[j];
      unscaledSRC.add(squaredSRC);
      r2[i] += squaredSRC;
    }
    indices.add(unscaledSRC);
  }

  // set results
  indices.setDescription(inputSample.getDescription());
  result_.SRCIndices_       = indices;
  result_.signedSRCIndices_ = signedIndices;
  result_.r2_               = r2;

  // Compute bootstrap confidence intervals
  const UnsignedInteger bootstrapSize = ResourceMap::GetAsUnsignedInteger("SobolIndicesAlgorithm-DefaultBootstrapSize");
  const Scalar alpha = ResourceMap::GetAsScalar("SobolIndicesAlgorithm-DefaultBootstrapConfidenceLevel");
  Indices inIndices(nbInputs);
  inIndices.fill();

  // - get bootstrap experiment
  Sample sample{inputSample};
  sample.stack(outputSample);
  BootstrapExperiment bootstrapExp{sample};
  // - compute signed SRC
  Collection signedSRCBootstrap(nbOutputs, Sample{0, nbInputs});
  Collection unscaledSRCBootstrap(nbOutputs, Sample{0, nbInputs});
  for (UnsignedInteger i = 0; i < bootstrapSize; ++i)
  {
    const Sample bootstrapSample{bootstrapExp.generate()};
    for (UnsignedInteger j = 0; j < nbOutputs; ++j)
    {
      const Point signedSRC{CorrelationAnalysis{bootstrapSample.getMarginal(inIndices), bootstrapSample.getMarginal(nbInputs + j)}.computeSRC()};
      signedSRCBootstrap[j].add(signedSRC);

      Point unscaledSRC;
      for (UnsignedInteger k = 0; k < nbInputs; ++k)
        unscaledSRC.add(signedSRC[k] * signedSRC[k]);
      unscaledSRCBootstrap[j].add(unscaledSRC);
    }
  }
  // - compute bounds
  PersistentCollection<Interval> signedSRCInterval{nbOutputs};
  PersistentCollection<Interval> unscaledSRCInterval{nbOutputs};
  for (UnsignedInteger i = 0; i < nbOutputs; ++i)
  {
    Point upperBound{signedSRCBootstrap[i].computeQuantilePerComponent(alpha)};
    Point lowerBound{signedSRCBootstrap[i].computeQuantilePerComponent(1 - alpha)};
    signedSRCInterval[i] = Interval(lowerBound, upperBound);

    Point upperBound2{unscaledSRCBootstrap[i].computeQuantilePerComponent(alpha)};
    Point lowerBound2{unscaledSRCBootstrap[i].computeQuantilePerComponent(1 - alpha)};
    unscaledSRCInterval[i] = Interval(lowerBound2, upperBound2);
  }
  result_.SRCIndicesInterval_ = unscaledSRCInterval;
  result_.signedSRCIndicesInterval_ = signedSRCInterval;
}

void DataSensitivityAnalysis::computeGlobalHSICIndices()
{

  const Sample sample{designOfExperiment_.getSample()};
  if (computeCovModelParameters_)
  {
    for(UnsignedInteger i = 0; i < covarianceModels_.getSize(); ++i)
    {
      covarianceModels_[i].setScale(sample.getMarginal(i).computeStandardDeviation());
    }
  }

  HSICStat stat;
  if (result_.useUStatistic_)
    stat = HSICUStat();
  else
    stat = HSICVStat();

  const Sample inSample{designOfExperiment_.getInputSample()};
  const Sample outSample{designOfExperiment_.getOutputSample()};
  const UnsignedInteger nbOutputs = outSample.getDimension();

  result_.globalHSICIndices_ = PersistentCollection<Point>(nbOutputs);
  result_.globalR2HSICIndices_ = PersistentCollection<Point>(nbOutputs);
  if (result_.computeAsymptoticPValues_)
    result_.globalPValuesAsymptotic_ = PersistentCollection<Point>(nbOutputs);
  if (result_.computePermutationPValues_)
    result_.globalPValuesPermutation_ = PersistentCollection<Point>(nbOutputs);
  
  Indices covarianceModelIndices(inSample.getDimension() + 1u);
  covarianceModelIndices.fill();

  for (UnsignedInteger i = 0 ; i < nbOutputs ; ++i)
  {
    
    covarianceModelIndices[inSample.getDimension()] = inSample.getDimension() + i; // the last covariance model is for the output variable

    auto estimator = HSICEstimatorGlobalSensitivity(
      result_.covarianceModels_.select(covarianceModelIndices), 
      inSample, 
      outSample.getMarginal(i), 
      stat
    );

    result_.globalHSICIndices_[i] = estimator.getHSICIndices();
    result_.globalR2HSICIndices_[i] = estimator.getR2HSICIndices();
    if (result_.computeAsymptoticPValues_)
      result_.globalPValuesAsymptotic_[i] = estimator.getPValuesAsymptotic();
    if (result_.computePermutationPValues_)
      result_.globalPValuesPermutation_.add(estimator.getPValuesPermutation());
  }
  
}

void DataSensitivityAnalysis::checkIndependance()
{
  const Sample inSample{designOfExperiment_.getInputSample()};
  const Description inputNames{inSample.getDescription()};
  const auto nbInputs = inSample.getDimension();

  OSS warningMessage;
  warningMessage << "Warning: ";
  for (UnsignedInteger i = 0; i < nbInputs; ++i)
  {
    for (UnsignedInteger j = i + 1; j < nbInputs; ++j)
    {
      const TestResult testResult = HypothesisTest::Spearman(inSample.getMarginal(i), inSample.getMarginal(j));
      const bool isIndependent = testResult.getBinaryQualityMeasure();
      if (!isIndependent)
      {
        warningMessage << "Variables " << inputNames[i] << " and " << inputNames[j] << " are not independent (Spearman test failed with p-value " << testResult.getPValue() << ").\n";
      }
      result_.isIndependent_ = result_.isIndependent_ && isIndependent;
    }
  }
  result_.independenceWarningMessage_ = warningMessage.str();
  if (result_.isIndependent_)
  {
    result_.independenceWarningMessage_ = "";
  }
}

String DataSensitivityAnalysis::__repr__() const
{
  return OSS() << "class=" << getClassName()
               << " name=" << getName()
               << " designOfExperiment=" << designOfExperiment_;
}

void DataSensitivityAnalysis::save(OT::Advocate & adv) const
{
  DesignOfExperimentAnalysis::save(adv);
  adv.saveAttribute("result_", result_);
  adv.saveAttribute("type_", static_cast<UnsignedInteger>(type_.getType()));
  adv.saveAttribute("covarianceModels_", covarianceModels_);
  adv.saveAttribute("computeAsymptoticPValues_", computeAsymptoticPValues_);
  adv.saveAttribute("computePermutationPValues_", computePermutationPValues_);
  adv.saveAttribute("useUStatistic_", useUStatistic_);
}

void DataSensitivityAnalysis::load(OT::Advocate & adv)
{
  DesignOfExperimentAnalysis::load(adv);
  adv.loadAttribute("result_", result_);
  if (adv.hasAttribute("type_"))
  {
    UnsignedInteger typeInt;
    adv.loadAttribute("type_", typeInt);
    type_ = DataSensitivityAnalysisResult::AnalysisType(std::byte{static_cast<unsigned char>(typeInt)});
    adv.loadAttribute("covarianceModels_", covarianceModels_);
    adv.loadAttribute("computeAsymptoticPValues_", computeAsymptoticPValues_);
    adv.loadAttribute("computePermutationPValues_", computePermutationPValues_);
    adv.loadAttribute("useUStatistic_", useUStatistic_);
  }
}

bool DataSensitivityAnalysis::CanBeLaunched(String &errorMessage, const DesignOfExperiment &doe)
{
  if (!DesignOfExperimentAnalysis::CanBeLaunched(errorMessage, doe))
    return false;
  // doe must have in/output data
  DataModel::Type type = doe.getType();
  if (!(type == DataModel::MC || type == DataModel::QMC || type == DataModel::OLHS || type == DataModel::RLHS))
    errorMessage = "The design of experiment must be of type MC, QMC or LHS.";
  if (!doe.getOutputSample().getSize() || !doe.getInputSample().getSize())
    errorMessage = "The sample must contain data for input and output variables.";
  if (doe.getInputSample().getDimension() < 2)
    errorMessage = "The sample must contain at least two input variables.";
  return errorMessage.empty();
}

} // namespace PERSALYS