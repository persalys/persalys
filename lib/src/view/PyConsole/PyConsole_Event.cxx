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
// File   : PyConsole_Event.cxx
// Author : Vadim SANDLER (Open CASCADE S.A.S), Adrien Bruneton (CEA/DEN)

#include "PyConsole_Event.h"

/*!
  \class PyConsole_PrintEvent
  \brief Python command output backend event.
  \internal
*/

/*!
  \brief Constructor
  \param message message text (python trace)
  \param isError default to \c false - if \c true indicates that an error is being printed.
*/
PyConsole_PrintEvent::PyConsole_PrintEvent( const QString& message, bool isError )
  : QEvent( (QEvent::Type)EVENT_ID ), myText( message ), myError( isError )
{
}

/*!
  \brief Get message
  \return message text (python trace)
*/
QString PyConsole_PrintEvent::text() const
{
  return myText;
}

/*!
  \brief Get error flag
  \return \c true if this is an error message
*/
bool PyConsole_PrintEvent::isError() const
{
  return myError;
}

/*!
  \class PyConsole_CompletionEvent
  \brief Python command completion event.
  \internal
*/

/*!
  \brief Constructor
  \param request python request
  \param s status of execution of completion command
  \param ms command matches (completions)
  \param d docstring of the match (in case if there is sinlge match)
*/
PyConsole_CompletionEvent::PyConsole_CompletionEvent( PyInterp_Request* request,
                                                      bool s,
                                                      const QStringList& ms,
                                                      const QString& d )
  : PyInterp_Event( (QEvent::Type)EVENT_ID, request ),
    myStatus( s ), myMatches( ms ), myDoc( d )
{}

/*!
  \brief Get status of execution of completion command
  \return execution status
*/
bool PyConsole_CompletionEvent::status() const
{
  return myStatus;
}

/*!
  \brief Get matches (completions)
  \return detected command matches (completions)
*/
QStringList PyConsole_CompletionEvent::matches() const
{
  return myMatches;
}

/*!
  \brief Get docstring
  \return docstring of the match (in case if there is sinlge match)
*/
QString PyConsole_CompletionEvent::doc() const
{
  return myDoc;
}
