//                                               -*- C++ -*-
/**
 *  @brief MorrisResult contains the Standard Regression Coefficients
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
#ifndef OTGUI_MORRISRESULT_HXX
#define OTGUI_MORRISRESULT_HXX

#include "otgui/OTGuiprivate.hxx"

#include <otmorris/Morris.hxx>

namespace OTGUI
{
class OTGUI_API MorrisResult : public OTMORRIS::Morris
{
  CLASSNAME

public:

  friend class MorrisAnalysis;

  /** Default constructor */
  MorrisResult();
  /** Standard constructor with in/out designs */
  MorrisResult(const OT::Sample & inputSample, const OT::Sample & outputSample);

  /** Virtual constructor */
  virtual MorrisResult * clone() const;

  OT::Indices getInputsSelection(const OT::UnsignedInteger marginal) const;
  void setInputsSelection(const OT::UnsignedInteger marginal, const OT::Indices& values);

  OT::Scalar getNoEffectBoundary(const OT::UnsignedInteger marginal) const;
  void setNoEffectBoundary(const OT::UnsignedInteger marginal, const OT::Scalar value);

  /** String converter */
  virtual OT::String __repr__() const;

  /** Method save() stores the object through the StorageManager */
  void save(OT::Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  void load(OT::Advocate & adv);

private:
  OT::PersistentCollection<OT::Indices > inputsSelection_;
  OT::Point noEffectBoundary_;
};
}
#endif
