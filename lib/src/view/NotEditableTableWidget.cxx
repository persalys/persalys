//                                               -*- C++ -*-
/**
 *  @brief QTableWidget with not editable items
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
#include "otgui/NotEditableTableWidget.hxx"

#include <QHeaderView>
#include <iostream>

namespace OTGUI {

NotEditableTableWidget::NotEditableTableWidget(QWidget * parent)
  : DataTableWidget(parent)
{
  setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  verticalHeader()->resizeSections(QHeaderView::ResizeToContents);
}


NotEditableTableWidget::NotEditableTableWidget(int rows, int columns, QWidget * parent)
  : DataTableWidget(rows, columns, parent)
{
  setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  verticalHeader()->resizeSections(QHeaderView::ResizeToContents);
}


void NotEditableTableWidget::createItem(int row, int column, QString text)
{
  QTableWidgetItem * item = new QTableWidgetItem(text);
  item->setFlags(item->flags() ^ Qt::ItemIsEditable);
  setItem(row, column, item);
}


void NotEditableTableWidget::createItem(int row, int column, double value)
{
  QTableWidgetItem * item = new QTableWidgetItem(QString::number(value));
  item->setFlags(item->flags() ^ Qt::ItemIsEditable);
  setItem(row, column, item);
}


void NotEditableTableWidget::createHeaderItem(int row, int column, QString text)
{
  QTableWidgetItem * item = new QTableWidgetItem(text);
  item->setFlags(item->flags() ^ Qt::ItemIsEditable);
  item->setBackgroundColor(verticalHeader()->palette().color(QPalette::Active, QPalette::Background));
  item->setTextAlignment(Qt::AlignCenter);
  setItem(row, column, item);
}


void NotEditableTableWidget::resizeToContents()
{
  // resize columns To Contents: set optimal size
  resizeColumnsToContents();

  QSize size = sizeHint();
  // width
  int width = 0;
  for (int i=0; i<columnCount(); ++i)
    width += columnWidth(i);
  size.setWidth(width+2);
  // height
  int height = 0;
  if (!horizontalHeader()->isHidden())
    height = horizontalHeader()->height();
  for (int i=0; i<rowCount(); ++i)
    height += rowHeight(i);
  size.setHeight(height+2);

  // resize
  setMinimumSize(size);
  setMaximumSize(size);
  updateGeometry();
}
}