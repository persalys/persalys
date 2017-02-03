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
//  File   : PyInterp_Interp.cxx
//  Author : Christian CAREMOLI, Paul RASCLE, Adrien BRUNETON

#include "PyInterp_Interp.h"  // !!! WARNING !!! THIS INCLUDE MUST BE THE VERY FIRST !!!
#include "PyInterp_Utils.h"

#include <pythread.h>
#include <cStringIO.h>
#include <structmember.h>
#include <string>
#include <vector>
#include <map>
#include <iostream>
#include <sstream>
#include <algorithm>

#include <QRegExp>

#define TOP_HISTORY_PY   "--- top of history ---"
#define BEGIN_HISTORY_PY "--- begin of history ---"

/*
  The following functions are used to hook the Python
  interpreter output.
*/

static void
PyStdOut_dealloc(PyStdOut *self)
{
  PyObject_Del(self);
}

static PyObject*
PyStdOut_write(PyStdOut *self, PyObject *args)
{
  char *c;
  int l;
  if (!PyArg_ParseTuple(args, "t#:write",&c, &l))
    return NULL;
  if(self->_cb==NULL) {
    if ( self->_iscerr )
      std::cerr << c ;
    else
      std::cout << c ;
  }
  else {
    self->_cb(self->_data,c);
  }
  Py_INCREF(Py_None);
  return Py_None;
}

static PyObject*
PyStdOut_flush(PyStdOut *self)
{
  Py_INCREF(Py_None);
  return Py_None;
}

static PyMethodDef PyStdOut_methods[] = {
  {"write",  (PyCFunction)PyStdOut_write,  METH_VARARGS, PyDoc_STR("write(string) -> None")},
  {"flush",  (PyCFunction)PyStdOut_flush,  METH_NOARGS,  PyDoc_STR("flush() -> None")},
  {NULL,    NULL}   /* sentinel */
};

static PyMemberDef PyStdOut_memberlist[] = {
  {(char*)"softspace", T_INT,  offsetof(PyStdOut, softspace), 0,
   (char*)"flag indicating that a space needs to be printed; used by print"},
  {NULL} /* Sentinel */
};

static PyTypeObject PyStdOut_Type = {
  /* The ob_type field must be initialized in the module init function
   * to be portable to Windows without using C++. */
  PyObject_HEAD_INIT(NULL)
  0,                            /*ob_size*/
  "PyOut",                      /*tp_name*/
  sizeof(PyStdOut),             /*tp_basicsize*/
  0,                            /*tp_itemsize*/
  /* methods */
  (destructor)PyStdOut_dealloc, /*tp_dealloc*/
  0,                            /*tp_print*/
  0,                            /*tp_getattr*/
  0,                            /*tp_setattr*/
  0,                            /*tp_compare*/
  0,                            /*tp_repr*/
  0,                            /*tp_as_number*/
  0,                            /*tp_as_sequence*/
  0,                            /*tp_as_mapping*/
  0,                            /*tp_hash*/
  0,                            /*tp_call*/
  0,                            /*tp_str*/
  PyObject_GenericGetAttr,      /*tp_getattro*/
  /* softspace is writable:  we must supply tp_setattro */
  PyObject_GenericSetAttr,      /* tp_setattro */
  0,                            /*tp_as_buffer*/
  Py_TPFLAGS_DEFAULT,           /*tp_flags*/
  0,                            /*tp_doc*/
  0,                            /*tp_traverse*/
  0,                            /*tp_clear*/
  0,                            /*tp_richcompare*/
  0,                            /*tp_weaklistoffset*/
  0,                            /*tp_iter*/
  0,                            /*tp_iternext*/
  PyStdOut_methods,             /*tp_methods*/
  PyStdOut_memberlist,          /*tp_members*/
  0,                            /*tp_getset*/
  0,                            /*tp_base*/
  0,                            /*tp_dict*/
  0,                            /*tp_descr_get*/
  0,                            /*tp_descr_set*/
  0,                            /*tp_dictoffset*/
  0,                            /*tp_init*/
  0,                            /*tp_alloc*/
  0,                            /*tp_new*/
  0,                            /*tp_free*/
  0,                            /*tp_is_gc*/
};

#define PyStdOut_Check(v)  ((v)->ob_type == &PyStdOut_Type)

static PyStdOut* newPyStdOut( bool iscerr )
{
  PyStdOut *self;
  self = PyObject_New(PyStdOut, &PyStdOut_Type);
  if (self == NULL)
    return NULL;
  self->softspace = 0;
  self->_cb = NULL;
  self->_iscerr = iscerr;
  return self;
}

