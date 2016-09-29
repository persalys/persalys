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
// File   : PyConsole_Editor.cxx
// Author : Vadim SANDLER, Open CASCADE S.A.S. (vadim.sandler@opencascade.com)

/*!
  \class PyConsole_Editor
  \brief Python command line interpreter front-end GUI widget.
  
  This class provides simple GUI interface to the Python interpreter, including basic 
  navigation operations, executing commands (both interactively and programmatically), 
  copy-paste operations, history of the commands and so on.

  Here below is the shortcut keyboard boundings used for navigation and other operations:
  - <Enter>              : execute current command
  - <Ctrl><Break>        : clear current command
  - <Escape>             : clear current command
  - <Up>                 : previous command in the history
  - <Shift><Up>          : move cursor one row up with selection
  - <Ctrl><Up>           : move cursor one row up without selection
  - <Ctrl><Shift><Up>    : move cursor one row up with selection
  - <Down>               : next command in the history
  - <Shift><Down>        : move cursor one row down with selection
  - <Ctrl><Down>         : move cursor one row down without selection
  - <Ctrl><Shift><Down>  : move cursor one row down with selection
  - <Left>               : move one symbol left without selection
  - <Shift><Left>        : move one symbol left with selection
  - <Ctrl><Left>         : move one word left without selection
  - <Ctrl><Shift><Left>  : move one word left with selection
  - <Right>              : move one symbol right without selection
  - <Shift><Right>       : move one symbol right with selection
  - <Ctrl><Right>        : move one word right without selection
  - <Ctrl><Shift><Right> : move one word right with selection
  - <PgUp>               : first command in the history
  - <Shift><PgUp>        : move one page up with selection
  - <Ctrl><PgUp>         : move one page up without selection
  - <Ctrl><Shift><PgUp>  : scroll one page up
  - <PgDn>               : last command in the history
  - <Shift><PgDn>        : move one page down with selection
  - <Ctrl><PgDn>         : move one page down without selection
  - <Ctrl><Shift><PgDn>  : scroll one page down
  - <Home>               : move to the beginning of the line without selection
  - <Shift><Home>        : move to the beginning of the line with selection
  - <Ctrl><Home>         : move to the very first symbol without selection
  - <Ctrl><Shift><Home>  : move to the very first symbol with selection
  - <End>                : move to the end of the line without selection
  - <Shift><End>         : move to the end of the line with selection
  - <Ctrl><End>          : move to the very last symbol without selection
  - <Ctrl><Shift><End>   : move to the very last symbol with selection
  - <Backspace>          : delete symbol before the cursor
                           / remove selected text and put it to the clipboard (cut)
  - <Shift><Backspace>   : delete previous word
                           / remove selected text and put it to the clipboard (cut)
  - <Ctrl><Backspace>    : delete text from the cursor to the beginning of the line 
                           / remove selected text and put it to the clipboard (cut)
  - <Delete>             : delete symbol after the cursor 
                           / remove selected text and put it to the clipboard (cut)
  - <Shift><Delete>      : delete next word
                           / remove selected text and put it to the clipboard (cut)
  - <Ctrl><Delete>       : delete text from the cursor to the end of the line
                           / remove selected text and put it to the clipboard (cut)
  - <Ctrl><Insert>       : copy
  - <Shift><Insert>      : paste
  - <Ctrl><V>            : paste
  - <Ctrl><C>            : copy
  - <Ctrl><X>            : cut
  - <Ctrl><V>            : paste
  - <Tab>                : performs auto-completion
  - <Ctrl><Tab>          : undoes auto-completion
*/

#include "PyConsole_Editor.h"
#include "PyConsole_Interp.h"
#include "PyConsole_Event.h"
#include "PyInterp_Dispatcher.h"
#include "PyConsole_Request.h"

#include <QApplication>
#include <QClipboard>
#include <QDropEvent>
#include <QEvent>
#include <QKeyEvent>
#include <QMimeData>
#include <QMouseEvent>
#include <QScrollBar>
#include <QTextBlock>
#include <QTextCursor>
#include <QTextStream>
#include <QChar>
#include <QFileDialog>
#include <QMessageBox>

//VSR: uncomment below macro to support unicode text properly in SALOME
//     current commented out due to regressions
//#define PAL22528_UNICODE

namespace
{
  QString fromUtf8( const char* txt )
  {
#ifdef PAL22528_UNICODE
    return QString::fromUtf8( txt );
#else
    return QString( txt );
#endif
  }
}

static QString READY_PROMPT = ">>> ";
static QString DOTS_PROMPT  = "... ";

void PyConsole_CallbackStdout( void* data, char* c )
{
  if(!((PyConsole_Editor*)data)->isSuppressOutput()) {
    PyConsole_Editor* e = (PyConsole_Editor*)data;
    e->putLog( fromUtf8(c) );
    QApplication::postEvent( e, new PyConsole_PrintEvent( fromUtf8(c), false ) );
  }
}

void PyConsole_CallbackStderr( void* data, char* c )
{
  if(!((PyConsole_Editor*)data)->isSuppressOutput()) {
    PyConsole_Editor* e = (PyConsole_Editor*)data;
    e->putLog( fromUtf8(c) );
    QApplication::postEvent( e, new PyConsole_PrintEvent( fromUtf8(c), true ) );
  }
}

/*!
  \brief Constructor. 
  
  Creates python editor window.
  \param theInterp python interper
  \param theParent parent widget
*/
PyConsole_Editor::PyConsole_Editor( PyConsole_Interp* theInterp, 
                                    QWidget*          theParent )
: QTextEdit( theParent ),
  myInterp( theInterp ),
  myCmdInHistory( -1 ),
  myEventLoop( 0 ),
  myShowBanner( true ),
  myIsSync( true ),
  myIsSuppressOutput( false ),
  myMultiLinePaste( false ),
  myAutoCompletion( false ),
  myTabMode( false ),
  myComplCursorPos( -1 )
{
  setFont( QFont( "Courier", 11 ) ); // default font
  setUndoRedoEnabled( false );

  myPrompt = READY_PROMPT;
  setLineWrapMode( QTextEdit::WidgetWidth );
  setWordWrapMode( QTextOption::WrapAnywhere );
  setAcceptRichText( false );

  // set callbacks to interpeter
  myInterp->setvoutcb( PyConsole_CallbackStdout, this );
  myInterp->setverrcb( PyConsole_CallbackStderr, this );
  // print banner
  if ( isShowBanner() )
    addText( banner() );
  // clear command buffer
  myCommandBuffer.truncate(0);
  // unset read-only state
  setReadOnly( false );
  // unset history browsing mode
  myCmdInHistory = -1;
  // add prompt
  addText( myPrompt );
  // unset busy cursor
  viewport()->unsetCursor();
}

