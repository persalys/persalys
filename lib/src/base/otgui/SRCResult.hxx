//                                               -*- C++ -*-
/**
 *  @brief SRCResult contains the Standard Regression Coefficients
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
#ifndef OTGUI_SRCRESULT_HXX
#define OTGUI_SRCRESULT_HXX

#include "otgui/OTGuiprivate.hxx"
#include <NumericalSample.hxx>

namespace OTGUI {
class OTGUI_API SRCResult : public OT::PersistentObject
{
  CLASSNAME;

public:
  /** Default constructor */
  SRCResult();
  /** Constructor with parameters */
  SRCResult(const OT::NumericalSample indices, const OT::Description & outputNames);

  /** Virtual constructor */
  virtual SRCResult * clone() const;

  OT::Description getOutputNames() const;
  OT::Description getInputNames() const;
  OT::NumericalSample getIndices() const;

  /** Method save() stores the object through the StorageManager */
  void save(OT::Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  void load(OT::Advocate & adv);

private:
  OT::Description outputNames_;
  OT::NumericalSample indices_;
};
}
#endif