// Copyright (C) 2007-2016  CEA/DEN, EDF R&D, OPEN CASCADE
//
// Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
// CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//
//  File   : PyInterp_Utils.h
//  Author : Christian CAREMOLI, Paul RASCLE, Adrien BRUNETON

#ifndef PYINTERP_UTILS_H
#define PYINTERP_UTILS_H

#include "PyInterp.h"

#ifdef _DEBUG_
  #include <iostream>
#endif

/**
 * \class PyLockWrapper
 * \brief Python GIL wrapper.
 *
 * Utility class wrapping the Python GIL acquisition. This makes use of the high level
 * API (PyGILState_Ensure and PyGILState_Release), and is hence compatible with only
 * one running Python interpreter (no call to Py_NewInterpreter()).
 * When the class is instanciated the lock is acquired. It is released at destruction time.
 * Copy construction (and hence assignation) is forbidden.
 */
class PYINTERP_EXPORT PyLockWrapper
{
public:
  /**
   * \brief Constructor. Automatically acquires GIL.
   */
  PyLockWrapper()
  {
    _gil_state = PyGILState_Ensure();
    // Save current thread state for later comparison
    _state = PyGILState_GetThisThreadState();
  }

  /**
   * \brief Destructor. Automatically releases GIL.
   */
  ~PyLockWrapper()
  {
    PyThreadState* _currState = PyGILState_GetThisThreadState();
#ifdef _DEBUG_
    if (_currState != _state)
    {
      std::cout << "!!!!!!!!! PyLockWrapper inconsistency - now entering infinite loop for debugging\n";
      while(1);
    }
#endif
    PyGILState_Release(_gil_state);
  }

private:
  PyGILState_STATE _gil_state;
  PyThreadState* _state;

  // "Rule of 3" - Forbid usage of copy operator and copy-constructor
  PyLockWrapper(const PyLockWrapper & another);
  const PyLockWrapper & operator=(const PyLockWrapper & another);
};

/**
 * \class PyObjWrapper
 * \brief Utility class to properly handle the reference counting required on Python objects.
 */
class PYINTERP_EXPORT PyObjWrapper
{
  PyObject* myObject;
public:
  PyObjWrapper(PyObject* theObject) : myObject(theObject) {}
  PyObjWrapper() : myObject(0) {}
  virtual ~PyObjWrapper() { Py_XDECREF(myObject); }

  operator PyObject*()    { return myObject;  }
  PyObject* operator->()  { return myObject;  }
  PyObject* get()         { return myObject;  }
  bool operator!()        { return !myObject; }
  bool operator==(PyObject* theObject) { return myObject == theObject; }
  PyObject** operator&()  { return &myObject; }
  PyObjWrapper& operator=(PyObjWrapper* theObjWrapper)
  {
    Py_XDECREF(myObject);
    myObject = theObjWrapper->myObject;
    Py_XINCREF(myObject);
    return *this;
  }
  PyObjWrapper& operator=(const PyObjWrapper& theObjWrapper)
  {
    Py_XDECREF(myObject);
    myObject = theObjWrapper.myObject;
    Py_XINCREF(myObject);
    return *this;
  }
};

#endif // PYINTERP_UTILS_H
