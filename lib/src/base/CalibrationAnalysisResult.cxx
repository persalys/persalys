//                                               -*- C++ -*-
/**
 *  @brief Class to define designs of experiments
 *
 *  Copyright 2015-2021 EDF-Phimeca
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
#include "persalys/CalibrationAnalysisResult.hxx"

#include <openturns/PersistentObjectFactory.hxx>

using namespace OT;

namespace PERSALYS
{

CLASSNAMEINIT(CalibrationAnalysisResult)

static Factory<CalibrationAnalysisResult> Factory_CalibrationAnalysisResult;

CalibrationAnalysisResult::CalibrationAnalysisResult()
  : PersistentObject()
{
}


/* Virtual constructor */
CalibrationAnalysisResult* CalibrationAnalysisResult::clone() const
{
  return new CalibrationAnalysisResult(*this);
}


/* String converter */
String CalibrationAnalysisResult::__repr__() const
{
  OSS oss;
  oss << calibrationResult_.__repr__()
      << "confidenceInterval =" << getConfidenceInterval();
  return oss;
}


/* Method save() stores the object through the StorageManager */
void CalibrationAnalysisResult::save(Advocate & adv) const
{
  PersistentObject::save(adv);
  adv.saveAttribute("calibrationResult_", calibrationResult_);
  adv.saveAttribute("confidenceInterval_", confidenceInterval_);
  adv.saveAttribute("priorResidualsPDF_", priorResidualsPDF_);
  adv.saveAttribute("posteriorResidualsPDF_", posteriorResidualsPDF_);
}


/* Method load() reloads the object from the StorageManager */
void CalibrationAnalysisResult::load(Advocate & adv)
{
  PersistentObject::load(adv);
  adv.loadAttribute("calibrationResult_", calibrationResult_);
  adv.loadAttribute("confidenceInterval_", confidenceInterval_);
  adv.loadAttribute("priorResidualsPDF_", priorResidualsPDF_);
  adv.loadAttribute("posteriorResidualsPDF_", posteriorResidualsPDF_);
}
}
