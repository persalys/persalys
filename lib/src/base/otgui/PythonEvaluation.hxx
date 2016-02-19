//                                               -*- C++ -*-
/**
 *  @brief PythonEvaluation implements the evaluation of the models defined by a Python script
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
#ifndef OTGUI_PYTHONEVALUATION_HXX
#define OTGUI_PYTHONEVALUATION_HXX

#include "otgui/OTGuiprivate.hxx"

#include "NumericalMathEvaluationImplementation.hxx"
#include "Pointer.hxx"

namespace OTGUI {

/**
 * @class PythonEvaluation
 *
 * The class that implement the evaluation of an analytical function.
 */
class OTGUI_API PythonEvaluation : public OT::NumericalMathEvaluationImplementation
{
  CLASSNAME;

public:
  /** Default constructor */
  PythonEvaluation();
  /** Constructor with parameters */
  PythonEvaluation(const OT::UnsignedInteger & inputDimension,
                   const OT::UnsignedInteger & ouputDimension,
                   const OT::String & code);

  /** Virtual constructor */
  virtual PythonEvaluation * clone() const;

  /** Comparison operator */
  OT::Bool operator ==(const PythonEvaluation & other) const;

  /** String converter */
  virtual OT::String __repr__() const;
  virtual OT::String __str__(const OT::String & offset = "") const;

protected:

  void initialize();

public:
  /** Operator () */
  virtual OT::NumericalPoint operator() (const OT::NumericalPoint & inP) const;
//   virtual OT::NumericalSample operator() (const OT::NumericalSample & inS) const;

   /** Accessor for input point dimension */
  OT::UnsignedInteger getInputDimension() const;
  OT::UnsignedInteger getOutputDimension() const;
  
  /** Method save() stores the object through the StorageManager */
  void save(OT::Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  void load(OT::Advocate & adv);

protected:

private:
  OT::UnsignedInteger inputDimension_;
  OT::UnsignedInteger outputDimension_;
  OT::String code_;
 
};
}
#endif
