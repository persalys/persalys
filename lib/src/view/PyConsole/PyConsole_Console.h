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
// File   : PyConsole_Console.h
// Author : Vadim SANDLER, Open CASCADE S.A.S. (vadim.sandler@opencascade.com)

#ifndef PYCONSOLE_CONSOLE_H
#define PYCONSOLE_CONSOLE_H

#include "PyConsole.h"

#include <QWidget>
#include <QMap>

class QMenu;

class PyConsole_Interp;
class PyConsole_Editor;

class PYCONSOLE_EXPORT PyConsole_Console : public QWidget
{
  Q_OBJECT

public:
  //! Context popup menu actions flags
  enum
  {
    CopyId         = 0x01,  //!< "Copy" menu action
    PasteId        = 0x02,  //!< "Paste" menu action
    ClearId        = 0x04,  //!< "Clear" menu action
    SelectAllId    = 0x08,  //!< "Select All" menu action
    DumpCommandsId = 0x10,  //!< "DumpCommands" menu action
    StartLogId     = 0x20,  //!< "Start log" menu action
    StopLogId      = 0x40,  //!< "Stop log" menu action
    All            = 0xFF,  //!< all menu actions 
  };

public:
  PyConsole_Console( QWidget* = 0 );
  PyConsole_Console( QWidget*, PyConsole_Editor* );
  virtual ~PyConsole_Console();

  PyConsole_Interp*   getInterp() const;

  QFont               font() const;
  virtual void        setFont( const QFont& );

  bool                isSync() const;
  void                setIsSync( const bool );

  bool                isSuppressOutput() const;
  void                setIsSuppressOutput( const bool );

  bool                isShowBanner() const;
  void                setIsShowBanner( const bool );

  void                setAutoCompletion( bool );
  bool                autoCompletion() const;

  void                exec( const QString& );
  void                execAndWait( const QString& );

  void                setMenuActions( const int );
  int                 menuActions() const;

  void                startLog( const QString& );
  void                stopLog();

protected:
  void                createActions();
  void                updateActions();

  virtual void        contextMenuEvent( QContextMenuEvent* );

private:
  void                init( PyConsole_Editor* );

protected:
  PyConsole_Editor*   myEditor;    //!< python console editor widget
  QMap<int, QAction*> myActions;   //!< menu actions list
};

#endif // PYCONSOLE_CONSOLE_H
