//                                               -*- C++ -*-
/**
 *  @brief QMdiSubWindow to define the diagram of the data model
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
#include "otgui/DataModelDiagramWindow.hxx"

#include "otgui/Arrow.hxx"
#include "otgui/DiagramPushButton.hxx"

#include <QVBoxLayout>
#include <QGraphicsView>
#include <QGraphicsProxyWidget>
#include <QSplitter>
#include <QTextEdit>
#include <QLabel>

using namespace OT;

namespace OTGUI
{

DataModelDiagramWindow::DataModelDiagramWindow(DataModelDiagramItem * dataModelDiagramItem, QWidget * parent)
  : SubWindow(dataModelDiagramItem, parent)
{
  QVBoxLayout * mainLayout = new QVBoxLayout(this);

  QLabel * title = new QLabel(tr("Use pattern"));
  title->setStyleSheet("font: bold");
  mainLayout->addWidget(title, 0, Qt::AlignHCenter);

  // optimal button size
  int boxWidth = 0;
  int boxHeight = 0;

  // Widgets
  QSplitter * splitter = new QSplitter(Qt::Vertical);

  QGraphicsView * graphView = new QGraphicsView(this);
  QGraphicsScene * scene = new QGraphicsScene(this);

  // -- Buttons --
  DiagramPushButton * modelDefinitionButton = new DiagramPushButton;
  modelDefinitionButton->setText(tr("Model\ndefinition"));
  modelDefinitionButton->setWhatsThis(tr("Import a data file and define the input and output variables"));
  QGraphicsProxyWidget * modelDefinitionButtonProxy = new QGraphicsProxyWidget;
  modelDefinitionButtonProxy->setWidget(modelDefinitionButton);
  scene->addItem(modelDefinitionButtonProxy);
  connect(modelDefinitionButton, SIGNAL(clicked(bool)), dataModelDiagramItem, SLOT(appendDataModelItem()));
  boxWidth = std::max(boxWidth, modelDefinitionButton->width());
  boxHeight = std::max(boxHeight, modelDefinitionButton->height());

  DiagramPushButton * dataAnalysisButton = new DiagramPushButton;
  dataAnalysisButton->setText(tr("Data\nanalysis"));
  dataAnalysisButton->setWhatsThis(tr("Analyse each variable"));
  dataAnalysisButton->setErrorMessage(tr("Define at least one variable"));
  QGraphicsProxyWidget * dataAnalysisProxy = new QGraphicsProxyWidget;
  dataAnalysisProxy->setWidget(dataAnalysisButton);
  scene->addItem(dataAnalysisProxy);
  connect(dataAnalysisButton, SIGNAL(clicked(bool)), dataModelDiagramItem, SIGNAL(dataAnalysisRequested()));
  connect(dataModelDiagramItem, SIGNAL(dataModelValidityChanged(bool)), dataAnalysisButton, SLOT(setEnabled(bool)));
  boxWidth = std::max(boxWidth, dataAnalysisButton->width());
  boxHeight = std::max(boxHeight, dataAnalysisButton->height());

  DiagramPushButton * inferenceButton = new DiagramPushButton;
  inferenceButton->setText(tr("Marginals\ninference"));
  inferenceButton->setWhatsThis(tr("Make an inference analysis for variables with given distributions"));
  inferenceButton->setErrorMessage(tr("Define at least one variable"));
  QGraphicsProxyWidget * inferenceProxy = new QGraphicsProxyWidget;
  inferenceProxy->setWidget(inferenceButton);
  scene->addItem(inferenceProxy);
  connect(inferenceButton, SIGNAL(clicked(bool)), dataModelDiagramItem, SIGNAL(inferenceRequested()));
  connect(dataModelDiagramItem, SIGNAL(dataModelValidityChanged(bool)), inferenceButton, SLOT(setEnabled(bool)));
  boxWidth = std::max(boxWidth, inferenceButton->width());
  boxHeight = std::max(boxHeight, inferenceButton->height());

  DiagramPushButton * copulaInferenceButton = new DiagramPushButton;
  copulaInferenceButton->setText(tr("Dependence\ninference"));
  copulaInferenceButton->setWhatsThis(tr("Test the dependence of the variables"));
  copulaInferenceButton->setErrorMessage(tr("Define at least two variables"));
  QGraphicsProxyWidget * copulaInferenceProxy = new QGraphicsProxyWidget;
  copulaInferenceProxy->setWidget(copulaInferenceButton);
  scene->addItem(copulaInferenceProxy);
  connect(copulaInferenceButton, SIGNAL(clicked(bool)), dataModelDiagramItem, SIGNAL(copulaInferenceRequested()));
  connect(dataModelDiagramItem, SIGNAL(dependenciesValidityChanged(bool)), copulaInferenceButton, SLOT(setEnabled(bool)));
  boxWidth = std::max(boxWidth, copulaInferenceButton->width());
  boxHeight = std::max(boxHeight, copulaInferenceButton->height());

  DiagramPushButton * metamodelButton = new DiagramPushButton;
  metamodelButton->setText(tr("Metamodel\ncreation"));
  metamodelButton->setWhatsThis(tr("Two methods : Kriging and Functional chaos"));
  metamodelButton->setErrorMessage(tr("Define at least one input variable and one output variable"));
  QGraphicsProxyWidget * metamodelProxy = new QGraphicsProxyWidget;
  metamodelProxy->setWidget(metamodelButton);
  scene->addItem(metamodelProxy);
  connect(dataModelDiagramItem, SIGNAL(metaModelValidityChanged(bool)), metamodelButton, SLOT(setEnabled(bool)));
  connect(metamodelButton, SIGNAL(clicked(bool)), dataModelDiagramItem, SIGNAL(metaModelRequested()));
  boxWidth = std::max(boxWidth, metamodelButton->width());
  boxHeight = std::max(boxHeight, metamodelButton->height());

  // -- resize --
  // we want that all the buttons have the same dimensions
  modelDefinitionButton->resize(boxWidth, boxHeight);
  dataAnalysisButton->resize(boxWidth, boxHeight);
  inferenceButton->resize(boxWidth, boxHeight);
  copulaInferenceButton->resize(boxWidth, boxHeight);
  metamodelButton->resize(boxWidth, boxHeight);

  // -- buttons positions --
  const int horizontalSpace = 100;
  const int verticalSpace = 20;

  const int line1pos = boxHeight + verticalSpace;
  const int line2pos = 2 * line1pos;
  const int line3pos = 3 * line1pos;
  const int column1pos = boxWidth + horizontalSpace;

  // -- set buttons positions --
  // column 0
  modelDefinitionButtonProxy->setPos(0, 0);

  // column 1
  dataAnalysisProxy->setPos(column1pos, 0);
  inferenceProxy->setPos(column1pos, line1pos);
  copulaInferenceProxy->setPos(column1pos, line2pos);
  metamodelProxy->setPos(column1pos, line3pos);

  // -- arrows positions --
  // arrows start points
  const int buttonMargin = 3;
  const QPointF modelDefinition_rightPoint(boxWidth + buttonMargin, boxHeight / 2);
  const QPointF startCopulaInferencePoint(boxWidth + buttonMargin, line1pos + boxHeight / 2);
  const QPointF startMetaModelPoint(boxWidth + buttonMargin, line2pos + boxHeight / 2);

  // arrows ending points
  const QPointF dataAnalysis_leftPoint(column1pos, boxHeight / 2);
  const QPointF inference_leftPoint(column1pos, line1pos + boxHeight / 2);
  const QPointF copulaInference_leftPoint(column1pos, line2pos + boxHeight / 2);
  const QPointF metamodel_leftPoint(column1pos, line3pos + boxHeight / 2);

  // -- Arrows --

  // arrow model definition -> data analysis
  Arrow * modelDef_dataAnalysis = new Arrow(modelDefinition_rightPoint,
      dataAnalysis_leftPoint);
  scene->addItem(modelDef_dataAnalysis);
  connect(dataAnalysisButton, SIGNAL(enabledChanged(bool)), modelDef_dataAnalysis, SLOT(setValidity(bool)));

  // arrow model definition -> inference
  Arrow * modelDef_inference = new Arrow(modelDefinition_rightPoint,
                                         inference_leftPoint);
  scene->addItem(modelDef_inference);
  connect(inferenceButton, SIGNAL(enabledChanged(bool)), modelDef_inference, SLOT(setValidity(bool)));

  // arrow model definition -> copula inference
  Arrow * modelDef_copulaInference = new Arrow(startCopulaInferencePoint,
      copulaInference_leftPoint);
  scene->addItem(modelDef_copulaInference);
  connect(copulaInferenceButton, SIGNAL(enabledChanged(bool)), modelDef_copulaInference, SLOT(setValidity(bool)));

  // arrow model definition -> metamodel
  Arrow * modelDef_metamodel = new Arrow(startMetaModelPoint,
                                         metamodel_leftPoint);
  scene->addItem(modelDef_metamodel);
  connect(metamodelButton, SIGNAL(enabledChanged(bool)), modelDef_metamodel, SLOT(setValidity(bool)));

  // -- set scene --
  graphView->setScene(scene);
  splitter->addWidget(graphView);
  splitter->setStretchFactor(0, 3);

  // -- text area --
  QTextEdit * textArea = new QTextEdit;
  textArea->setReadOnly(true);
  connect(modelDefinitionButton, SIGNAL(messageChanged(QString)), textArea, SLOT(setHtml(QString)));
  connect(dataAnalysisButton, SIGNAL(messageChanged(QString)), textArea, SLOT(setHtml(QString)));
  connect(inferenceButton, SIGNAL(messageChanged(QString)), textArea, SLOT(setHtml(QString)));
  connect(copulaInferenceButton, SIGNAL(messageChanged(QString)), textArea, SLOT(setHtml(QString)));
  connect(metamodelButton, SIGNAL(messageChanged(QString)), textArea, SLOT(setHtml(QString)));
  splitter->addWidget(textArea);
  splitter->setStretchFactor(1, 1);

  mainLayout->addWidget(splitter);
}
}
