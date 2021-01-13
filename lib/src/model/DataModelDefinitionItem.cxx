//                                               -*- C++ -*-
/**
 *  @brief QStandardItem, observer of data models
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
#include "persalys/DataModelDefinitionItem.hxx"

#include "persalys/StudyItem.hxx"

#include <QDebug>

using namespace OT;

namespace PERSALYS
{

DataModelDefinitionItem::DataModelDefinitionItem(const DesignOfExperiment &doe, const DataModelDiagramItem *diagramItem)
  : DesignOfExperimentItem(doe, "DataModelDefinitionItem")
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
    appendAction(diagramItem->newInferenceAnalysis_);
    appendAction(diagramItem->newCopulaInferenceAnalysis_);
    appendAction(diagramItem->newMetaModel_);
  }
}


void DataModelDefinitionItem::update(Observable* /*source*/, const String& message)
{
  if (message == "variablesChanged")
  {
    // emit signal to DataModelWindow
    emit variablesChanged();
  }
  else if (message == "objectRemoved")
  {
    if (hasChildren())
      qDebug() << "DataModelDefinitionItem::update(objectRemoved) has not to contain child\n";
    emit removeRequested(row());
  }
}
}