/*!
  \brief Destructor.
*/
PyConsole_Editor::~PyConsole_Editor()
{
  myInterp = 0;
}

/*!
  \brief Get Python interpreter
*/
PyConsole_Interp* PyConsole_Editor::getInterp() const
{
  return myInterp;
}

/*!
  \brief Get synchronous mode flag value.
  
  \sa setIsSync()
  \return \c true if python console works in synchronous mode
*/
bool PyConsole_Editor::isSync() const
{
  return myIsSync;
}

/*!
  \brief Set synchronous mode flag value.

  In synhronous mode the Python commands are executed in the GUI thread
  and the GUI is blocked until the command is finished. In the asynchronous
  mode each Python command is executed in the separate thread that does not
  block the main GUI loop.

  \param on synhronous mode flag
*/
void PyConsole_Editor::setIsSync( const bool on )
{
  myIsSync = on;
}

/*!
  \brief Get suppress output flag value.
  
  \sa setIsSuppressOutput()
  \return \c true if python console output is suppressed.
*/
bool PyConsole_Editor::isSuppressOutput() const
{
  return myIsSuppressOutput;
}

/*!
  \brief Set suppress output flag value.

  In case if suppress output flag is \c true, the python 
  console output suppressed.

  \param on suppress output flag
*/
void PyConsole_Editor::setIsSuppressOutput( const bool on )
{
  myIsSuppressOutput = on;
}

/*!
  \brief Get 'show banner' flag value.
  
  \sa setIsShowBanner()
  \return \c true if python console shows banner
*/
bool PyConsole_Editor::isShowBanner() const
{
  return myShowBanner;
}

/*!
  \brief Set 'show banner' flag value.

  The banner is shown in the top of the python console window.

  \sa isShowBanner()
  \param on 'show banner' flag
*/
void PyConsole_Editor::setIsShowBanner( const bool on )
{
  if ( myShowBanner != on ) {
    myShowBanner = on;
    clear();
  }
}

/*!
  \brief Switch on/off commands auto-completion feature
  \sa autoCompletion()
*/
void PyConsole_Editor::setAutoCompletion( bool on )
{
  myAutoCompletion = on;
  document()->setUndoRedoEnabled( myAutoCompletion );
}
  
/*!
  \brief Returns \c true if auto-completion feature is switched on
  or \c false otherwise
  \sa setAutoCompletion()
*/
bool PyConsole_Editor::autoCompletion() const
{
  return myAutoCompletion;
}

/*!
  \brief Check if trace logging is switched on.
  
  \sa startLog(), stopLog()
  \return \c true if trace logging is switched on
*/
bool PyConsole_Editor::isLogging() const
{
  return !myLogFile.isEmpty();
}

/*!
  \brief Get size hint for the Python console window
  \return size hint value
*/
QSize PyConsole_Editor::sizeHint() const
{
  QFontMetrics fm( font() );
  int nbLines = ( isShowBanner() ? banner().split("\n").count() : 0 ) + 1;
  QSize s(100, fm.lineSpacing()*nbLines);
  return s;
}

/*!
  \brief Put the string \a str to the python editor.
  \param str string to be put in the command line of the editor
  \param newBlock if \c true, then the string is printed on a new line
  \param isError if \c true, the text is printed in dark red
*/
void PyConsole_Editor::addText( const QString& str, 
                                const bool     newBlock,
                                const bool     isError )
{
  QTextCursor aCursor = textCursor();
  QTextCharFormat cf;

  moveCursor( QTextCursor::End );
  if ( newBlock )
    aCursor.insertBlock();
  if ( isError )
    cf.setForeground( QBrush( Qt::red ) );
  else
    cf.setForeground( QBrush( Qt::black ) );
  aCursor.insertText( str, cf );
  moveCursor( QTextCursor::End );
  ensureCursorVisible();
}

/*!
  \brief Convenient method for executing a Python command,
  as if the user typed it manually.
  \param command python command to be executed

  !!! WARNING: doesn't work properly with multi-line commands. !!!
*/
void PyConsole_Editor::exec( const QString& command )
{
  if ( isReadOnly() ) {
    // some interactive command is being executed in this editor...
    // shedule the command to the queue
    myQueue.push_back( command );
    return;
  }

  // remove last line
  moveCursor( QTextCursor::End );
  moveCursor( QTextCursor::StartOfBlock, QTextCursor::KeepAnchor );
  textCursor().removeSelectedText();

  // set "ready" prompt
  myPrompt = READY_PROMPT;

  // clear command buffer
  myCommandBuffer.truncate( 0 );

  // unset history browsing mode
  myCmdInHistory = -1;

  // print command line by line
  QString cmd = command;
  if ( !cmd.endsWith( "\n" ) ) cmd += "\n";
  QStringList lines = command.split( "\n" );
  for ( int i = 0; i < lines.size(); i++ ) {
    if ( !lines[i].trimmed().isEmpty() )
      myHistory.push_back( lines[i] );
    addText( ( i == 0 ? READY_PROMPT : DOTS_PROMPT ) + lines[i], i != 0 );
    putLog( QString( "%1%2\n" ).arg( i == 0 ? READY_PROMPT : DOTS_PROMPT ).arg( lines[i] ) );
  }

  // IPAL20182
  addText( "", true );

  // set read-only mode
  setReadOnly( true );

  // set busy cursor
  setCursor( Qt::BusyCursor );
  
  // post a request to execute Python command;
  // editor will be informed via a custom event that execution has been completed
  PyInterp_Dispatcher::Get()->Exec( createCmdRequest( cmd ) );
}

/*!
  \brief Create request to the python dispatcher for the command execution.
  \param command python command to be executed
 */
PyInterp_Request* PyConsole_Editor::createCmdRequest( const QString& command )
{
  return new PyConsole_ExecCommand( myInterp, command, this, isSync() );
}

