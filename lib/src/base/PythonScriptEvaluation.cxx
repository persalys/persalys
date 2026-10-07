//                                               -*- C++ -*-
/**
 *  @brief PythonScriptEvaluation implements the evaluation of the models defined by a Python script
 *
 *  Copyright 2015-2026 EDF-Phimeca
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

#include "persalys/PythonScriptEvaluation.hxx"

#include "persalys/InterpreterUnlocker.hxx"
#include "persalys/PythonEnvironment.hxx"
#include "persalys/BaseTools.hxx"
#include "persalys/AppliException.hxx"

#include <openturns/PersistentObjectFactory.hxx>
#include <openturns/PythonWrappingFunctions.hxx>
#include <openturns/BatchFailedException.hxx>

#include <sstream>
#include <string>

using namespace OT;

namespace PERSALYS
{
std::size_t PythonScriptEvaluation::LastCodeHash_ = 0;

CLASSNAMEINIT(PythonScriptEvaluation)

static const Factory<PythonScriptEvaluation> Factory_PythonScriptEvaluation;

/* Default constructor */
PythonScriptEvaluation::PythonScriptEvaluation()
  : EvaluationImplementation()
{
}


/* Constructor with parameters */
PythonScriptEvaluation::PythonScriptEvaluation(const Description & inputVariablesNames,
    const Description & outputVariablesNames,
    const String & code)
  : EvaluationImplementation()
  , inputDimension_(inputVariablesNames.getSize())
  , outputDimension_(outputVariablesNames.getSize())
  , code_(code)
  , codeHash_(std::hash<std::string> {}(code))
{
  setInputDescription(inputVariablesNames);
  setOutputDescription(outputVariablesNames);
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


UnsignedInteger PythonScriptEvaluation::getInputDimension() const
{
  return inputDimension_;
}


UnsignedInteger PythonScriptEvaluation::getOutputDimension() const
{
  return outputDimension_;
}


void PythonScriptEvaluation::setParallel(bool flag)
{
  isParallel_ = flag;
}


void PythonScriptEvaluation::setProcessNumber(UnsignedInteger processNumber)
{
  processNumber_ = processNumber;
}

void PythonScriptEvaluation::resetCallsNumber()
{
  callsNumber_ = 0;
}


namespace
{

/** Name of the module driving the worker processes evaluating a model. */
const char * const EvaluationModuleName = "persalys._script_evaluation";


/**
 * @brief The Python module driving the worker processes evaluating a model.
 *
 * Everything concerning those processes, starting them, reusing them and
 * distributing the points, is implemented there: see _script_evaluation.py. It is
 * imported once, the module holding the pool between two evaluations.
 */
PyObject * GetEvaluationModule()
{
  static PyObject * module = nullptr;
  if (!module)
  {
    module = PyImport_ImportModule(EvaluationModuleName);
    handleException();
  }
  else
    Py_INCREF(module); // the caller owns a reference
  return module;
}


/** @brief Get an item of the tuple returned by the evaluation. */
PyObject * GetTupleItem(PyObject * tuple, UnsignedInteger index, const String & what)
{
  if ((!PyTuple_Check(tuple)) || (PyTuple_Size(tuple) != 4))
    throw InternalException(HERE) << "wrong result of the evaluation of the model: no " << what;
  PyObject * const item = PyTuple_GetItem(tuple, index); // Borrowed reference.
  if (item == nullptr)
    throw InternalException(HERE) << "wrong result of the evaluation of the model: no " << what;
  return item;
}

}


/* Operator () */
Point PythonScriptEvaluation::operator() (const Point & inP) const
{
  Point outP;
  InterpreterUnlocker iul;
  PyObject * module = PyImport_AddModule("__main__");// Borrowed reference.
  PyObject * dict = PyModule_GetDict(module);// Borrowed reference.

  // define the script on the first run only to allow one to save a state
  if (LastCodeHash_ != codeHash_)
  {
    ScopedPyObjectPointer retValue(PyRun_String(code_.c_str(), Py_file_input, dict, dict));
    handleException();
    LastCodeHash_ = codeHash_;
  }
  callsNumber_.increment();
  PyObject * script = PyDict_GetItemString(dict, "_exec");
  if (script == NULL)
    throw InternalException(HERE) << "no _exec function";

  ScopedPyObjectPointer inputTuple(convert< Point, _PySequence_ >(inP));
  ScopedPyObjectPointer outputList(PyObject_Call(script, inputTuple.get(), NULL));
  handleException();

  if (getOutputDimension() > 1)
  {
    outP = checkAndConvert<_PySequence_, Point>(outputList.get());
  }
  else
  {
    Scalar value = checkAndConvert<_PyFloat_, Scalar>(outputList.get());
    outP = Point(1, value);
  }

  return outP;
}


Sample PythonScriptEvaluation::operator() (const Sample & inS) const
{
  const UnsignedInteger size = inS.getSize();
  if (!isParallel_ || (size <= smallSize_))
  {
    Sample outS(0, getOutputDimension());
    Indices nokIdx;
    Description errorDesc;
    for (UnsignedInteger i = 0; i < inS.getSize(); ++i)
    {
      try
      {
        outS.add(operator() (inS[i]));
      }
      catch (const Exception& ex)
      {
        errorDesc.add(ex.what());
        nokIdx.add(i);
      }
    }
    if (nokIdx.getSize())
      throw BatchFailedException(HERE, nokIdx, errorDesc,
                                 nokIdx.complement(inS.getSize()), outS)
          << "Batch evaluation " << nokIdx.getSize() << "/" << size << " failed: " << errorDesc[0];
    return outS;
  }

  const UnsignedInteger outDim = getOutputDimension();
  const UnsignedInteger inDim = inS.getDimension();

  if (inDim != getInputDimension())
    throw InvalidDimensionException(HERE) << "Sample has incorrect dimension. Got " << inDim << ". Expected " << getInputDimension();

  // the code and the points are handed over to Python, which takes care of the
  // worker processes: it starts them once and reuses them for every batch
  // evaluating the same code
  InterpreterUnlocker iul;

  ScopedPyObjectPointer module(GetEvaluationModule());
  ScopedPyObjectPointer evaluate(PyObject_GetAttrString(module.get(), "evaluate"));
  if (!evaluate.get())
    throw InternalException(HERE) << "no evaluate function in " << EvaluationModuleName;

  ScopedPyObjectPointer code(PyUnicode_FromString(code_.c_str()));
  ScopedPyObjectPointer workers(PyLong_FromUnsignedLong(processNumber_));

  // the points are given to the workers as tuples of values
  ScopedPyObjectPointer points(PyTuple_New(size));
  for (UnsignedInteger i = 0; i < size; ++ i)
    PyTuple_SetItem(points.get(), i, convert< Point, _PySequence_ >(inS[i]));

  ScopedPyObjectPointer arguments(PyTuple_Pack(3, code.get(), points.get(), workers.get()));

  ScopedPyObjectPointer retValue(PyObject_Call(evaluate.get(), arguments.get(), NULL));
  handleException();

  // what comes back: the outputs of the points that succeeded, their indices,
  // the indices of the points that failed and the reason of each failure
  Sample outputSample(convert<_PySequence_, Sample>(GetTupleItem(retValue.get(), 0, "result")));
  Indices okIdx(convert<_PySequence_, Indices>(GetTupleItem(retValue.get(), 1, "succeeded indices")));
  Indices nokIdx(convert<_PySequence_, Indices>(GetTupleItem(retValue.get(), 2, "failed indices")));
  Description errorDesc(convert<_PySequence_, Description>(GetTupleItem(retValue.get(), 3, "failure messages")));

  // if all points failed, return an empty sample with correct dimension
  if(!outputSample.getSize())
    outputSample = Sample(0, getOutputDimension());

  // if any failed indices, throw
  if (nokIdx.getSize())
    throw BatchFailedException(HERE, nokIdx, errorDesc, okIdx, outputSample)
        << "Batch evaluation " << nokIdx.getSize() << "/" << size << " failed: " << errorDesc[0];

  // check
  if (outputSample.getSize() != size)
    throw InvalidArgumentException(HERE) << "Python Function returned a sequence object with incorrect size (got "
                                         << outputSample.getSize() << ", expected " << size << ")";
  if (outputSample.getDimension() != outDim)
    throw InvalidArgumentException(HERE) << "Python Function returned a sequence object with incorrect dimension (got "
                                         << outputSample.getDimension() << ", expected " << outDim << ")";

  outputSample.setDescription(getOutputDescription());
  callsNumber_.fetchAndAdd(size);

  return outputSample;
}


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
