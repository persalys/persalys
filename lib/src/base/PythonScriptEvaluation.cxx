//                                               -*- C++ -*-
/**
 *  @brief PythonScriptEvaluation implements the evaluation of the models defined by a Python script
 *
 *  Copyright 2015-2017 EDF-Phimeca
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
#include "otgui/PythonScriptEvaluation.hxx"
#include "otgui/InterpreterUnlocker.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/PythonWrappingFunctions.hxx"

using namespace OT;

namespace OTGUI {

CLASSNAMEINIT(PythonScriptEvaluation)

static Factory<PythonScriptEvaluation> Factory_PythonScriptEvaluation;

/* Default constructor */
PythonScriptEvaluation::PythonScriptEvaluation()
  : EvaluationImplementation()
  , inputDimension_(0)
  , outputDimension_(0)
{
}


/* Constructor with parameters */
PythonScriptEvaluation::PythonScriptEvaluation(const UnsignedInteger & inputDimension,
                                   const UnsignedInteger & outputDimension,
                                   const String & code)
  : EvaluationImplementation()
  , inputDimension_(inputDimension)
  , outputDimension_(outputDimension)
  , code_(code)
{
}


/* Virtual constructor */
PythonScriptEvaluation* PythonScriptEvaluation::clone() const
{
  PythonScriptEvaluation * result = new PythonScriptEvaluation(*this);
  return result;
}


/* Comparison operator */
Bool PythonScriptEvaluation::operator ==(const PythonScriptEvaluation & other) const
{
  return (code_ == other.code_);
}


/* String converter */
String PythonScriptEvaluation::__repr__() const
{
  OSS oss(true);
  oss << "class=" << PythonScriptEvaluation::GetClassName()
      << " name=" << getName()
      << " code=" << code_;
  return oss;
}


/* String converter */
String PythonScriptEvaluation::__str__(const String & offset) const
{
  OSS oss(false);
  oss << offset << getInputDescription() << " code=" << code_;
  return oss;
}


void PythonScriptEvaluation::initialize()
{
//   Py_CompileStringFlags
}


UnsignedInteger PythonScriptEvaluation::getInputDimension() const
{
  return inputDimension_;
}


UnsignedInteger PythonScriptEvaluation::getOutputDimension() const
{
  return outputDimension_;
}


/* Operator () */
Point PythonScriptEvaluation::operator() (const Point & inP) const
{
  Point outP;

  CacheKeyType inKey(inP.getCollection());
  if (p_cache_->isEnabled() && p_cache_->hasKey(inKey))
  {
    outP = Point::ImplementationType(p_cache_->find(inKey));
  }
  else
  {
    InterpreterUnlocker iul;
    PyObject *module = PyImport_AddModule("__main__");// Borrowed reference.
    PyObject *dict = PyModule_GetDict(module);// Borrowed reference.

    // define the script on the first run only to allow to save a state
    if (callsNumber_ == 0)
    {
      ScopedPyObjectPointer retValue(PyRun_String(code_.c_str(), Py_file_input, dict, dict));
      handleException();
    }

    ++ callsNumber_;
    PyObject *script = PyDict_GetItemString(dict, "_exec");
    if (script == NULL) throw InternalException(HERE) << "no _exec function";

    ScopedPyObjectPointer inputTuple(convert< Point, _PySequence_ >(inP));
    ScopedPyObjectPointer outputList(PyObject_Call(script, inputTuple.get(), NULL));
    handleException();

    if (outputDimension_ > 1)
    {
      outP = convert<_PySequence_, Point>(outputList.get());
    }
    else
    {
      Scalar value = convert<_PyFloat_, Scalar>(outputList.get());
      outP = Point(1, value);
    }

    if (p_cache_->isEnabled())
    {
      CacheValueType outValue(outP.getCollection());
      p_cache_->add(inKey, outValue);
    }
  }
  if (isHistoryEnabled_)
  {
    inputStrategy_.store(inP);
    outputStrategy_.store(outP);
  }
  return outP;
}


/* Operator () */
// Sample PythonScriptEvaluation::operator() (const Sample & inS) const
// {
//   
// }

/* Method save() stores the object through the StorageManager */
void PythonScriptEvaluation::save(Advocate & adv) const
{
  EvaluationImplementation::save(adv);
  adv.saveAttribute("inputDimension_", inputDimension_);
  adv.saveAttribute("outputDimension_", outputDimension_);
  adv.saveAttribute("code_", code_);
}


/* Method load() reloads the object from the StorageManager */
void PythonScriptEvaluation::load(Advocate & adv)
{
  EvaluationImplementation::load(adv);
  adv.loadAttribute("inputDimension_", inputDimension_);
  adv.loadAttribute("outputDimension_", outputDimension_);
  adv.loadAttribute("code_", code_);
}
}
