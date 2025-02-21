//                                               -*- C++ -*-
/**
 *  @brief TaylorExpansionMomentsResult contains the moments computed with the Taylor Expansion method
 *
 *  Copyright 2015-2025 EDF-Phimeca
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
#include "persalys/TaylorExpansionMomentsResult.hxx"

#include <openturns/PersistentObjectFactory.hxx>

using namespace OT;

namespace PERSALYS
{

CLASSNAMEINIT(TaylorExpansionMomentsResult)

static Factory<TaylorExpansionMomentsResult> Factory_TaylorExpansionMomentsResult;

/* Default constructor */
TaylorExpansionMomentsResult::TaylorExpansionMomentsResult()
  : AnalysisResult()
{
}


/* Constructor with parameters */
TaylorExpansionMomentsResult::TaylorExpansionMomentsResult(const OT::Description & outputNames,
    const OT::Point & meanFirstOrder,
    const OT::Point & meanSecondOrder,
    const OT::Point & standardDeviation,
    const OT::Point & variance)
  : AnalysisResult()
  , outputNames_(outputNames)
  , meanFirstOrder_(meanFirstOrder)
  , meanSecondOrder_(meanSecondOrder)
  , standardDeviation_(standardDeviation)
  , variance_(variance)
{
}


/* Virtual constructor */
TaylorExpansionMomentsResult* TaylorExpansionMomentsResult::clone() const
{
  return new TaylorExpansionMomentsResult(*this);
}


Description TaylorExpansionMomentsResult::getOutputNames() const
{
  return outputNames_;
}


Point TaylorExpansionMomentsResult::getMeanFirstOrder() const
{
  return meanFirstOrder_;
}


Point TaylorExpansionMomentsResult::getMeanSecondOrder() const
{
  return meanSecondOrder_;
}


Point TaylorExpansionMomentsResult::getStandardDeviation() const
{
  return standardDeviation_;
}


Point TaylorExpansionMomentsResult::getVariance() const
{
  return variance_;
}


/* String converter */
String TaylorExpansionMomentsResult::__repr__() const
{
  OSS oss;
  oss << "class=" << GetClassName()
      << " name=" << getName()
      << " outputNames=" << getOutputNames()
      << " meanFirstOrder=" << getMeanFirstOrder()
      << " meanSecondOrder=" << getMeanSecondOrder()
      << " standardDeviation=" << getStandardDeviation()
      << " variance=" << getVariance();
  return oss;
}


/* Method save() stores the object through the StorageManager */
void TaylorExpansionMomentsResult::save(Advocate & adv) const
{
  AnalysisResult::save(adv);
  adv.saveAttribute("outputNames_", outputNames_);
  adv.saveAttribute("meanFirstOrder_", meanFirstOrder_);
  adv.saveAttribute("meanSecondOrder_", meanSecondOrder_);
  adv.saveAttribute("standardDeviation_", standardDeviation_);
  adv.saveAttribute("variance_", variance_);
}


/* Method load() reloads the object from the StorageManager */
void TaylorExpansionMomentsResult::load(Advocate & adv)
{
  AnalysisResult::load(adv);
  adv.loadAttribute("outputNames_", outputNames_);
  adv.loadAttribute("meanFirstOrder_", meanFirstOrder_);
  adv.loadAttribute("meanSecondOrder_", meanSecondOrder_);
  adv.loadAttribute("standardDeviation_", standardDeviation_);
  adv.loadAttribute("variance_", variance_);
}
}
