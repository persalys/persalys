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
// File   : PyConsole_Event.h
// Author : Vadim SANDLER (Open CASCADE S.A.S), Adrien Bruneton (CEA/DEN)

#ifndef PYCONSOLE_EVENT_H
#define PYCONSOLE_EVENT_H

#include "PyConsole.h"
#include "PyInterp_Event.h"

#include <QEvent>
#include <QString>
#include <QStringList>

class PyConsole_PrintEvent : public QEvent
{
public:
  static const int EVENT_ID = 65432;

  PyConsole_PrintEvent( const QString&, bool = false );

  QString text() const;
  bool isError() const;

private:
  QString myText;  //!< Event message (python trace)
  bool    myError; //!< Set to \c true if an error msg is to be displayed
};

class PyConsole_CompletionEvent : public PyInterp_Event
{
public:
  static const int EVENT_ID = 65433;
  
  PyConsole_CompletionEvent( PyInterp_Request*, bool, const QStringList&, const QString& );

  bool status() const;
  QStringList matches() const;
  QString doc() const;

protected:
  bool        myStatus;  //!< Status of execution
  QStringList myMatches; //!< Command matches (completions)
  QString     myDoc;     //!< Docstring of the match (in case if there is sinlge match)
};

#endif // PYCONSOLE_EVENT_H
