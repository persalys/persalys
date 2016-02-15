//                                               -*- C++ -*-
/**
 *  @brief SobolResult contains the Sobol indices
 *
 *  Copyright 2015-2016 EDF
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
#include "otgui/SobolResult.hxx"

#include "PersistentObjectFactory.hxx"

using namespace OT;

namespace OTGUI {

CLASSNAMEINIT(SobolResult);

static Factory<SobolResult> RegisteredFactory("SobolResult");
static Factory<PersistentCollection<SymmetricMatrix> > RegisteredFactorySymMtrColl("PersistentCollection<SymmetricMatrix>");

/* Default constructor */
SobolResult::SobolResult()
  : PersistentObject()
{
}


/* Constructor with parameters */
SobolResult::SobolResult(const NumericalSample firstOrderIndices,
                         const Collection<SymmetricMatrix> secondOrderIndices,
                         const NumericalSample totalOrderIndices,
                         const Description & outputNames)
  : PersistentObject()
  , outputNames_(outputNames)
  , firstOrderIndices_(firstOrderIndices)
  , secondOrderIndices_(secondOrderIndices)
  , totalOrderIndices_(totalOrderIndices)
{
}


/* Virtual constructor */
SobolResult* SobolResult::clone() const
{
  return new SobolResult(*this);
}


Description SobolResult::getOutputNames() const
{
  return outputNames_;
}


Description SobolResult::getInputNames() const
{
  return firstOrderIndices_.getDescription();
}


NumericalSample SobolResult::getFirstOrderIndices() const
{
  return firstOrderIndices_;
}


Collection<SymmetricMatrix> SobolResult::getSecondOrderIndices() const
{
  return secondOrderIndices_;
}


NumericalSample SobolResult::getTotalOrderIndices() const
{
  return totalOrderIndices_;
}


/* Method save() stores the object through the StorageManager */
void SobolResult::save(Advocate & adv) const
{
  PersistentObject::save(adv);
  adv.saveAttribute("outputNames_", outputNames_);
  adv.saveAttribute("firstOrderIndices_", firstOrderIndices_);
  adv.saveAttribute("secondOrderIndices_", secondOrderIndices_);
  adv.saveAttribute("totalOrderIndices_", totalOrderIndices_);
}


/* Method load() reloads the object from the StorageManager */
void SobolResult::load(Advocate & adv)
{
  PersistentObject::load(adv);
  adv.loadAttribute("outputNames_", outputNames_);
  adv.loadAttribute("firstOrderIndices_", firstOrderIndices_);
  adv.loadAttribute("secondOrderIndices_", secondOrderIndices_);
  adv.loadAttribute("totalOrderIndices_", totalOrderIndices_);
}
}