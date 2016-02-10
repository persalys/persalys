//                                               -*- C++ -*-
/**
 *  @brief QMainWindow, main window of the interface
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
#ifndef OTGUI_MAINWINDOW_HXX
#define OTGUI_MAINWINDOW_HXX

#include "otgui/StudyTreeView.hxx"
#include "otgui/qpyconsole.h"
#include "otgui/OTguiMenuBar.hxx"
#include "otgui/OTguiToolBar.hxx"
#include "otgui/OTguiStatusBar.hxx"

#include <QMainWindow>
#include <QMdiArea>
#include <QDockWidget>

namespace OTGUI {
class MainWindow : public QMainWindow
{
  Q_OBJECT

public:
  MainWindow();

  QMdiArea * getMdiArea() const;
  void launchInitialMessageBox();

public slots:
  void showSubWindow(QMdiSubWindow * win);
  void showSubWindow(QStandardItem * item);
  void checkIfWindowResultExists(ObserverItem * item);
  void showGraphConfigurationTabWidget(QWidget*);

protected:
  void buildInterface();
  void buildConnections();

private:
  StudyTreeView * studyTree_;
  QDockWidget * configurationDock_;
  QMdiArea * mdiArea_;
  OTguiMenuBar * menuBar_;
  OTguiToolBar * toolBar_;
  OTguiStatusBar * statusBar_;
  QPyConsole * console_;
  QDockWidget * pythonConsoleDock_;
};
}
#endif