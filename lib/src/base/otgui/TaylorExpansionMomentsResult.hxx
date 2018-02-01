//                                               -*- C++ -*-
/**
 *  @brief TaylorExpansionMomentsResult contains the moments computed with the Taylor Expansion method
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
#ifndef OTGUI_TAYLOREXPANSIONMOMENTSRESULT_HXX
#define OTGUI_TAYLOREXPANSIONMOMENTSRESULT_HXX

#include "otgui/OTGuiprivate.hxx"

#include <openturns/OTType.hxx>

namespace OTGUI
{
class OTGUI_API TaylorExpansionMomentsResult : public OT::PersistentObject
{
  CLASSNAME

public:
  /** Default constructor */
  TaylorExpansionMomentsResult();
  /** Constructor with parameters */
  TaylorExpansionMomentsResult(const OT::Description & outputNames, const OT::Point & meanFirstOrder,
                               const OT::Point & meanSecondOrder,
                               const OT::Point & standardDeviation,
                               const OT::Point & variance);

  /** Virtual constructor */
  virtual TaylorExpansionMomentsResult * clone() const;

  OT::Description getOutputNames() const;
  OT::Point getMeanFirstOrder() const;
  OT::Point getMeanSecondOrder() const;
  OT::Point getStandardDeviation() const;
  OT::Point getVariance() const;

  /** String converter */
  virtual OT::String __repr__() const;

  /** Method save() stores the object through the StorageManager */
  void save(OT::Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  void load(OT::Advocate & adv);

private:
  OT::Description outputNames_;
  OT::Point meanFirstOrder_;
  OT::Point meanSecondOrder_;
  OT::Point standardDeviation_;
  OT::Point variance_;
};
}
#endif
