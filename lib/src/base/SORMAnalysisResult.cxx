//                                               -*- C++ -*-
/**
 *  @brief Results of a SORM analysis
 *
 *  Copyright 2015-2020 EDF-Phimeca
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
#include "persalys/SORMAnalysisResult.hxx"

#include <openturns/PersistentObjectFactory.hxx>

using namespace OT;

namespace PERSALYS
{

CLASSNAMEINIT(SORMAnalysisResult)

static Factory<SORMAnalysisResult> Factory_SORMAnalysisResult;

/* Default constructor */
SORMAnalysisResult::SORMAnalysisResult()
  : AnalysisResult()
  , sormResult_()
{

}


/* Constructor with parameters */
SORMAnalysisResult::SORMAnalysisResult(const SORMResult& sormResult)
  : AnalysisResult()
  , sormResult_(sormResult)
{

}


SORMAnalysisResult* SORMAnalysisResult::clone() const
{
  return new SORMAnalysisResult(*this);
}


SORMResult SORMAnalysisResult::getSORMResult() const
{
  return sormResult_;
}


/* String converter */
String SORMAnalysisResult::__repr__() const
{
  OSS oss;
  oss << "class=" << GetClassName()
      << " name=" << getName();
  return oss;
}


/* Method save() stores the object through the StorageManager */
void SORMAnalysisResult::save(Advocate & adv) const
{
  AnalysisResult::save(adv);
  adv.saveAttribute("sormResult_", sormResult_);
}


/* Method load() reloads the object from the StorageManager */
void SORMAnalysisResult::load(Advocate & adv)
{
  AnalysisResult::load(adv);
  adv.loadAttribute("sormResult_", sormResult_);
}
}
