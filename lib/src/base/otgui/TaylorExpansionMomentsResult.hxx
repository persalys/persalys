//                                               -*- C++ -*-
/**
 *  @brief TaylorExpansionMomentsResult contains the moments computed with the Taylor Expansion method
 *
 *  Copyright 2015-2016 EDF-Phimeca
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
#ifndef OTGUI_TAYLOREXPANSIONMOMENTSRESULT_HXX
#define OTGUI_TAYLOREXPANSIONMOMENTSRESULT_HXX

#include "Output.hxx"

#include "openturns/NumericalSample.hxx"

namespace OTGUI {
class OTGUI_API TaylorExpansionMomentsResult : public OT::PersistentObject
{
  CLASSNAME;

public:
  /** Default constructor */
  TaylorExpansionMomentsResult();
  /** Constructor with parameters */
  TaylorExpansionMomentsResult(const OT::Description & outputNames, const OT::NumericalPoint & meanFirstOrder,
                                const OT::NumericalPoint & meanSecondOrder,
                                const OT::NumericalPoint & standardDeviation,
                                const OT::NumericalPoint & variance);

  /** Virtual constructor */
  virtual TaylorExpansionMomentsResult * clone() const;

  OT::Description getOutputNames() const;
  OT::NumericalPoint getMeanFirstOrder() const;
  OT::NumericalPoint getMeanSecondOrder() const;
  OT::NumericalPoint getStandardDeviation() const;
  OT::NumericalPoint getVariance() const;

  /** Method save() stores the object through the StorageManager */
  void save(OT::Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  void load(OT::Advocate & adv);

private:
  OT::Description outputNames_;
  OT::NumericalPoint meanFirstOrder_;
  OT::NumericalPoint meanSecondOrder_;
  OT::NumericalPoint standardDeviation_;
  OT::NumericalPoint variance_;
};
}
#endif