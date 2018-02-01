//                                               -*- C++ -*-
/**
 *  @brief MorrisResult
 *
 *  Copyright 2015-2018 EDF-Phimeca
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
#include "otgui/MorrisResult.hxx"

#include <openturns/PersistentObjectFactory.hxx>

using namespace OT;
using namespace OTMORRIS;

namespace OTGUI
{

CLASSNAMEINIT(MorrisResult)

static Factory<MorrisResult> Factory_MorrisResult;
static Factory<PersistentCollection<Indices > > Factory_PersistentCollection_Indices;

/* Default constructor */
MorrisResult::MorrisResult()
  : Morris()
  , inputsSelection_()
  , noEffectBoundary_()
{
}

/** Standard constructor */
MorrisResult::MorrisResult(const Sample& inputSample, const Sample& outputSample)
  : Morris(inputSample, outputSample)
  , inputsSelection_(outputSample.getDimension(), Indices(inputSample.getDimension(), 1))
  , noEffectBoundary_(outputSample.getDimension())
{
}


/* Virtual constructor */
MorrisResult* MorrisResult::clone() const
{
  return new MorrisResult(*this);
}


Indices MorrisResult::getInputsSelection(const UnsignedInteger marginal) const
{
  if (marginal < inputsSelection_.getSize())
    return inputsSelection_[marginal];
  return Indices();
}


void MorrisResult::setInputsSelection(const UnsignedInteger marginal, const Indices& values)
{
  if (marginal < inputsSelection_.getSize() && values.getSize() == inputsSelection_[marginal].getSize())
    inputsSelection_[marginal] = values;
}


Scalar MorrisResult::getNoEffectBoundary(const UnsignedInteger marginal) const
{
  if (marginal < noEffectBoundary_.getSize())
    return noEffectBoundary_[marginal];
  return 0.;
}


void MorrisResult::setNoEffectBoundary(const UnsignedInteger marginal, const Scalar value)
{
  if (marginal < noEffectBoundary_.getSize())
    noEffectBoundary_[marginal] = value;
}


/* String converter */
String MorrisResult::__repr__() const
{
  OSS oss;
  oss << "class=" << GetClassName()
      << " name=" << getName()
      << " inputs selections =" << inputsSelection_
      << " no effect boundaries=" << noEffectBoundary_;
  return oss;
}


/* Method save() stores the object through the StorageManager */
void MorrisResult::save(Advocate & adv) const
{
  Morris::save(adv);
  adv.saveAttribute("inputsSelection_", inputsSelection_);
  adv.saveAttribute("noEffectBoundary_", noEffectBoundary_);
}


/* Method load() reloads the object from the StorageManager */
void MorrisResult::load(Advocate & adv)
{
  Morris::load(adv);
  adv.loadAttribute("inputsSelection_", inputsSelection_);
  adv.loadAttribute("noEffectBoundary_", noEffectBoundary_);
}
}
