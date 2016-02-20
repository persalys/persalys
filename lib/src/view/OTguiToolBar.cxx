//                                               -*- C++ -*-
/**
 *  @brief QToolBar
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
#include "otgui/OTguiToolBar.hxx"

#include <QAction>

namespace OTGUI {

OTguiToolBar::OTguiToolBar()
  : QToolBar()
{
  buildActions();
}


void OTguiToolBar::buildActions()
{

  QAction * action = new QAction(QIcon(":/images/document-new22x22.png"), tr("&New OTStudy"), this);
  action->setStatusTip(tr("Create a new OTStudy"));
  connect(action, SIGNAL(triggered()), this, SIGNAL(createNewOTStudy()));
  addAction(action);

  action = new QAction(QIcon(":/images/document-open22x22.png"), tr("&Open an OTStudy"), this);
  action->setStatusTip(tr("Open an existing OTStudy"));
  connect(action, SIGNAL(triggered()), this, SIGNAL(openOTStudy()));
  addAction(action);

  action = new QAction(QIcon(":/images/document-import22x22.png"), tr("&Import Python"), this);
  action->setStatusTip(tr("Import a Python Script"));
  connect(action, SIGNAL(triggered()), this, SIGNAL(importPython()));
  addAction(action);

  action = new QAction(QIcon(":/images/document-save22x22.png"), tr("&Save the OTStudy"), this);
  action->setStatusTip(tr("Save the current OTStudy"));
  connect(action, SIGNAL(triggered()), this, SIGNAL(saveOTStudy()));
  addAction(action);
}
}