/*!
  \brief Create the Python request that will be posted to the interpreter to
  get the completions.
  \param input line entered by the user at the time <TAB> was pressed
  \return completion command
  \sa CompletionCommand
*/
PyInterp_Request* PyConsole_Editor::createTabRequest( const QString& input )
{
  // valid separators
  static QStringList separators;
  if ( separators.isEmpty() ) {
    separators << " " << "(" << "[" << "+" << "-" << "*" << "/" << ";" << "^" << "=";
  }

  // parse input to extract on what part the dir() has to be executed

  QString input2( input );

  // split up to the last syntaxical separator
  int lastSp = -1;
  foreach ( QString separator, separators ) {
    int j = input2.lastIndexOf( separator );
    if ( j > lastSp )
      lastSp = j;
  }
  if ( lastSp >= 0 )
    input2 = input.mid( lastSp + 1 );

  // detect a qualified name (with a point)
  int lastPt = input2.lastIndexOf( "." );

  if ( lastPt != -1 ) {
    // split the 2 surrounding parts of the qualified name
    myComplBeforePoint = input2.left( lastPt );
    myComplAfterPoint = input2.mid( lastPt+1 );
  }
  else {
    // no point found - do a global matching
    // (the following will call dir() with an empty string)
    myComplAfterPoint = input2;
    myComplBeforePoint = "";
  }

  return new PyConsole_CompletionCommand( myInterp, myComplBeforePoint,
                                          myComplAfterPoint, this, isSync() );
}

/*!
  \brief Execute command in the python interpreter
  and wait until it is finished.

  \param command python command to be executed
 */
void PyConsole_Editor::execAndWait( const QString& command )
{
  // already running ?
  if ( myEventLoop )
    return;

  // create new event loop
  bool sync = isSync();
  if ( !sync ) {
    myEventLoop = new QEventLoop( this );
  }

  // execute command
  exec( command );

  if ( !sync ) {
    // run event loop
    myEventLoop->exec();
    // delete event loop after command is processed
    delete myEventLoop;
    myEventLoop = 0;
  }
}

/*!
  \brief Process <Enter> key press event. 
  
  Execute the command entered by the user.
*/
void PyConsole_Editor::handleReturn()
{
  // Position cursor at the end
  QTextCursor aCursor = textCursor();
  aCursor.movePosition( QTextCursor::End );
  setTextCursor( aCursor );

  // get last line
  QTextBlock par = document()->end().previous();
  if ( !par.isValid() ) return;

  // get command
  QString cmd = par.text().remove( 0, promptSize() );

  // extend the command buffer with the current command 
  myCommandBuffer.append( cmd );

  // add command to the history
  if ( !cmd.trimmed().isEmpty() )
    myHistory.push_back( cmd );
  putLog( QString( "%1%2\n" ).arg( myPrompt ).arg( cmd ) );

  // IPAL19397
  addText( "", true ); 
  
  // set read-only mode
  setReadOnly( true );

  // set busy cursor
  setCursor( Qt::BusyCursor );
  
  // post a request to execute Python command;
  // editor will be informed via a custom event that execution has been completed
  PyInterp_Dispatcher::Get()->Exec( createCmdRequest( myCommandBuffer ) );
}

/*!
  \brief Process <Tab> key press event. 
  
  Perform auto-completion of the currently entered command, if this feature is enabled
*/
void PyConsole_Editor::handleTab()
{
  if ( !autoCompletion() )
    return; // auto-completion feature is disabled

  if ( myTabMode )
    return; // already in tab mode

  QTextCursor aCursor = textCursor();

  // move cursor to the end of input
  aCursor.movePosition( QTextCursor::End );
  setTextCursor( aCursor );

  // save cursor position if needed
  if ( myComplCursorPos == -1 )
    myComplCursorPos = textCursor().position();

  // get last line
  QTextBlock par = document()->end().previous();
  if ( !par.isValid() ) return; // empty line

  // switch to completion mode
  myTabMode = true;

  // get currently entered command
  QString cmd = par.text().mid( promptSize() );

  // post completion request
  // editor will be informed that completion has been done via a custom event 
  PyInterp_Dispatcher::Get()->Exec( createTabRequest( cmd ) );
}

/*!
  \brief Process <Ctrl><Tab> key press event. 
  
  Undoe last auto-completion
*/
void PyConsole_Editor::handleBackTab()
{
  if ( !autoCompletion() )
    return; // auto-completion feature is disabled

  QTextCursor aCursor = textCursor();

  if ( myComplCursorPos == -1 )
    return; // invalid cursor position

  // ensure cursor is at the end of command line
  aCursor.setPosition( myComplCursorPos );
  aCursor.movePosition( QTextCursor::EndOfLine );
  //setCursor( aCursor );

  // delete last completed text
  int i = aCursor.position() - myComplCursorPos;
  aCursor.movePosition( QTextCursor::Left, QTextCursor::KeepAnchor, i );
  aCursor.removeSelectedText();
  myComplCursorPos = -1;
}

/*!
  \brief Process drop event.

  Paste dragged text.
  \param event drop event
*/
void PyConsole_Editor::dropEvent( QDropEvent* event )
{
  // get the initial drop position
  QPoint pos = event->pos();
  QTextCursor aCursor = cursorForPosition( event->pos() );

  // if the position is not in the last line move it to the end of the command line
  if ( aCursor.position() < document()->end().previous().position() + promptSize() ) {
    moveCursor( QTextCursor::End );
    pos = cursorRect().center();
  }

  // create new drop event and use it instead of the original
  QDropEvent de( pos,
                 event->possibleActions(),
                 event->mimeData(),
                 event->mouseButtons(),
                 event->keyboardModifiers(),
                 event->type() );
  QTextEdit::dropEvent( &de );

  // accept the original event
  event->acceptProposedAction();
}

/*!
  \brief Process mouse press event

  Clear the completion when any mouse button is pressed.

  \param e mouse press event
*/
void PyConsole_Editor::mousePressEvent( QMouseEvent* event )
{
  if ( autoCompletion() ) {
    clearCompletion();
    myComplCursorPos = -1;
  }
  QTextEdit::mousePressEvent( event );
}

/*!
  \brief Process mouse button release event.

  Left mouse button: copy selection to the clipboard.
  Middle mouse button: paste clipboard's contents.

  \param event mouse event
*/
void PyConsole_Editor::mouseReleaseEvent( QMouseEvent* event )
{
  if ( event->button() == Qt::LeftButton ) {
    QTextEdit::mouseReleaseEvent( event );
  }
  else if ( event->button() == Qt::MidButton ) {
    QTextCursor aCursor = cursorForPosition( event->pos() );
    // if the position is not in the last line move it to the end of the command line
    if ( aCursor.position() < document()->end().previous().position() + promptSize() ) {
      moveCursor( QTextCursor::End );
    }
    else {
      setTextCursor( aCursor );
    }
    const QMimeData* md = QApplication::clipboard()->mimeData( QApplication::clipboard()->supportsSelection() ? 
							       QClipboard::Selection : QClipboard::Clipboard );
    if ( md )
      insertFromMimeData( md );
  }
  else {
    QTextEdit::mouseReleaseEvent( event );
  }
}

