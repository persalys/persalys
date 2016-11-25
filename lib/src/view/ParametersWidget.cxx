//                                               -*- C++ -*-
/**
 *  @brief Widget for Parameters tab of the ResultWindows
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

#include "otgui/ResizableTableViewWithoutScrollBar.hxx"
#include "otgui/CustomStandardItemModel.hxx"

#include "openturns/Exception.hxx"

#include <QVBoxLayout>
#include <QHeaderView>
#include <QLabel>

using namespace OT;

namespace OTGUI {

ParametersWidget::ParametersWidget(const QString title, const QStringList names, const QStringList values)
  : QWidget()
{
  if (!names.size() * values.size() > 0)
    throw InvalidArgumentException(HERE) << "To build the widget, the data vectors must have the same (not null) dimension\n";

  QVBoxLayout * layout = new QVBoxLayout(this);

  QLabel * titleLabel = new QLabel(title);
  titleLabel->setWordWrap(false);
  layout->addWidget(titleLabel);

  ResizableTableViewWithoutScrollBar * table = new ResizableTableViewWithoutScrollBar;
  table->horizontalHeader()->hide();
  table->verticalHeader()->hide();
  CustomStandardItemModel * tableModel = new CustomStandardItemModel(names.size(), 2, table);
  table->setModel(tableModel);

  // vertical header
  for (int i=0; i<names.size(); ++i)
  {
    tableModel->setNotEditableItem(i, 0, names[i]);
    tableModel->setNotEditableItem(i, 1, values[i]);
  }
  table->setShowGrid(false);
  table->resizeToContents();
  layout->addWidget(table);
  layout->addStretch();
}
}