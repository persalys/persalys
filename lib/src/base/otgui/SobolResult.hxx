//                                               -*- C++ -*-
/**
 *  @brief SobolResult contains the Sobol indices
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
#ifndef OTGUI_SOBOLRESULT_HXX
#define OTGUI_SOBOLRESULT_HXX

#include "otgui/OTGuiprivate.hxx"
#include "openturns/NumericalSample.hxx"

namespace OTGUI {
class OTGUI_API SobolResult : public OT::PersistentObject
{
  CLASSNAME;

public:
  /** Default constructor */
  SobolResult();
  /** Constructor with parameters */
  SobolResult(const OT::NumericalSample firstOrderIndices,
              const OT::Collection<OT::SymmetricMatrix> secondOrderIndices,
              const OT::NumericalSample totalOrderIndices,
              const OT::Description & outputNames);

  /** Virtual constructor */
  virtual SobolResult * clone() const;

  OT::Description getOutputNames() const;
  OT::Description getInputNames() const;
  OT::NumericalSample getFirstOrderIndices() const;
  OT::Collection<OT::SymmetricMatrix> getSecondOrderIndices() const;
  OT::NumericalSample getTotalOrderIndices() const;

  /** Method save() stores the object through the StorageManager */
  void save(OT::Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  void load(OT::Advocate & adv);

private:
  OT::Description outputNames_;
  OT::NumericalSample firstOrderIndices_;
  OT::PersistentCollection<OT::SymmetricMatrix> secondOrderIndices_;
  OT::NumericalSample totalOrderIndices_;
};
}
#endif