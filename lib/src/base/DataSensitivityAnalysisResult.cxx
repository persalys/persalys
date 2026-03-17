//                                               -*- C++ -*-
/**
 *  @brief Results of a data sensitivity analysis
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

#include "persalys/DataSensitivityAnalysisResult.hxx"
#include <openturns/PersistentObjectFactory.hxx>

using namespace OT;

namespace PERSALYS
{

CLASSNAMEINIT(DataSensitivityAnalysisResult)
const static Factory<DataSensitivityAnalysisResult> Factory_DataSensitivityAnalysisResult;

DataSensitivityAnalysisResult::DataSensitivityAnalysisResult()
  : EvaluationResult()
{
}

DataSensitivityAnalysisResult::DataSensitivityAnalysisResult(const DesignOfExperiment& design)
  : EvaluationResult(design)
{
}

DataSensitivityAnalysisResult* DataSensitivityAnalysisResult::clone() const
{
  return new DataSensitivityAnalysisResult(*this);
}

const Collection<Point>& DataSensitivityAnalysisResult::getFirstOrderSobolIndices() const
{
  return firstOrderSobolIndices_;
}

const Collection<Interval>& DataSensitivityAnalysisResult::getFirstOrderSobolIndicesInterval() const
{
  return firstOrderSobolIndicesInterval_;
}

const Sample& DataSensitivityAnalysisResult::getSRCIndices() const
{
  return SRCIndices_;
}

const Sample& DataSensitivityAnalysisResult::getSignedSRCIndices() const
{
  return signedSRCIndices_;
}

const Collection<Interval>& DataSensitivityAnalysisResult::getSRCIndicesInterval() const
{
  return SRCIndicesInterval_;
}

const Collection<Interval>& DataSensitivityAnalysisResult::getSignedSRCIndicesInterval() const
{
  return signedSRCIndicesInterval_;
}

const Point& DataSensitivityAnalysisResult::getR2() const
{
  return r2_;
}

const Collection<Point>& DataSensitivityAnalysisResult::getHSICIndices(HSICType hsicType) const
{
  switch (hsicType)
  {
    case Global:      
      return globalHSICIndices_;
    case Target:      
      return targetHSICIndices_;
    case Conditional:          
      return conditionalHSICIndices_;
    default:
      throw InvalidArgumentException(HERE) << "Invalid HSIC type";
  }
}

const Collection<Point>& DataSensitivityAnalysisResult::getR2HSICIndices(HSICType hsicType) const
{
  switch (hsicType)
  {
    case Global:      
      return globalR2HSICIndices_;
    case Target:      
      return targetR2HSICIndices_;
    case Conditional:          
      return conditionalR2HSICIndices_;
    default:
      throw InvalidArgumentException(HERE) << "Invalid HSIC type";
  }
}

const Collection<Point>& DataSensitivityAnalysisResult::getPValuesAsymptotic(HSICType hsicType) const
{
  switch (hsicType)
  {
    case Global:      
      return globalPValuesAsymptotic_;
    case Target:      
      return targetPValuesAsymptotic_;
    case Conditional:          
      throw InvalidArgumentException(HERE) << "Asymptotic p-values are not available for conditional HSIC indices";
    default:
      throw InvalidArgumentException(HERE) << "Invalid HSIC type";
  }
}

const Collection<Point>& DataSensitivityAnalysisResult::getPValuesPermutation(HSICType hsicType) const
{
  switch (hsicType)
  {
    case Global:      
      return globalPValuesPermutation_;
    case Target:      
      return targetPValuesPermutation_;
    case Conditional:          
      return conditionalPValuesPermutation_;
    default:
      throw InvalidArgumentException(HERE) << "Invalid HSIC type";
  }
}

bool DataSensitivityAnalysisResult::computeHSICPValuesAsymptotic(HSICType hsicType) const
{
  switch (hsicType)
  {
    case Global:      
      return computeGlobalAsymptoticPValues_;
    case Target:      
      return computeTargetAsymptoticPValues_;
    case Conditional:          
      return false;
    default:
      throw InvalidArgumentException(HERE) << "Invalid HSIC Type";
  }
}

bool DataSensitivityAnalysisResult::computeHSICPValuesPermutation(HSICType hsicType) const
{
  switch (hsicType)
  {
    case Global:      
      return computeGlobalPermutationPValues_;
    case Target:      
      return computeTargetPermutationPValues_;
    case Conditional:         
      return computeConditionalPermutationPValues_;
    default:
      throw InvalidArgumentException(HERE) << "Invalid HSIC Type";
  }
}

const Description& DataSensitivityAnalysisResult::getInterestVariables() const
{
  return interestVariables_;
}

#ifndef SWIG
const DataSensitivityAnalysisResult::AnalysisType& DataSensitivityAnalysisResult::getAnalysisType() const
{
  return analysisType_;
}
#endif

bool DataSensitivityAnalysisResult::isIndependent() const
{
  return isIndependent_;
}

const OT::String& DataSensitivityAnalysisResult::getIndependenceWarningMessage() const
{
  return independenceWarningMessage_;
}

/* String converter */
String DataSensitivityAnalysisResult::__repr__() const
{
  return OSS() << "class=" << getClassName()
               << " firstOrderSobolIndices=" << firstOrderSobolIndices_
               << " firstOrderSobolIndicesInterval=" << firstOrderSobolIndicesInterval_
               << " isIndependent=" << isIndependent_
               << " independenceWarningMessage=" << independenceWarningMessage_;
}

