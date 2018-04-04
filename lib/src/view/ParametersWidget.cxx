//                                               -*- C++ -*-
/**
 *  @brief QWidget for Parameters tab of results windows
 *
 *  Copyright 2015-2018 EDF-Phimeca
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
#include "otgui/ParametersWidget.hxx"

#include "otgui/ParametersTableView.hxx"
#include "otgui/TranslationManager.hxx"

#include <QVBoxLayout>

using namespace OT;

namespace OTGUI
{

ParametersWidget::ParametersWidget(const QString title,           // table title
                                   const QStringList names,       // parameters names
                                   const QStringList values,      // parameters values
                                   const bool showGrid,           // show the grid of the table
                                   const bool namesHasHeaderType, // parameters names display has table header
                                   QWidget * parent
                                  )
  : QGroupBox(parent)
{
  if (names.size() != values.size() || names.size() * values.size() == 0)
    throw InvalidArgumentException(HERE) << "To build the ParametersWidget, the data vectors must have the same (not null) dimension\n";

  QVBoxLayout * groupBoxLayout = new QVBoxLayout(this);

  // title
  if (!title.isEmpty())
    setTitle(title);

  // table view
  ParametersTableView * table = new ParametersTableView(names, values, showGrid, namesHasHeaderType);
  groupBoxLayout->addWidget(table);
}


ParametersWidget::ParametersWidget(const QString title,                  // table title
                                   const Parameters& analysisParameters, // parameters names and values
                                   QWidget * parent
                                  )
  : QGroupBox(parent)
{
  QStringList names;
  QStringList values;
  for (UnsignedInteger i = 0; i < analysisParameters.getSize(); ++i)
  {
    names << TranslationManager::GetTranslatedParameterName(analysisParameters[i].first);
    values << TranslationManager::GetTranslatedParameterName(analysisParameters[i].second);
  }

  QVBoxLayout * groupBoxLayout = new QVBoxLayout(this);

  // title
  setTitle(title);

  // table view
  ParametersTableView * table = new ParametersTableView(names, values, false, false);
  groupBoxLayout->addWidget(table);
}
}
