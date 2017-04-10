//                                               -*- C++ -*-
/**
 *  @brief Python environment handling 
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
#include "otgui/PythonEnvironment.hxx"
#include <Python.h>
#include "otgui/PythonEvaluation.hxx"

namespace OTGUI {

PythonEnvironment::PythonEnvironment()
{
  // Py_Initialize should be done by PyInterp_Interp
  Py_Initialize();
  PyEval_InitThreads(); // Create (and acquire) the Python global interpreter lock (GIL)
  PyEval_SaveThread(); /* Release the thread state */
  //here we do not have the Global Interpreter Lock
}

PythonEnvironment::~PythonEnvironment()
{
  PyGILState_Ensure();
  Py_Finalize();
}

}
