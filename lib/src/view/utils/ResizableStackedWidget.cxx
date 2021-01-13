//                                               -*- C++ -*-
/**
 *  @brief Resizable QStackedWidget
 *
 *  Copyright 2015-2021 EDF-Phimeca
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
#include "persalys/ResizableStackedWidget.hxx"

namespace PERSALYS
{

ResizableStackedWidget::ResizableStackedWidget(QWidget * parent)
  : QStackedWidget(parent)
{
}


QSize ResizableStackedWidget::sizeHint() const
{
  if (currentWidget())
    return currentWidget()->sizeHint();
  return QStackedWidget::sizeHint();
}


QSize ResizableStackedWidget::minimumSizeHint() const
{
  if (currentWidget())
    return currentWidget()->minimumSizeHint();
  return QStackedWidget::minimumSizeHint();
}
}