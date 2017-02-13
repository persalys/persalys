//                                               -*- C++ -*-
/**
 *  @brief QWidget for Parameters tab of results windows
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
#include "otgui/ParametersWidget.hxx"

#include "otgui/ParametersTableView.hxx"

#include "openturns/Exception.hxx"

#include <QVBoxLayout>
#include <QGroupBox>

using namespace OT;

namespace OTGUI {

ParametersWidget::ParametersWidget(const QString title,           // table title
                                   const QStringList names,       // parameters names
                                   const QStringList values,      // parameters values
                                   const bool showGrid,           // show the grid of the table
                                   const bool namesHasHeaderType, // parameters names display has table header
                                   QWidget * parent
                                  )
  : QWidget(parent)
{
  if (!names.size() * values.size() > 0)
    throw InvalidArgumentException(HERE) << "To build the ParametersWidget, the data vectors must have the same (not null) dimension\n";

  QVBoxLayout * widgetLayout = new QVBoxLayout(this);

  QGroupBox * groupBox = new QGroupBox;
  QVBoxLayout * groupBoxLayout = new QVBoxLayout(groupBox);

  // title
  if (!title.isEmpty())
    groupBox->setTitle(title);

  // table view
  ParametersTableView * table = new ParametersTableView(names, values, showGrid, namesHasHeaderType);
  groupBoxLayout->addWidget(table);

  groupBoxLayout->addStretch();

  // fill layout
  widgetLayout->addWidget(groupBox);
}
}