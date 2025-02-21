//                                               -*- C++ -*-
/**
 *  @brief QStandardItem, observer of data models
 *
 *  Copyright 2015-2025 EDF-Phimeca
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
#include "persalys/DataFieldModelDefinitionItem.hxx"
#include "persalys/DataFieldModelDiagramItem.hxx"

#include "persalys/StudyItem.hxx"

#include <QDebug>

using namespace OT;

namespace PERSALYS
{

DataFieldModelDefinitionItem::DataFieldModelDefinitionItem(const DataFieldModel &doe, const DataFieldModelDiagramItem *diagramItem)
  : DataFieldModelItem(doe, "DataFieldModelDefinitionItem")
{
  setData(tr("Definition"), Qt::DisplayRole);
  QFont font;
  font.setWeight(QFont::Medium);
  setData(font, Qt::FontRole);
  setEditable(false);

  if (diagramItem)
  {
    // add actions
    appendSeparator(tr("Analysis"));
    appendAction(diagramItem->newDataAnalysis_);
  }
}


void DataFieldModelDefinitionItem::update(Observable* /*source*/, const String& message)
{
  if (message == "objectRemoved")
  {
    if (hasChildren())
      qDebug() << "DataFieldModelDefinitionItem::update(objectRemoved) has not to contain child\n";
    emit removeRequested(row());
  }
  else if (message == "meshChanged")
  {
    emit meshChanged();
  }
}
}
