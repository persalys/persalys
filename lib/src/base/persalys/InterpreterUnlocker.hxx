//                                               -*- C++ -*-
/**
 *  @brief Python lock handling
 *
 *  Copyright 2015-2021 EDF-Phimeca
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
#ifndef PERSALYS_INTERPRETERUNLOCKER_HXX
#define PERSALYS_INTERPRETERUNLOCKER_HXX

#include "persalys/PersalysPrivate.hxx"
#include <Python.h>

namespace PERSALYS
{

class InterpreterUnlocker
{
public:
  InterpreterUnlocker()
  {
    gstate_ = PyGILState_Ensure();
  }
  ~InterpreterUnlocker()
  {
    PyGILState_Release(gstate_);
  }
private:
  PyGILState_STATE gstate_;
};

}

#endif


