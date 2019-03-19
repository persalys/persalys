//                                               -*- C++ -*-
/**
 *  @brief PythonScriptFieldFunction implements the evaluation of the models defined by a Python script
 *
 *  Copyright 2015-2019 EDF-Phimeca
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
#include "otgui/PythonScriptFieldFunction.hxx"

#include "otgui/InterpreterUnlocker.hxx"
#include "otgui/PythonEnvironment.hxx"
#include "otgui/BaseTools.hxx"

#include <openturns/PersistentObjectFactory.hxx>
#include <openturns/PythonWrappingFunctions.hxx>

using namespace OT;

namespace OTGUI
{

CLASSNAMEINIT(PythonScriptFieldFunction)

static Factory<PythonScriptFieldFunction> Factory_PythonScriptFieldFunction;

// add conversion method
template <class Sample, class _PySequence_> static inline PyObject * customConvert(Sample);
template <>
inline
PyObject *
customConvert< Sample, _PySequence_ >(Sample inS)
{
  UnsignedInteger size = inS.getSize();
  PyObject * sample = PyTuple_New(size);
  for (UnsignedInteger i = 0; i < size; ++ i)
    PyTuple_SetItem(sample, i, convert< Point, _PySequence_ >(inS[i]));
  return sample;
}

// Temporary template to overwrite the convert template of OT defined in PythonWrappingFunctions.hxx
// We replace throw; with throw InvalidArgumentException(HERE)
// TODO: rm this template with the next OT version
template <class _PySequence_, class Sample> static inline Sample     tempConvert(PyObject * pyObj);
template <>
inline
Sample
tempConvert< _PySequence_, Sample >(PyObject * pyObj)
{
  // Check whether pyObj follows the buffer protocol
  if (PyObject_CheckBuffer(pyObj))
  {
    Py_buffer view;
    if (PyObject_GetBuffer(pyObj, &view, PyBUF_FORMAT | PyBUF_ND | PyBUF_ANY_CONTIGUOUS) >= 0)
    {
      if (view.ndim == 2 &&
          view.itemsize == traitsPythonType<Scalar>::buf_itemsize &&
          view.format != NULL &&
          strcmp(view.format, pyBuf_formats[traitsPythonType<Scalar>::buf_format_idx]) == 0)
      {
        const Scalar* data = static_cast<const Scalar*>(view.buf);
        const UnsignedInteger size = view.shape[0];
        const UnsignedInteger dimension = view.shape[1];
        Sample sample(size, dimension);
        if (PyBuffer_IsContiguous(&view, 'C'))
        {
          // 2-d contiguous array in C notation, we can directly copy memory chunk
          std::copy(data, data + size * dimension, (Scalar *)sample.__baseaddress__());
        }
        else
        {
          for (UnsignedInteger j = 0; j < dimension; ++j)
            for(UnsignedInteger i = 0; i < size; ++i, ++data)
              sample(i, j) = *data;
        }
        PyBuffer_Release(&view);
        return sample;
      }
      PyBuffer_Release(&view);
    }
    else
      PyErr_Clear();
  }

  // use the same conversion function for numpy array/matrix, knowing numpy matrix is not a sequence
  if ( PyObject_HasAttrString(pyObj, const_cast<char *>("shape")) )
  {
    ScopedPyObjectPointer shapeObj(PyObject_GetAttrString( pyObj, "shape" ));
    if ( !shapeObj.get() ) throw InvalidArgumentException(HERE) << "Not a sequence object";

    Indices shape( checkAndConvert< _PySequence_, Indices >( shapeObj.get() ) );
    if ( shape.getSize() == 2 )
    {
      UnsignedInteger size = shape[0];
      UnsignedInteger dimension = shape[1];
      ScopedPyObjectPointer askObj(PyTuple_New(2));
      ScopedPyObjectPointer methodObj(convert< String, _PyString_ >("__getitem__"));
      Sample sample( size, dimension );
      for ( UnsignedInteger i = 0; i < size; ++ i )
      {
        PyTuple_SetItem( askObj.get(), 0, convert< UnsignedInteger, _PyInt_ >(i) );
        for ( UnsignedInteger j = 0; j < dimension; ++ j )
        {
          PyTuple_SetItem( askObj.get(), 1, convert< UnsignedInteger, _PyInt_ >(j) );
          ScopedPyObjectPointer elt(PyObject_CallMethodObjArgs( pyObj, methodObj.get(), askObj.get(), NULL));
          if (elt.get())
          {
            sample( i, j ) = checkAndConvert<_PyFloat_, Scalar>(elt.get());
          }
        }
      }
      return sample;
    }
    else
      throw InvalidArgumentException(HERE) << "Invalid array dimension: " << shape.getSize();
  }
  check<_PySequence_>(pyObj);
  ScopedPyObjectPointer newPyObj(PySequence_Fast( pyObj, "" ));
  if (!newPyObj.get()) throw InvalidArgumentException(HERE) << "Not a sequence object";
  const UnsignedInteger size = PySequence_Fast_GET_SIZE( newPyObj.get() );
  if (size == 0) return Sample();

  // Get dimension of first point
  PyObject * firstPoint = PySequence_Fast_GET_ITEM( newPyObj.get(), 0 );
  check<_PySequence_>( firstPoint );
  ScopedPyObjectPointer newPyFirstObj(PySequence_Fast( firstPoint, "" ));
  const UnsignedInteger dimension = PySequence_Fast_GET_SIZE( newPyFirstObj.get() );
  // Allocate result Sample
  Sample sample( size, dimension );
  for(UnsignedInteger i = 0; i < size; ++i)
  {
    PyObject * pointObj = PySequence_Fast_GET_ITEM( newPyObj.get(), i );
    ScopedPyObjectPointer newPyPointObj(PySequence_Fast( pointObj, "" ));
    if (i > 0)
    {
      // Check that object is a sequence, and has the right size
      check<_PySequence_>( pointObj );

      if (static_cast<UnsignedInteger>(PySequence_Fast_GET_SIZE( newPyPointObj.get() )) != dimension)
        throw InvalidArgumentException(HERE) << "The sequences must have the same size";
    }
    for(UnsignedInteger j = 0; j < dimension; ++j)
    {
      PyObject * value = PySequence_Fast_GET_ITEM( newPyPointObj.get(), j );
      sample(i, j) = checkAndConvert<_PyFloat_, Scalar>(value);
    }
  }
  return sample;
}

template <class Sample>
static inline
Collection<Sample> *
tempBuildCollectionFromPySequence(PyObject * pyObj, int sz = 0)
{
  check<_PySequence_>(pyObj);
  ScopedPyObjectPointer newPyObj(PySequence_Fast( pyObj, "" ));
  if (!newPyObj.get()) throw InvalidArgumentException(HERE) << "Not a sequence object";
  const UnsignedInteger size = PySequence_Fast_GET_SIZE( newPyObj.get() );
  if ((sz != 0) && (sz != (int)size))
  {
    throw InvalidArgumentException(HERE) << "Sequence object has incorrect size " << size << ". Must be " << sz << ".";
  }
  Collection<Sample> * p_coll = new Collection< Sample >( size );

  for(UnsignedInteger i = 0; i < size; ++i)
  {
    PyObject * elt = PySequence_Fast_GET_ITEM( newPyObj.get(), i );
    try
    {
      check<_PySequence_>( elt );
    }
    catch (InvalidArgumentException &)
    {
      delete p_coll;
      throw;
    }
    (*p_coll)[i] = tempConvert< _PySequence_, Sample >( elt );
  }

  return p_coll;
}


/* Default constructor */
PythonScriptFieldFunction::PythonScriptFieldFunction()
  : PointToFieldFunctionImplementation()
  , scriptHasBeenEvaluated_(false)
  , outputIndices_()
  , outputModelDimension_(0)
  , code_("")
  , isParallel_(false)
{
}


