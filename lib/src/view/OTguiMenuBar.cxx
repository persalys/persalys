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
  // File menu
  QMenu * fileMenu = new QMenu(tr("&File"));

  QAction * action = new QAction(QIcon(":/images/document-new.png"), tr("&New OTStudy"), this);
  action->setStatusTip(tr("Create a new OTStudy"));
  connect(action, SIGNAL(triggered()), this, SIGNAL(createNewOTStudy()));
  fileMenu->addAction(action);

  action = new QAction(QIcon(":/images/document-open.png"), tr("&Open OTStudy..."), this);
  action->setStatusTip(tr("Open an OTStudy"));
  connect(action, SIGNAL(triggered()), this, SIGNAL(openOTStudy()));
  fileMenu->addAction(action);

  action = new QAction(QIcon(":/images/document-save.png"), tr("Save"), this);
  action->setStatusTip(tr("Save the OTStudy"));
  connect(action, SIGNAL(triggered()), this, SIGNAL(saveOTStudy()));
  fileMenu->addAction(action);

  action = new QAction(QIcon(":/images/document-save-as.png"), tr("Save As..."), this);
  action->setStatusTip(tr("Save the OTStudy with a new name"));
  connect(action, SIGNAL(triggered()), this, SIGNAL(saveAsOTStudy()));
  fileMenu->addAction(action);

  addSeparator();

  action = new QAction(tr("Dump OTStudy"), this);
  action->setStatusTip(tr("Dump the OTStudy"));
  connect(action, SIGNAL(triggered()), this, SIGNAL(dumpOTStudy()));
  fileMenu->addAction(action);

  action = new QAction(QIcon(":/images/document-import.png"), tr("&Load Script..."), this);
  action->setStatusTip(tr("Load a Python Script"));
  connect(action, SIGNAL(triggered()), this, SIGNAL(loadOTStudy()));
  fileMenu->addAction(action);

  addSeparator();

  action = new QAction(QIcon(":/images/window-close.png"), tr("E&xit"), this);
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
}