/*!
  \brief Check if the string is command.
  
  Return \c true if the string \a str is likely to be the command
  (i.e. it is started from the '>>>' or '...').
  \param str string to be checked
*/
bool PyConsole_Editor::isCommand( const QString& str ) const
{
  return str.startsWith( READY_PROMPT ) || str.startsWith( DOTS_PROMPT );
}

/*!
  \brief Handle keyboard event.

  Implement navigation, history browsing, copy/paste and other common
  operations.

  \param event keyboard event
*/
void PyConsole_Editor::keyPressEvent( QKeyEvent* event )
{
  // get cursor position
  QTextCursor aCursor = textCursor();
  int curLine = aCursor.blockNumber();
  int curCol  = aCursor.columnNumber();

  // get last edited line
  int endLine = document()->blockCount()-1;

  // get pressed key code
  int aKey = event->key();

  // check if <Ctrl> is pressed
  bool ctrlPressed = event->modifiers() & Qt::ControlModifier;
  // check if <Shift> is pressed
  bool shftPressed = event->modifiers() & Qt::ShiftModifier;

  if ( autoCompletion() ) {
    // auto-completion support
    if ( aKey == Qt::Key_Tab && !shftPressed ) {
      // process <Tab> key
      if ( !ctrlPressed ) {
        handleTab();
      }
      else {
        clearCompletion();
        handleBackTab();
      }
      return;
    }

    // If <Ctrl> is not pressed (or if something else is pressed with <Ctrl>),
    // or if <Ctrl> is not pressed alone, we have to clear completion
    if ( !ctrlPressed || ( ctrlPressed && aKey != Qt::Key_Control ) ) {
      clearCompletion();
      myComplCursorPos = -1;
    }
    
    // Discard <Ctrl> pressed alone:
    if ( aKey == Qt::Key_Control )
      return;
  }

  if ( aKey == Qt::Key_Escape || ( ctrlPressed && aKey == -1 ) ) {
    // process <Ctrl>+<Break> key-binding and <Escape> key: clear current command
    myCommandBuffer.truncate( 0 );
    myPrompt = READY_PROMPT;
    addText( myPrompt, true );
    horizontalScrollBar()->setValue( horizontalScrollBar()->minimum() );
    return;
  }
  else if ( ctrlPressed && aKey == Qt::Key_C ) {
    // process <Ctrl>+<C> key-binding : copy
    copy();
    return;
  }
  else if ( ctrlPressed && aKey == Qt::Key_X ) {
    // process <Ctrl>+<X> key-binding : cut
    cut();
    return;
  }
  else if ( ctrlPressed && aKey == Qt::Key_V ) {
    // process <Ctrl>+<V> key-binding : paste
    paste();
    return;
  }

  // check for printed key
  // #### aKey = ( aKey < Qt::Key_Space || aKey > Qt::Key_ydiaeresis ) ? aKey : 0;
  // Better:
  aKey = !(QChar(aKey).isPrint()) ? aKey : 0;

  switch ( aKey ) {
  case 0 :
    // any printed key: just print it
    {
      if ( curLine < endLine || curCol < promptSize() ) {
        moveCursor( QTextCursor::End );
      }
      QTextEdit::keyPressEvent( event );
      break;
    }
  case Qt::Key_Return:
  case Qt::Key_Enter:
    // <Enter> key: process the current command
    {
      handleReturn();
      break;
    }
  case Qt::Key_Up:
    // <Up> arrow key: process as follows:
    // - without <Ctrl>, <Shift> modifiers: previous command in history
    // - with <Ctrl> modifier key pressed:  move cursor one row up without selection
    // - with <Shift> modifier key pressed: move cursor one row up with selection
    // - with <Ctrl>+<Shift> modifier keys pressed: scroll one row up
    {
      if ( ctrlPressed && shftPressed ) {
        int value   = verticalScrollBar()->value();
        int spacing = fontMetrics().lineSpacing();
        verticalScrollBar()->setValue( value > spacing ? value-spacing : 0 );
      }
      else if ( shftPressed || ctrlPressed ) {
        if ( curLine > 0 )
          moveCursor( QTextCursor::Up, 
                      shftPressed ? QTextCursor::KeepAnchor : QTextCursor::MoveAnchor );
      }
      else { 
        if ( myCmdInHistory < 0 && myHistory.count() > 0 ) {
          // set history browsing mode
          myCmdInHistory = myHistory.count();
          // remember current command
          QTextBlock par = document()->end().previous();
          myCurrentCommand = par.text().remove( 0, promptSize() );
        }
        if ( myCmdInHistory > 0 ) {
          myCmdInHistory--;
          // get previous command in the history
          QString previousCommand = myHistory.at( myCmdInHistory );
          // print previous command
          moveCursor( QTextCursor::End );
          moveCursor( QTextCursor::StartOfBlock, QTextCursor::KeepAnchor );
          textCursor().removeSelectedText();
          addText( myPrompt + previousCommand ); 
          // move cursor to the end
          moveCursor( QTextCursor::End );
        }
      }
      break;
    }
  case Qt::Key_Down:
    // <Down> arrow key: process as follows:
    // - without <Ctrl>, <Shift> modifiers: next command in history
    // - with <Ctrl> modifier key pressed:  move cursor one row down without selection
    // - with <Shift> modifier key pressed: move cursor one row down with selection
    // - with <Ctrl>+<Shift> modifier keys pressed: scroll one row down
    {
      if ( ctrlPressed && shftPressed ) {
        int value   = verticalScrollBar()->value();
        int maxval  = verticalScrollBar()->maximum();
        int spacing = fontMetrics().lineSpacing();
        verticalScrollBar()->setValue( value+spacing < maxval ? value+spacing : maxval );
      }
      else if ( shftPressed || ctrlPressed) {
        if ( curLine < endLine )
          moveCursor( QTextCursor::Down, 
                      shftPressed ? QTextCursor::KeepAnchor : QTextCursor::MoveAnchor );
      }
      else { 
        if ( myCmdInHistory >= 0 ) {
          // get next command in the history
          myCmdInHistory++;
          QString nextCommand;
          if ( myCmdInHistory < myHistory.count() ) {
            // next command in history
            nextCommand = myHistory.at( myCmdInHistory );
          }
          else {
            // end of history is reached
            // last printed command
            nextCommand = myCurrentCommand;
            // unset history browsing mode
            myCmdInHistory = -1;
          }
          // print next or current command
          moveCursor( QTextCursor::End );
          moveCursor( QTextCursor::StartOfBlock, QTextCursor::KeepAnchor );
          textCursor().removeSelectedText();
          addText( myPrompt + nextCommand );
          // move cursor to the end
          moveCursor( QTextCursor::End );
        }
      }
      break;
    }
  case Qt::Key_Left:
    // <Left> arrow key: process as follows:
    // - without <Ctrl>, <Shift> modifiers: move one symbol left (taking into account prompt)
    // - with <Ctrl> modifier key pressed:  move one word left (taking into account prompt)
    // - with <Shift> modifier key pressed: move one symbol left with selection
    // - with <Ctrl>+<Shift> modifier keys pressed: move one word left with selection
    {
      QString txt = textCursor().block().text();
      if ( !shftPressed && isCommand( txt ) && curCol <= promptSize() ) {
        moveCursor( QTextCursor::Up );
        moveCursor( QTextCursor::EndOfBlock );
      }
      else {
        QTextEdit::keyPressEvent( event );
      }
      break;
    }
  case Qt::Key_Right:
    // <Right> arrow key: process as follows:
    // - without <Ctrl>, <Shift> modifiers: move one symbol right (taking into account prompt)
    // - with <Ctrl> modifier key pressed:  move one word right (taking into account prompt)
    // - with <Shift> modifier key pressed: move one symbol right with selection
    // - with <Ctrl>+<Shift> modifier keys pressed: move one word right with selection
    {
      QString txt = textCursor().block().text();
      if ( !shftPressed ) {
        if ( curCol < txt.length() ) {
          if ( isCommand( txt ) && curCol < promptSize() ) {
            aCursor.setPosition( aCursor.block().position() + promptSize() );
            setTextCursor( aCursor );
            break;
          }
        }
        else {
          if ( curLine < endLine && isCommand( textCursor().block().next().text() ) ) {
            aCursor.setPosition( aCursor.position() + promptSize()+1 );
            setTextCursor( aCursor );
            horizontalScrollBar()->setValue( horizontalScrollBar()->minimum() );
            break;
          }
        }
      }
      QTextEdit::keyPressEvent( event );
      break;
    }
  case Qt::Key_PageUp:
    // <PageUp> key: process as follows:
    // - without <Ctrl>, <Shift> modifiers: first command in history
    // - with <Ctrl> modifier key pressed:  move cursor one page up without selection
    // - with <Shift> modifier key pressed: move cursor one page up with selection
    // - with <Ctrl>+<Shift> modifier keys pressed: scroll one page up
    {
      if ( ctrlPressed && shftPressed ) {
        verticalScrollBar()->triggerAction(QAbstractSlider::SliderPageStepSub);
      }
      else if ( shftPressed || ctrlPressed ) {
        bool moved = false;
        qreal lastY = cursorRect( aCursor ).top();
        qreal distance = 0;
        // move using movePosition to keep the cursor's x
        do {
          qreal y = cursorRect( aCursor ).top();
          distance += qAbs( y - lastY );
          lastY = y;
          moved = aCursor.movePosition( QTextCursor::Up, 
                                        shftPressed ? QTextCursor::KeepAnchor : 
                                        QTextCursor::MoveAnchor );
        } while ( moved && distance < viewport()->height() );
        if ( moved ) {
          aCursor.movePosition( QTextCursor::Down, 
                                shftPressed ? QTextCursor::KeepAnchor : 
                                QTextCursor::MoveAnchor );
          verticalScrollBar()->triggerAction( QAbstractSlider::SliderPageStepSub );
        }
        setTextCursor( aCursor );
      }
      else { 
        if ( myCmdInHistory < 0 && myHistory.count() > 0 ) {
          // set history browsing mode
          myCmdInHistory = myHistory.count();
          // remember current command
          QTextBlock par = document()->end().previous();
          myCurrentCommand = par.text().remove( 0, promptSize() );
        }
        if ( myCmdInHistory > 0 ) {
          myCmdInHistory = 0;
          // get very first command in the history
          QString firstCommand = myHistory.at( myCmdInHistory );
          // print first command
          moveCursor( QTextCursor::End );
          moveCursor( QTextCursor::StartOfBlock, QTextCursor::KeepAnchor );
          textCursor().removeSelectedText();
          addText( myPrompt + firstCommand ); 
          // move cursor to the end
          moveCursor( QTextCursor::End );
        }
      }
      break;
    }
  case Qt::Key_PageDown:
    // <PageDown> key: process as follows:
    // - without <Ctrl>, <Shift> modifiers: last command in history
    // - with <Ctrl> modifier key pressed:  move cursor one page down without selection
    // - with <Shift> modifier key pressed: move cursor one page down with selection
    // - with <Ctrl>+<Shift> modifier keys pressed: scroll one page down
    {
      if ( ctrlPressed && shftPressed ) {
        verticalScrollBar()->triggerAction(QAbstractSlider::SliderPageStepAdd);
      }
      else if ( shftPressed || ctrlPressed ) {
        bool moved = false;
        qreal lastY = cursorRect( aCursor ).top();
        qreal distance = 0;
        // move using movePosition to keep the cursor's x
        do {
          qreal y = cursorRect( aCursor ).top();
          distance += qAbs( y - lastY );
          lastY = y;
          moved = aCursor.movePosition( QTextCursor::Down, 
                                        shftPressed ? QTextCursor::KeepAnchor : 
                                        QTextCursor::MoveAnchor );
        } while ( moved && distance < viewport()->height() );
        if ( moved ) {
          aCursor.movePosition( QTextCursor::Up, 
                                shftPressed ? QTextCursor::KeepAnchor : 
                                QTextCursor::MoveAnchor );
          verticalScrollBar()->triggerAction( QAbstractSlider::SliderPageStepSub );
        }
        setTextCursor( aCursor );
      }
      else { 
        if ( myCmdInHistory >= 0 ) {
          // unset history browsing mode
          myCmdInHistory = -1;
          // print current command
          moveCursor( QTextCursor::End );
          moveCursor( QTextCursor::StartOfBlock, QTextCursor::KeepAnchor );
          textCursor().removeSelectedText();
          addText( myPrompt + myCurrentCommand ); 
          // move cursor to the end
          moveCursor( QTextCursor::End );
        }
      }
      break;
    }
  case Qt::Key_Home: 
    // <Home> key: process as follows:
    // - without <Ctrl>, <Shift> modifiers: move cursor to the beginning of the current line without selection
    // - with <Ctrl> modifier key pressed:  move cursor to the very first symbol without selection
    // - with <Shift> modifier key pressed: move cursor to the beginning of the current line with selection
    // - with <Ctrl>+<Shift> modifier keys pressed: move cursor to the very first symbol with selection
    {
      if ( ctrlPressed ) { 
        moveCursor( QTextCursor::Start, 
                    shftPressed ? QTextCursor::KeepAnchor : QTextCursor::MoveAnchor );
      }
      else {
        QString txt = textCursor().block().text();
        if ( isCommand( txt ) ) {
          if ( shftPressed ) {
            if ( curCol > promptSize() ) {
              aCursor.movePosition( QTextCursor::StartOfLine, QTextCursor::KeepAnchor );
              aCursor.movePosition( QTextCursor::Right, QTextCursor::KeepAnchor, promptSize() );
            }
          }
          else {
            aCursor.movePosition( QTextCursor::StartOfLine );
            aCursor.movePosition( QTextCursor::Right, QTextCursor::MoveAnchor, promptSize() );
          }
          setTextCursor( aCursor );
        }
        else {
          moveCursor( QTextCursor::StartOfBlock, 
                      shftPressed ? QTextCursor::KeepAnchor : QTextCursor::MoveAnchor );
        }
        horizontalScrollBar()->setValue( horizontalScrollBar()->minimum() );
      }
      break;
    }
  case Qt::Key_End:
    // <End> key: process as follows:
    // - without <Ctrl>, <Shift> modifiers: move cursor to the end of the current line without selection
    // - with <Ctrl> modifier key pressed:  move cursor to the very last symbol without selection
    // - with <Shift> modifier key pressed: move cursor to the end of the current line with selection
    // - with <Ctrl>+<Shift> modifier keys pressed: move cursor to the very last symbol with selection
    {
      moveCursor( ctrlPressed ? QTextCursor::End : QTextCursor::EndOfBlock, 
                  shftPressed ? QTextCursor::KeepAnchor : QTextCursor::MoveAnchor );
      break;
    }  
  case Qt::Key_Backspace :
    // <Backspace> key: process as follows
    // - without any modifiers : delete symbol before the cursor / selection (taking into account prompt)
    // - with <Shift> modifier key pressed: delete previous word
    // - with <Ctrl> modifier key pressed: delete text from the cursor to the line beginning
    // works only for last (command) line
    {
      if ( aCursor.hasSelection() ) {
        cut();
      }
      else if ( aCursor.position() > document()->end().previous().position() + promptSize() ) {
        if ( shftPressed ) {
          moveCursor( QTextCursor::PreviousWord, QTextCursor::KeepAnchor );
          textCursor().removeSelectedText();
        }
        else if ( ctrlPressed ) {
          aCursor.setPosition( document()->end().previous().position() + promptSize(),
                               QTextCursor::KeepAnchor );
          setTextCursor( aCursor );
          textCursor().removeSelectedText();
        }
        else {
          QTextEdit::keyPressEvent( event );
        }
      }
      else {
        aCursor.setPosition( document()->end().previous().position() + promptSize() );
        setTextCursor( aCursor );
        horizontalScrollBar()->setValue( horizontalScrollBar()->minimum() );
      }
      break;
    }
  case Qt::Key_Delete :
    // <Delete> key: process as follows
    // - without any modifiers : delete symbol after the cursor / selection (taking into account prompt)
    // - with <Shift> modifier key pressed: delete next word
    // - with <Ctrl> modifier key pressed: delete text from the cursor to the end of line
    // works only for last (command) line
    {
      if ( aCursor.hasSelection() ) {
        cut();
      }
      else if ( aCursor.position() > document()->end().previous().position() + promptSize()-1 ) {
        if ( shftPressed ) {
          moveCursor( QTextCursor::NextWord, QTextCursor::KeepAnchor );
          textCursor().removeSelectedText();
        }
        else if ( ctrlPressed ) {
          moveCursor( QTextCursor::EndOfBlock, QTextCursor::KeepAnchor );
          textCursor().removeSelectedText();
        }
        else {
          QTextEdit::keyPressEvent( event );
        }
      }
      else {
        aCursor.setPosition( document()->end().previous().position() + promptSize() );
        setTextCursor( aCursor );
        horizontalScrollBar()->setValue( horizontalScrollBar()->minimum() );
      }
      break;
    }
  case Qt::Key_Insert :
    // <Insert> key: process as follows
    // - with <Ctrl> modifier key pressed:  copy()
    // - with <Shift> modifier key pressed: paste() to the command line
    {
      if ( ctrlPressed ) {
        copy();
      }
      else if ( shftPressed ) {
        paste();
      }
      else
        QTextEdit::keyPressEvent( event );
      break;
    }
  default:
    break;
  }
}

