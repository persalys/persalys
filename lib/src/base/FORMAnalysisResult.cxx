//                                               -*- C++ -*-
/**
 *  @brief Results of a FORM analysis
 *
 *  Copyright 2015-2026 EDF-Phimeca
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
#include "persalys/FORMAnalysisResult.hxx"

#include <openturns/PersistentObjectFactory.hxx>

using namespace OT;

namespace PERSALYS
{

CLASSNAMEINIT(FORMAnalysisResult)

const static Factory<FORMAnalysisResult> Factory_FORMAnalysisResult;

/* Constructor with parameters */
FORMAnalysisResult::FORMAnalysisResult(const FORMResult& formResult)
  : formResult_(formResult)
{

}

FORMAnalysisResult::FORMAnalysisResult(const MultiFORMResult& multiFormResult)
  : multiFormResult_(multiFormResult)
  , isSystemFormResult_(true)
{

}


FORMAnalysisResult* FORMAnalysisResult::clone() const
{
  return new FORMAnalysisResult(*this);
}

Scalar FORMAnalysisResult::getEventProbability() const
{
  if (isSystemFormResult_)
    return multiFormResult_.getEventProbability();
  else
    return formResult_.getEventProbability();
}

Scalar FORMAnalysisResult::getGeneralisedReliabilityIndex() const
{
  if (isSystemFormResult_)
    return multiFormResult_.getGeneralisedReliabilityIndex();
  else
    return formResult_.getGeneralisedReliabilityIndex();
}


FORMResult FORMAnalysisResult::getFORMResult() const
{
  if (isSystemFormResult_)
    throw InvalidArgumentException(HERE) << "This result is a MultiFORMResult, not a FORMResult";
  
  return formResult_;
}

MultiFORMResult FORMAnalysisResult::getMultiFORMResult() const
{
  if (!isSystemFormResult_)
    throw InvalidArgumentException(HERE) << "This result is a FORMResult, not a MultiFORMResult";
  
  return multiFormResult_;
}


/* String converter */
OT::String FORMAnalysisResult::__repr__() const
{
  OSS oss;
  oss << "class=" << GetClassName()
      << " name=" << getName()
      << " isSystemFormResult=" << (isSystemFormResult_ ? "true" : "false");
  return oss;
}


/* Method save() stores the object through the StorageManager */
void FORMAnalysisResult::save(Advocate & adv) const
{
  AnalysisResult::save(adv);
  adv.saveAttribute("formResult_", formResult_);
  adv.saveAttribute("multiFormResult_", multiFormResult_);
  adv.saveAttribute("isSystemFormResult_", isSystemFormResult_);
}


/* Method load() reloads the object from the StorageManager */
void FORMAnalysisResult::load(Advocate & adv)
{
  AnalysisResult::load(adv);
  adv.loadAttribute("formResult_", formResult_);
  if (adv.hasAttribute("multiFormResult_"))
  {
    adv.loadAttribute("multiFormResult_", multiFormResult_);
    adv.loadAttribute("isSystemFormResult_", isSystemFormResult_);
  }
}
}
