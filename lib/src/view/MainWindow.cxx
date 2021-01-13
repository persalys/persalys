//                                               -*- C++ -*-
/**
 *  @brief QMainWindow, main window of the interface
 *
 *  Copyright 2015-2021 EDF-Phimeca
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
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
#include <PyConsole_Interp.h>   // !!! WARNING !!! THIS INCLUDE MUST BE THE VERY FIRST !!!

#include "persalys/MainWindow.hxx"

#include "persalys/MenuBar.hxx"
#include "persalys/ToolBar.hxx"
#include "persalys/StatusBar.hxx"
#include "persalys/QtTools.hxx"

#include <QSplitter>
#include <QSettings>
#include <QApplication>

#ifdef PERSALYS_HAVE_PARAVIEW
#include <vtkSMProxyManager.h>
#include <vtkSMWriterFactory.h>
#include <pqParaViewBehaviors.h>
#include "persalys/PVServerManagerInterface.hxx"
#include "persalys/PVServerManagerSingleton.hxx"
#define PARAVIEW_BUILDING_PLUGIN
#include "vtkPVPlugin.h"
PV_PLUGIN_IMPORT_INIT(XYChartRepresentationColumns)
#endif

namespace PERSALYS
{



MainWindow::MainWindow()
  : QMainWindow()
  , manager_(0)
  , pythonConsole_(new PyConsole_Console(this))
{
  setWindowTitle("Persalys");
  setWindowIcon(QIcon(":/images/persalys.ico"));

#ifdef PERSALYS_HAVE_PARAVIEW
  if (SubWindow::SupportsOpenGL_3_2())
  {
    // from ParaView 5.6 the toolBar is hidden if PersistentMainWindowStateBehavior is enabled
    pqParaViewBehaviors::setEnablePersistentMainWindowStateBehavior(false);
    new pqParaViewBehaviors(this, this);
    // UpdateAvailableWriters : to be able to export the data from spread sheets
    vtkSMProxyManager::GetProxyManager()->GetWriterFactory()->UpdateAvailableWriters();
    PVServerManagerSingleton::Init(new PVServerManagerInterface);

    // import XYChartRepresentationColumns plugin
    // It is used to plot trajectories which can be selected
    PV_PLUGIN_IMPORT(XYChartRepresentationColumns);
  }
#endif

  buildInterface();
}


void MainWindow::buildInterface()
{
  // main widget
  QSplitter * mainSplitter = new QSplitter(Qt::Vertical);
  mainSplitter->setObjectName("splitterMainWindow");
  mainSplitter->setStyleSheet("QSplitter::handle:vertical#splitterMainWindow {height: 2px; background: lightGray;}");

  // Main widget
  MainWidget * mainWidget = new MainWidget(this);
  mainSplitter->addWidget(mainWidget);
  mainSplitter->setStretchFactor(0, 10);

  // set manager_
  manager_ = new StudyManager(mainWidget, this);
  connect(manager_, SIGNAL(commandExecutionRequested(QString)), this, SLOT(executePythonCommand(QString)));

  // Python Console
  pythonConsole_->getInterp()->decrRef();
  pythonConsole_->setIsShowBanner(false);
  pythonConsole_->setAutoCompletion(true);

  CustomDockWidget * pythonConsoleDock = new CustomDockWidget(tr("Python Console"));
  pythonConsoleDock->setWidget(pythonConsole_);
  pythonConsoleDock->setFeatures(QDockWidget::DockWidgetClosable);
  pythonConsoleDock->setVisible(QSettings().value("pythonConsoleVisibility", true).toBool());
  mainSplitter->addWidget(pythonConsoleDock);
  mainSplitter->setStretchFactor(1, 1);

  setCentralWidget(mainSplitter);

  // get actions
  Actions * actions = mainWidget->getActions();
  connect(actions->exitAction(), SIGNAL(triggered()), this, SLOT(close()));

  // menu bar
  MenuBar * menuBar = new MenuBar(actions);

  connect(menuBar->pythonConsoleDisplayAction(), SIGNAL(triggered(bool)), pythonConsoleDock, SLOT(setVisible(bool)));
  connect(pythonConsoleDock, SIGNAL(customVisibilityChanged(bool)), menuBar, SLOT(updateConsoleStatus(bool)));
  connect(pythonConsoleDock, SIGNAL(customVisibilityChanged(bool)), menuBar->pythonConsoleDisplayAction(), SLOT(setChecked(bool)));

  connect(menuBar, SIGNAL(openStudy(QString)), manager_, SLOT(open(QString)));
  connect(manager_, SIGNAL(recentFilesListChanged(QString)), menuBar, SLOT(updateRecentFilesList(QString)));
  setMenuBar(menuBar);

  // tool bar
  ToolBar * toolBar = new ToolBar(actions);
  addToolBar(toolBar);

  // status bar
  StatusBar * statusBar = new StatusBar;
  setStatusBar(statusBar);
}


void MainWindow::executePythonCommand(const QString& command)
{
  // execute command in the Python console
  QApplication::setOverrideCursor(Qt::WaitCursor);
  pythonConsole_->execAndWait(command);
  QApplication::restoreOverrideCursor();
}


void MainWindow::closeEvent(QCloseEvent * event)
{
  if (manager_->closeAll())
  {
    event->accept();
    QMainWindow::closeEvent(event);
  }
  else
  {
    event->ignore();
  }
}
}
