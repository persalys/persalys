//                                               -*- C++ -*-
/**
 *  @brief Welcome window of persalys
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
#include "persalys/WelcomeWindow.hxx"

#include "persalys/DiagramPushButton.hxx"

#include <QVBoxLayout>
#include <QScrollArea>

namespace PERSALYS
{

WelcomeWindow::WelcomeWindow(const Actions* actions, QWidget * parent)
  : QWidget(parent)
{
  QVBoxLayout * widgetLayout = new QVBoxLayout(this);
  QMargins margins(widgetLayout->contentsMargins());
  margins.setTop(0);
  margins.setBottom(0);
  widgetLayout->setContentsMargins(margins);

  widgetLayout->addWidget(new TitleLabel(tr("Study creation"), "user_manual/graphical_interface/getting_started/user_manual_getting_started.html#create-a-study"));

  QScrollArea * scrollArea = new QScrollArea;
  scrollArea->setWidgetResizable(true);
  widgetLayout->addWidget(scrollArea);

  QWidget * mainWidget = new QWidget;
  QVBoxLayout * mainLayout = new QVBoxLayout(mainWidget);
  scrollArea->setWidget(mainWidget);

  // buttons
  QHBoxLayout * hlayout = new QHBoxLayout;
  hlayout->addStretch();
  QGridLayout * layout = new QGridLayout;

  DiagramPushButton * button = new DiagramPushButton(tr("New study"));
  button->setIcon(QIcon(":/images/document-new22x22.png"));
  button->setStatusTip(tr("Create a new study"));
  connect(button, SIGNAL(clicked(bool)), actions->newAction(), SIGNAL(triggered()));
  layout->addWidget(button, 0, 0);

  button = new DiagramPushButton(tr("Open study"));
  button->setIcon(QIcon(":/images/document-open22x22.png"));
  button->setStatusTip(tr("Open an existing study"));
  connect(button, SIGNAL(clicked(bool)), actions->openAction(), SIGNAL(triggered()));
  layout->addWidget(button, 1, 0);

  button = new DiagramPushButton(tr("Import Python script"));
  button->setIcon(QIcon(":/images/document-import22x22.png"));
  button->setStatusTip(tr("Import a Python Script"));
  connect(button, SIGNAL(clicked(bool)), actions->importPyAction(), SIGNAL(triggered()));
  layout->addWidget(button, 2, 0);

  layout->setRowStretch(3, 1);
  hlayout->addLayout(layout);
  hlayout->addStretch();

  mainLayout->addLayout(hlayout);

  // Persalys logo
  QLabel * imageLabel = new QLabel;
  QPixmap pixmap(":/images/persalys-logo.png");
  imageLabel->setPixmap(pixmap);

  mainLayout->addWidget(imageLabel, 0, Qt::AlignCenter);
}
}
