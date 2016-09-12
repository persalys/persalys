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
//  File   : PyInterp_Dispatcher.h
//  Author : Sergey Anikin (OPEN CASCADE S.A.S.)

#ifndef PYINTERP_DISPATCHER_H
#define PYINTERP_DISPATCHER_H

#include "PyInterp.h"   // !!! WARNING !!! THIS INCLUDE MUST BE THE VERY FIRST !!!

#include "PyInterp_Request.h"   // full include instead of forward declaration
                                // everyone inc'ing the Dispatcher will get the requests for free.

#include <QMutex>
#include <QThread>
#include <QQueue>

class PYINTERP_EXPORT PyInterp_Dispatcher : protected QThread
{
  PyInterp_Dispatcher(); // private constructor
  Q_OBJECT
public:
  static PyInterp_Dispatcher* Get();

  virtual                     ~PyInterp_Dispatcher();

  bool                        IsBusy() const;  
  void                        Exec( PyInterp_Request* );

private:
  virtual void                run();
  void                        processRequest( PyInterp_Request* );

private slots:
  void                        objectDestroyed( QObject* );

private:
  typedef PyInterp_Request*   RequestPtr;

  QQueue<RequestPtr>          myQueue;
  QMutex                      myQueueMutex;

  static PyInterp_Dispatcher* myInstance;
};

#endif // PYINTERP_DISPATCHER_H
