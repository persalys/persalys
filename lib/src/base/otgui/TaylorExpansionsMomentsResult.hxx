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
#ifndef OTGUI_TAYLOREXPANSIONSMOMENTSRESULT_HXX
#define OTGUI_TAYLOREXPANSIONSMOMENTSRESULT_HXX

#include "Output.hxx"

#include "QuadraticCumul.hxx"

namespace OTGUI {
class TaylorExpansionsMomentsResult : public OT::PersistentObject
{
public:
  TaylorExpansionsMomentsResult();
  TaylorExpansionsMomentsResult(const OT::QuadraticCumul & algoTaylorExpansionsMoments, const OutputCollection & outputs);

  virtual TaylorExpansionsMomentsResult * clone() const;

  OT::Description getOutputNames() const;
  OT::NumericalPoint getMeanFirstOrder() const;
  OT::NumericalPoint getMeanSecondOrder() const;
  OT::NumericalPoint getStandardDeviation();
  OT::NumericalPoint getVariance();

  /** Method save() stores the object through the StorageManager */
  void save(OT::Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  void load(OT::Advocate & adv);

private:
  OT::QuadraticCumul algoTaylorExpansionsMoments_;
  OT::Description outputNames_;
  OT::NumericalPoint standardDeviation_;
  OT::NumericalPoint variance_;
};
}
#endif