/*!
  \class PyInterp_Interp
  \brief Generic embedded Python interpreter.
*/

int   PyInterp_Interp::_argc   = 1;
char* PyInterp_Interp::_argv[] = {(char*)""};

/*!
  \brief Basic constructor.

  After construction the interpreter instance successor classes
  must call virtual method initalize().
*/
PyInterp_Interp::PyInterp_Interp():
  _vout(0), _verr(0), _local_context(0), _global_context(0), _initialized(false)
{
}

/*!
  \brief Destructor.
*/
PyInterp_Interp::~PyInterp_Interp()
{
  destroy();
}

/*!
  \brief Initialize embedded interpreter.

  This method shoud be called after construction of the interpreter.
  The method initialize() calls virtuals methods
  - initPython()  to initialize global Python interpreter
  - initContext() to initialize interpreter internal context
  - initRun()     to prepare interpreter for running commands
  which should be implemented in the successor classes, according to the
  embedded Python interpreter policy (mono or multi interpreter, etc).
*/
void PyInterp_Interp::initialize()
{
  if ( initialized() )
    return; // prevent repeating intitialization

  _initialized = true;

  _history.clear();       // start a new list of user's commands
  _ith = _history.begin();

  initPython();  // This also inits the multi-threading for Python (but w/o acquiring GIL)

  // ---- The rest of the initialisation process is done hodling the GIL
  PyLockWrapper lck;

  initContext();

  // used to interpret & compile commands - this is really imported here
  // and only added again (with PyImport_AddModule) later on
  PyObjWrapper m(PyImport_ImportModule("codeop"));
  if(!m) {
    PyErr_Print();
    return;
  }

  // Create python objects to capture stdout and stderr
  _vout=(PyObject*)newPyStdOut( false ); // stdout
  _verr=(PyObject*)newPyStdOut( true );  // stderr

  // All the initRun outputs are redirected to the standard output (console)
  initRun();
}

void PyInterp_Interp::destroy()
{
  PyLockWrapper lck;
  closeContext();
}

/*!
  \brief Initialize Python interpreter.

  In case if Python is not initialized, it sets program name, initializes the single true Python
  interpreter, sets program arguments, and initializes threads.
  Otherwise, does nothing. This is important for light SALOME configuration,
  as in full SALOME this is done at SalomeApp level.
  \sa SalomeApp_PyInterp class and main() in SALOME_Session_Server
 */
void PyInterp_Interp::initPython()
{
  if (!Py_IsInitialized()){
    // Python is not initialized
    Py_SetProgramName(_argv[0]);
    Py_Initialize(); // Initialize the interpreter
    PySys_SetArgv(_argc, _argv);

    PyEval_InitThreads(); // Create (and acquire) the Python global interpreter lock (GIL)
    PyEval_ReleaseLock();
  }
}

/*!
  \brief Get embedded Python interpreter banner.
  \return banner string
 */
std::string PyInterp_Interp::getBanner() const
{
  PyLockWrapper lck;
  std::string aBanner("Python ");
  aBanner = aBanner + Py_GetVersion() + " on " + Py_GetPlatform() ;
  aBanner = aBanner + "\ntype help to get general information on environment\n";
  return aBanner;
}

/*!
  \brief Initialize run command.

  This method is used to prepare interpreter for running
  Python commands.

  \return \c true on success and \c false on error
*/
bool PyInterp_Interp::initRun()
{
  return true;
}

/*!
 * Initialize context dictionaries. GIL is held already.
 * The code executed in an embedded interpreter is expected to be run at the module
 * level, in which case local and global context have to be the same dictionary.
 * See: http://stackoverflow.com/questions/12265756/c-python-running-python-code-within-a-context
 * for an explanation.
 */
bool PyInterp_Interp::initContext()
{
  PyObject *m = PyImport_AddModule("__main__");  // interpreter main module (module context)
  if(!m){
    PyErr_Print();
    return false;
  }
  _global_context = PyModule_GetDict(m);          // get interpreter global variable context
  Py_INCREF(_global_context);
  _local_context = _global_context;

  return true;
}

/*!
 * Destroy context dictionaries. GIL is held already.
 */
void PyInterp_Interp::closeContext()
{
  Py_XDECREF(_global_context);
  // both _global_context and _local_context may point to the same Python object
  if ( _global_context != _local_context)
    Py_XDECREF(_local_context);
}

/*!
  \brief Compile Python command and evaluate it in the
         python dictionary contexts if possible. This is not thread-safe.
         This is the caller's responsability to make this thread-safe.
  \internal
  \param command Python command string
  \return -1 on fatal error, 1 if command is incomplete and 0
         if command is executed successfully
 */