/* Constructor with parameters */
PythonScriptFieldFunction::PythonScriptFieldFunction(const Description& inputVariablesNames,
                                               const Description& outputVariablesNames,
                                               const Indices & outputIndices,
                                               const UnsignedInteger outputModelDimension,
                                               const String& code,
                                               const Mesh& mesh,
                                               const Bool isParallel)
  : PointToFieldFunctionImplementation(inputVariablesNames.getSize(), mesh, outputIndices.getSize())
  , scriptHasBeenEvaluated_(false)
  , outputIndices_(outputIndices)
  , outputModelDimension_(outputModelDimension)
  , code_(code)
  , isParallel_(isParallel)
{
  // check
  if (!outputIndices_.check(outputModelDimension))
    throw InvalidArgumentException(HERE) << "The indices of a marginal function must be in the range [0, dim-1] and must be different";
  if (outputIndices.getSize() != outputVariablesNames.getSize())
    throw InvalidArgumentException(HERE) << "List of output names and the list of indices must have the same dimension";

  setInputDescription(inputVariablesNames);
  setOutputDescription(outputVariablesNames);

  // create python variable : mesh
  InterpreterUnlocker iul;
  PyObject * module = PyImport_AddModule("__main__");// Borrowed reference.
  PyObject * dict = PyModule_GetDict(module);// Borrowed reference.
  PyObject * meshVerticesTuple(customConvert< Sample, _PySequence_ >(mesh.getVertices()));
  PyModule_AddObject(module, "mesh_vertices_sample_converted_in_Python_Object", meshVerticesTuple);
  const String meshCode("import openturns as ot\ndef getMesh():\n    return ot.Mesh(mesh_vertices_sample_converted_in_Python_Object)\n");
  ScopedPyObjectPointer retValue(PyRun_String(meshCode.c_str(), Py_file_input, dict, dict));
  handleException();
}


