//                                               -*- C++ -*-
/**
 *  @brief SRCResult contains the Standard Regression Coefficients
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
#include "otgui/SRCResult.hxx"

#include "openturns/PersistentObjectFactory.hxx"

using namespace OT;

namespace OTGUI {

CLASSNAMEINIT(SRCResult);

static Factory<SRCResult> RegisteredFactory;

/* Default constructor */
SRCResult::SRCResult()
  : PersistentObject()
{
}


/* Constructor with parameters */
SRCResult::SRCResult(const Sample indices, const Description & outputNames)
  : PersistentObject()
  , outputNames_(outputNames)
  , indices_(indices)
{
}


/* Virtual constructor */
SRCResult* SRCResult::clone() const
{
  return new SRCResult(*this);
}


Description SRCResult::getOutputNames() const
{
  return outputNames_;
}


Description SRCResult::getInputNames() const
{
  return indices_.getDescription();
}


Sample SRCResult::getIndices() const
{
  return indices_;
}


/* String converter */
String SRCResult::__repr__() const
{
  OSS oss;
  oss << "class=" << GetClassName()
      << " name=" << getName()
      << " outputNames=" << getOutputNames()
      << " indices=" << getIndices();
  return oss;
}


/* Method save() stores the object through the StorageManager */
void SRCResult::save(Advocate & adv) const
{
  PersistentObject::save(adv);
  adv.saveAttribute("outputNames_", outputNames_);
  adv.saveAttribute("indices_", indices_);
}


/* Method load() reloads the object from the StorageManager */
void SRCResult::load(Advocate & adv)
{
  PersistentObject::load(adv);
  adv.loadAttribute("outputNames_", outputNames_);
  adv.loadAttribute("indices_", indices_);
}
}