/*!
  \brief Handle notification event coming from Python dispatcher.
  \param event notification event
*/
void PyConsole_Editor::customEvent( QEvent* event )
{
  switch( event->type() )
  {
  case PyConsole_PrintEvent::EVENT_ID:
  {
    PyConsole_PrintEvent* pe = (PyConsole_PrintEvent*)event;
    addText( pe->text(), false, pe->isError() );
    return;
  }
  case PyConsole_CompletionEvent::EVENT_ID:
  {
    PyConsole_CompletionEvent* ce = (PyConsole_CompletionEvent*)event;
    bool status = ce->status();
    QStringList matches = ce->matches();
    QString doc = ce->doc();

    if ( status ) {
      // completion was successful
      QTextCursor aCursor = textCursor();

      if ( matches.isEmpty() ) {
	// completion successful but there are no matches.
        myTabMode = false;
        myComplCursorPos = -1;
        return;
      }
      
      if ( matches.size() == 1 ) {
        // there's only one match - complete directly and update doc string window
        aCursor.insertText( matches[0].mid( myComplAfterPoint.size() ) );
        myTabMode = false;
        if ( doc.isEmpty() )
          emit updateDoc( formatDocHTML( QString( "(%1)\n" ).arg( tr( "NO_DOC_AVAILABLE" ) ) ) );
        else
          emit updateDoc( formatDocHTML( doc ) );
      }
      else {
        // there are several matches
        
        // detect if there is a common base to all available completion
        // in this case append this base to the text
        QString base = extractCommon( matches );
        aCursor.insertText( base.mid( myComplAfterPoint.size() ) );
        
        // if this happens to match exactly the first completion
        // also provide doc
        if ( base == matches[0] )
          emit updateDoc( formatDocHTML( doc ) );
        
        // print all matching completion in a "undo-able" block
        int cursorPos = aCursor.position();
        aCursor.insertBlock();
        aCursor.beginEditBlock();
        
        // insert all matches
        QTextCharFormat cf;
        cf.setForeground( QBrush( Qt::darkGreen ) );
        aCursor.setCharFormat( cf );
        aCursor.insertText( formatCompletion( matches ) );
        aCursor.endEditBlock();
        
        // position cursor where it was before inserting the completion list
        aCursor.setPosition( cursorPos );
        setTextCursor( aCursor );
      }
    }
    else {
      // completion failed
      myTabMode = false;
      myComplCursorPos = -1;
    }
    return;
  }
  case PyInterp_Event::ES_OK:
  case PyInterp_Event::ES_ERROR:
  {
    // clear command buffer
    myCommandBuffer.truncate( 0 );
    // add caret return line if necessary
    QTextBlock par = document()->end().previous();
    QString txt = par.text();
    txt.truncate( txt.length() - 1 );
    // IPAL19397 : addText moved to handleReturn() method
    //if ( !txt.isEmpty() )
    //  addText( "", true );
    // set "ready" prompt
    myPrompt = READY_PROMPT;
    addText( myPrompt );
    // unset busy cursor
    unsetCursor();
    // stop event loop (if running)
    if ( myEventLoop )
      myEventLoop->exit();
    // if we are in multi_paste_mode, process the next item
    multiLineProcessNextLine();
    break;
  }
  case PyInterp_Event::ES_INCOMPLETE:
  {
    // extend command buffer (multi-line command)
    myCommandBuffer.append( "\n" );
    // add caret return line if necessary
    QTextBlock par = document()->end().previous();
    QString txt = par.text();
    txt.truncate( txt.length() - 1 );
    // IPAL19397 : addText moved to handleReturn() method
    //if ( !txt.isEmpty() )
    //  addText( "", true );
    // set "dot" prompt
    myPrompt = DOTS_PROMPT;
    addText( myPrompt/*, true*/ ); // IPAL19397
    // unset busy cursor
    unsetCursor();
    // stop event loop (if running)
    if ( myEventLoop )
      myEventLoop->exit();
    // if we are in multi_paste_mode, process the next item
    multiLineProcessNextLine();
    break;
  }
  default:
    QTextEdit::customEvent( event );
  }
  
  // unset read-only state
  setReadOnly( false );
  // unset history browsing mode
  myCmdInHistory = -1;

  if ( (int)event->type() == (int)PyInterp_Event::ES_OK && myQueue.count() > 0 )
  {
    // process the next sheduled command from the queue (if there is any)
    QString nextcmd = myQueue[0];
    myQueue.pop_front();
    exec( nextcmd );
  }
}

