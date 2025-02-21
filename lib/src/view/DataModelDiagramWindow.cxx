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
#include "persalys/DataModelDiagramWindow.hxx"

namespace PERSALYS
{

DataModelDiagramWindow::DataModelDiagramWindow(DataModelDiagramItem * dataModelDiagramItem, QWidget * parent)
  : DiagramWindow(dataModelDiagramItem, parent)
{
  titleLabel_->setDocLink("user_manual/graphical_interface/data_model/user_manual_data_model.html#datamodeldiagram");

  int row = 0;

  DiagramPushButton * modelDefinitionButton = new DiagramPushButton;
  modelDefinitionButton->setText(tr("Model\ndefinition"));
  modelDefinitionButton->setWhatsThis(tr("Import a data file and define the input and output variables"));
  connect(modelDefinitionButton, SIGNAL(clicked(bool)), dataModelDiagramItem, SLOT(appendDataModelItem()));

  appendButton(modelDefinitionButton, row);

  DiagramPushButton * dataAnalysisButton = new DiagramPushButton;
  dataAnalysisButton->setText(tr("Data\nanalysis"));
  dataAnalysisButton->setWhatsThis(tr("Analyse each variable"));
  dataAnalysisButton->setErrorMessage(tr("Define at least one variable"));
  connect(dataAnalysisButton, SIGNAL(clicked(bool)), dataModelDiagramItem->newDataAnalysis_, SIGNAL(triggered()));
  connect(dataModelDiagramItem, SIGNAL(dataModelValidityChanged(bool)), dataAnalysisButton, SLOT(setEnabled(bool)));

  appendButton(dataAnalysisButton, row, modelDefinitionButton);

  DiagramPushButton * inferenceButton = new DiagramPushButton;
  inferenceButton->setText(tr("Marginals\ninference"));
  inferenceButton->setWhatsThis(tr("Make an inference analysis for variables with given distributions"));
  inferenceButton->setErrorMessage(tr("Define at least one variable"));
  connect(inferenceButton, SIGNAL(clicked(bool)), dataModelDiagramItem->newInferenceAnalysis_, SIGNAL(triggered()));
  connect(dataModelDiagramItem, SIGNAL(dataModelValidityChanged(bool)), inferenceButton, SLOT(setEnabled(bool)));

  appendButton(inferenceButton, ++row, modelDefinitionButton);

  DiagramPushButton * copulaInferenceButton = new DiagramPushButton;
  copulaInferenceButton->setText(tr("Dependence\ninference"));
  copulaInferenceButton->setWhatsThis(tr("Test the dependence of the variables"));
  copulaInferenceButton->setErrorMessage(tr("Define at least two variables"));
  connect(copulaInferenceButton, SIGNAL(clicked(bool)), dataModelDiagramItem->newCopulaInferenceAnalysis_, SIGNAL(triggered()));
  connect(dataModelDiagramItem, SIGNAL(dependenciesValidityChanged(bool)), copulaInferenceButton, SLOT(setEnabled(bool)));

  appendButton(copulaInferenceButton, ++row, modelDefinitionButton);

  DiagramPushButton * metamodelButton = new DiagramPushButton;
  metamodelButton->setText(tr("Metamodel\ncreation"));
  metamodelButton->setWhatsThis(tr("Two methods : Kriging and Functional chaos"));
  metamodelButton->setErrorMessage(tr("Define at least one input variable and one output variable"));
  connect(dataModelDiagramItem, SIGNAL(metaModelValidityChanged(bool)), metamodelButton, SLOT(setEnabled(bool)));
  connect(metamodelButton, SIGNAL(clicked(bool)), dataModelDiagramItem->newMetaModel_, SIGNAL(triggered()));

  appendButton(metamodelButton, ++row, modelDefinitionButton);

  updateDiagram();
}
}
