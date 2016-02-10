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
#ifndef OTGUI_SOBOLRESULT_HXX
#define OTGUI_SOBOLRESULT_HXX

#include "NumericalSample.hxx"

namespace OTGUI {
class SobolResult : public OT::PersistentObject
{
public:
  SobolResult();
  SobolResult(const OT::NumericalSample firstOrderIndices,
              const OT::Collection<OT::SymmetricMatrix> secondOrderIndices,
              const OT::NumericalSample totalOrderIndices,
              const OT::Description & outputNames);

  virtual SobolResult * clone() const;

  OT::Description getOutputNames() const;
  OT::Description getInputNames() const;
  OT::NumericalSample getFirstOrderIndices() const;
  OT::Collection<OT::SymmetricMatrix> getSecondOrderIndices() const;
  OT::NumericalSample getTotalOrderIndices() const;

private:
  OT::Description outputNames_;
  OT::NumericalSample firstOrderIndices_;
  OT::Collection<OT::SymmetricMatrix> secondOrderIndices_;
  OT::NumericalSample totalOrderIndices_;
};
}
#endif