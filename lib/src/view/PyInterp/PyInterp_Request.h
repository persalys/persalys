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
//  File   : PyInterp_Request.h
//  Author : Sergey Anikin (OPEN CASCADE S.A.S.), Adrien Bruneton (CEA/DEN)

#ifndef PYINTERP_REQUEST_H
#define PYINTERP_REQUEST_H

#include "PyInterp.h"
#include "PyInterp_Event.h"

#include <QMutex>

class QObject;
class PyInterp_Interp;

/**
   \class PyInterp_Request
   \brief Base Python interpreter request; does not not acquire GIL during execution.
 */
class PYINTERP_EXPORT PyInterp_Request
{
  friend class PyInterp_Dispatcher;

private:
  PyInterp_Request();
  PyInterp_Request( const PyInterp_Request& );

protected:
  // protected destructor - to control deletion of requests
  virtual ~PyInterp_Request() {};

public:
  // Constructor
  PyInterp_Request( QObject* listener, bool sync = true )
    : myIsSync( sync ), myListener( listener ) {};

  // Deletes a request
  static void     Destroy( PyInterp_Request* );

  // Returns true if this request should be processed synchronously,
  // without putting it to a queue
  bool            IsSync() const { return myIsSync; }

protected:
  // Performs safe execution of the request
  virtual void    safeExecute();

  // Should be redefined in successors, contains actual request code
  virtual void    execute() = 0;

  // This method can be overridden to customize notification event creation
  virtual QEvent* createEvent();

  virtual void    processEvent( QObject* );

  // Provide access to the listener of this request
  QObject*        listener() const { return myListener; }
  void            setListener( QObject* );

private:
  // Process request, invoked from Dispatcher
  void            process();

private:
  QMutex          myMutex;
  bool            myIsSync;
  QObject*        myListener;
};

/**
   \class PyInterp_LockRequest
   \brief Python interpreter request; automatically acquires GIL during execution.
 */
class PYINTERP_EXPORT PyInterp_LockRequest : public PyInterp_Request
{
public:
  // Constructor
  PyInterp_LockRequest( PyInterp_Interp* interp, QObject* listener=0, bool sync=true )
    : PyInterp_Request( listener, sync ), myInterp( interp )
  {}

protected:
  // Get interpreter
  PyInterp_Interp*  getInterp() const { return myInterp; }

  // Performs safe execution of the request
  virtual void      safeExecute();

private:
  PyInterp_Interp*  myInterp;
};

#endif // PYINTERP_REQUEST_H
