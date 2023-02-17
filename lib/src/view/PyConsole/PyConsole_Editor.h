// Copyright (C) 2007-2022  CEA/DEN, EDF R&D, OPEN CASCADE
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
// File   : PyConsole_Editor.h
// Author : Vadim SANDLER, Open CASCADE S.A.S. (vadim.sandler@opencascade.com)

#ifndef PYCONSOLE_EDITOR_H
#define PYCONSOLE_EDITOR_H

#include "PyConsole.h"

#include "PyInterp_RefCounterObj.h"

#include <QTextEdit>
#include <QQueue>

class PyConsole_Interp;
class PyInterp_Request;
class QEventLoop;

class PYCONSOLE_EXPORT PyConsole_Editor : public QTextEdit
{
  Q_OBJECT;

public:
  PyConsole_Editor( QWidget* = 0 );
  PyConsole_Editor( QWidget*, PyConsole_Interp* );
  ~PyConsole_Editor();

  PyConsole_Interp* getInterp() const;
  
  virtual void   addText( const QString&, const bool = false, const bool = false );
  bool           isCommand( const QString& ) const;

  virtual void   exec( const QString& );
  void           execAndWait( const QString& );

  bool           isSync() const;
  void           setIsSync( const bool );

  bool           isSuppressOutput() const;
  void           setIsSuppressOutput( const bool );

  bool           isShowBanner() const;
  void           setIsShowBanner( const bool );

  void           setAutoCompletion( bool );
  bool           autoCompletion() const;

  bool           isLogging() const;

  virtual QSize  sizeHint() const;

  bool           startLog( const QString& );
  bool           dump( const QString& );

signals:
  void updateDoc( const QString& );

public slots:
  void           cut();
  void           paste();
  void           clear();
  void           dump();
  void           startLog();
  void           stopLog();

protected:
  virtual void   dropEvent( QDropEvent* );
  virtual void   mousePressEvent( QMouseEvent* );
  virtual void   mouseReleaseEvent( QMouseEvent* );
  virtual void   keyPressEvent ( QKeyEvent* );
  virtual void   customEvent( QEvent* );

  virtual void   insertFromMimeData( const QMimeData* );

  void           putLog( const QString& );

  virtual QString getDumpFileName();
  virtual QString getLogFileName();

private:
  void           init();

  void           multilinePaste( const QString& );
  void           multiLineProcessNextLine();

  void           handleReturn();
  void           handleTab();
  void           handleBackTab();
  void           clearCompletion();
  QString        formatCompletion( const QStringList& ) const;
  QString        formatDocHTML( const QString& ) const;
  QString        extractCommon( const QStringList& ) const;

  PyInterp_Request* createCmdRequest( const QString& );
  PyInterp_Request* createTabRequest( const QString& );

  QString        banner() const;
  inline int     promptSize() const { return myPrompt.size(); }

  PyInterp_Auto<PyConsole_Interp> myInterp;           //!< python interpreter
  QString                         myCommandBuffer;    //!< python command buffer
  QString                         myCurrentCommand;   //!< currently being printed command
  QString                         myPrompt;           //!< current command line prompt
  int                             myCmdInHistory;     //!< current history command index
  QString                         myLogFile;          //!< current output log
  QStringList                     myHistory;          //!< commands history buffer
  QEventLoop*                     myEventLoop;        //!< internal event loop
  bool                            myShowBanner;       //!< 'show banner' flag
  QStringList                     myQueue;            //!< python commands queue
  bool                            myIsSync;           //!< synchronous mode flag
  bool                            myIsSuppressOutput; //!< suppress output flag
  bool                            myMultiLinePaste;   //!< true when pasting several lines
  QQueue<QString>                 myMultiLineContent; //!< queue of lines being pasted
  bool                            myAutoCompletion;   //!< auto-completion mode flag
  bool                            myTabMode;          //!< flag that is \c true when editor performs completion
  QString                         myComplBeforePoint; //!< string on which the dir() command is executed
  QString                         myComplAfterPoint;  //!< string on which the results of the dir() are matched
  int                             myComplCursorPos;   //!< cursor position when <TAB> is hit

  friend void PyConsole_CallbackStdout( void*, char* );
  friend void PyConsole_CallbackStderr( void*, char* );
};

#endif // PYCONSOLE_EDITOR_H
