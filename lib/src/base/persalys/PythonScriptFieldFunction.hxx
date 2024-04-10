//                                               -*- C++ -*-
/**
 *  @brief PythonScriptFieldFunction implements the evaluation of the models defined by a Python script
 *
 *  Copyright 2015-2024 EDF-Phimeca
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
#ifndef PERSALYS_PYTHONSCRIPTFIELDFUNCTION_HXX
#define PERSALYS_PYTHONSCRIPTFIELDFUNCTION_HXX

#include "persalys/PersalysPrivate.hxx"

#include <openturns/PointToFieldFunction.hxx>

namespace PERSALYS
{

class PERSALYS_BASE_API PythonScriptFieldFunction : public OT::PointToFieldFunctionImplementation
{
  CLASSNAME

public:
  /** Default constructor */
  PythonScriptFieldFunction();
  /** Constructor with parameters */
  PythonScriptFieldFunction(const OT::Description & inputVariablesNames,
                         const OT::Description & outputVariablesNames,
                         const OT::Indices & outputIndices,
                         const OT::UnsignedInteger outputModelDimension,
                         const OT::String & code,
                         const OT::Mesh& mesh,
                         const OT::Bool isParallel);

  /** Virtual constructor */
  PythonScriptFieldFunction * clone() const override;

  /** Comparison operator */
  OT::Bool operator ==(const PythonScriptFieldFunction & other) const;

public:
  void resetCallsNumber();

  /** Operator () */
  OT::Sample operator() (const OT::Point & inP) const override;
  OT::ProcessSample operator() (const OT::Sample & inS) const override;

private:
  mutable bool scriptHasBeenEvaluated_;
  OT::Indices outputIndices_;
  OT::UnsignedInteger outputModelDimension_;
  OT::String code_;
  bool isParallel_;
};
}
#endif
