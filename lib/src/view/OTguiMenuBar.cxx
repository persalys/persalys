//                                               -*- C++ -*-
/**
 *  @brief QMenuBar
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
#include "otgui/OTguiMenuBar.hxx"

#include <QAction>
#include <QSettings>
#include <QFileInfo>

namespace OTGUI {

OTguiMenuBar::OTguiMenuBar()
  : QMenuBar()
{
  buildActions();
}


void OTguiMenuBar::buildActions()
{
  // File menu
  QMenu * fileMenu = new QMenu(tr("&File"));

  QAction * action = new QAction(QIcon(":/images/document-new.png"), tr("&New"), this);
  action->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_N));
  action->setShortcutContext(Qt::WidgetWithChildrenShortcut);
  action->setStatusTip(tr("Create a new OTStudy"));
  connect(action, SIGNAL(triggered()), this, SIGNAL(createNewOTStudy()));
  fileMenu->addAction(action);

  action = new QAction(QIcon(":/images/document-open.png"), tr("&Open..."), this);
  action->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_O));
  action->setShortcutContext(Qt::WidgetWithChildrenShortcut);
  action->setStatusTip(tr("Open an existing OTStudy"));
  connect(action, SIGNAL(triggered()), this, SIGNAL(openOTStudy()));
  fileMenu->addAction(action);

  for (int i=0; i<NbMaxRecentFiles; ++i)
  {
    recentFileActions_[i] = new QAction(this);
    recentFileActions_[i]->setVisible(false);
    connect(recentFileActions_[i], SIGNAL(triggered()), this, SLOT(openRecentFile()));
  }
  recentFilesMenu_ = fileMenu->addMenu(QIcon(":/images/document-open-recent.png"), tr("Open Recent"));
  for(int i=0; i<NbMaxRecentFiles; ++i)
    recentFilesMenu_->addAction(recentFileActions_[i]);
  updateRecentFilesActionsList();

  recentFilesMenu_->addSeparator();
  action = new QAction(tr("Clear list"), this);
  connect(action, SIGNAL(triggered()), this, SLOT(clearRecentFilesActions()));
  recentFilesMenu_->addAction(action);

  addSeparator();

  action = new QAction(QIcon(":/images/document-save.png"), tr("Save"), this);
  action->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_S));
  action->setShortcutContext(Qt::WidgetWithChildrenShortcut);
  action->setStatusTip(tr("Save the current OTStudy"));
  connect(action, SIGNAL(triggered()), this, SIGNAL(saveOTStudy()));
  fileMenu->addAction(action);

  action = new QAction(QIcon(":/images/document-save-as.png"), tr("Save As..."), this);
  action->setStatusTip(tr("Save the current OTStudy with a new name"));
  connect(action, SIGNAL(triggered()), this, SIGNAL(saveAsOTStudy()));
  fileMenu->addAction(action);

  addSeparator();

  importPythonAction_ = new QAction(QIcon(":/images/document-import.png"), tr("&Import Python..."), this);
  importPythonAction_->setStatusTip(tr("Import a Python Script"));
  connect(importPythonAction_, SIGNAL(triggered()), this, SIGNAL(importPython()));
  fileMenu->addAction(importPythonAction_);

  addSeparator();

  action = new QAction(QIcon(":/images/window-close.png"), tr("Close"), this);
  action->setStatusTip(tr("Close the current OTStudy"));
  connect(action, SIGNAL(triggered()), this, SIGNAL(closeOTStudy()));
  fileMenu->addAction(action);

  action = new QAction(QIcon(":/images/window-close.png"), tr("E&xit"), this);
  action->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_Q));
  action->setShortcutContext(Qt::WidgetWithChildrenShortcut);
  connect(action, SIGNAL(triggered()), this, SIGNAL(closeWindow()));
  fileMenu->addAction(action);

  addMenu(fileMenu);

  // View menu
  QMenu * viewMenu = new QMenu(tr("&View"));
  QMenu * windowMenu = new QMenu(tr("W&indow"));
  action = new QAction(tr("Python Console"), this);
  action->setCheckable(true);
  action->setChecked(true);
  connect(action, SIGNAL(triggered(bool)), this, SIGNAL(showHidePythonConsole(bool)));
  connect(this, SIGNAL(pythonConsoleVisibilityChanged(bool)), action, SLOT(setChecked(bool)));
  windowMenu->addAction(action);
  viewMenu->addMenu(windowMenu);

  addMenu(viewMenu);
}


void OTguiMenuBar::openRecentFile()
{
  QAction * action = qobject_cast<QAction *>(sender());
  if (action)
    emit openOTStudy(action->data().toString());
}



void OTguiMenuBar::updateRecentFilesList(const QString & fileName)
{
  QSettings settings;
  QStringList files = settings.value("recentFilesList").toStringList();
  files.removeAll(fileName);
  files.prepend(fileName);
  while (files.size() > OTguiMenuBar::NbMaxRecentFiles)
    files.removeLast();

  settings.setValue("recentFilesList", files);
  updateRecentFilesActionsList();
}


void OTguiMenuBar::updateRecentFilesActionsList()
{
  QSettings settings;
  QStringList files = settings.value("recentFilesList").toStringList();

  int numRecentFiles = qMin(files.size(), (int)NbMaxRecentFiles);

  for (int i=0; i<numRecentFiles; ++i)
  {
    QFileInfo info = QFileInfo(files[i]);
    QString text = QString("%1  [%2]").arg(info.baseName()).arg(info.absoluteFilePath());;
    recentFileActions_[i]->setText(text);
    recentFileActions_[i]->setData(files[i]);
    recentFileActions_[i]->setVisible(true);
  }
  for (int i=numRecentFiles; i<NbMaxRecentFiles; ++i)
    recentFileActions_[i]->setVisible(false);

  recentFilesMenu_->setEnabled(numRecentFiles > 0);
}


void OTguiMenuBar::clearRecentFilesActions()
{
  QSettings settings;
  settings.setValue("recentFilesList", QStringList());
  updateRecentFilesActionsList();
}


void OTguiMenuBar::updateActionsAvailability(const bool analysisInProgress)
{
  // can not import a Python script when an analysis is running
  importPythonAction_->setDisabled(analysisInProgress);
}
}
