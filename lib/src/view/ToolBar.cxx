//                                               -*- C++ -*-
/**
 *  @brief QToolBar
 *
 *  Copyright 2015-2023 EDF-Phimeca
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
#include "persalys/ToolBar.hxx"

namespace PERSALYS
{

ToolBar::ToolBar(const Actions* actions, QWidget * parent)
  : QToolBar(parent)
{
  // we set the object name to avoid the following warning
  // when closing the interface :
  // QMainWindow::saveState(): 'objectName' not set for QToolBar ...
  setObjectName("ToolBar");
  buildActions(actions);
}


void ToolBar::buildActions(const Actions* actions)
{
  addAction(actions->newAction());
  addAction(actions->openAction());
  addAction(actions->importPyAction());
  addAction(actions->saveAction());
}
}
