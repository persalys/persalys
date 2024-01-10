//                                               -*- C++ -*-
/**
 *  @brief Main file launching the interface
 *
 *  Copyright 2015-2023 EDF-Phimeca
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

using namespace PERSALYS;

int main(int argc, char *argv[])
{
  int ret = 0;

  // Python Environment
  PythonEnvironment env;
  // make sure sys.stdout/stderr are not None on win32 because the Qt gui is disconnected from console
  const std::string streamCMD = "import sys as _sys; import io as _io; _sys.stdout = _io.StringIO() if _sys.stdout is None else _sys.stdout; _sys.stderr = _io.StringIO() if _sys.stderr is None else _sys.stderr";
  env.runString(streamCMD);
#ifdef PERSALYS_NSIS
  // allow extra modules to load msvc dlls from the Python root dir
  const std::string dllCMD = "import os; os.add_dll_directory(os.environ['PYTHONHOME'])";
  env.runString(dllCMD);
#endif
  {
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

  // increase function cache
  OT::ResourceMap::SetAsUnsignedInteger("Cache-MaxSize", 16384);

  // set number of parallel processes
  if (!QSettings().contains("nProcesses"))
  {
#ifdef _WIN32
    // ProcessPoolExecutor startup penalty is much higher
    QSettings().setValue("nProcesses", 1);
#else
    QSettings().setValue("nProcesses", QThread::idealThreadCount());
#endif
  }

  // main window
  MainWindow window;
  window.resize(1024, 768);
  window.show();

  ret = app.exec();
  }
  return ret;
}
