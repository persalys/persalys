//                                               -*- C++ -*-
/**
 *  @brief Main file launching the interface
 *
 *  Copyright 2015-2019 EDF-Phimeca
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

#include "persalys/MainWindow.hxx"
#include "persalys/PythonEnvironment.hxx"

#ifdef PERSALYS_HAVE_PARAVIEW
#include <QSurfaceFormat>
#include <pqPVApplicationCore.h>
#include <QVTKRenderWindowAdapter.h>
#endif

using namespace PERSALYS;

int main(int argc, char *argv[])
{
  int ret = 0;

  // Python Environment
  PythonEnvironment env;
  {
  // Application
  QApplication app(argc, argv);

#ifdef PERSALYS_HAVE_PARAVIEW
  QSurfaceFormat::setDefaultFormat(QVTKRenderWindowAdapter::defaultFormat(false));
  pqPVApplicationCore appPV(argc, argv);
  QApplication::instance()->installEventFilter(&appPV);
#endif

  // Settings
  QCoreApplication::setOrganizationName("EDF_Phimeca");
  QCoreApplication::setApplicationName("persalys");
  QSettings::setDefaultFormat(QSettings::IniFormat);
  if (!QSettings().contains("currentDir"))
    QSettings().setValue("currentDir", QSettings().fileName());

  // translations
  QTranslator translator;
  translator.load("qt_" + QLocale::system().name(), ":/translations");
  translator.load("qtbase_" + QLocale::system().name(), ":/translations");
  translator.load("persalys_" + QLocale::system().name(), ":/translations");
  app.installTranslator(&translator);

  // main window
  MainWindow window;
  window.resize(1024, 768);
  window.show();

  ret = app.exec();
  }
  return ret;
}
