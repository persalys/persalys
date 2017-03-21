//                                               -*- C++ -*-
/**
 *  @brief PythonEvaluation implements the evaluation of the models defined by a Python script
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
#include "otgui/PythonEvaluation.hxx"

#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/PythonWrappingFunctions.hxx"

using namespace OT;

namespace OTGUI {

CLASSNAMEINIT(PythonEvaluation);

static Factory<PythonEvaluation> RegisteredFactory;

/* Default constructor */
PythonEvaluation::PythonEvaluation()
  : NumericalMathEvaluationImplementation()
  , inputDimension_(0)
  , outputDimension_(0)
{
}


/* Constructor with parameters */
PythonEvaluation::PythonEvaluation(const UnsignedInteger & inputDimension,
                                   const UnsignedInteger & outputDimension,
                                   const String & code)
  : NumericalMathEvaluationImplementation()
  , inputDimension_(inputDimension)
  , outputDimension_(outputDimension)
  , code_(code)
{
}


/* Virtual constructor */
PythonEvaluation* PythonEvaluation::clone() const
{
  PythonEvaluation * result = new PythonEvaluation(*this);
  return result;
}


/* Comparison operator */
Bool PythonEvaluation::operator ==(const PythonEvaluation & other) const
{
  return (code_ == other.code_);
}


/* String converter */
String PythonEvaluation::__repr__() const
{
  OSS oss(true);
  oss << "class=" << PythonEvaluation::GetClassName()
      << " name=" << getName()
      << " code=" << code_;
  return oss;
}


/* String converter */
String PythonEvaluation::__str__(const String & offset) const
{
  OSS oss(false);
  oss << offset << getInputDescription() << " code=" << code_;
  return oss;
}


void PythonEvaluation::initialize()
{
//   Py_CompileStringFlags
}


UnsignedInteger PythonEvaluation::getInputDimension() const
{
  return inputDimension_;
}


UnsignedInteger PythonEvaluation::getOutputDimension() const
{
  return outputDimension_;
}

class InterpreterUnlocker
{
public:
  InterpreterUnlocker() { gstate_ = PyGILState_Ensure(); }
  ~InterpreterUnlocker() { PyGILState_Release(gstate_); }
private:
  PyGILState_STATE gstate_;
};

/* Operator () */
NumericalPoint PythonEvaluation::operator() (const NumericalPoint & inP) const
{
  NumericalPoint outP(outputDimension_);

  CacheKeyType inKey(inP.getCollection());
  if (p_cache_->isEnabled() && p_cache_->hasKey(inKey))
  {
    outP = NumericalPoint::ImplementationType(p_cache_->find(inKey));
  }
  else
  {
    ++ callsNumber_;
    InterpreterUnlocker iul;
    PyObject *module = PyImport_AddModule("__main__");// Borrowed reference.
    PyObject *dict = PyModule_GetDict(module);// Borrowed reference.
    ScopedPyObjectPointer retValue(PyRun_String(code_.c_str(), Py_file_input, dict, dict));
    handleException();
    PyObject *script = PyDict_GetItemString(dict, "_exec");
    if (script == NULL) throw InternalException(HERE) << "no _exec function";

    ScopedPyObjectPointer inputTuple(PyTuple_New(inputDimension_));
    for (UnsignedInteger i = 0; i < inputDimension_; ++ i)
    {
      PyObject *outputItem = PyFloat_FromDouble(inP[i]);// new reference
      PyTuple_SetItem(inputTuple.get(), i, outputItem);
    }

    ScopedPyObjectPointer outputList(PyObject_Call(script, inputTuple.get(), NULL));
    handleException();

    for (UnsignedInteger i = 0; i < outputDimension_; ++ i)
    {
      PyObject *inputItem = PyList_GetItem(outputList.get(), i);// borrowed reference
      outP[i] = PyFloat_AsDouble(inputItem);
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
// NumericalSample PythonEvaluation::operator() (const NumericalSample & inS) const
// {
//   
// }

/* Method save() stores the object through the StorageManager */
void PythonEvaluation::save(Advocate & adv) const
{
  NumericalMathEvaluationImplementation::save(adv);
  adv.saveAttribute("inputDimension_", inputDimension_);
  adv.saveAttribute("outputDimension_", outputDimension_);
  adv.saveAttribute("code_", code_);
}


/* Method load() reloads the object from the StorageManager */
void PythonEvaluation::load(Advocate & adv)
{
  NumericalMathEvaluationImplementation::load(adv);
  adv.loadAttribute("inputDimension_", inputDimension_);
  adv.loadAttribute("outputDimension_", outputDimension_);
  adv.loadAttribute("code_", code_);
}
}