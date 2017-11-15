//                                               -*- C++ -*-
/**
 *  @brief Inference analysis result
 *
 *  Copyright 2015-2017 EDF-Phimeca
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
 *  You should have received a copy of the GNU Lesser General Public
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
#include "otgui/InferenceResult.hxx"

#include <openturns/PersistentObjectFactory.hxx>

using namespace OT;

namespace OTGUI
{

CLASSNAMEINIT(InferenceResult)

static Factory<InferenceResult> Factory_InferenceResult;
static Factory<PersistentCollection<FittingTestResult> > Factory_PersistentCollection_FittingTestResult;

/* Default constructor */
InferenceResult::InferenceResult()
  : PersistentObject()
{
}


/* Virtual constructor */
InferenceResult* InferenceResult::clone() const
{
  return new InferenceResult(*this);
}


InferenceResult::FittingTestResultCollection InferenceResult::getFittingTestResultCollection() const
{
  return fittingTestResultCollection_;
}


FittingTestResult InferenceResult::getFittingTestResultForVariable(const String& variableName) const
{
  if (!fittingTestResultCollection_.getSize())
    throw InvalidArgumentException(HERE) << "There is no FittingTest result";

  for (UnsignedInteger i = 0; i < fittingTestResultCollection_.getSize(); ++i)
  {
    if (fittingTestResultCollection_[i].getVariableName() == variableName)
      return fittingTestResultCollection_[i];
  }
  throw InvalidArgumentException(HERE) << "No result for a variable named " << variableName;
}


/* String converter */
String InferenceResult::__repr__() const
{
  OSS oss;
  oss << "class=" << GetClassName()
      << " FittingTestResultCollection=" << getFittingTestResultCollection();
  return oss;
}


/* Method save() stores the object through the StorageManager */
void InferenceResult::save(Advocate& adv) const
{
  PersistentObject::save(adv);
  adv.saveAttribute("fittingTestResultCollection_", fittingTestResultCollection_);
}


/* Method load() reloads the object from the StorageManager */
void InferenceResult::load(Advocate& adv)
{
  PersistentObject::load(adv);
  adv.loadAttribute("fittingTestResultCollection_", fittingTestResultCollection_);
}
}
