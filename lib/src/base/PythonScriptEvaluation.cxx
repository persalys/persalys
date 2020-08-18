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

#include "persalys/PythonScriptEvaluation.hxx"

#include "persalys/InterpreterUnlocker.hxx"
#include "persalys/PythonEnvironment.hxx"
#include "persalys/BaseTools.hxx"

#include <openturns/PersistentObjectFactory.hxx>
#include <openturns/PythonWrappingFunctions.hxx>

using namespace OT;

namespace PERSALYS
{

CLASSNAMEINIT(PythonScriptEvaluation)

static Factory<PythonScriptEvaluation> Factory_PythonScriptEvaluation;

/* Default constructor */
PythonScriptEvaluation::PythonScriptEvaluation()
  : EvaluationImplementation()
  , scriptHasBeenEvaluated_(false)
  , inputDimension_(0)
  , outputDimension_(0)
  , code_("")
  , isParallel_(false)
{
}


/* Constructor with parameters */
PythonScriptEvaluation::PythonScriptEvaluation(const Description & inputVariablesNames,
                                               const Description & outputVariablesNames,
                                               const String & code,
                                               const Bool isParallel)
  : EvaluationImplementation()
  , scriptHasBeenEvaluated_(false)
  , inputDimension_(inputVariablesNames.getSize())
  , outputDimension_(outputVariablesNames.getSize())
  , code_(code)
  , isParallel_(isParallel)
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


void PythonScriptEvaluation::resetCallsNumber()
{
  callsNumber_ = 0;
}


/* Operator () */
Point PythonScriptEvaluation::operator() (const Point & inP) const
{
  Point outP;

  InterpreterUnlocker iul;
  PyObject * module = PyImport_AddModule("__main__");// Borrowed reference.
  PyObject * dict = PyModule_GetDict(module);// Borrowed reference.

  // define the script on the first run only to allow to save a state
  if (!scriptHasBeenEvaluated_)
  {
    ScopedPyObjectPointer retValue(PyRun_String(code_.c_str(), Py_file_input, dict, dict));
    handleExceptionTraceback();
    scriptHasBeenEvaluated_ = true;
  }

  callsNumber_.increment();
  PyObject * script = PyDict_GetItemString(dict, "_exec");
  if (script == NULL)
    throw InternalException(HERE) << "no _exec function";

  ScopedPyObjectPointer inputTuple(convert< Point, _PySequence_ >(inP));
  ScopedPyObjectPointer outputList(PyObject_Call(script, inputTuple.get(), NULL));
  handleExceptionTraceback();

  if (getOutputDimension() > 1)
  {
    outP = convert<_PySequence_, Point>(outputList.get());
  }
  else
  {
    Scalar value = convert<_PyFloat_, Scalar>(outputList.get());
    outP = Point(1, value);
  }

  return outP;
}


Sample PythonScriptEvaluation::operator() (const Sample & inS) const
{
  if (!isParallel_)
    return EvaluationImplementation::operator() (inS);

  const UnsignedInteger outDim = getOutputDimension();
  const UnsignedInteger inDim = inS.getDimension();
  const UnsignedInteger size = inS.getSize();

  if (inDim != getInputDimension())
    throw InvalidDimensionException(HERE) << "Sample has incorrect dimension. Got " << inDim << ". Expected " << getInputDimension();

  // build script
  InterpreterUnlocker iul;
  PyObject * module = PyImport_AddModule("__main__");// Borrowed reference.
  PyObject * dict = PyModule_GetDict(module);// Borrowed reference.

  // add input sample variable
  ScopedPyObjectPointer inputSample(PyTuple_New(size));
  for (UnsignedInteger i = 0; i < size; ++ i)
    PyTuple_SetItem(inputSample.get(), i, convert< Point, _PySequence_ >(inS[i]));
  PyDict_SetItemString(dict, "X", inputSample.get());

  // code has to be separate
  String tempDir = Path::CreateTemporaryDirectory("persalys");
  std::ofstream code_file;
  code_file.open(tempDir + Os::GetDirectorySeparator() + "persalys_code.py");
  code_file << code_ << "\n";
  code_file.close();

  OSS oss;
  oss << "import multiprocessing as mp\n";
  oss << "from concurrent.futures import ProcessPoolExecutor, as_completed\n";
  oss << "import os\n";
  oss << "import sys\n";
  oss << "from importlib import reload\n";
  oss << "sys.path.append('" << EscapePath(tempDir) << "')\n";
  oss << "import persalys_code\n";
  oss << "reload(persalys_code)\n";
  oss << "if __name__== '__main__':\n";
  oss << "    if sys.platform == 'win32':\n";
  oss << "        mp.set_executable(os.path.join(sys.exec_prefix, 'python.exe'))\n";
  oss << "    with ProcessPoolExecutor() as executor:\n";
  oss << "        resu = {executor.submit(persalys_code._exec, *x): x for x in X}\n";
  oss << "        for future in as_completed(resu):\n";
  oss << "            try:\n";
  oss << "                y = future.result()\n";
  oss << "            except Exception as exc:\n";
  oss << "                raise Exception('Error when evaluating %r. ' % resu[future])\n";
  if (outDim < 2)
    oss << "        Y = [[task.result()] for task in resu]\n";
  else
    oss << "        Y = [task.result() for task in resu]\n";

  ScopedPyObjectPointer retValue(PyRun_String(oss.str().c_str(), Py_file_input, dict, dict));
  handleExceptionTraceback();

  Path::DeleteTemporaryDirectory(tempDir.c_str());

  PyObject * sampleResult = PyDict_GetItemString(dict, "Y");
  if (sampleResult == NULL)
    throw InternalException(HERE) << "no Y";

  // build output sample
  Sample outputSample(convert<_PySequence_, Sample>(sampleResult));

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
