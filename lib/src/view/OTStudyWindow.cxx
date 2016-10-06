//                                               -*- C++ -*-
/**
 *  @brief OTStudy
 *
 *  Copyright 2015-2016 EDF-Phimeca
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
#include "otgui/OTStudyWindow.hxx"

#include <QGridLayout>
#include <QLabel>
#include <QPushButton>

namespace OTGUI {

OTStudyWindow::OTStudyWindow(OTStudyItem * item)
  : OTguiSubWindow(item)
{
  buildInterface();
  setWindowFlags(windowFlags() & ~Qt::WindowMaximizeButtonHint & ~Qt::WindowMinimizeButtonHint);
}


void OTStudyWindow::buildInterface()
{
  QWidget * mainWidget = new QWidget;
  QVBoxLayout * mainLayout = new QVBoxLayout(mainWidget);
  mainLayout->addWidget(new QLabel(tr("To get started, select one model type.")), 0, Qt::AlignCenter);

  QHBoxLayout * hlayout = new QHBoxLayout;
  hlayout->addStretch();
  QGridLayout * layout = new QGridLayout;

  QPushButton * button = new QPushButton(tr("New analytical physical model"));
  button->setStatusTip(tr("Create a physical model with outputs defined by analytical formulae"));
  connect(button, SIGNAL(pressed()), this, SIGNAL(createNewAnalyticalPhysicalModel()));
  layout->addWidget(button, 0, 0);

  button = new QPushButton(tr("New Python physical model"));
  button->setStatusTip(tr("Create a physical model defined with a Python script"));
  connect(button, SIGNAL(pressed()), this, SIGNAL(createNewPythonPhysicalModel()));
  layout->addWidget(button, 1, 0);

  int row = 2;
#ifdef OTGUI_HAVE_YACS
  button = new QPushButton(tr("New YACS physical model"));
  button->setStatusTip(tr("Create a physical model defined with an YACS file"));
  connect(button, SIGNAL(pressed()), this, SIGNAL(createNewYACSPhysicalModel()));
  layout->addWidget(button, row, 0);
  ++row;
#endif

  button = new QPushButton(tr("New data model"));
  button->setStatusTip(tr("Import a sample to create a model"));
  connect(button, SIGNAL(pressed()), this, SIGNAL(createNewDataModel()));
  layout->addWidget(button, row, 0);
  ++row;

  layout->setRowStretch(row, 1);
  hlayout->addLayout(layout);
  hlayout->addStretch();

  mainLayout->addLayout(hlayout);

  setWidget(mainWidget);
}
}