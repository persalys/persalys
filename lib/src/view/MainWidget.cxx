//                                               -*- C++ -*-
/**
 *  @brief QMainWidget, main window of the interface
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
#include "persalys/MainWidget.hxx"

#include "persalys/WelcomeWindow.hxx"
#include "persalys/QtTools.hxx"
#include "persalys/SubWindow.hxx"

#include <QSplitter>
#include <QVBoxLayout>
#include <QTabBar>

namespace PERSALYS
{

MainWidget::MainWidget(QWidget* parent)
  : QWidget(parent)
  , studyTree_(new StudyTreeView(this))
  , subWindowsStackedWidget_(new SubWindowsStackedWidget(this))
  , graphSettingDockWidget_(new QDockWidget(this))
  , actions_(new Actions(this))
{
  buildInterface();
}


void MainWidget::buildInterface()
{
  QVBoxLayout * widgetLayout = new QVBoxLayout(this);

  // main widget
  QSplitter * mainSplitter = new QSplitter(Qt::Horizontal);

  // left side of the mainSplitter
  QSplitter * leftSideSplitter = new QSplitter(Qt::Vertical);
  // - study tree
  leftSideSplitter->addWidget(studyTree_);
  leftSideSplitter->setStretchFactor(0, 8);

  // - dock widget for graph setting
  graphSettingDockWidget_->setFeatures(QDockWidget::NoDockWidgetFeatures);
  graphSettingDockWidget_->setTitleBarWidget(new TitleLabel(tr("Graph settings")));
  leftSideSplitter->addWidget(graphSettingDockWidget_);
  graphSettingDockWidget_->close();
  leftSideSplitter->setStretchFactor(1, 2);

  mainSplitter->addWidget(leftSideSplitter);
  mainSplitter->setStretchFactor(0, 0);

  // right side of the mainSplitter
  // - welcome page
  WelcomeWindow * welcomeWindow = new WelcomeWindow(actions_, this);
  subWindowsStackedWidget_->addWidget(welcomeWindow);

  mainSplitter->addWidget(subWindowsStackedWidget_);
  mainSplitter->setStretchFactor(1, 3);

  widgetLayout->addWidget(mainSplitter);

  // change style of all the QTabWidget of the application
  QColor background = QTabBar().palette().color(QPalette::Base);
  QString styleSheet = "\
      QTabBar::tab {border: 1px solid #C4C4C3; padding: 8px; border-bottom: 0px; border-radius: 2px;}\
      QTabBar::tab:selected {border-bottom: 2px solid " + ApplicationColor["darkColor"] + "; background: " + background.name() + ";}";
  setStyleSheet(styleSheet);
}


StudyTreeView * MainWidget::getStudyTree() const
{
  return studyTree_;
}


SubWindowsStackedWidget * MainWidget::getSubWindowsStackedWidget() const
{
  return subWindowsStackedWidget_;
}


Actions * MainWidget::getActions() const
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