/* Virtual constructor */
PythonScriptFieldFunction* PythonScriptFieldFunction::clone() const
{
  PythonScriptFieldFunction * result = new PythonScriptFieldFunction(*this);
  return result;
}


/* Comparison operator */
Bool PythonScriptFieldFunction::operator ==(const PythonScriptFieldFunction & other) const
{
  return (code_ == other.code_);
}


void PythonScriptFieldFunction::resetCallsNumber()
{
  callsNumber_ = 0;
}


/* Operator () */
Sample PythonScriptFieldFunction::operator() (const Point & inP) const
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

  callsNumber_.increment();
  PyObject * script = PyDict_GetItemString(dict, "_exec");
  if (script == NULL)
    throw InternalException(HERE) << "no _exec function";

  ScopedPyObjectPointer inputTuple(convert< Point, _PySequence_ >(inP));
  ScopedPyObjectPointer outputList(PyObject_Call(script, inputTuple.get(), NULL));
  handleExceptionTraceback();

  Sample outS(getOutputMesh().getVerticesNumber(), outputModelDimension_);
  if (outputModelDimension_ > 1)
  {
    Sample outS_t;
    try
    {
      outS_t = tempConvert<_PySequence_, Sample>(outputList.get());
      if (outS_t.getDimension() != getOutputMesh().getVerticesNumber() || outS_t.getSize() != outputModelDimension_)
        throw InvalidDimensionException(HERE);
    }
    catch (std::exception& e)
    {
      throw InvalidArgumentException(HERE) << "The Python Function must return " << outputModelDimension_ << " sequence objects of size equal to the mesh vertices number " << getOutputMesh().getVerticesNumber();
    }

    for (UnsignedInteger i = 0; i < outS.getSize(); ++i)
      for (UnsignedInteger j = 0; j < outS.getDimension(); ++j)
        outS(i, j) = outS_t(j, i);
  }
  else
  {
    Point outP;
    try
    {
      outP = convert<_PySequence_, Point>(outputList.get());
      if (outP.getSize() != getOutputMesh().getVerticesNumber())
        throw InvalidDimensionException(HERE);
    }
    catch (std::exception& e)
    {
      throw InvalidArgumentException(HERE) << "The Python Function must return a sequence object of size equal to the mesh vertices number " << getOutputMesh().getVerticesNumber();
    }

    for (UnsignedInteger i = 0; i < outP.getDimension(); ++i)
      outS(i, 0) = outP[i];
  }

  outS = outS.getMarginal(outputIndices_);
  outS.setDescription(getOutputDescription());

  return outS;
}


