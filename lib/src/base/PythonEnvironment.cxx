//                                               -*- C++ -*-
/**
 *  @brief Python environment handling
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
#include "persalys/PythonEnvironment.hxx"
#include "persalys/InterpreterUnlocker.hxx"

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


void PythonEnvironment::runString(const std::string & cmd)
{
  InterpreterUnlocker iul;
  PyObject * module = PyImport_AddModule("__main__");// Borrowed reference.
  PyObject * dict = PyModule_GetDict(module);// Borrowed reference.
  PyRun_String(cmd.c_str(), Py_file_input, dict, dict);
  handleException();
}


PythonEnvironment::~PythonEnvironment()
{
  PyGILState_Ensure();
  Py_Finalize();
}


}
