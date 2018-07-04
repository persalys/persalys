// Copyright (C) 2007-2016  CEA/DEN, EDF R&D, OPEN CASCADE
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
// File   : PyConsole_Request.cxx
// Author : Vadim SANDLER (OPEN CASCADE), Adrien Bruneton (CEA/DEN)

#include "PyConsole_Request.h"
#include "PyConsole_Interp.h"
#include "PyConsole_Event.h"

#include <QCoreApplication>

/*!
  \class PyConsole_ExecCommand
  \brief Python command execution request.
  \internal
*/

/*!
  \brief Constructor.
  
  Creates new python command execution request.

  \param theInterp   python interpreter
  \param theCommand  python command
  \param theListener widget to get the notification messages
  \param theSync     if \c true, the request is processed synchronously
*/
PyConsole_ExecCommand::PyConsole_ExecCommand( PyInterp_Interp*        theInterp,
                                              const QString&          theCommand,
                                              QObject*                theListener,
                                              bool                    theSync )
  : PyInterp_LockRequest( theInterp, theListener, theSync ),
    myCommand( theCommand ), myState( PyInterp_Event::ES_OK )
{}

/*!
  \brief Execute the python command in the interpreter and
  get its execution status.
*/
void PyConsole_ExecCommand::execute()
{
  if ( myCommand != "" ) {
    int ret = getInterp()->run( myCommand.toLatin1().data() );
    if ( ret < 0 )
      myState = PyInterp_Event::ES_ERROR;
    else if ( ret > 0 )
      myState = PyInterp_Event::ES_INCOMPLETE;
  }
}

/*!
  \brief Create and return a notification event.
  \return new notification event
*/
QEvent* PyConsole_ExecCommand::createEvent()
{
  if ( IsSync() )
    QCoreApplication::sendPostedEvents( listener(), PyConsole_PrintEvent::EVENT_ID );
  return new PyInterp_Event( myState, this );
}

/*!
  \class PyConsole_CompletionCommand
  \brief Python command completion request.
  \internal
*/

/*!
  \brief Constructor.

  Creates a new python completion request.

  \param theInterp python interpreter
  \param theInput  string containing the dir() command to be executed
  \param theStartMatch part to be matched with the results of the dir() command
  \param theListener widget to get the notification messages
  \param theSync if \c true the request is processed synchronously
*/
PyConsole_CompletionCommand::PyConsole_CompletionCommand( PyInterp_Interp*   theInterp,
                                                          const QString&     theInput,
                                                          const QString&     theStartMatch,
                                                          QObject*           theListener,
                                                          bool               theSync )
  : PyInterp_LockRequest( theInterp, theListener, theSync ),
    myDirArg( theInput ), myStartMatch( theStartMatch ), myStatus( false )
{}

/*!
  \brief Execute the completion command by invoking runDirCommand() function
  of interpreter.
*/
void PyConsole_CompletionCommand::execute()
{
  myStatus = static_cast<PyConsole_Interp*>( getInterp() )->runDirCommand( myDirArg,  myStartMatch, myMatches, myDoc );
}

/*!
 \brief Create and return completion event
  \return new completion event
 */
QEvent* PyConsole_CompletionCommand::createEvent()
{
  return new PyConsole_CompletionEvent( this, myStatus, myMatches, myDoc );
}