ProcessSample PythonScriptFieldFunction::operator() (const Sample & inS) const
{
  if (!isParallel_)
    return PointToFieldFunctionImplementation::operator() (inS);

  const UnsignedInteger inDim = inS.getDimension();
  const UnsignedInteger size = inS.getSize();

  if (inDim != getInputDimension())
    throw InvalidDimensionException(HERE) << "Input sample has incorrect dimension. Got " << inDim << ". Expected inputs number " << getInputDimension();

  // build script
  InterpreterUnlocker iul;
  PyObject * module = PyImport_AddModule("__main__");// Borrowed reference.
  PyObject * dict = PyModule_GetDict(module);// Borrowed reference.

  OSS oss;
  oss << code_ << "\n";
  oss << "from concurrent.futures import ThreadPoolExecutor, as_completed\n";
  oss << "import openturns as ot\n";
  oss << "def _exec_sample():\n";
  oss << "    X = " << Parameters::GetOTSampleStr(inS) << "\n";
  oss << "    with ThreadPoolExecutor() as executor:\n";
  oss << "        resu = {executor.submit(_exec, *x): x for x in X}\n";
  oss << "        for future in as_completed(resu):\n";
  oss << "            try:\n";
  oss << "                y = future.result()\n";
  oss << "            except Exception as exc:\n";
  oss << "                raise Exception('Error when evaluating %r. ' % resu[future])\n";
  if (outputModelDimension_ < 2)
    oss << "        Y = [[task.result()] for task in resu]\n";
  else
    oss << "        Y = [task.result() for task in resu]\n";
  oss << "    return Y\n";

  ScopedPyObjectPointer retValue(PyRun_String(oss.str().c_str(), Py_file_input, dict, dict));
  handleExceptionTraceback();

  PyObject * script = PyDict_GetItemString(dict, "_exec_sample");
  if (script == NULL)
    throw InternalException(HERE) << "no _exec_sample function";

  // run _exec_sample
  ScopedPyObjectPointer sampleResult(PyObject_CallObject(script, NULL));
  handleExceptionTraceback();

  // build output sample
  Pointer< Collection< Sample > > ptr;
  try
  {
    ptr = tempBuildCollectionFromPySequence< Sample >(sampleResult.get());
  }
  catch (std::exception& e)
  {
    throw InvalidArgumentException(HERE) << "The Python Function must return " << outputModelDimension_ << " sequence objects of size equal to the mesh vertices number " << getOutputMesh().getVerticesNumber();
  }

  // check
  if (ptr->getSize() != size)
    throw InvalidArgumentException(HERE) << "The Python Function returned a sequence object with incorrect size (got "
                                          << ptr->getSize() << ", expected input sample size " << size << ")";

  ProcessSample outPS(getOutputMesh(), ptr->getSize(), outputModelDimension_);
  for (UnsignedInteger k = 0; k < ptr->getSize(); ++k)
  {
    Sample outS_k(ptr->at(k));
    // check
    if (outS_k.getDimension() != getOutputMesh().getVerticesNumber())
      throw InvalidArgumentException(HERE) << "The Python Function returned a sequence object with incorrect size (got "
                                            << outS_k.getDimension() << ", expected mesh vertices number " << getOutputMesh().getVerticesNumber() << ")";
    if (outS_k.getSize() != outputModelDimension_)
      throw InvalidArgumentException(HERE) << "The Python Function returned a sequence object with incorrect dimension (got "
                                            << outS_k.getSize() << ", expected outputs number " << outputModelDimension_ << ")";

    Sample outS(outS_k.getDimension(), outS_k.getSize());
    for (UnsignedInteger i = 0; i < outS.getSize(); ++i)
      for (UnsignedInteger j = 0; j < outS.getDimension(); ++j)
        outS(i, j) = outS_k(j, i);

    outS = outS.getMarginal(outputIndices_);
    outS.setDescription(getOutputDescription());
    outPS[k] = outS;
  }

  callsNumber_.fetchAndAdd(size);

  return outPS;
}
}
