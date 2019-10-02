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
//  File   : PyInterp_Dispatcher.cxx
//  Author : Sergey Anikin (OPEN CASCADE S.A.S.)

#include "PyInterp_Dispatcher.h"   // !!! WARNING !!! THIS INCLUDE MUST BE THE VERY FIRST !!!

/**
   \class PyInterp_Dispatcher
   \brief Dispatcher of Python events; used to serialize requests to Python interpreter.
*/

PyInterp_Dispatcher* PyInterp_Dispatcher::myInstance = 0;

PyInterp_Dispatcher* PyInterp_Dispatcher::Get()
{
  if ( !myInstance )
    myInstance = new PyInterp_Dispatcher();
  return myInstance;
}

PyInterp_Dispatcher::PyInterp_Dispatcher() 
: QThread()
{
}

PyInterp_Dispatcher::~PyInterp_Dispatcher()
{
  // Clear the request queue
  myQueueMutex.lock();

  QListIterator<RequestPtr> it( myQueue );
  while ( it.hasNext() )
    PyInterp_Request::Destroy( it.next() );
  myQueue.clear();

  myQueueMutex.unlock();

  // Wait for run() to finish
  wait();
}

bool PyInterp_Dispatcher::IsBusy() const
{
  return isRunning();
}

void PyInterp_Dispatcher::Exec( PyInterp_Request* theRequest )
{
  if ( !theRequest )
    return;

  if ( theRequest->IsSync() /*&& !IsBusy()*/)
  {
    // synchronous processing
    processRequest( theRequest );
  }
  else
  {
    // asynchronous processing
    myQueueMutex.lock();

    myQueue.enqueue( theRequest );
    if ( theRequest->listener() ) {
      connect( theRequest->listener(), SIGNAL( destroyed( QObject* ) ),
               this, SLOT( objectDestroyed( QObject* ) ) );
    }

    myQueueMutex.unlock();  

    if ( !IsBusy() )
      start();
  }
}

void PyInterp_Dispatcher::run()
{
  PyInterp_Request* aRequest;

  // prepare for queue size check
  myQueueMutex.lock();

  while ( myQueue.size() )
  {
    aRequest = myQueue.head();

    // let other threads append their requests to the end of the queue
    myQueueMutex.unlock();

    // processRequest() may delete a request, so this pointer must not be used
    // after request is processed!
    processRequest( aRequest );

    // prepare for removal of the first request in the queue
    myQueueMutex.lock();
  
    // IMPORTANT: the first item could have been removed by objectDestroyed() --> we have to check it
    if ( myQueue.head() == aRequest ) // if it is still here --> remove it
      myQueue.dequeue();
  }

  myQueueMutex.unlock();
}

void PyInterp_Dispatcher::processRequest( PyInterp_Request* theRequest )
{
  theRequest->process();
}

void PyInterp_Dispatcher::objectDestroyed( QObject* o )
{
  // prepare for modification of the queue
  myQueueMutex.lock();

  QMutableListIterator<RequestPtr> it( myQueue );
  while ( it.hasNext() )
  {
    RequestPtr r = it.next();
    if ( o == r->listener() )
    {
      r->setListener( 0 ); // to prevent event posting
      it.remove();
    }
  }

  myQueueMutex.unlock();
}
