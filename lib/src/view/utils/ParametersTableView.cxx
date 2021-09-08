//                                               -*- C++ -*-
/**
 *  @brief QWidget to display Parameters in a table
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

#include "persalys/ParametersTableView.hxx"
#include <QHeaderView>

namespace PERSALYS {

ParametersTableView::ParametersTableView(const QStringList names,             // parameters names
                      const QStringList values,            // parameters values
                      const bool showGrid,         // show the grid of the table
                      const bool namesHasHeaderType, // parameters names display has table header
                      QWidget * parent)
    : CopyableTableView(parent)
  {
    if (!names.size() * values.size() > 0)
      throw OT::InvalidArgumentException(HERE) << "To build the ParametersTableView, the data vectors must have the same (not null) dimension\n";

    // headers
    horizontalHeader()->hide();
    verticalHeader()->hide();

    // table model
    CustomStandardItemModel * tableModel = new CustomStandardItemModel(names.size(), 2, this);
    setModel(tableModel);

    // vertical header
    for (int i = 0; i < names.size(); ++i)
    {
      if (namesHasHeaderType)
        tableModel->setNotEditableHeaderItem(i, 0, names[i]);
      else
        tableModel->setNotEditableItem(i, 0, names[i]);
      const int length = 100;
      int pos = 0;
      QString value = values[i];
      // Check if string is long and single lined
      if(values[i].size() > length && !value.contains('\n')) {
        while(pos < values[i].size()) {
          pos += length;
          // add new line
          if(pos < values[i].size())
            value.insert(value.indexOf(QString(','), pos)+1, '\n');
        }
        value.replace(QString(","), QString(", "));
      }
      tableModel->setNotEditableItem(i, 1, value);
    }

    // show grid
    setShowGrid(showGrid);

    // resize to contents
    resizeToContents();
  }
}