/* Method save() stores the object through the StorageManager */
void DataSensitivityAnalysisResult::save(OT::Advocate & adv) const
{
  EvaluationResult::save(adv);
  adv.saveAttribute("analysisType_", static_cast<UnsignedInteger>(analysisType_.getType()));
  adv.saveAttribute("firstOrderSobolIndices_", firstOrderSobolIndices_);
  adv.saveAttribute("firstOrderSobolIndicesInterval_", firstOrderSobolIndicesInterval_);
  adv.saveAttribute("SRCIndices_", SRCIndices_);
  adv.saveAttribute("signedSRCIndices_", signedSRCIndices_);
  adv.saveAttribute("r2_", r2_);
  adv.saveAttribute("SRCIndicesInterval_", SRCIndicesInterval_);
  adv.saveAttribute("signedSRCIndicesInterval_", signedSRCIndicesInterval_);
  adv.saveAttribute("globalHSICIndices_", globalHSICIndices_);
  adv.saveAttribute("globalR2HSICIndices_", globalR2HSICIndices_);
  adv.saveAttribute("globalPValuesAsymptotic_", globalPValuesAsymptotic_);
  adv.saveAttribute("globalPValuesPermutation_", globalPValuesPermutation_);
  adv.saveAttribute("globalCovarianceModels_", globalCovarianceModels_);
  adv.saveAttribute("computeGlobalAsymptoticPValues_", computeGlobalAsymptoticPValues_);
  adv.saveAttribute("computeGlobalPermutationPValues_", computeGlobalPermutationPValues_);
  adv.saveAttribute("useUStatisticGlobal_", useUStatisticGlobal_);
  adv.saveAttribute("targetHSICIndices_", targetHSICIndices_);
  adv.saveAttribute("targetR2HSICIndices_", targetR2HSICIndices_);
  adv.saveAttribute("targetPValuesAsymptotic_", targetPValuesAsymptotic_);
  adv.saveAttribute("targetPValuesPermutation_", targetPValuesPermutation_);
  adv.saveAttribute("targetCovarianceModels_", targetCovarianceModels_);
  adv.saveAttribute("computeTargetAsymptoticPValues_", computeTargetAsymptoticPValues_);
  adv.saveAttribute("computeTargetPermutationPValues_", computeTargetPermutationPValues_);
  adv.saveAttribute("useUStatisticTarget_", useUStatisticTarget_);
  adv.saveAttribute("conditionalHSICIndices_", conditionalHSICIndices_);
  adv.saveAttribute("conditionalR2HSICIndices_", conditionalR2HSICIndices_);
  adv.saveAttribute("conditionalPValuesPermutation_", conditionalPValuesPermutation_);
  adv.saveAttribute("conditionalCovarianceModels_", conditionalCovarianceModels_);
  adv.saveAttribute("computeConditionalPermutationPValues_", computeConditionalPermutationPValues_);
  adv.saveAttribute("isIndependent_", isIndependent_);
  adv.saveAttribute("independenceWarningMessage_", independenceWarningMessage_);
  adv.saveAttribute("interestVariables_", interestVariables_);
}

