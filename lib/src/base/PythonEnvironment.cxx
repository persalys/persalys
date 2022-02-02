//                                               -*- C++ -*-
/**
 *  @brief Python environment handling
 *
 *  Copyright 2015-2022 EDF-Phimeca
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
#include "persalys/PythonEnvironment.hxx"

#include <openturns/PythonWrappingFunctions.hxx>

using namespace OT;

namespace PERSALYS
{

PythonEnvironment::PythonEnvironment()
{
  // Py_Initialize should be done by PyInterp_Interp
  Py_Initialize();
#if PY_VERSION_HEX < 0x03070000
  PyEval_InitThreads(); // Create (and acquire) the Python global interpreter lock (GIL)
#endif
  // avoid error: "Exception ignored in: <module 'threading' from '/usr/lib/python3.6/threading.py'>" when closing the interface if the user used parallelisation
  Py_DECREF(PyImport_ImportModule("threading"));
  PyEval_SaveThread(); /* Release the thread state */
  //here we do not have the Global Interpreter Lock
}


PythonEnvironment::~PythonEnvironment()
{
  PyGILState_Ensure();
  Py_Finalize();
}


void handleExceptionTraceback()
{
  PyObject * exception = PyErr_Occurred();

  if (exception)
  {
    PyObject *ptype = NULL, *pvalue = NULL, *ptraceback = NULL;
    PyErr_Fetch(&ptype, &pvalue, &ptraceback);
    PyErr_NormalizeException(&ptype, &pvalue, &ptraceback);

    String exceptionMessage;

    if (ptraceback)
    {
      // See if we can get a full traceback
      ScopedPyObjectPointer pyth_module(PyImport_ImportModule("traceback"));

      ScopedPyObjectPointer pyth_func(PyObject_GetAttrString(pyth_module.get(), "format_exception"));
      if (pyth_func.get() && PyCallable_Check(pyth_func.get()))
      {
        ScopedPyObjectPointer pyth_val(PyObject_CallFunctionObjArgs(pyth_func.get(), ptype, pvalue, ptraceback, NULL));

        if (pyth_val.get())
        {
          // The return value of format_exception() is a list of strings
          const Description desc = checkAndConvert< _PySequence_, Description >(pyth_val.get());
          exceptionMessage += "Python exception:\n";
          for (UnsignedInteger i = 0; i < desc.getSize(); i++)
          {
            exceptionMessage += desc[i];
          }
        }
      }
    }
    if (!ptraceback || exceptionMessage.empty())
    {
      exceptionMessage = "Python exception";
      // get the name of the exception
      if (ptype)
      {
        ScopedPyObjectPointer nameObj(PyObject_GetAttrString(ptype, "__name__"));
        if (nameObj.get())
        {
          String typeString = checkAndConvert< _PyString_, String >(nameObj.get());
          exceptionMessage += ": " + typeString;
        }
      }

      // try to get error msg, value and traceback can be NULL
      if (pvalue)
      {
        ScopedPyObjectPointer valueObj(PyObject_Str(pvalue));
        if (valueObj.get())
        {
          String valueString = checkAndConvert< _PyString_, String >(valueObj.get());
          exceptionMessage += ": " + valueString;
        }
      }
    }

    PyErr_Restore(ptype, pvalue, ptraceback);
    PyErr_Print();
    throw InternalException(HERE) << exceptionMessage;
  }
}
}
