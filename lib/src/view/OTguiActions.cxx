//                                               -*- C++ -*-
/**
 *  @brief QAction
 *
 *  Copyright 2015-2018 EDF-Phimeca
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
#include "otgui/OTguiActions.hxx"

namespace OTGUI
{

OTguiActions::OTguiActions(QObject * parent)
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
  new_->setStatusTip(tr("Create a new OTStudy"));

  open_ = new QAction(QIcon(":/images/document-open.png"), tr("&Open..."), parent);
  open_->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_O));
  open_->setShortcutContext(Qt::ApplicationShortcut);
  open_->setStatusTip(tr("Open an existing OTStudy"));

  save_ = new QAction(QIcon(":/images/document-save.png"), tr("Save"), parent);
  save_->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_S));
  save_->setShortcutContext(Qt::ApplicationShortcut);
  save_->setStatusTip(tr("Save the current OTStudy"));

  saveAs_ = new QAction(QIcon(":/images/document-save-as.png"), tr("Save As..."), parent);
  saveAs_->setStatusTip(tr("Save the current OTStudy with a new name"));

  importPy_ = new QAction(QIcon(":/images/document-import.png"), tr("&Import Python..."), parent);
  importPy_->setStatusTip(tr("Import a Python Script"));

  close_ = new QAction(QIcon(":/images/window-close.png"), tr("Close"), parent);
  close_->setStatusTip(tr("Close the current OTStudy"));

  exit_ = new QAction(QIcon(":/images/window-close.png"), tr("E&xit"), parent);
  exit_->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_Q));
  exit_->setShortcutContext(Qt::ApplicationShortcut);
}


QAction * OTguiActions::newAction() const
{
  return new_;
}


QAction * OTguiActions::openAction() const
{
  return open_;
}


QAction * OTguiActions::saveAction() const
{
  return save_;
}


QAction * OTguiActions::saveAsAction() const
{
  return saveAs_;
}


QAction * OTguiActions::importPyAction() const
{
  return importPy_;
}


QAction * OTguiActions::closeAction() const
{
  return close_;
}


QAction * OTguiActions::exitAction() const
{
  return exit_;
}


void OTguiActions::updateActionsAvailability(bool analysisInProgress)
{
  // can not import a Python script when an analysis is running
  importPy_->setDisabled(analysisInProgress);
}
}
