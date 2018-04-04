//                                               -*- C++ -*-
/**
 *  @brief PythonScriptEvaluation implements the evaluation of the models defined by a Python script
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
#include "otgui/PythonScriptEvaluation.hxx"

#include "otgui/InterpreterUnlocker.hxx"

#include <openturns/PersistentObjectFactory.hxx>
#include <openturns/PythonWrappingFunctions.hxx>

#ifdef HAVE_BOOST_PROCESS
#include <boost/process.hpp>
#endif
#include <boost/filesystem.hpp>

using namespace OT;

namespace OTGUI
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


void PythonScriptEvaluation::resetCallsNumber()
{
  callsNumber_ = 0;
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
    PyObject * module = PyImport_AddModule("__main__");// Borrowed reference.
    PyObject * dict = PyModule_GetDict(module);// Borrowed reference.

    // define the script on the first run only to allow to save a state
    if (!scriptHasBeenEvaluated_)
    {
      ScopedPyObjectPointer retValue(PyRun_String(code_.c_str(), Py_file_input, dict, dict));
      handleException();
      scriptHasBeenEvaluated_ = true;
    }

    ++ callsNumber_;
    PyObject * script = PyDict_GetItemString(dict, "_exec");
    if (script == NULL)
      throw InternalException(HERE) << "no _exec function";

    ScopedPyObjectPointer inputTuple(convert< Point, _PySequence_ >(inP));
    ScopedPyObjectPointer outputList(PyObject_Call(script, inputTuple.get(), NULL));
    handleException();

    if (getOutputDimension() > 1)
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


Sample PythonScriptEvaluation::operator() (const Sample & inS) const
{
  // no multiprocessing if no boost process include files
#ifndef HAVE_BOOST_PROCESS
  return EvaluationImplementation::operator() (inS);
#else
  if (!isParallel_)
    return EvaluationImplementation::operator() (inS);

  const UnsignedInteger outDim = getOutputDimension();
  const UnsignedInteger inDim = inS.getDimension();
  const UnsignedInteger size = inS.getSize();

  if (inDim != getInputDimension())
    throw InvalidDimensionException(HERE) << "Sample has incorrect dimension. Got " << inDim << ". Expected " << getInputDimension();

  // get input sample string
  OSS sampleOss;
  sampleOss << "[";
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    Point pt(inS[i]);
    sampleOss << "[";
    std::copy(pt.begin(), pt.end(), OSS_iterator<Scalar>(sampleOss, ","));
    sampleOss << "],";
  }
  sampleOss << "]";

  // get temporary directory path
  boost::filesystem::path tempDir = boost::filesystem::temp_directory_path() / boost::filesystem::unique_path("otgui-%%%%-%%%%-%%%%-%%%%");
  bool ret = boost::filesystem::create_directories(tempDir);
  if (!ret)
    throw InvalidArgumentException(HERE) << "Impossible to create a temporary directory. Do not use multiprocessing.\n";

  // set temporary file path to save the evaluation result
  const boost::filesystem::path tempResuFilePath = tempDir / "outputSample.csv";

  // define the Python script
  OSS ossEssai;
  ossEssai << "#!/usr/bin/env python\n";
  ossEssai << "# coding: utf-8\n";

  ossEssai << "from multiprocessing import Pool\n";
  ossEssai << "import openturns as ot\n";
  ossEssai << "import traceback\n";
  ossEssai << "import sys\n";

  ossEssai << code_.c_str();

  ossEssai << "def tmp_exec(x):\n";
  ossEssai << "    try:\n";
  ossEssai << "        return _exec(*x)\n";
  ossEssai << "    except Exception as e:\n";
  ossEssai << "        sys.stderr.write('Caught exception in worker thread (x = %s):\\n' % x)\n";
  ossEssai << "        # This prints the type, value, and stack trace of the\n";
  ossEssai << "        # current exception being handled.\n";
  ossEssai << "        traceback.print_exc()\n";
  ossEssai << "        raise e\n";

  ossEssai << "def _exec_sample(X):\n";
  ossEssai << "    p = Pool(" << size << ")\n";
  ossEssai << "    try:\n";
  ossEssai << "        Y = list(p.imap(tmp_exec, X))\n";
  ossEssai << "        p.close()\n";
  ossEssai << "        p.join()\n";
  ossEssai << (outDim > 1 ? "        return Y\n" : "        return [[y] for y in Y]\n");
  ossEssai << "    except Exception as e:\n";
  ossEssai << "        p.close()\n";
  ossEssai << "        p.join()\n";
  ossEssai << "        raise e\n";

  ossEssai << "if __name__ == '__main__':\n";

  ossEssai << "  X = " << sampleOss.str() << "\n";
  ossEssai << "  sample = ot.Sample(_exec_sample(X))\n";
  ossEssai << "  sample.exportToCSVFile('" << tempResuFilePath.string() << "')\n";

  // create the Python script file
  const boost::filesystem::path tempScriptPath = tempDir / "evaluatePythonModel.py";
  std::ofstream myfile;
  myfile.open(tempScriptPath.string());
  myfile << ossEssai.str();
  myfile.close();

  // get Python executable path
  // this searches a Python executable in the directories of the "PATH" environment variable
  const String pythonExePath = boost::process::search_path("python").string();
  if (pythonExePath.empty())
    throw InvalidArgumentException(HERE) << "Impossible to find a Python executable. Do not use multiprocessing.\n";

  // launch Python script in a subprocess
  boost::process::ipstream error;
  boost::process::child subprocess(pythonExePath, tempScriptPath.string(), boost::process::std_err > error);

  // retrieve error message
  String errorMessage;
  String line;
  while (subprocess.running() && std::getline(error, line) && !line.empty())
    errorMessage += line + "\n";

  subprocess.wait();

  // handle exception
  if (!errorMessage.empty())
  {
    // remove script file
    boost::filesystem::remove_all(tempDir);
    throw InvalidArgumentException(HERE) << "Python function evaluation failed.\n" << errorMessage;
  }

  // retrieve output sample
  Sample outputSample(Sample::ImportFromCSVFile(tempResuFilePath.string()));

  // remove script file + result file
  boost::filesystem::remove_all(tempDir);

  // check
  if (outputSample.getSize() != size)
    throw InvalidArgumentException(HERE) << "Python Function returned a sequence object with incorrect size (got "
                                          << outputSample.getSize() << ", expected " << size << ")";
  if (outputSample.getDimension() != outDim)
    throw InvalidArgumentException(HERE) << "Python Function returned a sequence object with incorrect dimension (got "
                                          << outputSample.getDimension() << ", expected " << outDim << ")";


  outputSample.setDescription(getOutputDescription());
  callsNumber_ += size;

  return outputSample;
#endif
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
