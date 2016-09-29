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
// File   : PyConsole_Request.h
// Author : Vadim SANDLER (OPEN CASCADE), Adrien Bruneton (CEA/DEN)

#ifndef PYCONSOLE_REQUEST_H
#define PYCONSOLE_REQUEST_H

#include "PyInterp_Request.h"

#include <QString>
#include <QStringList>

class QEvent;
class PyInterp_Interp;

class PyConsole_ExecCommand : public PyInterp_LockRequest
{
public:
  PyConsole_ExecCommand( PyInterp_Interp*, const QString&, QObject*, bool = false );

protected:
  virtual void execute();
  virtual QEvent* createEvent();

private:
  QString myCommand;   //!< Python command
  int     myState;     //!< Python command execution status
};

class PyConsole_CompletionCommand : public PyInterp_LockRequest
{
public:
  PyConsole_CompletionCommand( PyInterp_Interp*, const QString&, const QString&, QObject*, bool = false );

protected:
  virtual void execute();
  virtual QEvent* createEvent();

private:
  QString     myDirArg;       //!< String to be passed to the dir() comman
  QString     myStartMatch;   //!< Begining of the command (as typed by the user)
  bool        myStatus;       //!< Status of completion command execution
  QStringList myMatches;      //!< Matches
  QString     myDoc;          //!< Docstring of single match
};

#endif // PYCONSOLE_REQUEST_H
