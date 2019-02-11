//                                               -*- C++ -*-
/**
 *  @brief QStackedWidget containing all sub windows
 *
 *  Copyright 2015-2018 EDF-Phimeca
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
#include "otgui/SubWindowsStackedWidget.hxx"

#include <QDebug>

namespace OTGUI
{

SubWindowsStackedWidget::SubWindowsStackedWidget(QWidget * parent)
  : QStackedWidget(parent)
{
}


int SubWindowsStackedWidget::addSubWindow(SubWindow * win)
{
  // connections
  connect(win, SIGNAL(showWindowRequested(QWidget*)), this, SLOT(setCurrentWidget(QWidget*)));
  connect(win, SIGNAL(removeWindowRequested(QWidget*)), this, SLOT(removeSubWindow(QWidget*)));

  int ret = QStackedWidget::addWidget(win);
  setCurrentWidget(win);

  return ret;
}


void SubWindowsStackedWidget::removeSubWindow(QWidget* win)
{
  QStackedWidget::removeWidget(win);
  delete win;
}
}
