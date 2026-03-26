//                                               -*- C++ -*-
/**
 *  @brief Main file launching the interface
 *
 *  Copyright 2015-2026 EDF-Phimeca
 *
 *  This library is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
#include <QApplication>
#include <QSettings>
#include <QTranslator>
#include <QThread>

#include "persalys/MainWindow.hxx"
#include "persalys/PythonEnvironment.hxx"

#ifdef PERSALYS_HAVE_PARAVIEW
#include <QSurfaceFormat>
#include <pqPVApplicationCore.h>
#include <QVTKRenderWindowAdapter.h>
#endif

#include <openturns/ResourceMap.hxx>

#ifdef PERSALYS_STACKTRACE
#define BOOST_STACKTRACE_USE_BACKTRACE
#include <boost/stacktrace.hpp>
#include <csignal>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <fstream>
#include <filesystem>

namespace fs = std::filesystem;

static void PrintStack(int signum)
{
  // write stack to stderr
  std::ostringstream oss1;
  oss1 << boost::stacktrace::stacktrace();
  const std::string stackTrace = oss1.str();
  std::cerr << "Stack trace (most recent call first):" << std::endl << stackTrace << std::flush;

  // write stack trace into file
  std::string fileName = "persalys_stack_trace_";
  try
  {
    std::ostringstream oss2;
    std::time_t currentTime = std::time(nullptr);
    oss2 << std::put_time(std::localtime(&currentTime), "%Y_%m_%d_%H_%M_%S.txt");
    fileName += oss2.str();
    fs::path userbase_dir = fs::current_path();
#ifdef _WIN32
    const char* env_appdata = std::getenv("APPDATA");
    if (env_appdata)
      userbase_dir = fs::canonical(env_appdata) / "Persalys";
#else
    const char* env_home = std::getenv("HOME");
    if (env_home)
      userbase_dir = fs::canonical(env_home) / ".persalys_base";
#endif
    if (userbase_dir != fs::current_path())
    {
      fs::create_directory(userbase_dir);
      fileName = (userbase_dir / fileName).string();
    }
    std::ofstream stackFile;
    stackFile.open(fileName);
    if (stackFile.good())
    {
      stackFile << stackTrace;
      stackFile.close();
      std::cerr << "Written stack trace into " << fileName << std::endl << std::flush;
    }
    else
      std::cerr << "Cannot write into " << fileName << std::endl << std::flush;
  }
  catch (const std::exception & exc)
  {
    std::cerr << "Failed to write stack trace into " << fileName << ": " << exc.what() << std::endl << std::flush;
  }

  // Restore default handler and re-raise the signal to avoid infinite loop
  std::signal(signum, SIG_DFL);
  std::raise(signum);
}
#endif

using namespace PERSALYS;

int main(int argc, char *argv[])
{
#ifdef PERSALYS_STACKTRACE
  std::signal(SIGSEGV, PrintStack);
  std::signal(SIGABRT, PrintStack);
#endif

  try
  {
    // Python Environment
    PythonEnvironment env;
    // make sure sys.stdout/stderr are not None on win32 because the Qt gui is disconnected from console
    env.runString("import sys as _sys; import io as _io; _sys.stdout = _io.StringIO() if _sys.stdout is None else _sys.stdout; _sys.stderr = _io.StringIO() if _sys.stderr is None else _sys.stderr");
#ifdef PERSALYS_NSIS
    // allow extra modules to load msvc dlls from the Python root dir
    env.runString("import os; os.add_dll_directory(os.environ['PYTHONHOME'])");
#endif

    // Application
    QApplication app(argc, argv);

#ifdef PERSALYS_HAVE_PARAVIEW
    QSurfaceFormat::setDefaultFormat(QVTKRenderWindowAdapter::defaultFormat(false));
    pqPVApplicationCore appPV(argc, argv);
    QApplication::instance()->installEventFilter(&appPV);
#endif

    // Settings
    QCoreApplication::setOrganizationName("Persalys");
    QCoreApplication::setApplicationName("persalys");
    QSettings::setDefaultFormat(QSettings::IniFormat);
    if (!QSettings().contains("currentDir"))
      QSettings().setValue("currentDir", QSettings().fileName());

    // translations
    QTranslator qtTranslator;
    if (qtTranslator.load("qt_" + QLocale::system().name(), ":/translations"))
      app.installTranslator(&qtTranslator);

    QTranslator qtbaseTranslator;
    if (qtbaseTranslator.load("qtbase_" + QLocale::system().name(), ":/translations"))
      app.installTranslator(&qtbaseTranslator);

    QTranslator appTranslator;
    if (appTranslator.load("persalys_" + QLocale::system().name(), ":/translations"))
      app.installTranslator(&appTranslator);

    QTranslator pyConsoleTranslator;
    if (pyConsoleTranslator.load("PyConsole_msg_" + QLocale::system().name(), ":/translations"))
      app.installTranslator(&pyConsoleTranslator);

    // main window
    MainWindow window;
    window.resize(1024, 768);
    window.show();

    return app.exec();
  }
  catch (const std::exception& exc)
  {
    std::cerr << exc.what() << std::endl;
#ifdef PERSALYS_STACKTRACE
    PrintStack(0);
#endif
    return EXIT_FAILURE;
  }
}
