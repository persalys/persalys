//                                               -*- C++ -*-
/**
 *  @brief Defines physical models thanks analytical formulas
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
#ifndef OTGUI_ANALYTICALPHYSICALMODEL_HXX
#define OTGUI_ANALYTICALPHYSICALMODEL_HXX

#include "PhysicalModelImplementation.hxx"

namespace OTGUI {
class AnalyticalPhysicalModel : public PhysicalModelImplementation
{
  CLASSNAME;

public:
  AnalyticalPhysicalModel(const OT::String & name);
  AnalyticalPhysicalModel(const OT::String & name, const InputCollection & inputs,
                          const OutputCollection & outputs);
  AnalyticalPhysicalModel(const AnalyticalPhysicalModel & other);
  virtual AnalyticalPhysicalModel * clone() const;

  OT::Description getFormulas();
  virtual OT::NumericalMathFunction getFunction();

  OT::String dump() const;

  /** Method save() stores the object through the StorageManager */
  virtual void save(OT::Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  virtual void load(OT::Advocate & adv);
};
}
#endif