/*!
  \brief "Copy" operation.
  
  Reimplemented from Qt.
  Warning! In Qt this method is not virtual.
*/
void PyConsole_Editor::cut()
{
  QTextCursor aCursor = textCursor();
  if ( aCursor.hasSelection() ) {
    QApplication::clipboard()->setText( aCursor.selectedText() );
    int startSelection = aCursor.selectionStart();
    if ( startSelection < document()->end().previous().position() + promptSize() )
      startSelection = document()->end().previous().position() + promptSize();
    int endSelection = aCursor.selectionEnd();
    if ( endSelection < document()->end().previous().position() + promptSize() )
      endSelection = document()->end().previous().position() + promptSize();
    aCursor.setPosition( startSelection );
    aCursor.setPosition( endSelection, QTextCursor::KeepAnchor );
    horizontalScrollBar()->setValue( horizontalScrollBar()->minimum() );
    setTextCursor( aCursor );
    textCursor().removeSelectedText();
  }
}

/*!
  \brief "Paste" operation.

  Reimplemented from Qt.
  Warning! In Qt this method is not virtual.
*/
void PyConsole_Editor::paste()
{
  QTextCursor aCursor = textCursor();
  if ( aCursor.hasSelection() ) {
    int startSelection = aCursor.selectionStart();
    if ( startSelection < document()->end().previous().position() + promptSize() )
      startSelection = document()->end().previous().position() + promptSize();
    int endSelection = aCursor.selectionEnd();
    if ( endSelection < document()->end().previous().position() + promptSize() )
      endSelection = document()->end().previous().position() + promptSize();
    aCursor.setPosition( startSelection );
    aCursor.setPosition( endSelection, QTextCursor::KeepAnchor );
    horizontalScrollBar()->setValue( horizontalScrollBar()->minimum() );
    setTextCursor( aCursor );
    textCursor().removeSelectedText();
  }
  if ( textCursor().position() < document()->end().previous().position() + promptSize() )
    moveCursor( QTextCursor::End );
  QTextEdit::paste();
}

