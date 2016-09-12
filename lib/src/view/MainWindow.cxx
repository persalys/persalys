//                                               -*- C++ -*-
/**
 *  @brief QMainWindow, main window of the interface
 *
 *  Copyright 2015-2016 EDF-Phimeca
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
#include "otgui/MainWindow.hxx"

#include "otgui/OTguiMenuBar.hxx"
#include "otgui/OTguiToolBar.hxx"
#include "otgui/OTguiStatusBar.hxx"
#include "otgui/OTguiMdiArea.hxx"
#include "otgui/WelcomeWindow.hxx"

#include <QSplitter>
#include <QDockWidget>
#include <QMessageBox>
#include <QPushButton>

namespace OTGUI {

MainWindow::MainWindow()
  : QMainWindow()
  , studyTree_(new StudyTreeView(this))
{
  setWindowTitle("OTGui");
  setWindowIcon(QIcon(":/images/OT_icon32x32.png"));
  buildInterface();
}


void MainWindow::buildInterface()
{
  // main widget
  QSplitter * mainSplitter = new QSplitter(Qt::Horizontal);

  // left side of the mainSplitter
  QSplitter * leftSideSplitter = new QSplitter(Qt::Vertical);
  leftSideSplitter->addWidget(studyTree_);
  leftSideSplitter->setStretchFactor(0, 8);

  configurationDock_ = new QDockWidget(tr("Graph settings"));
  configurationDock_->setFeatures(QDockWidget::NoDockWidgetFeatures);
  connect(studyTree_, SIGNAL(graphWindowActivated(QWidget*)), this, SLOT(showGraphConfigurationTabWidget(QWidget*)));
  connect(studyTree_, SIGNAL(graphWindowDeactivated()), configurationDock_, SLOT(close()));
  leftSideSplitter->addWidget(configurationDock_);
  configurationDock_->close();
  leftSideSplitter->setStretchFactor(1, 2);

  mainSplitter->addWidget(leftSideSplitter);
  mainSplitter->setStretchFactor(0, 0);

  // right side of the mainSplitter
  QSplitter * rightSideSplitter = new QSplitter(Qt::Vertical);

  // welcome page
  WelcomeWindow * welcomeWindow = new WelcomeWindow;
  rightSideSplitter->addWidget(welcomeWindow);
  connect(welcomeWindow, SIGNAL(createNewOTStudy()), studyTree_, SLOT(createNewOTStudy()));
  connect(welcomeWindow, SIGNAL(openOTStudy()), studyTree_, SLOT(openOTStudy()));
  connect(welcomeWindow, SIGNAL(importPython()), studyTree_, SLOT(importPython()));
  rightSideSplitter->setStretchFactor(0, 5);

  // MdiArea
  OTguiMdiArea * mdiArea = new OTguiMdiArea;
  connect(studyTree_, SIGNAL(showWindow(QMdiSubWindow *)), mdiArea, SLOT(showSubWindow(QMdiSubWindow *)));
  connect(studyTree_, SIGNAL(itemSelected(QStandardItem*)), mdiArea, SLOT(showSubWindow(QStandardItem *)));
  connect(studyTree_, SIGNAL(removeSubWindow(QStandardItem *)), mdiArea, SLOT(removeSubWindow(QStandardItem *)));
  rightSideSplitter->addWidget(mdiArea);
  mdiArea->hide();
  rightSideSplitter->setStretchFactor(1, 4);

  connect(mdiArea, SIGNAL(mdiAreaEmpty(bool)), welcomeWindow, SLOT(setVisible(bool)));
  connect(mdiArea, SIGNAL(mdiAreaEmpty(bool)), mdiArea, SLOT(setHidden(bool)));

  // Python Console
  pythonConsole_ = new PyConsole_Console(this);
  pythonConsole_->setIsShowBanner(false);
  pythonConsole_->setAutoCompletion(true);
  QDockWidget * pythonConsoleDock = new QDockWidget(tr("Python Console"));
  pythonConsoleDock->setWidget(pythonConsole_);
  pythonConsoleDock->setFeatures(QDockWidget::DockWidgetClosable);
  connect(studyTree_, SIGNAL(importPythonScript(const QString &)), this, SLOT(loadScript(const QString &)));
  rightSideSplitter->addWidget(pythonConsoleDock);
  rightSideSplitter->setStretchFactor(2, 1);

  mainSplitter->addWidget(rightSideSplitter);
  mainSplitter->setStretchFactor(1, 3);
  setCentralWidget(mainSplitter);

  // menu bar
  OTguiMenuBar * menuBar = new OTguiMenuBar;
  connect(pythonConsoleDock, SIGNAL(visibilityChanged(bool)), menuBar, SIGNAL(pythonConsoleVisibilityChanged(bool)));
  connect(menuBar, SIGNAL(showHidePythonConsole(bool)), pythonConsoleDock, SLOT(setVisible(bool)));
  connect(menuBar, SIGNAL(createNewOTStudy()), studyTree_, SLOT(createNewOTStudy()));
  connect(menuBar, SIGNAL(openOTStudy()), studyTree_, SLOT(openOTStudy()));
  connect(menuBar, SIGNAL(openOTStudy(QString)), studyTree_, SLOT(openOTStudy(QString)));
  connect(menuBar, SIGNAL(saveOTStudy()), studyTree_, SLOT(saveOTStudy()));
  connect(menuBar, SIGNAL(saveAsOTStudy()), studyTree_, SLOT(saveAsOTStudy()));
  connect(menuBar, SIGNAL(exportPython()), studyTree_, SLOT(exportPython()));
  connect(menuBar, SIGNAL(importPython()), studyTree_, SLOT(importPython()));
  connect(menuBar, SIGNAL(closeOTStudy()), studyTree_, SLOT(closeOTStudy()));
  connect(menuBar, SIGNAL(closeWindow()), this, SLOT(exitApplication()));
  connect(studyTree_, SIGNAL(recentFilesListChanged(QString)), menuBar, SLOT(updateRecentFilesList(QString)));
  setMenuBar(menuBar);

  // tool bar
  OTguiToolBar * toolBar = new OTguiToolBar;
  connect(toolBar, SIGNAL(createNewOTStudy()), studyTree_, SLOT(createNewOTStudy()));
  connect(toolBar, SIGNAL(openOTStudy()), studyTree_, SLOT(openOTStudy()));
  connect(toolBar, SIGNAL(importPython()), studyTree_, SLOT(importPython()));
  connect(toolBar, SIGNAL(saveOTStudy()), studyTree_, SLOT(saveOTStudy()));
  addToolBar(toolBar);

  // status bar
  OTguiStatusBar * statusBar = new OTguiStatusBar;
  connect(studyTree_, SIGNAL(errorMessageEmitted(QString)), statusBar, SLOT(showErrorMessage(QString)));
  setStatusBar(statusBar);
}


void MainWindow::loadScript(const QString & fileName)
{
  QString command("execfile(\"" + fileName + "\")");
  pythonConsole_->execAndWait(command);
}

void MainWindow::showGraphConfigurationTabWidget(QWidget * graph)
{
  configurationDock_->setWidget(graph);
  configurationDock_->show();
}


void MainWindow::exitApplication()
{
  int ret = studyTree_->closeAllOTStudies();
  if (ret)
    close();
}
}