static int run_command(const char *command, PyObject * global_ctxt, PyObject * local_ctxt)
{
  PyObject *m = PyImport_AddModule("codeop");
  if(!m) {
    // Fatal error. No way to go on.
    PyErr_Print();
    return -1;
  }

  PyObjWrapper v(PyObject_CallMethod(m,(char*)"compile_command",(char*)"s",command));
  if(!v) {
    // Error encountered. It should be SyntaxError,
    //so we don't write out traceback
    PyObjWrapper exception, value, tb;
    PyErr_Fetch(&exception, &value, &tb);
    PyErr_NormalizeException(&exception, &value, &tb);
    PyErr_Display(exception, value, NULL);
    return -1;
  }
  else if (v == Py_None) {
    // Incomplete text we return 1 : we need a complete text to execute
    return 1;
  }
  else {
    PyObjWrapper r(PyEval_EvalCode((PyCodeObject *)(void *)v,global_ctxt, local_ctxt));
    if(!r) {
      // Execution error. We return -1
      PyErr_Print();
      return -1;
    }
    // The command has been successfully executed. Return 0
    return 0;
  }
}

void replaceAll(std::string& str, const std::string& from, const std::string& to) {
    if(from.empty())
        return;
    size_t start_pos = 0;
    while((start_pos = str.find(from, start_pos)) != std::string::npos) {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length(); // In case 'to' contains 'from', like replacing 'x' with 'yx'
    }
}

std::vector<std::string>
__split(const std::string& str, char delimiter)
{
  std::vector<std::string> internal;
  std::stringstream ss(str); // Turn the string into a stream.
  std::string tok;

  while (getline(ss, tok, delimiter)) {
    internal.push_back(tok);
  }

  return internal;
}

std::string
__join(const std::vector<std::string>& v, int begin=0, int end=-1)
{
  if (end == -1)
    end = v.size();
  std::stringstream ss;
  for (size_t i = begin; i < end; ++i) {
    if (i != begin)
      ss << ",";
    ss << v[i];
  }
  return ss.str();
}

std::vector<std::string>
__getArgsList(std::string argsString)
{
  // Special process if some items of 'args:' list are themselves lists
  // Note that an item can be a list, but not a list of lists...
  // So we can have something like this:
  // myscript.py args:[\'file1\',\'file2\'],\'val1\',\"done\",[1,2,3],[True,False],\"ok\",kwarg1=\'kwarg1\',kwarg2=\'kwarg2\',\'fin\'
  // With such a call, argsString variable contains the string representing ['file1','file2'],'val1','done',[1,2,3],[True,False],'ok',kwarg1='kwarg1',kwarg2='kwarg2','fin'
  // We have to split argsString to obtain a 9 string elements list
  std::vector<std::string> x = __split(argsString, ',');
  bool containsList = (argsString.find('[') != std::string::npos);
  if (containsList) {
    std::vector<int> listBeginIndices, listEndIndices;
    for (int pos = 0; pos < x.size(); ++pos) {
      if (x[pos][0] == '[')
        listBeginIndices.push_back(pos);
      else if (x[pos][x[pos].size()-1] == ']')
        listEndIndices.push_back(pos);
    }
    std::vector<std::string> extractedArgs;
    int start = 0;
    for (int pos = 0; pos < listBeginIndices.size(); ++pos) {
      int lbeg = listBeginIndices[pos];
      int lend = listEndIndices[pos];
      if (lbeg > start)
        for (int k = start; k < lbeg; ++k)
          extractedArgs.push_back(x[k]);
      extractedArgs.push_back(__join(x, lbeg, lend+1));
      start = lend+1;
    }
    if (start < x.size())
      for (int k = start; k < x.size(); ++k)
        extractedArgs.push_back(x[k]);
    return extractedArgs;
  }
  else {
    return x;
  }
}

/*!
  \brief Compile Python command and evaluate it in the
         python dictionary context if possible. Command might correspond to
         the execution of a script with optional arguments.
         In this case, command is:
         execfile(r"/absolute/path/to/script.py [args:arg1,...,argn]")
  \internal
  \param command Python command string
  \param context Python context (dictionary)
  \return -1 on fatal error, 1 if command is incomplete and 0
         if command is executed successfully
 */
