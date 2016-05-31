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
  verticalHeader()->resizeSections(QHeaderView::ResizeToContents);
}


void ResizableTableViewWithoutScrollBar::resizeToContents()
{
  // resize columns To Contents: set optimal size
  resizeColumnsToContents();

  // get the dimensions
  QSize size = sizeHint();
  // -- width
  int width = 0;
  for (int i=0; i<model()->columnCount(); ++i)
    width += columnWidth(i);
  size.setWidth(width+2);
  // -- height
  int height = 0;
  if (!horizontalHeader()->isHidden())
    height = horizontalHeader()->height();
  for (int i=0; i<model()->rowCount(); ++i)
    height += rowHeight(i);
  size.setHeight(height+2);

  // resize
  setMinimumSize(size);
  setMaximumSize(size);
  updateGeometry();
}
}