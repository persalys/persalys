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
#include "persalys/BaseTools.hxx"

#include <openturns/RankSobolSensitivityAlgorithm.hxx>
#include <openturns/PersistentObjectFactory.hxx>
#include <openturns/HypothesisTest.hxx>
#include <openturns/CorrelationAnalysis.hxx>
#include <openturns/BootstrapExperiment.hxx>
#include <openturns/HSICEstimatorGlobalSensitivity.hxx>
#include <openturns/HSICEstimatorTargetSensitivity.hxx>
#include <openturns/HSICEstimatorConditionalSensitivity.hxx>
#include <openturns/HSICUStat.hxx>
#include <openturns/HSICVStat.hxx>
#include <openturns/SymbolicFunction.hxx>
#include <openturns/ParametricFunction.hxx>
#include <openturns/OSS.hxx>

using namespace OT;

namespace PERSALYS
{
CLASSNAMEINIT(DataSensitivityAnalysis)

const static Factory<DataSensitivityAnalysis> Factory_DataSensitivityAnalysis;

DataSensitivityAnalysis::DataSensitivityAnalysis(const String &name, 
  const DesignOfExperiment & design, 
  const unsigned char analysisType,
  const OT::Description & interestVariables)
  : DesignOfExperimentAnalysis(name, design)
  , type_(std::byte{analysisType})
{
  if (interestVariables.isEmpty())
    interestVariables_ = design.getOutputSample().getDescription();
  else
  {
    for(const String & var : interestVariables)
    {
      if (!design.getOutputSample().getDescription().contains(var))
        throw InvalidArgumentException(HERE) << "Interest variable '" << var << "' is not in the design of experiment output sample description";
    }
    interestVariables_ = interestVariables;
  }
}

DataSensitivityAnalysis * DataSensitivityAnalysis::clone() const
{
  return new DataSensitivityAnalysis(*this);
}

void DataSensitivityAnalysis::setType(unsigned char analysisType)
{
  type_ = std::byte{analysisType};
}

void DataSensitivityAnalysis::setCovarianceModels(const OT::Collection<OT::CovarianceModel> &covarianceModels, HSICType hsicType)
{
  switch (hsicType)
  {
    case DataSensitivityAnalysisResult::Global:
      globalCovarianceModels_ = covarianceModels;
      break;
    case DataSensitivityAnalysisResult::Target:
      targetCovarianceModels_ = covarianceModels;
      break;
    case DataSensitivityAnalysisResult::Conditional:
      conditionalCovarianceModels_ = covarianceModels;
      break;
    default:
      throw InvalidArgumentException(HERE) << "Invalid HSIC Type";
  }
}

void DataSensitivityAnalysis::computeCovModelParameters(bool computeCovModelParameters)
{
  computeCovModelParameters_ = computeCovModelParameters;
}

void DataSensitivityAnalysis::setFilterAlphas(const OT::Point & filterAlphas)
{
  filterAlphas_ = filterAlphas;

  filterFunctions_.clear();
  Sample outSample = designOfExperiment_.getOutputSample().getMarginal(interestVariables_);
  const Point stdDevs = outSample.computeStandardDeviation();
  for (UnsignedInteger i = 0; i < filterAlphas_.getSize(); ++i)
  {
    const Scalar s = filterAlphas_[i] * stdDevs[i];
    const auto f = SymbolicFunction(Description({"x", "s"}), {"exp(-x/s)"});
    const auto phi = ParametricFunction(f, {1}, {s});
    filterFunctions_.add(phi);
  }

  userDefinedFilterFunctions_ = false;
}

void DataSensitivityAnalysis::setWeightAlphas(const OT::Point & weightAlphas)
{
  weightAlphas_ = weightAlphas;

  weightFunctions_.clear();
  Sample outSample = designOfExperiment_.getOutputSample().getMarginal(interestVariables_);
  const Point stdDevs = outSample.computeStandardDeviation();
  for (UnsignedInteger i = 0; i < weightAlphas_.getSize(); ++i)
  {
    const Scalar s = weightAlphas_[i] * stdDevs[i];
    const auto f = SymbolicFunction(Description({"x", "s"}), {"exp(-x/s)"});
    const auto phi = ParametricFunction(f, {1}, {s});
    weightFunctions_.add(phi);
  }

  userDefinedWeightFunctions_ = false;
}

void DataSensitivityAnalysis::setFilterFunctions(const OT::Collection<OT::Function> &filterFunctions)
{ 
  filterAlphas_.clear();
  filterFunctions_ = filterFunctions;
  userDefinedFilterFunctions_ = true;
}

void DataSensitivityAnalysis::setWeightFunctions(const OT::Collection<OT::Function> &weightFunctions)
{
  weightAlphas_.clear();
  weightFunctions_ = weightFunctions;
  userDefinedWeightFunctions_ = true;
}

void DataSensitivityAnalysis::setHSICParameters(bool computePermutationPValues, bool computeAsymptoticPValues, bool useUStatistic, HSICType hsicType)
{
  switch (hsicType)
  {
    case DataSensitivityAnalysisResult::Global:
      globalHSICParameters_ = HSICParameters(computePermutationPValues, computeAsymptoticPValues, useUStatistic);
      break;
    case DataSensitivityAnalysisResult::Target:
      targetHSICParameters_ = HSICParameters(computePermutationPValues, computeAsymptoticPValues, useUStatistic);
      break;
    case DataSensitivityAnalysisResult::Conditional:
      if (computeAsymptoticPValues || useUStatistic)
        throw InvalidArgumentException(HERE) << "Asymptotic p-values and U-statistic are not supported for conditional HSIC indices";
      conditionalHSICParameters_ = HSICParameters(computePermutationPValues, false, false);
      break;
    default:
      throw InvalidArgumentException(HERE) << "Invalid HSIC Type";
  }
  defaultHSICParametersChanged_ = true;
}


bool DataSensitivityAnalysis::canBeLaunched(String &errorMessage) const
{
  return DataSensitivityAnalysis::CanBeLaunched(errorMessage, designOfExperiment_);
}

Parameters DataSensitivityAnalysis::getParameters() const
{
  Parameters parameters;
  
  OT::String analysisTypesStr;
  if (type_.computeRankSobol())
    analysisTypesStr += "Rank Sobol indices, ";
  if (type_.computeSRC())
    analysisTypesStr += "SRC indices, ";
  if (type_.computeGlobalHSIC())
    analysisTypesStr += "Global HSIC indices, ";
  if (type_.computeTargetHSIC())
    analysisTypesStr += "Target HSIC indices, ";
  if (type_.computeConditionalHSIC())
    analysisTypesStr += "Conditional HSIC indices, ";
  analysisTypesStr = analysisTypesStr.substr(0, analysisTypesStr.size() - 2); // remove last ", "
  parameters.add("Analysis types", analysisTypesStr);
  if (type_.computeGlobalHSIC())
  {
    parameters.add("Compute asymptotic p-values for global HSIC", globalHSICParameters_.computeAsymptoticPValues() ? "Yes" : "No");
    parameters.add("Compute permutation p-values for global HSIC", globalHSICParameters_.computePermutationPValues() ? "Yes" : "No");
    parameters.add("Statistic used for global HSIC: ", globalHSICParameters_.useUStatistic() ? "U-statistic" : "V-statistic");
  }
  if (type_.computeTargetHSIC())
  {
    parameters.add("Compute asymptotic p-values for target HSIC", targetHSICParameters_.computeAsymptoticPValues() ? "Yes" : "No");
    parameters.add("Compute permutation p-values for target HSIC", targetHSICParameters_.computePermutationPValues() ? "Yes" : "No");
    parameters.add("Statistic used for target HSIC: ", targetHSICParameters_.useUStatistic() ? "U-statistic" : "V-statistic");
  }
  if (type_.computeConditionalHSIC())
  {
    parameters.add("Compute permutation p-values for conditional HSIC", conditionalHSICParameters_.computePermutationPValues() ? "Yes" : "No");
  }

  return parameters;
}

bool DataSensitivityAnalysis::computeRankSobol() const
{
  return type_.computeRankSobol();
}

bool DataSensitivityAnalysis::computeSRC() const
{
  return type_.computeSRC();
}

bool DataSensitivityAnalysis::computeHSIC(HSICType hsicType) const
{
  switch (hsicType)
  {
    case DataSensitivityAnalysisResult::Global:
      return type_.computeGlobalHSIC();
    case DataSensitivityAnalysisResult::Target:
      return type_.computeTargetHSIC();
    case DataSensitivityAnalysisResult::Conditional:
      return type_.computeConditionalHSIC();
    default:
      throw InvalidArgumentException(HERE) << "Invalid HSIC Type";
  }
}

bool DataSensitivityAnalysis::computeAsymptoticPValues(HSICType hsicType) const
{
  switch (hsicType)
  {
    case DataSensitivityAnalysisResult::Global:
      return globalHSICParameters_.computeAsymptoticPValues();
    case DataSensitivityAnalysisResult::Target:
      return targetHSICParameters_.computeAsymptoticPValues();
    case DataSensitivityAnalysisResult::Conditional:
      return conditionalHSICParameters_.computeAsymptoticPValues();
    default:
      throw InvalidArgumentException(HERE) << "Invalid HSIC Type";
  }
}

bool DataSensitivityAnalysis::computePermutationPValues(HSICType hsicType) const
{
  switch (hsicType)
  {
    case DataSensitivityAnalysisResult::Global:
      return globalHSICParameters_.computePermutationPValues();
    case DataSensitivityAnalysisResult::Target:
      return targetHSICParameters_.computePermutationPValues();
    case DataSensitivityAnalysisResult::Conditional:
      return conditionalHSICParameters_.computePermutationPValues();
    default:
      throw InvalidArgumentException(HERE) << "Invalid HSIC Type";
  }
}

bool DataSensitivityAnalysis::useUStatistic(HSICType hsicType) const
{
  switch (hsicType)
  {
    case DataSensitivityAnalysisResult::Global:
      return globalHSICParameters_.useUStatistic();
    case DataSensitivityAnalysisResult::Target:
      return targetHSICParameters_.useUStatistic();
    case DataSensitivityAnalysisResult::Conditional:
      return conditionalHSICParameters_.useUStatistic();
    default:
      throw InvalidArgumentException(HERE) << "Invalid HSIC Type";
  }
}

Collection<CovarianceModel> DataSensitivityAnalysis::getCovarianceModels(HSICType hsicType) const
{
  switch (hsicType)
  {
    case DataSensitivityAnalysisResult::Global:
      return globalCovarianceModels_;
    case DataSensitivityAnalysisResult::Target:
      return targetCovarianceModels_;
    case DataSensitivityAnalysisResult::Conditional:
      return conditionalCovarianceModels_;
    default:
      throw InvalidArgumentException(HERE) << "Invalid HSIC Type";
  }
}

Point DataSensitivityAnalysis::getFilterAlphas() const
{
  return filterAlphas_;
}

Point DataSensitivityAnalysis::getWeightAlphas() const
{
  return weightAlphas_;
}

Collection<Function> DataSensitivityAnalysis::getFilterFunctions() const
{  
  return filterFunctions_;
}

Collection<Function> DataSensitivityAnalysis::getWeightFunctions() const
{
  return weightFunctions_;
}

bool DataSensitivityAnalysis::defaultHSICParametersChanged() const
{
  return defaultHSICParametersChanged_;
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
    if (globalHSICParameters_.computeAsymptoticPValues())
      hasValidResult = hasValidResult && !result_.globalPValuesAsymptotic_.isEmpty();
    if (globalHSICParameters_.computePermutationPValues())
      hasValidResult = hasValidResult && !result_.globalPValuesPermutation_.isEmpty();
  }
  if (type_.computeTargetHSIC())
  {
    hasValidResult = hasValidResult && !result_.targetHSICIndices_.isEmpty();
    hasValidResult = hasValidResult && !result_.targetR2HSICIndices_.isEmpty();
    if (targetHSICParameters_.computeAsymptoticPValues())
      hasValidResult = hasValidResult && !result_.targetPValuesAsymptotic_.isEmpty();
    if (targetHSICParameters_.computePermutationPValues())
      hasValidResult = hasValidResult && !result_.targetPValuesPermutation_.isEmpty();
  }
  if (type_.computeConditionalHSIC())
  {
    hasValidResult = hasValidResult && !result_.conditionalHSICIndices_.isEmpty();
    hasValidResult = hasValidResult && !result_.conditionalR2HSICIndices_.isEmpty();
    if (conditionalHSICParameters_.computePermutationPValues())
      hasValidResult = hasValidResult && !result_.conditionalPValuesPermutation_.isEmpty();
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

  result_.analysisType_                     = type_;
  
  result_.globalCovarianceModels_           = globalCovarianceModels_;
  result_.computeGlobalAsymptoticPValues_   = globalHSICParameters_.computeAsymptoticPValues();
  result_.computeGlobalPermutationPValues_  = globalHSICParameters_.computePermutationPValues();
  result_.useUStatisticGlobal_              = globalHSICParameters_.useUStatistic();

  result_.targetCovarianceModels_           = targetCovarianceModels_;
  result_.computeTargetAsymptoticPValues_   = targetHSICParameters_.computeAsymptoticPValues();
  result_.computeTargetPermutationPValues_  = targetHSICParameters_.computePermutationPValues();
  result_.useUStatisticTarget_              = targetHSICParameters_.useUStatistic();

  result_.conditionalCovarianceModels_           = conditionalCovarianceModels_;
  result_.computeConditionalPermutationPValues_  = conditionalHSICParameters_.computePermutationPValues();

  result_.interestVariables_ = interestVariables_;
}

void DataSensitivityAnalysis::launch()
{
  if (result_.analysisType_.computeRankSobol())
    computeSobolIndices();
  if (result_.analysisType_.computeSRC())
    computeSRCIndices();
  if (result_.analysisType_.computeGlobalHSIC())
    computeGlobalHSICIndices();
  if (result_.analysisType_.computeTargetHSIC())
    computeTargetHSICIndices();
  if (result_.analysisType_.computeConditionalHSIC())
    computeConditionalHSICIndices();

  checkIndependance();
}

void DataSensitivityAnalysis::computeSobolIndices()
{
  const Sample outSample{designOfExperiment_.getOutputSample().getMarginal(interestVariables_)};
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
  const Sample outputSample{designOfExperiment_.getOutputSample().getMarginal(interestVariables_)};

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
  Sample sample = designOfExperiment_.getInputSample();
  sample.stack(designOfExperiment_.getOutputSample().getMarginal(interestVariables_));

  if (globalCovarianceModels_.getSize() != sample.getDimension())
    throw InvalidArgumentException(HERE) << "The number of covariance models for global HSIC indices (" << globalCovarianceModels_.getSize() << ") must be equal to the number of variables (" << sample.getDimension() << ").";

  if (computeCovModelParameters_)
  {
    const Point stdDevs = sample.computeStandardDeviation();
    for(UnsignedInteger i = 0; i < globalCovarianceModels_.getSize(); ++i)
    {
      if (stdDevs[i] == 0.)
        throw InvalidArgumentException(HERE) << "The standard deviation of variable " << sample.getDescription()[i] << " is zero, the covariance model parameters cannot be estimated.";
      globalCovarianceModels_[i].setScale(Point{stdDevs[i]});
    }
  }

  HSICStat stat;
  if (globalHSICParameters_.useUStatistic())
    stat = HSICUStat();
  else
    stat = HSICVStat();

  const Sample inSample{designOfExperiment_.getInputSample()};
  const Sample outSample{designOfExperiment_.getOutputSample().getMarginal(interestVariables_)};
  const UnsignedInteger nbOutputs = outSample.getDimension();
  const UnsignedInteger nbInputs  = inSample.getDimension();

  result_.globalHSICIndices_ = PersistentCollection<Point>(nbOutputs);
  result_.globalR2HSICIndices_ = PersistentCollection<Point>(nbOutputs);
  if (globalHSICParameters_.computeAsymptoticPValues())
    result_.globalPValuesAsymptotic_ = PersistentCollection<Point>(nbOutputs);
  if (globalHSICParameters_.computePermutationPValues())
    result_.globalPValuesPermutation_ = PersistentCollection<Point>(nbOutputs);
  
  Indices covarianceModelIndices(nbInputs + 1u);
  covarianceModelIndices.fill();

  for (UnsignedInteger i = 0 ; i < nbOutputs ; ++i)
  {
    
    covarianceModelIndices[nbInputs] = nbInputs + i; // the last covariance model is for the output variable

    auto estimator = HSICEstimatorGlobalSensitivity(
      globalCovarianceModels_.select(covarianceModelIndices), 
      inSample, 
      outSample.getMarginal(i), 
      stat
    );

    result_.globalHSICIndices_[i] = estimator.getHSICIndices();
    result_.globalR2HSICIndices_[i] = estimator.getR2HSICIndices();
    if (globalHSICParameters_.computeAsymptoticPValues())
      result_.globalPValuesAsymptotic_[i] = estimator.getPValuesAsymptotic();
    if (globalHSICParameters_.computePermutationPValues())
      result_.globalPValuesPermutation_[i] = estimator.getPValuesPermutation();
  }
}

void DataSensitivityAnalysis::computeTargetHSICIndices()
{
  const Sample inSample{designOfExperiment_.getInputSample()};
  const Sample outSample{designOfExperiment_.getOutputSample().getMarginal(interestVariables_)};

  const UnsignedInteger nbInputs  = inSample.getDimension();
  const UnsignedInteger nbOutputs = outSample.getDimension();

  if (filterFunctions_.getSize() != nbOutputs)
    throw InvalidArgumentException(HERE) << "The number of filter functions (" << filterFunctions_.getSize() << ") must be equal to the number of output variables (" << nbOutputs << ").";
  
  if (targetCovarianceModels_.getSize() != nbInputs + nbOutputs)
    throw InvalidArgumentException(HERE) << "The number of covariance models for target HSIC indices (" << targetCovarianceModels_.getSize() << ") must be equal to the number of variables (" << nbInputs + nbOutputs << ").";
  
  if (computeCovModelParameters_)
  {
    const Point stdDevs = inSample.computeStandardDeviation();
    for(UnsignedInteger i = 0; i < nbInputs ; ++i)
    {
      if (stdDevs[i] == 0.)
        throw InvalidArgumentException(HERE) << "The standard deviation of variable " << designOfExperiment_.getSample().getDescription()[i] << " is zero, the covariance model parameters cannot be estimated.";
      targetCovarianceModels_[i].setScale(Point{stdDevs[i]});
    }
    for (UnsignedInteger i = 0 ; i < nbOutputs ; ++i)
    {
      Sample outputMarginal = outSample.getMarginal(i);
      Sample filteredOutputMarginal = filterFunctions_[i](outputMarginal);
      Point stdDevsFiltered = filteredOutputMarginal.computeStandardDeviation();
      if (stdDevsFiltered[0] == 0.)
        throw InvalidArgumentException(HERE) << "The standard deviation of the filtered output variable "<< outSample.getDescription()[i] << " is zero, the covariance model parameters cannot be estimated.";
      targetCovarianceModels_[nbInputs + i].setScale(Point{stdDevsFiltered[0]});
    }
  }

  HSICStat stat;
  if (targetHSICParameters_.useUStatistic())
    stat = HSICUStat();
  else
    stat = HSICVStat();

  result_.targetHSICIndices_ = PersistentCollection<Point>(nbOutputs);
  result_.targetR2HSICIndices_ = PersistentCollection<Point>(nbOutputs);
  if (targetHSICParameters_.computeAsymptoticPValues())
    result_.targetPValuesAsymptotic_ = PersistentCollection<Point>(nbOutputs);
  if (targetHSICParameters_.computePermutationPValues())
    result_.targetPValuesPermutation_ = PersistentCollection<Point>(nbOutputs);
  
  Indices covarianceModelIndices(nbInputs + 1u);
  covarianceModelIndices.fill();

  for (UnsignedInteger i = 0 ; i < nbOutputs ; ++i)
  {
    covarianceModelIndices[nbInputs] = nbInputs + i; // the last covariance model is for the output variable

    auto estimator = HSICEstimatorTargetSensitivity(
      targetCovarianceModels_.select(covarianceModelIndices), 
      inSample, 
      outSample.getMarginal(i), 
      stat,
      filterFunctions_[i]
    );

    result_.targetHSICIndices_[i] = estimator.getHSICIndices();
    result_.targetR2HSICIndices_[i] = estimator.getR2HSICIndices();
    if (targetHSICParameters_.computeAsymptoticPValues())
      result_.targetPValuesAsymptotic_[i] = estimator.getPValuesAsymptotic();
    if (targetHSICParameters_.computePermutationPValues())
      result_.targetPValuesPermutation_[i] = estimator.getPValuesPermutation();
  }
}

void DataSensitivityAnalysis::computeConditionalHSICIndices()
{
  const Sample inSample{designOfExperiment_.getInputSample()};
  const Sample outSample{designOfExperiment_.getOutputSample().getMarginal(interestVariables_)};

  const UnsignedInteger nbInputs  = inSample.getDimension();
  const UnsignedInteger nbOutputs = outSample.getDimension();

  if (weightFunctions_.getSize() != nbOutputs)
    throw InvalidArgumentException(HERE) << "The number of weight functions must be equal to the number of output variables.";
  
  if (conditionalCovarianceModels_.getSize() != nbInputs + nbOutputs)
    throw InvalidArgumentException(HERE) << "The number of covariance models for conditional HSIC indices (" << conditionalCovarianceModels_.getSize() << ") must be equal to the number of variables (" << nbInputs + nbOutputs << ").";
  
  if (computeCovModelParameters_)
  {
    const Point stDevs = designOfExperiment_.getSample().computeStandardDeviation();
    for(UnsignedInteger i = 0; i < nbInputs + nbOutputs; ++i)
    {
      if (stDevs[i] == 0.)
        throw InvalidArgumentException(HERE) << "The standard deviation of variable " << designOfExperiment_.getSample().getDescription()[i] << " is zero, the covariance model parameters cannot be estimated.";
      conditionalCovarianceModels_[i].setScale(Point{stDevs[i]});
    }
  }

  result_.conditionalHSICIndices_ = PersistentCollection<Point>(nbOutputs);
  result_.conditionalR2HSICIndices_ = PersistentCollection<Point>(nbOutputs);
  if (conditionalHSICParameters_.computePermutationPValues())
    result_.conditionalPValuesPermutation_ = PersistentCollection<Point>(nbOutputs);
  
  Indices covarianceModelIndices(nbInputs + 1u);
  covarianceModelIndices.fill();

  for (UnsignedInteger i = 0 ; i < nbOutputs ; ++i)
  {
    covarianceModelIndices[nbInputs] = nbInputs + i; // the last covariance model is for the output variable

    auto estimator = HSICEstimatorConditionalSensitivity(
      conditionalCovarianceModels_.select(covarianceModelIndices), 
      inSample, 
      outSample.getMarginal(i), 
      weightFunctions_[i]
    );

    result_.conditionalHSICIndices_[i] = estimator.getHSICIndices();
    result_.conditionalR2HSICIndices_[i] = estimator.getR2HSICIndices();
    if (conditionalHSICParameters_.computePermutationPValues())
      result_.conditionalPValuesPermutation_[i] = estimator.getPValuesPermutation();
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

String DataSensitivityAnalysis::getPythonScript() const
{
  if (userDefinedFilterFunctions_ || userDefinedWeightFunctions_)
    throw InvalidArgumentException(HERE) << "Python script cannot be generated when user-defined filter or weight functions are used.";
  OSS oss;
  oss << getName() << " = persalys.DataSensitivityAnalysis('" << getName() << "', " << designOfExperiment_.getName() << ", " << type_ << ", " << Parameters::GetOTDescriptionStr(interestVariables_)  << ")\n";
  oss << getName() << ".computeCovModelParameters(" << Parameters::GetOTBoolStr(computeCovModelParameters_) << ")\n";
  oss << "globalCovModels = " << Parameters::GetOTCovModelCollectionStr(globalCovarianceModels_) << "\n";
  oss << "targetCovModels = " << Parameters::GetOTCovModelCollectionStr(targetCovarianceModels_) << "\n";
  oss << "conditionalCovModels = " << Parameters::GetOTCovModelCollectionStr(conditionalCovarianceModels_) << "\n";
  for (UnsignedInteger i = 0 ; i < globalCovarianceModels_.getSize(); ++i)
  {
    if (type_.computeGlobalHSIC())
      oss << "globalCovModels[" << i << "].setFullParameter(" << Parameters::GetOTPointStr(globalCovarianceModels_[i].getFullParameter()) << ")\n";
    if (type_.computeTargetHSIC())
      oss << "targetCovModels[" << i << "].setFullParameter(" << Parameters::GetOTPointStr(targetCovarianceModels_[i].getFullParameter()) << ")\n";
    if (type_.computeConditionalHSIC())
      oss << "conditionalCovModels[" << i << "].setFullParameter(" << Parameters::GetOTPointStr(conditionalCovarianceModels_[i].getFullParameter()) << ")\n";
  }
  oss << getName() << ".setCovarianceModels(ot.CovarianceModelCollection(globalCovModels), persalys.DataSensitivityAnalysisResult.Global)\n";
  oss << getName() << ".setCovarianceModels(ot.CovarianceModelCollection(targetCovModels), persalys.DataSensitivityAnalysisResult.Target)\n";
  oss << getName() << ".setCovarianceModels(ot.CovarianceModelCollection(conditionalCovModels), persalys.DataSensitivityAnalysisResult.Conditional)\n";

  oss << getName() << ".setFilterAlphas(" << Parameters::GetOTPointStr(filterAlphas_) << ")\n";
  oss << getName() << ".setWeightAlphas(" << Parameters::GetOTPointStr(weightAlphas_) << ")\n";

  oss << getName() << ".setHSICParameters(" << Parameters::GetOTBoolStr(globalHSICParameters_.computePermutationPValues()) << ", " << Parameters::GetOTBoolStr(globalHSICParameters_.computeAsymptoticPValues()) << ", " << Parameters::GetOTBoolStr(globalHSICParameters_.useUStatistic()) << ", persalys.DataSensitivityAnalysisResult.Global)\n";
  oss << getName() << ".setHSICParameters(" << Parameters::GetOTBoolStr(targetHSICParameters_.computePermutationPValues()) << ", " << Parameters::GetOTBoolStr(targetHSICParameters_.computeAsymptoticPValues()) << ", " << Parameters::GetOTBoolStr(targetHSICParameters_.useUStatistic()) << ", persalys.DataSensitivityAnalysisResult.Target)\n";
  oss << getName() << ".setHSICParameters(" << Parameters::GetOTBoolStr(conditionalHSICParameters_.computePermutationPValues()) << ", " << Parameters::GetOTBoolStr(conditionalHSICParameters_.computeAsymptoticPValues()) << ", " << Parameters::GetOTBoolStr(conditionalHSICParameters_.useUStatistic()) << ", persalys.DataSensitivityAnalysisResult.Conditional)\n";

  return oss;
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
  adv.saveAttribute("globalCovarianceModels_", globalCovarianceModels_);
  adv.saveAttribute("targetCovarianceModels_", targetCovarianceModels_);
  adv.saveAttribute("conditionalCovarianceModels_", conditionalCovarianceModels_);
  adv.saveAttribute("globalComputePermutationPValues_", globalHSICParameters_.computePermutationPValues_);
  adv.saveAttribute("globalComputeAsymptoticPValues_", globalHSICParameters_.computeAsymptoticPValues_);
  adv.saveAttribute("globalUseUStatistic_", globalHSICParameters_.useUStatistic_);
  adv.saveAttribute("targetComputePermutationPValues_", targetHSICParameters_.computePermutationPValues_);
  adv.saveAttribute("targetComputeAsymptoticPValues_", targetHSICParameters_.computeAsymptoticPValues_);
  adv.saveAttribute("targetUseUStatistic_", targetHSICParameters_.useUStatistic_);
  adv.saveAttribute("conditionalComputePermutationPValues_", conditionalHSICParameters_.computePermutationPValues_);
  adv.saveAttribute("conditionalComputeAsymptoticPValues_", conditionalHSICParameters_.computeAsymptoticPValues_);
  adv.saveAttribute("conditionalUseUStatistic_", conditionalHSICParameters_.useUStatistic_);
  adv.saveAttribute("filterFunctions_", filterFunctions_);
  adv.saveAttribute("weightFunctions_", weightFunctions_);
  adv.saveAttribute("computeCovModelParameters_", computeCovModelParameters_);
  adv.saveAttribute("defaultHSICParametersChanged_", defaultHSICParametersChanged_);
  adv.saveAttribute("interestVariables_", interestVariables_);
  adv.saveAttribute("filterAlphas_", filterAlphas_);
  adv.saveAttribute("weightAlphas_", weightAlphas_);
  adv.saveAttribute("userDefinedFilterFunctions_", userDefinedFilterFunctions_);
  adv.saveAttribute("userDefinedWeightFunctions_", userDefinedWeightFunctions_);
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
    adv.loadAttribute("globalCovarianceModels_", globalCovarianceModels_);
    adv.loadAttribute("targetCovarianceModels_", targetCovarianceModels_);
    adv.loadAttribute("conditionalCovarianceModels_", conditionalCovarianceModels_);
    adv.loadAttribute("globalComputePermutationPValues_", globalHSICParameters_.computePermutationPValues_);
    adv.loadAttribute("globalComputeAsymptoticPValues_", globalHSICParameters_.computeAsymptoticPValues_);
    adv.loadAttribute("globalUseUStatistic_", globalHSICParameters_.useUStatistic_);
    adv.loadAttribute("targetComputePermutationPValues_", targetHSICParameters_.computePermutationPValues_);
    adv.loadAttribute("targetComputeAsymptoticPValues_", targetHSICParameters_.computeAsymptoticPValues_);
    adv.loadAttribute("targetUseUStatistic_", targetHSICParameters_.useUStatistic_);
    adv.loadAttribute("conditionalComputePermutationPValues_", conditionalHSICParameters_.computePermutationPValues_);
    adv.loadAttribute("conditionalComputeAsymptoticPValues_", conditionalHSICParameters_.computeAsymptoticPValues_);
    adv.loadAttribute("conditionalUseUStatistic_", conditionalHSICParameters_.useUStatistic_);
    adv.loadAttribute("filterFunctions_", filterFunctions_);
    adv.loadAttribute("weightFunctions_", weightFunctions_);
    adv.loadAttribute("computeCovModelParameters_", computeCovModelParameters_);
    adv.loadAttribute("defaultHSICParametersChanged_", defaultHSICParametersChanged_);
    adv.loadAttribute("interestVariables_", interestVariables_);
    adv.loadAttribute("filterAlphas_", filterAlphas_);
    adv.loadAttribute("weightAlphas_", weightAlphas_);
    adv.loadAttribute("userDefinedFilterFunctions_", userDefinedFilterFunctions_);
    adv.loadAttribute("userDefinedWeightFunctions_", userDefinedWeightFunctions_);
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