//                                               -*- C++ -*-
/**
 *  @brief QAction
 *
 *  Copyright 2015-2019 EDF-Phimeca
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
#include "persalys/Actions.hxx"

namespace PERSALYS
{

Actions::Actions(QObject * parent)
  : QObject(parent)
  , new_(0)
  , open_(0)
  , save_(0)
  , saveAs_(0)
  , importPy_(0)
  , close_(0)
  , exit_(0)
{
  new_ = new QAction(QIcon(":/images/document-new.png"), tr("&New"), parent);
  new_->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_N));
  new_->setShortcutContext(Qt::ApplicationShortcut);
  new_->setStatusTip(tr("Create a new study"));

  open_ = new QAction(QIcon(":/images/document-open.png"), tr("&Open..."), parent);
  open_->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_O));
  open_->setShortcutContext(Qt::ApplicationShortcut);
  open_->setStatusTip(tr("Open an existing study"));

  save_ = new QAction(QIcon(":/images/document-save.png"), tr("Save"), parent);
  save_->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_S));
  save_->setShortcutContext(Qt::ApplicationShortcut);
  save_->setStatusTip(tr("Save the current study"));

  saveAs_ = new QAction(QIcon(":/images/document-save-as.png"), tr("Save As..."), parent);
  saveAs_->setStatusTip(tr("Save the current study with a new name"));

  importPy_ = new QAction(QIcon(":/images/document-import.png"), tr("&Import Python..."), parent);
  importPy_->setStatusTip(tr("Import a Python Script"));

  close_ = new QAction(QIcon(":/images/window-close.png"), tr("Close"), parent);
  close_->setStatusTip(tr("Close the current study"));

  exit_ = new QAction(QIcon(":/images/window-close.png"), tr("E&xit"), parent);
  exit_->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_Q));
  exit_->setShortcutContext(Qt::ApplicationShortcut);
}


QAction * Actions::newAction() const
{
  return new_;
}


QAction * Actions::openAction() const
{
  return open_;
}


QAction * Actions::saveAction() const
{
  return save_;
}


QAction * Actions::saveAsAction() const
{
  return saveAs_;
}


QAction * Actions::importPyAction() const
{
  return importPy_;
}


QAction * Actions::closeAction() const
{
  return close_;
}


QAction * Actions::exitAction() const
{
  return exit_;
}


void Actions::updateActionsAvailability(bool analysisInProgress)
{
  // can not import a Python script when an analysis is running
  importPy_->setDisabled(analysisInProgress);
}
}
