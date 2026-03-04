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

const Collection<Point>& DataSensitivityAnalysisResult::getGlobalHSICIndices() const
{
  return globalHSICIndices_;
}

const Collection<Point>& DataSensitivityAnalysisResult::getGlobalR2HSICIndices() const
{
  return globalR2HSICIndices_;
}

const Collection<Point>& DataSensitivityAnalysisResult::getGlobalPValuesAsymptotic() const
{
  return globalPValuesAsymptotic_;
}

const Collection<Point>& DataSensitivityAnalysisResult::getGlobalPValuesPermutation() const
{
  return globalPValuesPermutation_;
}

bool DataSensitivityAnalysisResult::computeHSICPValuesAsymptotic() const
{
  return computeAsymptoticPValues_;
}

bool DataSensitivityAnalysisResult::computeHSICPValuesPermutation() const
{
  return computePermutationPValues_;
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
  adv.saveAttribute("SRCIndicesInterval_", SRCIndicesInterval_);
  adv.saveAttribute("signedSRCIndicesInterval_", signedSRCIndicesInterval_);
  adv.saveAttribute("r2_", r2_);
  adv.saveAttribute("isIndependent_", isIndependent_);
  adv.saveAttribute("independenceWarningMessage_", independenceWarningMessage_);
  adv.saveAttribute("globalHSICIndices_", globalHSICIndices_);
  adv.saveAttribute("globalR2HSICIndices_", globalR2HSICIndices_);
  adv.saveAttribute("globalPValuesAsymptotic_", globalPValuesAsymptotic_);
  adv.saveAttribute("globalPValuesPermutation_", globalPValuesPermutation_);
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
  if (analysisType_.computeGlobalHSIC())
  {
    adv.loadAttribute("globalHSICIndices_", globalHSICIndices_);
    adv.loadAttribute("globalR2HSICIndices_", globalR2HSICIndices_);
    adv.loadAttribute("globalPValuesAsymptotic_", globalPValuesAsymptotic_);
    adv.loadAttribute("globalPValuesPermutation_", globalPValuesPermutation_);
  }
}

} // namespace PERSALYS