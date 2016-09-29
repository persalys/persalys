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
//  File   : PyInterp_Interp.h
//  Author : Christian CAREMOLI, Paul RASCLE, Adrien BRUNETON

#ifndef PYINTERP_INTERP_H
#define PYINTERP_INTERP_H

#include "PyInterp.h"   // !!! WARNING !!! THIS INCLUDE MUST BE THE VERY FIRST !!!
#include "PyInterp_Utils.h"

#include <list>
#include <string>

typedef void PyOutChanged(void* data,char * c);

typedef struct {
  PyObject_HEAD
  int softspace;
  PyOutChanged* _cb;
  void* _data;
  bool _iscerr;
} PyStdOut;

/**
 * Main class representing a *virtual* Python interpreter. There is really only one true
 * Python interpreter in the whole application (no call to Py_NewInterpreter),
 * but the use of different execution contexts allow
 * to split the execution lines, and hence to emulate (relatively) independent interpreters.
 * This has some consequences: modules imported in one context are not re-imported in another context
 * (only there namespace is made available when importing in another context).
 * See also class PyConsole_Interp.
 */
class PYINTERP_EXPORT PyInterp_Interp
{
public:
  static int _argc;
  static char* _argv[];
  
  PyInterp_Interp();
  virtual ~PyInterp_Interp();
  
  void initialize();
  void destroy();

  virtual int run(const char *command); 
  virtual void initStudy() {};

  std::string getBanner() const;
  void setverrcb(PyOutChanged*, void*);
  void setvoutcb(PyOutChanged*, void*);

  const char* getPrevious();
  const char* getNext();

protected:
  /** Redirection of stdout and stderr */
  PyObject* _vout;
  PyObject* _verr;
  /** Execution context (local and global variables) */
  PyObject* _global_context;
  PyObject* _local_context;

  std::list<std::string> _history;
  std::list<std::string>::iterator _ith;

  virtual int beforeRun();
  virtual int afterRun();
  int simpleRun(const char* command, const bool addToHistory = true);

  virtual void initPython();

  /** Initialize execution context. */
  virtual bool initContext();
  virtual bool initRun();
  virtual void closeContext();

  bool initialized() const;

private:
  bool _initialized;
};

#endif // PYINTERP_INTERP_H
