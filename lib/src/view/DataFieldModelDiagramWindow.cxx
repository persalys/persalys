//                                               -*- C++ -*-
/**
 *  @brief QMdiSubWindow to define the diagram of the data model
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
#include "persalys/DataFieldModelDiagramWindow.hxx"

namespace PERSALYS
{

DataFieldModelDiagramWindow::DataFieldModelDiagramWindow(DataFieldModelDiagramItem * dataModelDiagramItem, QWidget * parent)
  : DiagramWindow(dataModelDiagramItem, parent)
{
  titleLabel_->setDocLink("user_manual/graphical_interface/data_model/user_manual_data_model.html#datafieldmodeldiagram");

  int row = 0;

  DiagramPushButton * modelDefinitionButton = new DiagramPushButton;
  modelDefinitionButton->setText(tr("Model\ndefinition"));
  modelDefinitionButton->setWhatsThis(tr("Import a data file and define the input and output variables"));
  connect(modelDefinitionButton, SIGNAL(clicked(bool)), dataModelDiagramItem, SLOT(appendDataFieldModelItem()));

  appendButton(modelDefinitionButton, row);

  DiagramPushButton * dataAnalysisButton = new DiagramPushButton;
  dataAnalysisButton->setText(tr("Data\nanalysis"));
  dataAnalysisButton->setWhatsThis(tr("Analyse each variable"));
  dataAnalysisButton->setErrorMessage(tr("Define at least one variable"));
  connect(dataAnalysisButton, SIGNAL(clicked(bool)), dataModelDiagramItem->newDataAnalysis_, SIGNAL(triggered()));
  connect(dataModelDiagramItem, SIGNAL(modelValidityChanged(bool)), dataAnalysisButton, SLOT(setEnabled(bool)));

  appendButton(dataAnalysisButton, row, modelDefinitionButton);

  updateDiagram();
}
}
