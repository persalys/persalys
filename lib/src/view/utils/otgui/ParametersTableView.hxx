//                                               -*- C++ -*-
/**
 *  @brief QWidget to display Parameters in a table
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
#ifndef OTGUI_PARAMETERSTABLEVIEW_HXX
#define OTGUI_PARAMETERSTABLEVIEW_HXX

#include "otgui/ResizableTableViewWithoutScrollBar.hxx"
#include "otgui/CustomStandardItemModel.hxx"

#include "openturns/Exception.hxx"

#include <QVBoxLayout>
#include <QHeaderView>

namespace OTGUI
{
class OTGUI_API ParametersTableView : public ResizableTableViewWithoutScrollBar
{
public:
  ParametersTableView(const QStringList names,             // parameters names
                      const QStringList values,            // parameters values
                      const bool showGrid = false,         // show the grid of the table
                      const bool namesHasHeaderType = false, // parameters names display has table header
                      QWidget * parent = 0
                     )
    : ResizableTableViewWithoutScrollBar(parent)
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
      tableModel->setNotEditableItem(i, 1, values[i]);
    }

    // show grid
    setShowGrid(showGrid);

    // resize to contents
    resizeToContents();
  }
};
}
#endif