static int compile_command(const char *command, PyObject * global_ctxt, PyObject * local_ctxt)
{
  // First guess if command is execution of a script with args, or a simple Python command
  std::string singleCommand = command;
  std::string commandArgs = "";

  QRegExp rx("execfile\\s*\\(.*(args:.*)\"\\s*\\)");
  if (rx.indexIn(command) != -1) {
    commandArgs = rx.cap(1).remove(0,5).toStdString(); // arguments of command
    singleCommand = rx.cap().remove(rx.cap(1)).remove(" ").toStdString(); // command for execution without arguments
  }

  if (commandArgs.empty()) {
    // process command: expression
    // process command: execfile(r"/absolute/path/to/script.py") (no args)
    return run_command(singleCommand.c_str(), global_ctxt, local_ctxt);
  }
  else {
    // process command: execfile(r"/absolute/path/to/script.py [args:arg1,...,argn]")
    std::string script = singleCommand.substr(11); // remove leading execfile(r"
    script = script.substr(0, script.length()-2); // remove trailing ")
    std::vector<std::string> argList = __getArgsList(commandArgs);

    std::string preCommandBegin = "import sys; save_argv = sys.argv; sys.argv=[";
    std::string preCommandEnd = "];";
    std::string completeCommand = preCommandBegin+"\""+script+"\",";
    for (std::vector<std::string>::iterator itr = argList.begin(); itr != argList.end(); ++itr) {
      if (itr != argList.begin())
        completeCommand += ",";
      completeCommand = completeCommand + "\"" + *itr + "\"";
    }
    completeCommand = completeCommand+preCommandEnd+singleCommand+";sys.argv=save_argv";
    return run_command(completeCommand.c_str(), global_ctxt, local_ctxt);
  }
}

/*!
  \brief Run Python command - the command has to fit on a single line (no \n!).
  Use ';' if you need multiple statements evaluated at once.
  \param command Python command
  \return command status
*/
int PyInterp_Interp::run(const char *command)
{
  beforeRun();
  int ret = simpleRun(command);
  afterRun();
  return ret;
}

/**
 * Called before a command is run (when calling run() method). Not thread-safe. Caller's responsability
 * to acquire GIL if needed.
 */
int PyInterp_Interp::beforeRun()
{
  return 0;
}

/**
 * Called after a command is run (when calling run() method). Not thread-safe. Caller's responsability
 * to acquire GIL if needed.
 */
int PyInterp_Interp::afterRun()
{
  return 0;
}

/*!
  \brief Run Python command (used internally). Not thread-safe. GIL acquisition is caller's responsability.
  \param command Python command
  \param addToHistory if \c true (default), the command is added to the commands history
  \return command status
*/
int PyInterp_Interp::simpleRun(const char *command, const bool addToHistory)
{
  if( addToHistory && strcmp(command,"") != 0 ) {
    _history.push_back(command);
    _ith = _history.end();
  }

  // Current stdout and stderr are saved
  PyObject * oldOut = PySys_GetObject((char*)"stdout");
  PyObject * oldErr = PySys_GetObject((char*)"stderr");
  // Keep them alive (PySys_GetObject returned a *borrowed* ref!)
  Py_INCREF(oldOut);
  Py_INCREF(oldErr);

  // Redirect outputs to SALOME Python console before treatment
  PySys_SetObject((char*)"stderr",_verr);
  PySys_SetObject((char*)"stdout",_vout);

  int ier = compile_command(command, _global_context, _local_context);

  // Outputs are redirected to what they were before
  PySys_SetObject((char*)"stdout",oldOut);
  PySys_SetObject((char*)"stderr",oldErr);

  return ier;
}

/*!
  \brief Get previous command in the commands history.
  \return previous command
*/
const char * PyInterp_Interp::getPrevious()
{
  if(_ith != _history.begin()){
    _ith--;
    return (*_ith).c_str();
  }
  else
    return BEGIN_HISTORY_PY;
}

/*!
  \brief Get next command in the commands history.
  \return next command
*/
const char * PyInterp_Interp::getNext()
{
  if(_ith != _history.end()){
    _ith++;
  }
  if (_ith == _history.end())
    return TOP_HISTORY_PY;
  else
    return (*_ith).c_str();
}

/*!
  \brief Set Python standard output device hook.
  \param cb callback function
  \param data callback function parameters
*/
void PyInterp_Interp::setvoutcb(PyOutChanged* cb, void* data)
{
  ((PyStdOut*)_vout)->_cb=cb;
  ((PyStdOut*)_vout)->_data=data;
}

/*!
  \brief Set Python standard error device hook.
  \param cb callback function
  \param data callback function parameters
*/
void PyInterp_Interp::setverrcb(PyOutChanged* cb, void* data)
{
  ((PyStdOut*)_verr)->_cb=cb;
  ((PyStdOut*)_verr)->_data=data;
}

/*!
  \bried Check if the interpreter is initialized
  \internal
*/
bool PyInterp_Interp::initialized() const
{
  return _initialized;
}
