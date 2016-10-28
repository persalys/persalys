//                                               -*- C++ -*-
/**
 *  @brief Welcome window of otgui
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
#include "otgui/WelcomeWindow.hxx"

#include <QGridLayout>
#include <QLabel>
#include <QPushButton>

namespace OTGUI {

WelcomeWindow::WelcomeWindow()
  : QWidget()
{
  buildInterface();
}


void WelcomeWindow::buildInterface()
{
  QVBoxLayout * mainLayout = new QVBoxLayout(this);
  mainLayout->addWidget(new QLabel(tr("To get started, select one action by pressing the corresponding button below.")), 0, Qt::AlignCenter);

  QHBoxLayout * hlayout = new QHBoxLayout;
  hlayout->addStretch();
  QGridLayout * layout = new QGridLayout;

  QPushButton * button = new QPushButton(tr("New study"));
  button->setIcon(QIcon(":/images/document-new22x22.png"));
  button->setStatusTip(tr("Create a new OTStudy"));
  connect(button, SIGNAL(clicked(bool)), this, SIGNAL(createNewOTStudy()));
  layout->addWidget(button, 0, 0);

  button = new QPushButton(tr("Open study"));
  button->setIcon(QIcon(":/images/document-open22x22.png"));
  button->setStatusTip(tr("Open an existing OTStudy"));
  connect(button, SIGNAL(clicked(bool)), this, SIGNAL(openOTStudy()));
  layout->addWidget(button, 1, 0);

  button = new QPushButton(tr("Import Python script"));
  button->setIcon(QIcon(":/images/document-import22x22.png"));
  button->setStatusTip(tr("Import a Python Script"));
  connect(button, SIGNAL(clicked(bool)), this, SIGNAL(importPython()));
  layout->addWidget(button, 2, 0);

  layout->setRowStretch(3, 1);
  hlayout->addLayout(layout);
  hlayout->addStretch();

  mainLayout->addLayout(hlayout);
}
}