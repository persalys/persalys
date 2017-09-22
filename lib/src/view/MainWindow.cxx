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

#include <QSplitter>
#include <QDockWidget>
#include <QMessageBox>
#include <QPushButton>
#include <QFileDialog>
#include <QFileInfo>
#include <QSettings>
#include <QApplication>

#include <PyConsole_Interp.h>

#ifdef OTGUI_HAVE_PARAVIEW
#include <vtkSMProxyManager.h>
#include <vtkSMWriterFactory.h>
#include <pqParaViewBehaviors.h>
#include "otgui/PVServerManagerInterface.hxx"
#include "otgui/PVServerManagerSingleton.hxx"
#endif

namespace OTGUI {

MainWindow::MainWindow()
  : QMainWindow()
  , mainWidget_(new MainWidget(this))
  , pythonConsole_(new PyConsole_Console(this))
{
  setWindowTitle("OTGui");
  setWindowIcon(QIcon(":/images/OT_icon32x32.png"));

#ifdef OTGUI_HAVE_PARAVIEW
  new pqParaViewBehaviors(this, this);
  // UpdateAvailableWriters : to be able to export the data from spread sheets
  vtkSMProxyManager::GetProxyManager()->GetWriterFactory()->UpdateAvailableWriters();
  PVServerManagerSingleton::Init(new PVServerManagerInterface);
#endif

  buildInterface();
  buildActions();
}


void MainWindow::buildInterface()
{
  // main widget
  QSplitter * mainSplitter = new QSplitter(Qt::Vertical);

  // Main widget
  mainSplitter->addWidget(mainWidget_);

  // Python Console
  pythonConsole_->getInterp()->decrRef();
  pythonConsole_->setIsShowBanner(false);
  pythonConsole_->setAutoCompletion(true);

  QDockWidget * pythonConsoleDock = new QDockWidget(tr("Python Console"));
  pythonConsoleDock->setWidget(pythonConsole_);
  pythonConsoleDock->setFeatures(QDockWidget::DockWidgetClosable);
  mainSplitter->addWidget(pythonConsoleDock);

  setCentralWidget(mainSplitter);

  // menu bar
  OTguiActions * actions = mainWidget_->getActions();
  OTguiMenuBar * menuBar = new OTguiMenuBar(actions);
  connect(pythonConsoleDock, SIGNAL(visibilityChanged(bool)), menuBar, SIGNAL(pythonConsoleVisibilityChanged(bool)));
  connect(menuBar, SIGNAL(showHidePythonConsole(bool)), pythonConsoleDock, SLOT(setVisible(bool)));
  connect(menuBar, SIGNAL(openOTStudy(QString)), mainWidget_->getStudyTree(), SLOT(openOTStudy(QString)));
  connect(actions->importPyAction(), SIGNAL(triggered()), this, SLOT(importPython()));
  connect(actions->exitAction(), SIGNAL(triggered()), this, SLOT(exitApplication()));
  connect(mainWidget_->getStudyTree(), SIGNAL(recentFilesListChanged(QString)), menuBar, SLOT(updateRecentFilesList(QString)));
  setMenuBar(menuBar);

  // tool bar
  OTguiToolBar * toolBar = new OTguiToolBar(actions);
  addToolBar(toolBar);

  // status bar
  OTguiStatusBar * statusBar = new OTguiStatusBar;
  connect(mainWidget_->getMdiArea(), SIGNAL(errorMessageChanged(QString)), statusBar, SLOT(showErrorMessage(QString)));
  setStatusBar(statusBar);
}


void MainWindow::buildActions()
{
}


void MainWindow::importPython()
{
  if (mainWidget_->getStudyTree()->model()->rowCount())
  {
    int ret = QMessageBox::warning(this, tr("Warning"),
                                   tr("Cannot import a Python script when other studies are opened.\nDo you want to continue and close the other studies?"),
                                   QMessageBox::Cancel | QMessageBox::Ok,
                                   QMessageBox::Ok);
    if (ret == QMessageBox::Ok)
    {
      bool allStudiesClosed = mainWidget_->getStudyTree()->closeAllOTStudies();
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
      QApplication::setOverrideCursor(Qt::WaitCursor);
      const QString command("execfile(\"" + fileName + "\")");
      pythonConsole_->execAndWait(command);
      QApplication::restoreOverrideCursor();
    }
  }
}


void MainWindow::closeEvent(QCloseEvent * event)
{
  int notCanceled = mainWidget_->getStudyTree()->closeAllOTStudies();

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
  int ret = mainWidget_->getStudyTree()->closeAllOTStudies();
  if (ret)
    close();
}
}
