//                                               -*- C++ -*-
/**
 *  @brief QWidget bounds to a dock widget
 *
 *  Copyright 2015-2019 EDF-Phimeca
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
#include "otgui/WidgetBoundToDockWidget.hxx"

#include "otgui/AppliException.hxx"

using namespace OT;

namespace OTGUI
{

WidgetBoundToDockWidget::WidgetBoundToDockWidget(QWidget* parent)
  : QWidget(parent)
  , dockWidget_(0)
{
  MainWidget * mainWidget = findMainWidgetInHierachy();
  if (!mainWidget)
    throw InvalidArgumentException(HERE) << "Internal error : No main window found !";

  connect(this, SIGNAL(showDockWidgetRequested(QWidget*)), mainWidget, SLOT(showGraphSettingDockWidget(QWidget*)));
  connect(this, SIGNAL(hideDockWidgetRequested(QWidget*)), mainWidget, SLOT(hideGraphSettingDockWidget(QWidget*)));
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
