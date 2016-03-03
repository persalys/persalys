//                                               -*- C++ -*-
/**
 *  @brief TaylorExpansionsMomentsResult contains the moments computed with the Taylor Expansion method
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
#include "otgui/TaylorExpansionsMomentsResult.hxx"

#include "PersistentObjectFactory.hxx"

using namespace OT;

namespace OTGUI {

CLASSNAMEINIT(TaylorExpansionsMomentsResult);

static Factory<TaylorExpansionsMomentsResult> RegisteredFactory("TaylorExpansionsMomentsResult");

/* Default constructor */
TaylorExpansionsMomentsResult::TaylorExpansionsMomentsResult()
  : PersistentObject()
{
}


/* Constructor with parameters */
TaylorExpansionsMomentsResult::TaylorExpansionsMomentsResult(const OT::Description & outputNames,
                                                             const OT::NumericalPoint & meanFirstOrder,
                                                             const OT::NumericalPoint & meanSecondOrder,
                                                             const OT::NumericalPoint & standardDeviation,
                                                             const OT::NumericalPoint & variance)
  : PersistentObject()
  , outputNames_(outputNames)
  , meanFirstOrder_(meanFirstOrder)
  , meanSecondOrder_(meanSecondOrder)
  , standardDeviation_(standardDeviation)
  , variance_(variance)
{
}


/* Virtual constructor */
TaylorExpansionsMomentsResult* TaylorExpansionsMomentsResult::clone() const
{
  return new TaylorExpansionsMomentsResult(*this);
}


Description TaylorExpansionsMomentsResult::getOutputNames() const
{
  return outputNames_;
}


NumericalPoint TaylorExpansionsMomentsResult::getMeanFirstOrder() const
{
  return meanFirstOrder_;
}


NumericalPoint TaylorExpansionsMomentsResult::getMeanSecondOrder() const
{
  return meanSecondOrder_;
}


NumericalPoint TaylorExpansionsMomentsResult::getStandardDeviation() const
{
  return standardDeviation_;
}


NumericalPoint TaylorExpansionsMomentsResult::getVariance() const
{
  return variance_;
}


/* Method save() stores the object through the StorageManager */
void TaylorExpansionsMomentsResult::save(Advocate & adv) const
{
  PersistentObject::save(adv);
  adv.saveAttribute("outputNames_", outputNames_);
  adv.saveAttribute("meanFirstOrder_", meanFirstOrder_);
  adv.saveAttribute("meanSecondOrder_", meanSecondOrder_);
  adv.saveAttribute("standardDeviation_", standardDeviation_);
  adv.saveAttribute("variance_", variance_);
}


/* Method load() reloads the object from the StorageManager */
void TaylorExpansionsMomentsResult::load(Advocate & adv)
{
  PersistentObject::load(adv);
  adv.loadAttribute("outputNames_", outputNames_);
  adv.loadAttribute("meanFirstOrder_", meanFirstOrder_);
  adv.loadAttribute("meanSecondOrder_", meanSecondOrder_);
  adv.loadAttribute("standardDeviation_", standardDeviation_);
  adv.loadAttribute("variance_", variance_);
}
}