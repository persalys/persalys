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
#ifndef OTGUI_NOTEDITABLETABLEWIDGET_HXX
#define OTGUI_NOTEDITABLETABLEWIDGET_HXX

#include "otgui/DataTableWidget.hxx"

namespace OTGUI {
class NotEditableTableWidget : public DataTableWidget
{
  Q_OBJECT

public:
  NotEditableTableWidget(QWidget * parent = 0);
  NotEditableTableWidget(int rows, int columns, QWidget * parent = 0);

  void createItem(int row, int column, QString text);
  void createItem(int row, int column, double value);
  void createHeaderItem(int row, int column, QString text);
  void resizeToContents();
};
}
#endif