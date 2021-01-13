//                                               -*- C++ -*-
/**
 *  @brief QItemDelegate to define Python script
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
#ifndef PERSALYS_RESIZABLESTACKEDWIDGET_HXX
#define PERSALYS_RESIZABLESTACKEDWIDGET_HXX

#include <QStackedWidget>
#include "persalys/PersalysPrivate.hxx"

namespace PERSALYS
{
class PERSALYS_UTILS_API ResizableStackedWidget : public QStackedWidget
{
  Q_OBJECT

public:
  ResizableStackedWidget(QWidget * parent = 0);

  QSize sizeHint() const;
  QSize minimumSizeHint() const;
};
}
#endif
