//                                               -*- C++ -*-
/**
 *  @brief QStandardItem, observer of a physical model
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
#include "persalys/PhysicalModelDefinitionItem.hxx"

#include "persalys/StudyItem.hxx"

#include <QDebug>

using namespace OT;

namespace PERSALYS
{

PhysicalModelDefinitionItem::PhysicalModelDefinitionItem(const PhysicalModel &model, const PhysicalModelDiagramItem *diagramItem)
  : PhysicalModelItem(model, "PhysicalModelDefinitionItem")
{
  setData(tr("Definition"), Qt::DisplayRole);
  QFont font;
  font.setWeight(QFont::Medium);
  setData(font, Qt::FontRole);
  setEditable(false);

  // add actions
  if (diagramItem)
  {
    appendAction(diagramItem->newProbabilisticModel_);
    appendAction(diagramItem->newDesignOfExperiment_);
    appendAction(diagramItem->newObservations_);
    appendSeparator(tr("Analysis"));
    appendAction(diagramItem->newModelEvaluation_);
    appendAction(diagramItem->newScreening_);
    appendAction(diagramItem->newOptimization_);
  }
}


void PhysicalModelDefinitionItem::update(Observable* /*source*/, const String & message)
{
  // emit signals to PhysicalModelWindowWidget

  // input signals
  if (message == "inputNumberChanged")
  {
    emit numberInputsChanged();
  }
  else if (message == "inputNameChanged")
  {
    emit inputListDefinitionChanged();
  }
  else if (message == "inputDescriptionChanged")
  {
    emit inputListDefinitionChanged();
  }
  else if (message == "inputValueChanged")
  {
    emit inputListDefinitionChanged();
    emit outputChanged();
  }
  else if (message == "inputStepChanged")
  {
    emit inputListDifferentiationChanged();
  }
  // output signals
  else if (message == "outputNumberChanged")
  {
    emit numberOutputsChanged();
  }
  else if (message == "outputNameChanged" ||
           message == "outputDescriptionChanged" ||
           message == "outputFormulaChanged" ||
           message == "outputSelectionChanged" ||
           message == "outputValueChanged"
          )
  {
    emit outputChanged();
  }
  else if (message == "codeChanged")
  {
    emit codeChanged();
  }
  else if (message == "parallelizationStatusChanged")
  {
    emit parallelizationStatusChanged();
  }
  else if (message == "meshChanged")
  {
    emit meshChanged();
  }
  else if (message == "objectRemoved")
  {
    if (hasChildren())
      qDebug() << "PhysicalModelDefinitionItem::update(objectRemoved) has not to contain child\n";
    emit removeRequested(row());
  }
}
}
