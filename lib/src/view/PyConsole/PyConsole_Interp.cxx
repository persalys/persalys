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
// File   : PyConsole_Interp.cxx
// Author : Nicolas REJNERI (OPEN CASCADE), Adrien BRUNETON (CEA/DEN), Vadim SANDLER (OPEN CASCADE)

#include "PyConsole_Interp.h"

/*!
  \class PyConsole_Interp
  \brief Python interpreter to be embedded to the SALOME study's GUI.

  There is only one Python interpreter for the whole SALOME environment.

  Call the initialize() method defined in the base class PyInterp_Interp,
  to initialize the interpreter after instance creation.

  The method initialize() calls virtuals methods
  - initPython()  to initialize global Python interpreter
  - initContext() to initialize interpreter internal context
  - initRun()     to prepare interpreter for running commands

  See PyInterp_Interp class for more details.
*/

/*!
  \brief Constructor.

  Creates new python interpreter.
*/
PyConsole_Interp::PyConsole_Interp()
  : PyInterp_Interp()
{
}

/*!
  \brief Destructor.
*/
PyConsole_Interp::~PyConsole_Interp()
{
}

/*!
  \brief Performs specific actions before each Python command
  
  Sets the variable "__IN_SALOME_GUI_CONSOLE" to True.
  This is not attached to a module (like salome_iapp.IN_SALOME_GUI_CONSOLE)
  since modules are shared across all interpreters in SALOME.

  \note GIL is already acquired here.
*/
int PyConsole_Interp::beforeRun()
{
  return PyRun_SimpleString("__builtins__.__IN_SALOME_GUI_CONSOLE=True");
}
 
/*!
  \brief Performs specific actions after each Python command

  Sets the variable "__IN_SALOME_GUI_CONSOLE" to False.
  \sa beforeRun()

  \note GIL is already acquired here.
*/
int PyConsole_Interp::afterRun()
{
  return PyRun_SimpleString("__builtins__.__IN_SALOME_GUI_CONSOLE=False");
}

/*!
  \brief Run Python dir() command to get matches.
  \internal
  \param dirArgument Python expression to pass to the dir command. The parsing of what the
  user actually started typing is dedicated to the caller
  \param startMatch string representing the begining of the patter to be completed. For example, when
  the user types "a_string_variable.rsp <TAB>", this is "rsp".
  \param[out] matches resulting list of matches
  \param[out] docString resulting docstring of single match
  \return \true if completion succeeded, \c false otherwise
*/
bool PyConsole_Interp::runDirCommand( const QString& dirArgument, const QString& startMatch, 
                                      QStringList& matches, QString& docString )
{
  static QStringList keywords;
  if ( keywords.isEmpty() ) {
    keywords << "and" << "as" << "assert" << "break" << "class" << "continue"
             << "def" << "del" << "elif" << "else" << "except" << "exec"
             << "finally" << "for" << "from" << "global" << "if" << "import"
             << "in" << "is" << "lambda" << "not" << "or" << "pass" << "print" << "raise"
             << "return" << "try" << "while" << "with" << "yield";
  }
  
  // run dir() command and extract completions
  if ( !runDirAndExtract( dirArgument, startMatch, matches ) )
    return false;
  
  // If dirArgument is empty, we append the __builtins__
  if ( dirArgument.isEmpty() ) {
    if ( !runDirAndExtract( QString( "__builtins__" ), startMatch, matches, false ) )
      return false;
    
    // ... and we match on Python's keywords as well
    foreach( QString keyword, keywords ) {
      if ( keyword.startsWith( startMatch ) )
        matches.append( keyword );
    }
  }
  
  // Try to get doc string of the first match
  if ( matches.size() > 0 ) {
    QString cmd = QString( "%1.__doc__" ).arg( matches[0] );
    if ( !dirArgument.trimmed().isEmpty() )
      cmd.prepend( QString( "%1." ).arg( dirArgument ) );
    
    PyObject* str = PyRun_String( cmd.toStdString().c_str(), Py_eval_input, _global_context, _local_context );
    if ( !str || str == Py_None || !PyString_Check( str ) )
      {
        if ( !str )
          PyErr_Clear();
      }
    else {
      docString = QString( PyString_AsString( str ) );
    }
    Py_XDECREF( str );
  }
  return true;
}

/*!
  \internal
  \sa runDirCommand()
  \param dirArgument see runDirCommand()
  \param startMatch  see runDirCommand()
  \param[out] result resulting list of matches
  \param discardSwig if \c true, a regular expression is used to discard all static method generated
  by SWIG. Typically: MEDCouplingUMesh_Blabla
  \return \c true if the call to dir() and parsing of the result succeeded, \false otherwise.
*/
bool PyConsole_Interp::runDirAndExtract( const QString& dirArgument,
                                         const QString& startMatch,
                                         QStringList& result,
                                         bool discardSwig ) const
{
  QRegExp re( "^[A-Z].+_[A-Z]+[a-z]+.+$" ); // REX to discard SWIG static method, e.g. MEDCouplingUMesh_Blabla

  // Execute dir() command
  QString command( "dir(" + dirArgument + ")" );
  PyObject* plst = PyRun_String( command.toStdString().c_str(), Py_eval_input, _global_context, _local_context );
  if ( !plst || plst == Py_None ) {
    if ( !plst )
      PyErr_Clear();
    Py_XDECREF( plst );
    return false;
  }

  // Check result
  if ( !PySequence_Check( plst ) ) {
    // Should never happen ...
    Py_XDECREF( plst );
    return false;
  }

  // Extract the returned list
  int n = PySequence_Length( plst );
  for ( int i = 0; i < n; i++ ) {
    PyObject* it;
    it = PySequence_GetItem( plst, i );
    QString s( PyString_AsString( it ) );
    // if the method is not from swig, not static (guessed from the reg exp) and matches
    // what is already there
    if ( s.startsWith( startMatch ) ) {
      if ( !discardSwig || ( !re.exactMatch( s ) && !s.contains( "swig" ) ) )
        result.append( s );
    }
    Py_DECREF( it );
  }
  Py_DECREF( plst );
  
  return true;
}
