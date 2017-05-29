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
#include <QFileDialog>
#include <QFileInfo>
#include <QSettings>

#include <PyConsole_Interp.h>

namespace OTGUI {

MainWindow::MainWindow()
  : QMainWindow()
  , studyTree_(new StudyTreeView(this))
{
  setWindowTitle("OTGui");
  setWindowIcon(QIcon(":/images/OT_icon32x32.png"));

  buildInterface();
  buildActions();
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
  connect(welcomeWindow, SIGNAL(importPython()), this, SLOT(importPython()));
  rightSideSplitter->setStretchFactor(0, 5);

  // MdiArea
  OTguiMdiArea * mdiArea = new OTguiMdiArea;
  connect(studyTree_, SIGNAL(showWindow(OTguiSubWindow *)), mdiArea, SLOT(showSubWindow(OTguiSubWindow *)));
  connect(studyTree_, SIGNAL(itemSelected(QStandardItem*)), mdiArea, SLOT(showSubWindow(QStandardItem *)));
  connect(studyTree_, SIGNAL(removeSubWindow(QStandardItem *)), mdiArea, SLOT(removeSubWindow(QStandardItem *)));
  rightSideSplitter->addWidget(mdiArea);
  mdiArea->hide();
  rightSideSplitter->setStretchFactor(1, 4);

  connect(mdiArea, SIGNAL(mdiAreaEmpty(bool)), welcomeWindow, SLOT(setVisible(bool)));
  connect(mdiArea, SIGNAL(mdiAreaEmpty(bool)), mdiArea, SLOT(setHidden(bool)));

  // Python Console
  pythonConsole_ = new PyConsole_Console(this);
  pythonConsole_->getInterp()->decrRef();
  pythonConsole_->setIsShowBanner(false);
  pythonConsole_->setAutoCompletion(true);

  QDockWidget * pythonConsoleDock = new QDockWidget(tr("Python Console"));
  pythonConsoleDock->setWidget(pythonConsole_);
  pythonConsoleDock->setFeatures(QDockWidget::DockWidgetClosable);
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
  connect(menuBar, SIGNAL(saveOTStudy()), studyTree_, SLOT(saveCurrentOTStudy()));
  connect(menuBar, SIGNAL(saveAsOTStudy()), studyTree_, SLOT(saveAsCurrentOTStudy()));
  connect(menuBar, SIGNAL(importPython()), this, SLOT(importPython()));
  connect(menuBar, SIGNAL(closeOTStudy()), studyTree_, SLOT(closeOTStudy()));
  connect(menuBar, SIGNAL(closeWindow()), this, SLOT(exitApplication()));
  connect(studyTree_, SIGNAL(recentFilesListChanged(QString)), menuBar, SLOT(updateRecentFilesList(QString)));
  connect(studyTree_, SIGNAL(actionsAvailabilityChanged(bool)), menuBar, SLOT(changeActionsAvailability(bool)));
  setMenuBar(menuBar);

  // tool bar
  OTguiToolBar * toolBar = new OTguiToolBar;
  connect(toolBar, SIGNAL(createNewOTStudy()), studyTree_, SLOT(createNewOTStudy()));
  connect(toolBar, SIGNAL(openOTStudy()), studyTree_, SLOT(openOTStudy()));
  connect(toolBar, SIGNAL(importPython()), this, SLOT(importPython()));
  connect(toolBar, SIGNAL(saveOTStudy()), studyTree_, SLOT(saveCurrentOTStudy()));
  connect(studyTree_, SIGNAL(actionsAvailabilityChanged(bool)), toolBar, SLOT(changeActionsAvailability(bool)));
  addToolBar(toolBar);

  // status bar
  OTguiStatusBar * statusBar = new OTguiStatusBar;
  connect(mdiArea, SIGNAL(errorMessageChanged(QString)), statusBar, SLOT(showErrorMessage(QString)));
  setStatusBar(statusBar);
}


void MainWindow::buildActions()
{
  QAction * action = new QAction(tr("&New"), this);
  action->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_N));
  connect(action, SIGNAL(triggered()), studyTree_, SLOT(createNewOTStudy()));
  addAction(action);

  action = new QAction(tr("&Open..."), this);
  action->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_O));
  connect(action, SIGNAL(triggered()), studyTree_, SLOT(openOTStudy()));
  addAction(action);

  action = new QAction(tr("Save"), this);
  action->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_S));
  connect(action, SIGNAL(triggered()), studyTree_, SLOT(saveCurrentOTStudy()));
  addAction(action);

  action = new QAction(QIcon(":/images/window-close.png"), tr("E&xit"), this);
  action->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_Q));
  connect(action, SIGNAL(triggered()), this, SLOT(exitApplication()));
  addAction(action);
}


void MainWindow::importPython()
{
  if (studyTree_->model()->rowCount())
  {
    int ret = QMessageBox::warning(this, tr("Warning"),
                                   tr("Cannot import a Python script when other studies are opened.\nDo you want to continue and close the other studies?"),
                                   QMessageBox::Cancel | QMessageBox::Ok,
                                   QMessageBox::Ok);
    if (ret == QMessageBox::Ok)
    {
      bool allStudiesClosed = studyTree_->closeAllOTStudies();
      if (!allStudiesClosed)
        return;
    }
    else
      return;
  }

  QSettings settings;
  QString currentDir = settings.value("currentDir").toString();
  if (currentDir.isEmpty())
    currentDir = QDir::homePath();
  const QString fileName = QFileDialog::getOpenFileName(this, tr("Import Python..."),
                           currentDir,
                           tr("Python source files (*.py)"));

  if (!fileName.isEmpty())
  {
    QFile file(fileName);
    settings.setValue("currentDir", QFileInfo(fileName).absolutePath());

    // check
    if (!file.open(QFile::ReadOnly))
    {
      QMessageBox::warning(this, tr("Warning"),
                           tr("Cannot read file %1:\n%2").arg(fileName).arg(file.errorString()));
    }
    // load
    {
      const QString command("execfile(\"" + fileName + "\")");
      pythonConsole_->execAndWait(command);
    }
  }
}


void MainWindow::showGraphConfigurationTabWidget(QWidget * graph)
{
  configurationDock_->setWidget(graph);
  configurationDock_->show();
}


void MainWindow::closeEvent(QCloseEvent * event)
{
  int notCanceled = studyTree_->closeAllOTStudies();

  if (notCanceled)
  {
    event->accept();
    QMainWindow::closeEvent(event);
  }
  else
  {
    event->ignore();
  }
}


void MainWindow::exitApplication()
{
  int ret = studyTree_->closeAllOTStudies();
  if (ret)
    close();
}
}
