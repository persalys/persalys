//                                               -*- C++ -*-
/**
 *  @brief QMenuBar
 *
 *  Copyright 2015-2016 EDF
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

namespace OTGUI {

OTguiMenuBar::OTguiMenuBar()
  : QMenuBar()
{
  buildActions();
}


void OTguiMenuBar::buildActions()
{
  QMenu * menu = new QMenu(tr("&Menu"));

  QAction * action = new QAction(QIcon(":/images/document-new.png"), tr("&New OTStudy"), this);
  action->setStatusTip(tr("Create a new OTStudy"));
  connect(action, SIGNAL(triggered()), this, SIGNAL(createNewOTStudy()));
  menu->addAction(action);

  action = new QAction(QIcon(":/images/document-import.png"), tr("&Load OTStudy"), this);
  action->setStatusTip(tr("Load an OTStudy"));
  connect(action, SIGNAL(triggered()), this, SIGNAL(loadOTStudy()));
  menu->addAction(action);

  addSeparator();

  action = new QAction(QIcon(":/images/window-close.png"), tr("E&xit"), this);
  connect(action, SIGNAL(triggered()), this, SIGNAL(closeWindow()));
  menu->addAction(action);

  addMenu(menu);
}
}