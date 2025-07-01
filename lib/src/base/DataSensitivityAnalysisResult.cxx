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
static Factory<DataSensitivityAnalysisResult> Factory_DataSensitivityAnalysisResult;

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

const Collection<Point>& DataSensitivityAnalysisResult::getFirstOrderIndices() const
{
  return firstOrderIndices_;
}

const Collection<Interval>& DataSensitivityAnalysisResult::getFirstOrderIndicesInterval() const
{
  return firstOrderIndicesInterval_;
}

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
               << " firstOrderIndices=" << firstOrderIndices_
               << " firstOrderIndicesInterval=" << firstOrderIndicesInterval_
               << " isIndependent=" << isIndependent_
               << " independenceWarningMessage=" << independenceWarningMessage_;
}

/* Method save() stores the object through the StorageManager */
void DataSensitivityAnalysisResult::save(OT::Advocate & adv) const
{
  EvaluationResult::save(adv);
  adv.saveAttribute("firstOrderIndices_", firstOrderIndices_);
  adv.saveAttribute("firstOrderIndicesInterval_", firstOrderIndicesInterval_);
  adv.saveAttribute("isIndependent_", isIndependent_);
  adv.saveAttribute("independenceWarningMessage_", independenceWarningMessage_);
}

/* Method load() reloads the object from the StorageManager */
void DataSensitivityAnalysisResult::load(OT::Advocate & adv)
{
  EvaluationResult::load(adv);
  adv.loadAttribute("firstOrderIndices_", firstOrderIndices_);
  adv.loadAttribute("firstOrderIndicesInterval_", firstOrderIndicesInterval_);
  adv.loadAttribute("isIndependent_", isIndependent_);
  adv.loadAttribute("independenceWarningMessage_", independenceWarningMessage_);
}

} // namespace PERSALYS