//                                               -*- C++ -*-
/**
 *  @brief PythonScriptEvaluation implements the evaluation of the models defined by a Python script
 *
 *  Copyright 2015-2020 EDF-Phimeca
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
#ifndef PERSALYS_PYTHONSCRIPTEVALUATION_HXX
#define PERSALYS_PYTHONSCRIPTEVALUATION_HXX

#include "persalys/PersalysPrivate.hxx"

#include <openturns/EvaluationImplementation.hxx>

namespace PERSALYS
{
/**
 * @class PythonScriptEvaluation
 *
 * The class that implement the evaluation of an analytical function.
 */
class PERSALYS_BASE_API PythonScriptEvaluation : public OT::EvaluationImplementation
{
  CLASSNAME

public:
  /** Default constructor */
  PythonScriptEvaluation();
  /** Constructor with parameters */
  PythonScriptEvaluation(const OT::Description & inputVariablesNames,
                         const OT::Description & outputVariablesNames,
                         const OT::String & code);

  /** Virtual constructor */
  virtual PythonScriptEvaluation * clone() const;

  /** Comparison operator */
  OT::Bool operator ==(const PythonScriptEvaluation & other) const;

  /** String converter */
  virtual OT::String __repr__() const;
  virtual OT::String __str__(const OT::String & offset = "") const;

  void resetCallsNumber();

  /** Operator () */
  virtual OT::Point operator() (const OT::Point & inP) const;
  virtual OT::Sample operator() (const OT::Sample & inS) const;

  /** Accessor for input point dimension */
  OT::UnsignedInteger getInputDimension() const;
  OT::UnsignedInteger getOutputDimension() const;

  void setParallel(bool flag);
  void setProcessNumber(OT::UnsignedInteger processNumber);

  /** Method save() stores the object through the StorageManager */
  void save(OT::Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  void load(OT::Advocate & adv);

private:
  mutable bool scriptHasBeenEvaluated_ = false;
  OT::UnsignedInteger inputDimension_ = 0;
  OT::UnsignedInteger outputDimension_ = 0;
  OT::String code_ = "";
  bool isParallel_ = false;
  OT::UnsignedInteger processNumber_ = 0;
};
}
#endif
