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
// File   : PyConsole_Console.cxx
// Author : Vadim SANDLER, Open CASCADE S.A.S. (vadim.sandler@opencascade.com)

#include "PyConsole_Console.h"
#include "PyConsole_Interp.h"
#include "PyConsole_Editor.h"

#include <QAction>
#include <QApplication>
#include <QClipboard>
#include <QContextMenuEvent>
#include <QMenu>
#include <QVBoxLayout>

/*!
  \class PyConsole_Console
  \brief Python console widget.

  To create a Python console, just use default contstructor, specifying only a parent widget:
  \code
  PyConsole_Console c(myWindow);
  \endcode

  This will create a console with default editor and interpreter.

  To use custom editor with the console, you can use alternative constructor:
  \code
  PyConsole_Console c(myWindow, new MyEditor());
  \endcode
*/  

/*!
  \brief Default constructor.

  Creates new python console widget.
  \param parent parent widget
*/
PyConsole_Console::PyConsole_Console( QWidget* parent )
: QWidget( parent )
{
  init( 0 );
}

/*!
  \brief Constructor.

  Creates new python console widget.
  \param parent parent widget
  \param editor python editor
*/
PyConsole_Console::PyConsole_Console( QWidget* parent, PyConsole_Editor* editor )
: QWidget( parent )
{
  init( editor );
}

/*!
  \brief Initialize console widget.
  \param editor python editor
*/
void PyConsole_Console::init( PyConsole_Editor* editor )
{
  // initialize Python interpretator
  PyConsole_Interp* interp = editor ? editor->getInterp() : new PyConsole_Interp();
  interp->initialize();
  
  // create editor console
  QVBoxLayout* lay = new QVBoxLayout( this );
  lay->setMargin( 0 );
  myEditor = editor ? editor : new PyConsole_Editor( this, interp );
  myEditor->setContextMenuPolicy( Qt::NoContextMenu );
  lay->addWidget( myEditor );

  // force synchronous mode
  QString synchronous = qgetenv( "PYTHON_CONSOLE_SYNC" );
  if ( !synchronous.isEmpty() && synchronous.toInt() > 0 )
    setIsSync( true );

  // create actions
  createActions();
}

/*!
  \brief Destructor.
*/
PyConsole_Console::~PyConsole_Console()
{
}

/*!
  \brief Get Python interpreter
  \return pointer to Python interpreter
*/
PyConsole_Interp* PyConsole_Console::getInterp() const
{
  return myEditor ? myEditor->getInterp() : 0;
}

/*!
  \brief Execute python command in the interpreter.
  \param command string with command and arguments
*/
void PyConsole_Console::exec( const QString& command )
{
  if ( myEditor )
    myEditor->exec( command );
}

/*!
  \brief Execute python command in the interpreter 
         and wait until it is finished.
  
  Block execution of main application until the python command is executed.
  \param command string with command and arguments
*/
void PyConsole_Console::execAndWait( const QString& command )
{
  if ( myEditor )
    myEditor->execAndWait( command );
}

/*!
  \brief Get synchronous mode flag value.
  
  \sa setIsSync()
  \return \c true if python console works in synchronous mode
*/
bool PyConsole_Console::isSync() const
{
  return myEditor ? myEditor->isSync() : false;
}

/*!
  \brief Set synchronous mode flag value.

  In synhronous mode the Python commands are executed in the GUI thread
  and the GUI is blocked until the command is finished. In the asynchronous
  mode each Python command is executed in the separate thread that does not
  block the main GUI loop.

  \param on synhronous mode flag
*/
void PyConsole_Console::setIsSync( const bool on )
{
  if ( myEditor ) 
    myEditor->setIsSync( on );
}

/*!
  \brief Get suppress output flag value.
  
  \sa setIsSuppressOutput()
  \return \c true if python console output is suppressed.
*/
bool PyConsole_Console::isSuppressOutput() const
{
  return myEditor ? myEditor->isSuppressOutput() : false;
}

/*!
  \brief Set suppress output flag value.

  In case if suppress output flag is \c true, the python 
  console output suppressed.

  \param on suppress output flag
*/
void PyConsole_Console::setIsSuppressOutput( const bool on )
{
  if ( myEditor )
    myEditor->setIsSuppressOutput( on );
}

/*!
  \brief Get 'show banner' flag value.
  
  \sa setIsShowBanner()
  \return \c true if python console shows banner
*/
bool PyConsole_Console::isShowBanner() const
{
  return myEditor ? myEditor->isShowBanner() : false;
}

/*!
  \brief Set 'show banner' flag value.

  The banner is shown in the top of the python console window.

  \sa isShowBanner()
  \param on 'show banner' flag
*/
void PyConsole_Console::setIsShowBanner( const bool on )
{
  if ( myEditor )
    myEditor->setIsShowBanner( on );
}

/*!
  \brief Returns \c true if auto-completion feature is switched on
  or \c false otherwise
  \sa setAutoCompletion()
*/
bool PyConsole_Console::autoCompletion() const
{
  return myEditor ? myEditor->autoCompletion() : false;
}

/*!
  \brief Switch on/off commands auto-completion feature
  \sa autoCompletion()
*/
void PyConsole_Console::setAutoCompletion( const bool on )
{
  if ( myEditor )
    myEditor->setAutoCompletion( on );
}

/*!
  \brief Change the python console's font.
  \param f new font
*/
void PyConsole_Console::setFont( const QFont& f )
{
  if ( myEditor )
    myEditor->setFont( f );
}

/*!
  \brief Get python console font.
  \return current python console font
*/
QFont PyConsole_Console::font() const
{
  return myEditor ? myEditor->font() : QFont();
}

