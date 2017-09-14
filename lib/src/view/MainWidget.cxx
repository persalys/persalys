//                                               -*- C++ -*-
/**
 *  @brief QMainWidget, main window of the interface
 *
 *  Copyright 2015-2017 EDF-Phimeca
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
#include "otgui/MainWidget.hxx"

#include "otgui/WelcomeWindow.hxx"

#include <QSplitter>
#include <QVBoxLayout>

namespace OTGUI {

MainWidget::MainWidget(QWidget* parent)
  : QWidget(parent)
  , studyTree_(new StudyTreeView(this))
  , mdiArea_(new OTguiMdiArea())
  , graphSettingDockWidget_(0)
  , actions_(new OTguiActions(this))
{
  buildInterface();
}


void MainWidget::buildInterface()
{
  // main widget
  QSplitter * mainSplitter = new QSplitter(Qt::Horizontal);

  // left side of the mainSplitter
  QSplitter * leftSideSplitter = new QSplitter(Qt::Vertical);
  leftSideSplitter->addWidget(studyTree_);
  leftSideSplitter->setStretchFactor(0, 8);

  graphSettingDockWidget_ = new QDockWidget(tr("Graph setting"));
  graphSettingDockWidget_->setFeatures(QDockWidget::NoDockWidgetFeatures);
  leftSideSplitter->addWidget(graphSettingDockWidget_);
  graphSettingDockWidget_->close();
  leftSideSplitter->setStretchFactor(1, 2);

  mainSplitter->addWidget(leftSideSplitter);
  mainSplitter->setStretchFactor(0, 0);

  // right side of the mainSplitter
  QSplitter * rightSideSplitter = new QSplitter(Qt::Vertical);

  // welcome page
  WelcomeWindow * welcomeWindow = new WelcomeWindow;
  rightSideSplitter->addWidget(welcomeWindow);
  connect(welcomeWindow, SIGNAL(createNewOTStudy()), studyTree_, SLOT(createNewOTStudy()));
  connect(welcomeWindow, SIGNAL(openOTStudy()), studyTree_, SLOT(openOTStudy()));
  connect(welcomeWindow, SIGNAL(importPython()), actions_->importPyAction(), SIGNAL(triggered()));
  rightSideSplitter->setStretchFactor(0, 5);

  // MdiArea
  connect(studyTree_, SIGNAL(showWindow(OTguiSubWindow *)), mdiArea_, SLOT(showSubWindow(OTguiSubWindow *)));
  connect(studyTree_, SIGNAL(itemSelected(QStandardItem*)), mdiArea_, SLOT(showSubWindow(QStandardItem *)));
  connect(studyTree_, SIGNAL(removeSubWindow(QStandardItem *)), mdiArea_, SLOT(removeSubWindow(QStandardItem *)));
  rightSideSplitter->addWidget(mdiArea_);
  mdiArea_->hide();
  rightSideSplitter->setStretchFactor(1, 4);

  connect(mdiArea_, SIGNAL(mdiAreaEmpty(bool)), welcomeWindow, SLOT(setVisible(bool)));
  connect(mdiArea_, SIGNAL(mdiAreaEmpty(bool)), mdiArea_, SLOT(setHidden(bool)));

  mainSplitter->addWidget(rightSideSplitter);
  mainSplitter->setStretchFactor(1, 3);

  QVBoxLayout *layout = new QVBoxLayout;
  layout->addWidget(mainSplitter);
  setLayout(layout);

  connect(actions_->newAction(), SIGNAL(triggered()), studyTree_, SLOT(createNewOTStudy()));
  connect(actions_->openAction(), SIGNAL(triggered()), studyTree_, SLOT(openOTStudy()));
  connect(actions_->saveAction(), SIGNAL(triggered()), studyTree_, SLOT(saveCurrentOTStudy()));
  connect(actions_->saveAsAction(), SIGNAL(triggered()), studyTree_, SLOT(saveAsCurrentOTStudy()));
  connect(actions_->closeAction(), SIGNAL(triggered()), studyTree_, SLOT(closeOTStudy()));
  connect(studyTree_, SIGNAL(analysisInProgressStatusChanged(bool)), actions_, SLOT(updateActionsAvailability(bool)));
}


StudyTreeView * MainWidget::getStudyTree() const
{
  return studyTree_;
}


OTguiMdiArea * MainWidget::getMdiArea() const
{
  return mdiArea_;
}


OTguiActions * MainWidget::getActions() const
{
  return actions_;
}


void MainWidget::showGraphSettingDockWidget(QWidget * graph)
{
  graphSettingDockWidget_->setWidget(graph);
  graphSettingDockWidget_->show();
}


void MainWidget::hideGraphSettingDockWidget(QWidget * widgetToBeHidden)
{
  // if the current dock widget is not widgetToBeHidden : do nothing
  if (graphSettingDockWidget_->widget() == widgetToBeHidden)
    graphSettingDockWidget_->close();
}
}
