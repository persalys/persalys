//                                               -*- C++ -*-
/**
 *  @brief QMenuBar
 *
 *  Copyright 2015-2022 EDF-Phimeca
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
#include "persalys/MenuBar.hxx"

#include "persalys/AboutDialog.hxx"
#include "persalys/SettingsDialog.hxx"
#include "persalys/FileTools.hxx"

#include <QSettings>
#include <QFileInfo>
#include <QDir>
#include <QUrl>
#include <QDesktopServices>
#include <QMessageBox>

namespace PERSALYS
{

MenuBar::MenuBar(const Actions* actions)
  : QMenuBar()
  , recentFilesMenu_(0)
  , recentFileActions_()
{
  buildActions(actions);
}


void MenuBar::buildActions(const Actions* actions)
{
  // File menu
  QMenu * fileMenu = new QMenu(tr("&File"), this);
  QAction * action;

  fileMenu->addAction(actions->newAction());
  fileMenu->addAction(actions->openAction());

  for (int i = 0; i < NbMaxRecentFiles; ++i)
  {
    recentFileActions_[i] = new QAction(this);
    recentFileActions_[i]->setVisible(false);
    connect(recentFileActions_[i], SIGNAL(triggered()), this, SLOT(openRecentFile()));
  }
  recentFilesMenu_ = fileMenu->addMenu(QIcon(":/images/document-open-recent.png"), tr("Open Recent"));
  for(int i = 0; i < NbMaxRecentFiles; ++i)
    recentFilesMenu_->addAction(recentFileActions_[i]);
  updateRecentFilesActionsList();

  recentFilesMenu_->addSeparator();
  action = new QAction(tr("Clear list"), this);
  connect(action, SIGNAL(triggered()), this, SLOT(clearRecentFilesActions()));
  recentFilesMenu_->addAction(action);

  fileMenu->addSeparator();

  fileMenu->addAction(actions->saveAction());
  fileMenu->addAction(actions->saveAsAction());

  fileMenu->addSeparator();

  fileMenu->addAction(actions->importPyAction());

  fileMenu->addSeparator();

  fileMenu->addAction(actions->closeAction());
  fileMenu->addAction(actions->exitAction());

  addMenu(fileMenu);

  // Tools menu
  QMenu * toolsMenu = new QMenu(tr("&Tools"), this);
  pythonConsoleDisplayAction_ = new QAction(tr("Python Console"), this);
  pythonConsoleDisplayAction_->setCheckable(true);
  toolsMenu->addAction(pythonConsoleDisplayAction_);

  action = new QAction(QIcon(":/images/run-build.png"), tr("Settings"), this);
  connect(action, SIGNAL(triggered()), this, SLOT(openSettingsWindow()));
  toolsMenu->addAction(action);

  addMenu(toolsMenu);

  // Help menu
  QMenu * helpMenu = new QMenu(tr("&Help"), this);

  action = new QAction(QIcon(":/images/help-contents.png"), tr("User's manual"), this);
  connect(action, SIGNAL(triggered()), this, SLOT(openUserManual()));
  helpMenu->addAction(action);

  helpMenu->addSeparator();

  action = new QAction(QIcon(":/images/persalys.ico"), tr("About Persalys"), this);
  connect(action, SIGNAL(triggered()), this, SLOT(openAboutDialog()));
  helpMenu->addAction(action);

  addMenu(helpMenu);
}


QAction * MenuBar::pythonConsoleDisplayAction() const
{
  return pythonConsoleDisplayAction_;
}


void MenuBar::openRecentFile()
{
  QAction * action = qobject_cast<QAction *>(sender());
  if (action)
    emit openStudy(action->data().toString());
}


void MenuBar::updateRecentFilesList(const QString & fileName)
{
  QSettings settings;
  QStringList files = settings.value("recentFilesList").toStringList();
  files.removeAll(fileName);
  files.prepend(fileName);
  while (files.size() > MenuBar::NbMaxRecentFiles)
    files.removeLast();

  settings.setValue("recentFilesList", files);
  updateRecentFilesActionsList();
}


void MenuBar::updateRecentFilesActionsList()
{
  QSettings settings;
  QStringList files = settings.value("recentFilesList").toStringList();

  int numRecentFiles = qMin(files.size(), (int)NbMaxRecentFiles);

  for (int i = 0; i < numRecentFiles; ++i)
  {
    QFileInfo info = QFileInfo(files[i]);
    QString text = QString("%1  [%2]").arg(info.baseName()).arg(info.absoluteFilePath());;
    recentFileActions_[i]->setText(text);
    recentFileActions_[i]->setData(files[i]);
    recentFileActions_[i]->setVisible(true);
  }
  for (int i = numRecentFiles; i < NbMaxRecentFiles; ++i)
    recentFileActions_[i]->setVisible(false);

  recentFilesMenu_->setEnabled(numRecentFiles > 0);
}


void MenuBar::clearRecentFilesActions()
{
  QSettings settings;
  settings.setValue("recentFilesList", QStringList());
  updateRecentFilesActionsList();
}


void MenuBar::openUserManual()
{
  const QString userManualFile(QDir::toNativeSeparators(FileTools::GetDocumentationDirectoryPath() + "/index.html"));

  // if file exists
  if (QFileInfo(userManualFile).exists())
  {
    FileTools::OpenUrl(QUrl::fromLocalFile(userManualFile));
  }
  else
  {
    QMessageBox::critical(this,
                          tr("Error"),
                          tr("Impossible to open the documentation. The requested file '%1' is not available.").arg(userManualFile));
  }
}


void MenuBar::openAboutDialog()
{
  AboutDialog aboutDialog(this);
  aboutDialog.exec();
}


void MenuBar::updateConsoleStatus(const bool visibility)
{
  QSettings settings;
  settings.setValue("pythonConsoleVisibility", visibility);
}

void MenuBar::openSettingsWindow() {
  SettingsDialog * settingsDialog = new SettingsDialog(this);
  connect(settingsDialog, &QDialog::accepted, [=]() {
      QSettings settings;
      settings.setValue("nProcesses", QVariant((uint)settingsDialog->getnProcesses()));
    });
  settingsDialog->exec();
}

}
