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
  titleLabel_->setDocLink("user_manual/graphical_interface/data_model/user_manual_data_set.html#datamodeldiagram");

  int row = 0;

  auto * modelDefinitionButton = new DiagramPushButton;
  modelDefinitionButton->setText(tr("Model\ndefinition"));
  modelDefinitionButton->setWhatsThis(tr("Import a data file and define the input and output variables"));
  connect(modelDefinitionButton, &QPushButton::clicked, dataModelDiagramItem, &DataModelDiagramItem::appendDataModelItem);

  appendButton(modelDefinitionButton, row);

  auto * dataAnalysisButton = new DiagramPushButton;
  dataAnalysisButton->setText(tr("Data\nanalysis"));
  dataAnalysisButton->setWhatsThis(tr("Analyse each variable"));
  connect(dataAnalysisButton, &QPushButton::clicked, dataModelDiagramItem->newDataAnalysis_, &QAction::triggered);
  connect(dataModelDiagramItem, &DataModelDiagramItem::dataSizeValidityChanged, dataAnalysisButton, &DiagramPushButton::setEnabled);

  appendButton(dataAnalysisButton, row, modelDefinitionButton);

  auto * sensitivityAnalysisButton = new DiagramPushButton;
  sensitivityAnalysisButton->setText(tr("Sensitivity\nanalysis"));
  sensitivityAnalysisButton->setWhatsThis(tr("Compute Rank Sobol' indices"));
  connect(sensitivityAnalysisButton, &QPushButton::clicked, dataModelDiagramItem->newDataSensitivityAnalysis_, &QAction::triggered);
  connect(dataModelDiagramItem, &DataModelDiagramItem::dataSensitivityValidityChanged, sensitivityAnalysisButton, &DiagramPushButton::setEnabled);

  appendButton(sensitivityAnalysisButton, ++row, modelDefinitionButton);

  auto * quantileAnalysisButton = new DiagramPushButton;
  quantileAnalysisButton->setText(tr("Quantile\nanalysis"));
  quantileAnalysisButton->setWhatsThis(tr("Estimate quantiles"));
  connect(quantileAnalysisButton, &QPushButton::clicked, dataModelDiagramItem->newQuantileAnalysis_, &QAction::triggered);
  connect(dataModelDiagramItem, &DataModelDiagramItem::dataModelValidityChanged, quantileAnalysisButton, &DiagramPushButton::setEnabled);

  appendButton(quantileAnalysisButton, ++row, modelDefinitionButton);

  auto * inferenceButton = new DiagramPushButton;
  inferenceButton->setText(tr("Marginals\ninference"));
  inferenceButton->setWhatsThis(tr("Make an inference analysis for variables with given distributions"));
  connect(inferenceButton, &QPushButton::clicked, dataModelDiagramItem->newInferenceAnalysis_, &QAction::triggered);
  connect(dataModelDiagramItem, &DataModelDiagramItem::inferenceValidityChanged, inferenceButton, &DiagramPushButton::setEnabled);

  appendButton(inferenceButton, ++row, modelDefinitionButton);

  auto * copulaInferenceButton = new DiagramPushButton;
  copulaInferenceButton->setText(tr("Dependence\ninference"));
  copulaInferenceButton->setWhatsThis(tr("Test the dependence of the variables"));
  connect(copulaInferenceButton, &QPushButton::clicked, dataModelDiagramItem->newCopulaInferenceAnalysis_, &QAction::triggered);
  connect(dataModelDiagramItem, &DataModelDiagramItem::dependenciesValidityChanged, copulaInferenceButton, &DiagramPushButton::setEnabled);

  appendButton(copulaInferenceButton, ++row, modelDefinitionButton);

  auto * metamodelButton = new DiagramPushButton;
  metamodelButton->setText(tr("Metamodel\ncreation"));
  metamodelButton->setWhatsThis(tr("Build a computationally cheap approximation model from input-output data"));
  connect(metamodelButton, &QPushButton::clicked, dataModelDiagramItem->newMetaModel_, &QAction::triggered);
  connect(dataModelDiagramItem, &DataModelDiagramItem::metaModelValidityChanged, metamodelButton, &DiagramPushButton::setEnabled);

  appendButton(metamodelButton, ++row, modelDefinitionButton);

  auto * metamodelExportButton = new DiagramPushButton;
  metamodelExportButton->setText(tr("Export\nas model"));
  metamodelExportButton->setWhatsThis(tr("Export metamodel as model"));
  connect(dataModelDiagramItem, &DataModelDiagramItem::metamodelNumberValidityChanged, metamodelExportButton, &DiagramPushButton::setEnabled);
  connect(metamodelExportButton, &QPushButton::clicked, dataModelDiagramItem, &DataModelDiagramItem::requestMetaModelExport);

  appendButton(metamodelExportButton, row, metamodelButton);

  updateDiagram();
}
}