/*!
  \brief "Clear" operation.

  Reimplemented from Qt.
  Warning! In Qt this method is not virtual.
*/
void PyConsole_Editor::clear()
{
  QTextEdit::clear();
  if ( isShowBanner() )
    addText( banner() );
  myPrompt = READY_PROMPT;
  addText( myPrompt );
}

/*!
  \brief Dumps recorded Python commands to the file
  \param fileName path to the dump file
  \return \c true if dump operation succeeded or \c false otherwise
*/
bool PyConsole_Editor::dump( const QString& fileName )
{
  bool ok = false;
  if ( !fileName.isEmpty() ) {
    QFile file( fileName ); 
    if ( file.open( QFile::WriteOnly ) ) {
      QTextStream out( &file );
      for ( int i = 0; i < myHistory.count(); i++ ) {
        out << myHistory[i] << endl;
      }
      file.close();
      ok = true;
    }
  }
  return ok;
}

/*!
  \brief Dump menu action slot
*/
void PyConsole_Editor::dump()
{
  forever {
    // get file name
    QString fileName = getDumpFileName();

    if ( fileName.isEmpty() )
      break; // cancelled
    
    if ( dump( fileName ) )
      break;
    else
      QMessageBox::warning( this,
                            tr( "WARNING" ),
                            tr( "ERR_FILE_NOT_WRITEABLE" ) );
  }
}

/*!
  \brief Get file name for Dump commands operation.
  
  This function can be redefined in successor classes to show application
  specific dialog box.

  \return path to the dump file
*/
QString PyConsole_Editor::getDumpFileName()
{
  return QFileDialog::getSaveFileName( this,
                                       tr( "GET_DUMP_COMMANDS_FILENAME" ),
                                       QString(),
                                       QString( "%1 (*.py)" ).arg( tr( "PYTHON_SCRIPTS" ) ) );
}

/*!
  \brief Get file name for Log Python trace operation.
  
  This function can be redefined in successor classes to show application
  specific dialog box.

  \return path to the log file
*/
QString PyConsole_Editor::getLogFileName()
{
  return QFileDialog::getSaveFileName( this,
                                       tr( "GET_PYTHON_TRACE_FILENAME" ),
                                       QString(),
                                       QString( "%1 (*.log *.txt)" ).arg( tr( "LOG_FILES" ) ) );
}

/*!
  \brief Start python trace logging
  \param fileName the path to the log file
  \return \c true if operation succeeded or \c false otherwise
          (for example, if file is not writeable)
  \sa stopLog()
 */
