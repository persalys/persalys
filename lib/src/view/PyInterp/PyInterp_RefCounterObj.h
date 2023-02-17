// Copyright (C) 2007-2022  CEA/DEN, EDF R&D, OPEN CASCADE
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
//  File   : PyInterp_RefCounterObj.h
//  Author : Anthony Geay (EDF R&D)

#ifndef PYINTERP_REFCOUNTEROBJ_H
#define PYINTERP_REFCOUNTEROBJ_H

#include "PyInterp.h"

template<class T>
class PyInterp_Auto
{
public:
  PyInterp_Auto(const PyInterp_Auto& other):_ptr(0) { referPtr(other._ptr); }
  PyInterp_Auto(T *ptr=0):_ptr(ptr) { }
  ~PyInterp_Auto() { destroyPtr(); }
  bool isNull() const { return _ptr==0; }
  bool isNotNull() const { return !isNull(); }
  void nullify() { destroyPtr(); _ptr=0; }
  bool operator==(const PyInterp_Auto& other) const { return _ptr==other._ptr; }
  bool operator==(const T *other) const { return _ptr==other; }
  PyInterp_Auto &operator=(const PyInterp_Auto<T>& other) { if(_ptr!=other._ptr) { destroyPtr(); referPtr(other._ptr); } return *this; }
  PyInterp_Auto &operator=(T *ptr) { if(_ptr!=ptr) { destroyPtr(); _ptr=ptr; } return *this; }
  void takeRef(T *ptr) { if(_ptr!=ptr) { destroyPtr(); _ptr=ptr; if(_ptr) _ptr->incrRef(); } }
  T *operator->() { return _ptr ; }
  const T *operator->() const { return _ptr; }
  T& operator*() { return *_ptr; }
  const T& operator*() const { return *_ptr; }
  operator T *() { return _ptr; }
  operator const T *() const { return _ptr; }
  T *retn() { if(_ptr) _ptr->incrRef(); return _ptr; }
  T *iAmATrollConstCast() const { return _ptr; }
 private:
  void referPtr(T *ptr) { _ptr=ptr; if(_ptr) _ptr->incrRef(); }
  void destroyPtr() { if(_ptr) _ptr->decrRef(); }
 private:
  T *_ptr;
};

class PYINTERP_EXPORT PyInterp_RefCounterObj
{
 protected:
  PyInterp_RefCounterObj():_cnt(1) { }
  PyInterp_RefCounterObj(const PyInterp_RefCounterObj& /*other*/):_cnt(1) { }
 public:
  bool decrRef() const
  {
    bool ret=((--_cnt)==0);
    if(ret)
      delete this;
    return ret;
  }
  void incrRef() const { _cnt++; }
  int getRCValue() const { return _cnt; }
  // copies using operator= should not copy the ref counter of \a other 
  PyInterp_RefCounterObj& operator=(const PyInterp_RefCounterObj& /*other*/) { return *this; }
 protected:
  virtual ~PyInterp_RefCounterObj() { }
 private:
  mutable int _cnt;
};

#endif // PYINTERP_REFCOUNTEROBJ_H
