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

#include "otgui/MainWindow.hxx"
#ifdef OTGUI_HAVE_YACS
#include "otgui/YACSEvalSessionSingleton.hxx"
#endif

using namespace OTGUI;

int main(int argc, char *argv[])
{
#ifdef OTGUI_HAVE_YACS
  YACSEvalSessionSingleton YACSSession;
#endif
  QApplication app(argc, argv);
  QSettings settings(QSettings::IniFormat, QSettings::UserScope, "EDF_Phimeca", "OTgui");
  settings.setValue("currentDir", QSettings().fileName());

  MainWindow window;
  window.resize(1024, 768);
  window.show();
  window.launchInitialMessageBox();

  return app.exec();
}