bool PyConsole_Editor::startLog( const QString& fileName )
{
  // stop possibly already running logging
  if ( isLogging() )
    stopLog();

  bool ok = false;
  if ( !fileName.isEmpty() ) {
    QFile file( fileName );
    if ( file.open( QFile::WriteOnly ) ) {
      file.close();
      myLogFile = fileName;
      ok = true;
    }
  }
  return ok;
}

/*!
  \brief Start log action slot
*/
void PyConsole_Editor::startLog()
{
  forever {
    // get file name
    QString fileName = getLogFileName();

    if ( fileName.isEmpty() )
      break; // cancelled
    
    if ( startLog( fileName ) )
      break;
    else
      QMessageBox::warning( this,
                            tr( "WARNING" ),
                            tr( "File is not writable" ) );
  }
}

/*!
  \brief Stop log action slot
  
  Stops Python trace logging.
*/
void PyConsole_Editor::stopLog()
{
  myLogFile = QString();
}

/*!
  \brief Put data to the log file
*/
void PyConsole_Editor::putLog( const QString& s )
{
  if ( !myLogFile.isEmpty() ) {
    QFile file( myLogFile );
    if ( !file.open( QFile::Append ) )
      return;
    
    QTextStream out( &file );
    out << s;
    
    file.close();
  }
}

/*!
  \brief Handle properly multi-line pasting. Qt documentation recommends overriding this function.
  If the pasted text doesn't contain a line return, no special treatment is done.
  \param source
*/
void PyConsole_Editor::insertFromMimeData(const QMimeData* source)
{
  if ( myMultiLinePaste )
    return;

  if ( source->hasText() ) {
    QString s = source->text();
    if ( s.contains( "\n" ) )
      multilinePaste( s );
    else
      QTextEdit::insertFromMimeData( source );
  }
  else {
    QTextEdit::insertFromMimeData( source );
  }
}

/*!
  Start multi-line paste operation
  \internal
*/
void PyConsole_Editor::multilinePaste( const QString& s )
{
  // Turn on multi line pasting mode
  myMultiLinePaste = true;

  // Split string data to lines
  QString s2 = s;
  s2.replace( "\r", "" ); // Windows string format converted to Unix style
  QStringList lst = s2.split( QChar('\n'), QString::KeepEmptyParts );

  // Perform the proper paste operation for the first line to handle the case where
  // something was already there
  QMimeData source;
  source.setText( lst[0] );
  QTextEdit::insertFromMimeData( &source );

  // Prepare what will have to be executed after the first line
  myMultiLineContent.clear();
  for ( int i = 1; i < lst.size(); ++i )
    myMultiLineContent.enqueue( lst[i] );

  // Trigger the execution of the first (mixed) line
  handleReturn();

  // See customEvent() and multiLineProcessNext() for the rest of the handling.
}

/*!
  \brief Process the next line in the queue of multi-line paste operation; called
  from customEvent() function
  \internal
*/
void PyConsole_Editor::multiLineProcessNextLine()
{
  // not in multi-line paste mode
  if ( !myMultiLinePaste || myMultiLineContent.isEmpty() )
    return;

  QString line = myMultiLineContent.dequeue();
  if ( myMultiLineContent.empty() )
  {
    // this isa last line in the queue, just paste it
    addText( line, false, false );
    myMultiLinePaste = false;
  }
  else
  {
    // paste the line and simulate a <RETURN> key stroke
    addText( line, false, false );
    handleReturn();
  }
}

/*!
  \brief Clear results of completion
*/
void PyConsole_Editor::clearCompletion()
{
  // delete completion text if present
  if ( myTabMode ) {
    // remove completion display
    document()->undo();
    // remove trailing line return:
    QTextCursor tc( textCursor() );
    tc.setPosition( document()->characterCount()-1 );
    setTextCursor( tc );
    textCursor().deletePreviousChar();
    // TODO: before wait for any <Tab> event to be completed
  }
  myTabMode = false;
}

/*!
 \brief Format completion results - this is where we should create 3 columns etc ...
 \param matches list of possible completions
 \return result string
*/
QString PyConsole_Editor::formatCompletion( const QStringList& matches ) const
{
  static const int MAX_COMPLETIONS = 70;

  QStringList result;
  int sz = matches.size();

  if ( sz > MAX_COMPLETIONS )
    result.append( QString( "[%1]" ).arg( tr( "TOO_MANY_MATCHES" ) ) );

  for ( int i = 0; i < qMin( sz, MAX_COMPLETIONS); ++i )
    result.append( matches[i] );

  return result.join( "\n" );
}

/*!
  \brief Format the doc string in HTML format with the first line in bold blue
  \param doc initial doc string
  \return HTML string
*/
QString PyConsole_Editor::formatDocHTML( const QString& doc ) const
{
  static const char* templ = "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" " \
    "\"http://www.w3.org/TR/REC-html40/strict.dtd\">\n" \
    "<html><head><meta name=\"qrichtext\" content=\"1\" /> " \
    "<style type=\"text/css\">\np, li { white-space: pre-wrap; }\n</style> " \
    "</head><body style=\" font-family:'Sans Serif'; font-size:10pt; font-weight:400; font-style:normal;\">\n" \
    "<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"> " \
    "<span style=\" font-weight:600; color:#0000ff;\">%1</span></p> " \
    "<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">%2</p> " \
    "</body></html>";
  
  QString fst, rest("");

  // Extract first line of doc
  int idx = doc.indexOf( "\n" );
  if ( idx > 0 ) {
    fst = doc.left( idx );
    rest = doc.mid( idx+1 );
  }
  else {
    fst = doc;
  }

  fst = fst.replace( "\n", " " );
  rest = rest.replace(" \n", " " );
  return QString( templ ).arg( fst ).arg( rest );
}

/*!
  \fn void PyConsole_Editor::updateDoc( const QString& doc);
  \brief Signal emitted by the editor widget when the doc string should be updated.
  \param doc a HTML block with the formatted doc string.
  \todo currently this signal is left uncaught.
*/

/*!
 \brief Extract the common leading part of all strings in matches.
 \param matches
 \param result
*/
QString PyConsole_Editor::extractCommon( const QStringList& matches ) const
{
  QString result = "";
  
  if ( matches.size() > 1 ) {
    int l = 0;
    bool ok = true;
    while ( ok && l+1 < matches[0].size() ) {
      QString match = matches[0].left( l+1 );
      for ( int j = 1; j < matches.size() && ok; j++ )
        ok = matches[j].startsWith( match );
      if ( ok )
        l++;
    }
    result = matches[0].left( l );
  }

  return result;
}

/*!
  \brief Useful method to get banner from Python interpreter
  \return banner
*/
QString PyConsole_Editor::banner() const
{
  return myInterp->getBanner().c_str();
}
