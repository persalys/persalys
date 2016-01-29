// PythonEvaluation.cxx

#include "otgui/PythonEvaluation.hxx"

#include "PersistentObjectFactory.hxx"
#include "PythonWrappingFunctions.hxx"

using namespace OT;

namespace OTGUI {

CLASSNAMEINIT(PythonEvaluation);

static Factory<PythonEvaluation> RegisteredFactory("PythonEvaluation");

PythonEvaluation::PythonEvaluation()
  : NumericalMathEvaluationImplementation()
  , inputDimension_(0)
  , outputDimension_(0)
{
}


/* Default constructor */
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

    PyObject *module = PyImport_AddModule("__main__");// Borrowed reference.
    PyObject *dict = PyModule_GetDict(module);// Borrowed reference.
    ScopedPyObjectPointer retValue(PyRun_String(code_.c_str(), Py_file_input, dict, dict));
    handleException();
    PyObject *script = PyDict_GetItemString(dict, "_exec");
    if (script == NULL) throw InternalException(HERE) << "no exec_ function";

    ScopedPyObjectPointer inputList(PyList_New(inputDimension_));
    for (UnsignedInteger i = 0; i < inputDimension_; ++ i)
    {
      PyObject *outputItem = PyFloat_FromDouble(inP[i]);// new reference
      PyList_SetItem(inputList.get(), i, outputItem);// steals reference
    }
    ScopedPyObjectPointer inputTuple(PyTuple_New(1));
    PyTuple_SetItem(inputTuple.get(), 0, inputList.get());

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