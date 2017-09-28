//                                               -*- C++ -*-
/**
 *  @brief QWidget bound to a dock widget
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
#include "otgui/WidgetBoundToDockWidget.hxx"

#include "otgui/OTguiException.hxx"

using namespace OT;

namespace OTGUI
{

WidgetBoundToDockWidget::WidgetBoundToDockWidget(QWidget* parent)
  : QWidget(parent)
  , dockWidget_(0)
  , dockWidgetAlreadyShown_(false)
  , parentSubWindow_(0)
{
  MainWidget * mainWidget = findMainWidgetInHierachy();
  if (!mainWidget)
    throw InvalidArgumentException(HERE) << "Internal error : No main window found !";

  connect(this, SIGNAL(showDockWidgetRequested(QWidget*)), mainWidget, SLOT(showGraphSettingDockWidget(QWidget*)));
  connect(this, SIGNAL(hideDockWidgetRequested(QWidget*)), mainWidget, SLOT(hideGraphSettingDockWidget(QWidget*)));

  parentSubWindow_ = findSubWindowInHierachy();
  if (!parentSubWindow_)
    throw InvalidArgumentException(HERE) << "Internal error : No mdi sub window found !";
  connect(parentSubWindow_, SIGNAL(windowStateChanged(Qt::WindowStates, Qt::WindowStates)), this, SLOT(updateDockWidgetVisibility(Qt::WindowStates, Qt::WindowStates)));
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


QMdiSubWindow * WidgetBoundToDockWidget::findSubWindowInHierachy()
{
  QObject * curPar(parent());
  while (curPar)
  {
    QMdiSubWindow * isMW(dynamic_cast<QMdiSubWindow *>(curPar));
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

  if (parentSubWindow_->windowState() == Qt::WindowActive ||
      parentSubWindow_->windowState() == (Qt::WindowActive | Qt::WindowMaximized))
  {
    dockWidgetAlreadyShown_ = true;
    emit showDockWidgetRequested(dockWidget_);
  }
}


void WidgetBoundToDockWidget::hideDockWidget()
{
  if (!dockWidget_ || !dockWidgetAlreadyShown_)
    return;

  dockWidgetAlreadyShown_ = false;
  emit hideDockWidgetRequested(dockWidget_);
}


void WidgetBoundToDockWidget::updateDockWidgetVisibility(Qt::WindowStates /*oldState*/, Qt::WindowStates newState)
{
  if ((newState == Qt::WindowActive || newState == (Qt::WindowActive | Qt::WindowMaximized)) &&
      !dockWidgetAlreadyShown_ &&
      isVisible())
  {
    showDockWidget();
  }
  else if (newState == Qt::WindowNoState &&
           dockWidgetAlreadyShown_)
  {
    hideDockWidget();
  }
}
}