/*!
  \brief Set actions to be visible in the context popup menu.
  
  Actions, which IDs are set in \a flags parameter, will be shown in the 
  context popup menu. Other actions will not be shown.

  \param flags ORed together actions flags
*/
void PyConsole_Console::setMenuActions( const int flags )
{
  myActions[CopyId]->setVisible( flags & CopyId );
  myActions[PasteId]->setVisible( flags & PasteId );
  myActions[ClearId]->setVisible( flags & ClearId );
  myActions[SelectAllId]->setVisible( flags & SelectAllId );
  myActions[DumpCommandsId]->setVisible( flags & DumpCommandsId );
  myActions[StartLogId]->setVisible( flags & StartLogId );
  myActions[StopLogId]->setVisible( flags & StopLogId );
}

/*!
  \brief Get menu actions which are currently visible in the context popup menu.
  \return ORed together actions flags
  \sa setMenuActions()
*/
int PyConsole_Console::menuActions() const
{
  int ret = 0;
  ret = ret | ( myActions[CopyId]->isVisible() ? CopyId : 0 );
  ret = ret | ( myActions[PasteId]->isVisible() ? PasteId : 0 );
  ret = ret | ( myActions[ClearId]->isVisible() ? ClearId : 0 );
  ret = ret | ( myActions[SelectAllId]->isVisible() ? SelectAllId : 0 );
  ret = ret | ( myActions[DumpCommandsId]->isVisible() ? DumpCommandsId : 0 );
  ret = ret | ( myActions[StartLogId]->isVisible() ? StartLogId : 0 );
  ret = ret | ( myActions[StopLogId]->isVisible() ? StopLogId : 0 );
  return ret;
}

/*!
  \brief Create menu actions.

  Create context popup menu actions.
*/
void PyConsole_Console::createActions()
{
  QAction* a = new QAction( tr( "EDIT_COPY_CMD" ), this );
  a->setStatusTip( tr( "EDIT_COPY_CMD" ) );
  connect( a, SIGNAL( triggered( bool ) ), myEditor, SLOT( copy() ) );
  myActions.insert( CopyId, a );

  a = new QAction( tr( "EDIT_PASTE_CMD" ), this );
  a->setStatusTip( tr( "EDIT_PASTE_CMD" ) );
  connect( a, SIGNAL( triggered( bool ) ), myEditor, SLOT( paste() ) );
  myActions.insert( PasteId, a );

  a = new QAction( tr( "EDIT_CLEAR_CMD" ), this );
  a->setStatusTip( tr( "EDIT_CLEAR_CMD" ) );
  connect( a, SIGNAL( triggered( bool ) ), myEditor, SLOT( clear() ) );
  myActions.insert( ClearId, a );

  a = new QAction( tr( "EDIT_SELECTALL_CMD" ), this );
  a->setStatusTip( tr( "EDIT_SELECTALL_CMD" ) );
  connect( a, SIGNAL( triggered( bool ) ), myEditor, SLOT( selectAll() ) );
  myActions.insert( SelectAllId, a );
  
  a = new QAction( tr( "EDIT_DUMPCOMMANDS_CMD" ), this );
  a->setStatusTip( tr( "EDIT_DUMPCOMMANDS_CMD" ) );
  connect( a, SIGNAL( triggered( bool ) ), myEditor, SLOT( dump() ) );
  myActions.insert( DumpCommandsId, a );

  a = new QAction( tr( "EDIT_STARTLOG_CMD" ), this );
  a->setStatusTip( tr( "EDIT_STARTLOG_CMD" ) );
  connect( a, SIGNAL( triggered( bool ) ), myEditor, SLOT( startLog() ) );
  myActions.insert( StartLogId, a );

  a = new QAction( tr( "EDIT_STOPLOG_CMD" ), this );
  a->setStatusTip( tr( "EDIT_STOPLOG_CMD" ) );
  connect( a, SIGNAL( triggered( bool ) ), myEditor, SLOT( stopLog() ) );
  myActions.insert( StopLogId, a );
}

/*!
  \brief Update menu actions.

  Update context popup menu action state.
*/
void PyConsole_Console::updateActions()
{
  myActions[CopyId]->setEnabled( myEditor && myEditor->textCursor().hasSelection() );
  myActions[PasteId]->setEnabled( myEditor && !myEditor->isReadOnly() && !QApplication::clipboard()->text().isEmpty() );
  myActions[SelectAllId]->setEnabled( myEditor && !myEditor->document()->isEmpty() );
}

/*!
  \brief Start python trace logging
  \param fileName the path to the log file
*/
void PyConsole_Console::startLog( const QString& fileName )
{
  if ( myEditor ) 
    myEditor->startLog( fileName );
}

/*!
  \brief Stop python trace logging
*/
void PyConsole_Console::stopLog()
{
  if ( myEditor ) 
    myEditor->stopLog();
}

/*!
  \brief Process context popup menu request

  Show the context popup menu.

  \param event context popup menu event
*/
void PyConsole_Console::contextMenuEvent( QContextMenuEvent* event )
{
  if ( !myEditor || myEditor->isReadOnly() )
    return;

  QMenu* menu = new QMenu( this );
 
  menu->addAction( myActions[CopyId] );
  menu->addAction( myActions[PasteId] );
  menu->addAction( myActions[ClearId] );
  menu->addSeparator();
  menu->addAction( myActions[SelectAllId] );
  menu->addSeparator();
  menu->addAction( myActions[DumpCommandsId] );
  if ( !myEditor->isLogging() )
    menu->addAction( myActions[StartLogId] );
  else
    menu->addAction( myActions[StopLogId] );

  updateActions();

  menu->exec( event->globalPos());

  delete menu;
}
