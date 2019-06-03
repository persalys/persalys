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
#ifndef PERSALYS_WIDGETBOUNDTODOCKWIDGET_HXX
#define PERSALYS_WIDGETBOUNDTODOCKWIDGET_HXX

#include "persalys/MainWidget.hxx"

#include <QWidget>

namespace PERSALYS
{
class PERSALYS_API WidgetBoundToDockWidget : public QWidget
{
  Q_OBJECT

public:
  WidgetBoundToDockWidget(QWidget* parent = 0);

  MainWidget * findMainWidgetInHierachy();

  void setDockWidget(QWidget*);

  virtual void showEvent(QShowEvent*);
  virtual void hideEvent(QHideEvent*);

protected:
  void showDockWidget();
  void hideDockWidget();

signals:
  void showDockWidgetRequested(QWidget*);
  void hideDockWidgetRequested(QWidget*);

private:
  QWidget * dockWidget_;
};
}
#endif
