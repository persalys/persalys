//                                               -*- C++ -*-
/**
 *  @brief Results of a FORM analysis
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
#include "persalys/FORMAnalysisResult.hxx"

#include <openturns/PersistentObjectFactory.hxx>

using namespace OT;

namespace PERSALYS
{

CLASSNAMEINIT(FORMAnalysisResult)

static Factory<FORMAnalysisResult> Factory_FORMAnalysisResult;

/* Default constructor */
FORMAnalysisResult::FORMAnalysisResult()
  : AnalysisResult()
  , formResult_()
{

}


/* Constructor with parameters */
FORMAnalysisResult::FORMAnalysisResult(const FORMResult& formResult)
  : AnalysisResult()
  , formResult_(formResult)
{

}


FORMAnalysisResult* FORMAnalysisResult::clone() const
{
  return new FORMAnalysisResult(*this);
}


FORMResult FORMAnalysisResult::getFORMResult() const
{
  return formResult_;
}


/* String converter */
String FORMAnalysisResult::__repr__() const
{
  OSS oss;
  oss << "class=" << GetClassName()
      << " name=" << getName();
  return oss;
}


/* Method save() stores the object through the StorageManager */
void FORMAnalysisResult::save(Advocate & adv) const
{
  AnalysisResult::save(adv);
  adv.saveAttribute("formResult_", formResult_);
}


/* Method load() reloads the object from the StorageManager */
void FORMAnalysisResult::load(Advocate & adv)
{
  AnalysisResult::load(adv);
  adv.loadAttribute("formResult_", formResult_);
}
}