/* Method load() reloads the object from the StorageManager */
void DataSensitivityAnalysisResult::load(OT::Advocate & adv)
{
  EvaluationResult::load(adv);
  if (adv.hasAttribute("analysisType_"))
  {
    UnsignedInteger analysisTypeInt;
    adv.loadAttribute("analysisType_", analysisTypeInt);
    analysisType_ = AnalysisType(std::byte{static_cast<unsigned char>(analysisTypeInt)});
  }
  else
    analysisType_ = AnalysisType(std::byte{Type::RankSobol | Type::SRC}); // default value for backward compatibility
  adv.loadAttribute("firstOrderSobolIndices_", firstOrderSobolIndices_);
  adv.loadAttribute("firstOrderSobolIndicesInterval_", firstOrderSobolIndicesInterval_);
  adv.loadAttribute("SRCIndices_", SRCIndices_);
  adv.loadAttribute("signedSRCIndices_", signedSRCIndices_);
  adv.loadAttribute("SRCIndicesInterval_", SRCIndicesInterval_);
  adv.loadAttribute("signedSRCIndicesInterval_", signedSRCIndicesInterval_);
  adv.loadAttribute("r2_", r2_);
  adv.loadAttribute("isIndependent_", isIndependent_);
  adv.loadAttribute("independenceWarningMessage_", independenceWarningMessage_);
  if (analysisType_.computeGlobalHSIC() || analysisType_.computeTargetHSIC() || analysisType_.computeConditionalHSIC())
  {
    adv.loadAttribute("globalHSICIndices_", globalHSICIndices_);
    adv.loadAttribute("globalR2HSICIndices_", globalR2HSICIndices_);
    adv.loadAttribute("globalPValuesAsymptotic_", globalPValuesAsymptotic_);
    adv.loadAttribute("globalPValuesPermutation_", globalPValuesPermutation_);
    adv.loadAttribute("globalCovarianceModels_", globalCovarianceModels_);
    adv.loadAttribute("computeGlobalAsymptoticPValues_", computeGlobalAsymptoticPValues_);
    adv.loadAttribute("computeGlobalPermutationPValues_", computeGlobalPermutationPValues_);
    adv.loadAttribute("useUStatisticGlobal_", useUStatisticGlobal_);
    adv.loadAttribute("targetHSICIndices_", targetHSICIndices_);
    adv.loadAttribute("targetR2HSICIndices_", targetR2HSICIndices_);
    adv.loadAttribute("targetPValuesAsymptotic_", targetPValuesAsymptotic_);
    adv.loadAttribute("targetPValuesPermutation_", targetPValuesPermutation_);
    adv.loadAttribute("targetCovarianceModels_", targetCovarianceModels_);
    adv.loadAttribute("computeTargetAsymptoticPValues_", computeTargetAsymptoticPValues_);
    adv.loadAttribute("computeTargetPermutationPValues_", computeTargetPermutationPValues_);
    adv.loadAttribute("useUStatisticTarget_", useUStatisticTarget_);
    adv.loadAttribute("conditionalHSICIndices_", conditionalHSICIndices_);
    adv.loadAttribute("conditionalR2HSICIndices_", conditionalR2HSICIndices_);
    adv.loadAttribute("conditionalPValuesPermutation_", conditionalPValuesPermutation_);
    adv.loadAttribute("conditionalCovarianceModels_", conditionalCovarianceModels_);
    adv.loadAttribute("computeConditionalPermutationPValues_", computeConditionalPermutationPValues_);
  }
  if (adv.hasAttribute("interestVariables_"))
  {
     adv.loadAttribute("interestVariables_", interestVariables_);
  }
}

} // namespace PERSALYS