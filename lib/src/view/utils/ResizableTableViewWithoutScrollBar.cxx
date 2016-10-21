//                                               -*- C++ -*-
/**
 *  @brief TableView with no scrollbar
 *
 *  Copyright 2015-2016 EDF-Phimeca
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
#include "otgui/ResizableTableViewWithoutScrollBar.hxx"

#include <QHeaderView>

namespace OTGUI {

ResizableTableViewWithoutScrollBar::ResizableTableViewWithoutScrollBar(QWidget * parent)
  : CopyableTableView(parent)
{
  setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

#if QT_VERSION >= 0x050000
  horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
  verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
#else
  horizontalHeader()->setResizeMode(QHeaderView::Fixed);
  verticalHeader()->setResizeMode(QHeaderView::Fixed);
#endif
}


QSize ResizableTableViewWithoutScrollBar::sizeHint() const
{
  return minimumSize();
}


QSize ResizableTableViewWithoutScrollBar::minimumSizeHint() const
{
  return minimumSize();
}


void ResizableTableViewWithoutScrollBar::resizeToContents()
{
  // resize columns To Contents: set optimal size
  resizeColumnsToContents();
  verticalHeader()->resizeSections(QHeaderView::ResizeToContents);

  // resize table
  const int w = horizontalHeader()->length() + verticalHeader()->width();
  const int h = verticalHeader()->length() + horizontalHeader()->height();
  int x1, y1, x2, y2;
  getContentsMargins(&x1, &y1, &x2, &y2);
  setFixedSize(w+x1+x2, h+y1+y2);
}
}