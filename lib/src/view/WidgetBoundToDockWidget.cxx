//                                               -*- C++ -*-
/**
 *  @brief QWidget bounds to a dock widget
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
#include "persalys/WidgetBoundToDockWidget.hxx"

#include "persalys/AppliException.hxx"

#include <QVBoxLayout>

using namespace OT;

namespace PERSALYS
{

WidgetBoundToDockWidget::WidgetBoundToDockWidget(QWidget *parent)
  : QWidget(parent)
  , dockWidget_(0)
{
  MainWidget * mainWidget = findMainWidgetInHierachy();
  if (!mainWidget)
    LOGWARN("No main window found ! no graph settings will be displayed");
  else
  {
    connect(this, SIGNAL(showDockWidgetRequested(QWidget*)), mainWidget, SLOT(showGraphSettingDockWidget(QWidget*)));
    connect(this, SIGNAL(hideDockWidgetRequested(QWidget*)), mainWidget, SLOT(hideGraphSettingDockWidget(QWidget*)));
  }
}


WidgetBoundToDockWidget::WidgetBoundToDockWidget(QWidget *mainObject, QWidget *dockWidget, QWidget *parent)
  : WidgetBoundToDockWidget(parent)
{
  QVBoxLayout * widgetLayout = new QVBoxLayout(this);
  widgetLayout->addWidget(mainObject);
  setDockWidget(dockWidget);
}


MainWidget * WidgetBoundToDockWidget::findMainWidgetInHierachy()
{
  QObject * curPar(parent());
  while (curPar)
  {
    MainWidget * isMW(qobject_cast<MainWidget *>(curPar));
    if (isMW)
      return isMW;
    curPar = curPar->parent();
  }
  return 0;
}


void WidgetBoundToDockWidget::setDockWidget(QWidget* dockWidget)
{
  dockWidget_ = dockWidget;
}


void WidgetBoundToDockWidget::showEvent(QShowEvent*)
{
  showDockWidget();
}


void WidgetBoundToDockWidget::hideEvent(QHideEvent*)
{
  hideDockWidget();
}


void WidgetBoundToDockWidget::showDockWidget()
{
  if (!dockWidget_)
    return;
  emit showDockWidgetRequested(dockWidget_);
}


void WidgetBoundToDockWidget::hideDockWidget()
{
  if (!dockWidget_)
    return;
  emit hideDockWidgetRequested(dockWidget_);
}
}
