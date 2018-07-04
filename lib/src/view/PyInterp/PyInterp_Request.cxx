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

#include "PyInterp_Request.h"
#include "PyInterp_Utils.h"

#include <QCoreApplication>

void PyInterp_Request::process()
{
  safeExecute();

  bool isSync = IsSync();

  if ( !isSync )
    myMutex.lock();

  if ( listener() )
    processEvent( listener() );

  if ( !isSync )
    myMutex.unlock();
}

void PyInterp_Request::safeExecute()
{
  execute();
}

void PyInterp_Request::Destroy( PyInterp_Request* request )
{
  // Lock and unlock the mutex to avoid errors on its deletion
  request->myMutex.lock();
  request->myMutex.unlock();
  delete request;
}

QEvent* PyInterp_Request::createEvent()
{
  return new PyInterp_Event( PyInterp_Event::ES_NOTIFY, this );
}

void PyInterp_Request::processEvent( QObject* o )
{
  if ( !o )
    return;

  QEvent* e = createEvent();
  if ( !e )
    return;

  if ( !IsSync() )
    QCoreApplication::postEvent( o, e );
  else
  {
    QCoreApplication::sendEvent( o, e );
    delete e;
  }
}

void PyInterp_Request::setListener( QObject* o )
{
  myMutex.lock();
  myListener = o;
  myMutex.unlock();
}

void PyInterp_LockRequest::safeExecute()
{
  PyLockWrapper aLock; // Acquire GIL
  execute();
}
