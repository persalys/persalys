//                                               -*- C++ -*-
/**
 *  @brief QMainWindow, main window of the interface
 *
 *  Copyright 2015-2017 EDF-Phimeca
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
#ifndef OTGUI_MAINWINDOW_HXX
#define OTGUI_MAINWINDOW_HXX

#include "otgui/MainWidget.hxx"

#include <QMainWindow>
#include <QCloseEvent>

#include <PyConsole_Console.h>

namespace OTGUI
{
class OTGUI_API MainWindow : public QMainWindow
{
  Q_OBJECT

public:
  MainWindow();

public slots:
  void exitApplication();

protected:
  void buildInterface();

private slots:
  void importPython();
  virtual void closeEvent(QCloseEvent * event);

private:
  MainWidget * mainWidget_;
  PyConsole_Console * pythonConsole_;
};
}
#endif
