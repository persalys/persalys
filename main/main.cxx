//                                               -*- C++ -*-
/**
 *  @brief Main file launching the interface
 *
 *  Copyright 2015-2016 EDF
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

#include "otgui/MainWindow.hxx"
#include "otgui/PythonEnvironment.hxx"

#ifdef OTGUI_HAVE_YACS
#include "otgui/YACSEvalSessionSingleton.hxx"
#endif

#ifdef OTGUI_HAVE_PARAVIEW
#include <pqPVApplicationCore.h>
#endif

using namespace OTGUI;

int main(int argc, char *argv[])
{
  int ret = 0;

  // Python Environment
  PythonEnvironment env;
  {
  // Application
  QApplication app(argc, argv);

#ifdef OTGUI_HAVE_PARAVIEW
  pqPVApplicationCore appPV(argc, argv);
  QApplication::instance()->installEventFilter(&appPV);
#endif

  // Settings
  QSettings settings(QSettings::IniFormat, QSettings::UserScope, "EDF_Phimeca", "OTgui");
  settings.setValue("currentDir", QSettings().fileName());

  // translations
  QTranslator qtTranslator;
  qtTranslator.load("qt_" + QLocale::system().name(), ":/translations");
  app.installTranslator(&qtTranslator);

  QTranslator qtbaseTranslator;
  qtbaseTranslator.load("qtbase_" + QLocale::system().name(), ":/translations");
  app.installTranslator(&qtbaseTranslator);

  QTranslator appTranslator;
  appTranslator.load("otgui_" + QLocale::system().name(), ":/translations");
  app.installTranslator(&appTranslator);

  // main window
  MainWindow window;
  window.resize(1024, 768);
  window.show();

  ret = app.exec();
#ifdef OTGUI_HAVE_YACS
  YACSEvalSessionSingleton::Reset();
#endif
  }
  return ret;
}
