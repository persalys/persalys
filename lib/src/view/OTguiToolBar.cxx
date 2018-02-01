//                                               -*- C++ -*-
/**
 *  @brief QToolBar
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
 *  You should have received a copy of the GNU Lesser General Public
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
#include "otgui/OTguiToolBar.hxx"

namespace OTGUI
{

OTguiToolBar::OTguiToolBar(const OTguiActions* actions, QWidget * parent)
  : QToolBar(parent)
{
  // we set the object name to avoid the following warning
  // when closing the interface :
  // QMainWindow::saveState(): 'objectName' not set for QToolBar ...
  setObjectName("OTguiToolBar");
  buildActions(actions);
}


void OTguiToolBar::buildActions(const OTguiActions* actions)
{
  addAction(actions->newAction());
  addAction(actions->openAction());
  addAction(actions->importPyAction());
  addAction(actions->saveAction());
}
}
