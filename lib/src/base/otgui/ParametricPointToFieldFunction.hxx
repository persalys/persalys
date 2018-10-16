//                                               -*- C++ -*-
/**
 *  @brief The class that implements parametric Point To Field functions.
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
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
#ifndef OPENTURNS_PARAMETRICPOINTTOFIELDFUNCTION_HXX
#define OPENTURNS_PARAMETRICPOINTTOFIELDFUNCTION_HXX

#include "otgui/OTGuiprivate.hxx"

#include <openturns/PointToFieldFunction.hxx>

namespace OTGUI
{
class OTGUI_API ParametricPointToFieldFunction : public OT::PointToFieldFunctionImplementation
{
  CLASSNAME

public:

  /* Default constructor */
  ParametricPointToFieldFunction();

  /** Parameter constructor */
  ParametricPointToFieldFunction (const OT::PointToFieldFunction & function,
                      const OT::Indices & set,
                      const OT::Point & referencePoint);

  /** Virtual constructor */
  virtual ParametricPointToFieldFunction * clone() const;

  virtual OT::Sample operator() (const OT::Point & point) const;
  virtual OT::ProcessSample operator() (const OT::Sample & inS) const;

  /** Dimension accessor */
  virtual OT::UnsignedInteger getInputDimension() const;

  /** String converter */
  virtual OT::String __repr__() const;
  virtual OT::String __str__(const OT::String & offset = "") const;

private:
  /** The underlying function */
  OT::PointToFieldFunction function_;

  /** The position of the parameters */
  OT::Indices parametersPositions_;

  /** The position of the input */
  OT::Indices inputPositions_;

  /** The value of the parameters */
  OT::Point parameter_;

  /** The description of the parameters */
  OT::Description parameterDescription_;
};
}
#endif
