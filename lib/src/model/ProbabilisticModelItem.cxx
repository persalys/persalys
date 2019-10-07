//                                               -*- C++ -*-
/**
 *  @brief QStandardItem, observer of a probabilistic model
 *
 *  Copyright 2015-2019 EDF-Phimeca
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
#include "persalys/ProbabilisticModelItem.hxx"

#include "persalys/PhysicalModelDiagramItem.hxx"

using namespace OT;

namespace PERSALYS
{

ProbabilisticModelItem::ProbabilisticModelItem(const PhysicalModel &physicalModel, const PhysicalModelDiagramItem *diagramItem)
  : PhysicalModelItem(physicalModel, "ProbabilisticModelItem")
{
  setData(tr("Probabilistic model"), Qt::DisplayRole);
  QFont font;
  font.setWeight(font.weight() + 10);
  setData(font, Qt::FontRole);
  setEditable(false);

  if (diagramItem)
  {
    appendAction(diagramItem->newDesignOfExperiment_);
    appendAction(diagramItem->newLimitState_);
    appendSeparator(tr("Analysis"));
    appendAction(diagramItem->newCentralTendency_);
    appendAction(diagramItem->newSensitivityAnalysis_);
  }
}


void ProbabilisticModelItem::update(Observable* source, const String & message)
{
  // emit signals to ProbabilisticModelWindow

  if (message == "inputNameChanged" ||
      message == "inputNumberChanged" ||
      message == "inputDistributionChanged"
     )
  {
    emit stochasticInputListChanged();
  }
  else if (message == "copulaChanged")
  {
    emit copulaChanged();
  }
  else if (message == "inputValueChanged")
  {
    emit inputListDefinitionChanged();
  }
  else if (message == "objectRemoved")
  {
    emit removeRequested(row());
  }
}
}
