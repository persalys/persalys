//                                               -*- C++ -*-
/**
 *  @brief Results of an analysis
 *
 *  Copyright 2015-2024 EDF-Phimeca
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
#include "persalys/AnalysisResult.hxx"

#include <openturns/PersistentObjectFactory.hxx>

using namespace OT;

namespace PERSALYS
{

CLASSNAMEINIT(AnalysisResult)

static Factory<AnalysisResult> Factory_AnalysisResult;

/* Default constructor */
AnalysisResult::AnalysisResult()
  : PersistentObject()
  , elapsedTime_(0.)
{

}


AnalysisResult* AnalysisResult::clone() const
{
  return new AnalysisResult(*this);
}


Scalar AnalysisResult::getElapsedTime() const
{
  return elapsedTime_;
}


/* String converter */
String AnalysisResult::__repr__() const
{
  OSS oss;
  oss << "class=" << GetClassName()
      << " name=" << getName()
      << " elapsed time=" << getElapsedTime();
  return oss;
}


/* Method save() stores the object through the StorageManager */
void AnalysisResult::save(Advocate & adv) const
{
  PersistentObject::save(adv);
  adv.saveAttribute("elapsedTime_", elapsedTime_);
}


/* Method load() reloads the object from the StorageManager */
void AnalysisResult::load(Advocate & adv)
{
  PersistentObject::load(adv);
  adv.loadAttribute("elapsedTime_", elapsedTime_);
}
}
