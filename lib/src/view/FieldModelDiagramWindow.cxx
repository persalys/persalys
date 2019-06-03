//                                               -*- C++ -*-
/**
 *  @brief QMdiSubWindow to define the diagram of the physical model
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
#include "persalys/FieldModelDiagramWindow.hxx"

#include "persalys/Arrow.hxx"
#include "persalys/DiagramPushButton.hxx"

#include <QVBoxLayout>
#include <QGraphicsView>
#include <QGraphicsProxyWidget>
#include <QSplitter>
#include <QTextEdit>

using namespace OT;

namespace PERSALYS
{

FieldModelDiagramWindow::FieldModelDiagramWindow(PhysicalModelDiagramItem * physicalModelDiagramItem, QWidget * parent)
  : SubWindow(physicalModelDiagramItem, parent)
{
  QVBoxLayout * mainLayout = new QVBoxLayout(this);

  mainLayout->addWidget(new TitleLabel(tr("Study workflow"), "user_manual/graphical_interface/field_model/user_manual_field_model.html#fieldmodeldiagram"));

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
  modelDefinitionButton->setWhatsThis(tr("Define the input and output variables"));
  QGraphicsProxyWidget * modelDefinitionButtonProxy = new QGraphicsProxyWidget;
  modelDefinitionButtonProxy->setWidget(modelDefinitionButton);
  scene->addItem(modelDefinitionButtonProxy);
  connect(modelDefinitionButton, SIGNAL(clicked(bool)), physicalModelDiagramItem, SLOT(appendPhysicalModelItem()));
  boxWidth = std::max(boxWidth, modelDefinitionButton->width());
  boxHeight = std::max(boxHeight, modelDefinitionButton->height());

  DiagramPushButton * modelEvaluationButton = new DiagramPushButton;
  modelEvaluationButton->setText(tr("Model\nevaluation"));
  modelEvaluationButton->setWhatsThis(tr("Evaluate the model at a given point"));
  modelEvaluationButton->setErrorMessage(tr("Define input and output variables in the model"));
  QGraphicsProxyWidget * modelEvaluationProxy = new QGraphicsProxyWidget;
  modelEvaluationProxy->setWidget(modelEvaluationButton);
  scene->addItem(modelEvaluationProxy);
  connect(modelEvaluationButton, SIGNAL(clicked(bool)), physicalModelDiagramItem, SIGNAL(evaluationModelRequested()));
  connect(physicalModelDiagramItem, SIGNAL(physicalModelValidityChanged(bool)), modelEvaluationButton, SLOT(setEnabled(bool)));
  boxWidth = std::max(boxWidth, modelEvaluationButton->width());
  boxHeight = std::max(boxHeight, modelEvaluationButton->height());

//   DiagramPushButton * doeCreationButton = new DiagramPushButton;
//   doeCreationButton->setText(tr("Design of\nexperiments\ncreation"));
//   doeCreationButton->setWhatsThis(tr("Create manually a design of experiments or import one"));
//   doeCreationButton->setErrorMessage(tr("Define at least one input variable"));
//   QGraphicsProxyWidget * doeCreationProxy = new QGraphicsProxyWidget;
//   doeCreationProxy->setWidget(doeCreationButton);
//   scene->addItem(doeCreationProxy);
//   connect(doeCreationButton, SIGNAL(clicked(bool)), physicalModelDiagramItem, SIGNAL(designOfExperimentRequested()));
//   connect(physicalModelDiagramItem, SIGNAL(inputNumberValidityChanged(bool)), doeCreationButton, SLOT(setEnabled(bool)));
//   boxWidth = std::max(boxWidth, doeCreationButton->width());
//   boxHeight = std::max(boxHeight, doeCreationButton->height());

  DiagramPushButton * probaModelButton = new DiagramPushButton;
  probaModelButton->setText(tr("Probabilistic\nmodel\ndefinition"));
  probaModelButton->setWhatsThis(tr("Define the stochastic input variables and the dependence"));
  probaModelButton->setErrorMessage(tr("Define at least one input variable"));
  QGraphicsProxyWidget * probaModelProxy = new QGraphicsProxyWidget;
  probaModelProxy->setWidget(probaModelButton);
  scene->addItem(probaModelProxy);
  connect(probaModelButton, SIGNAL(clicked(bool)), physicalModelDiagramItem, SLOT(appendProbabilisticModelItem()));
  connect(physicalModelDiagramItem, SIGNAL(inputNumberValidityChanged(bool)), probaModelButton, SLOT(setEnabled(bool)));
  boxWidth = std::max(boxWidth, probaModelButton->width());
  boxHeight = std::max(boxHeight, probaModelButton->height());

//   DiagramPushButton * doeEvaluationButton = new DiagramPushButton;
//   doeEvaluationButton->setText(tr("Design of\nexperiments\nevaluation"));
//   doeEvaluationButton->setWhatsThis(tr("Evaluate the model at the points of the design of experiments"));
//   doeEvaluationButton->setErrorMessage(tr("Create at least one design of experiments and define output variables in the model"));
//   QGraphicsProxyWidget * doeEvaluationProxy = new QGraphicsProxyWidget;
//   doeEvaluationProxy->setWidget(doeEvaluationButton);
//   scene->addItem(doeEvaluationProxy);
//   connect(doeEvaluationButton, SIGNAL(clicked(bool)), physicalModelDiagramItem, SLOT(requestDesignOfExperimentEvaluation()));
//   connect(physicalModelDiagramItem, SIGNAL(doeNumberValidityChanged(bool)), doeEvaluationButton, SLOT(setEnabled(bool)));
//   boxWidth = std::max(boxWidth, doeEvaluationButton->width());
//   boxHeight = std::max(boxHeight, doeEvaluationButton->height());

  DiagramPushButton * centralTendencyButton = new DiagramPushButton;
  centralTendencyButton->setText(tr("Central\ntendency"));
  centralTendencyButton->setWhatsThis(tr("Two methods : Monte Carlo and Taylor expansion"));
  centralTendencyButton->setErrorMessage(tr("Define stochastic input variables and output variables in the model"));
  QGraphicsProxyWidget * centralTendencyProxy = new QGraphicsProxyWidget;
  centralTendencyProxy->setWidget(centralTendencyButton);
  scene->addItem(centralTendencyProxy);
  connect(centralTendencyButton, SIGNAL(clicked(bool)), physicalModelDiagramItem, SIGNAL(centralTendencyRequested()));
  connect(physicalModelDiagramItem, SIGNAL(probabilisticModelValidityChanged(bool)), centralTendencyButton, SLOT(setEnabled(bool)));
  boxWidth = std::max(boxWidth, centralTendencyButton->width());
  boxHeight = std::max(boxHeight, centralTendencyButton->height());

  // -- resize --
  // we want that all the buttons have the same dimensions
  modelDefinitionButton->resize(boxWidth, boxHeight);
  modelEvaluationButton->resize(boxWidth, boxHeight);
//    doeCreationButton->resize(boxWidth, boxHeight);
  probaModelButton->resize(boxWidth, boxHeight);
//   doeEvaluationButton->resize(boxWidth, boxHeight);
  centralTendencyButton->resize(boxWidth, boxHeight);

  // -- buttons positions --
  const int horizontalSpace = 100;
  const int verticalSpace = 20;

  const int linePos = boxHeight + verticalSpace;
  const int column1pos = boxWidth + horizontalSpace;
  const int column2pos = 2 * column1pos;

  // -- set buttons positions --

  int lineCounter = 0;

  // line
  modelDefinitionButtonProxy->setPos(0, 0);
  modelEvaluationProxy->setPos(column1pos, 0);

  // ++line
//   doeCreationProxy->setPos(column1pos, ++lineCounter * linePos);
//   doeEvaluationProxy->setPos(column2pos, lineCounter * linePos);

  // ++line
  probaModelProxy->setPos(column1pos, ++lineCounter * linePos);

  // ++line
  centralTendencyProxy->setPos(column2pos, lineCounter * linePos);

  // -- arrows positions --
  // arrows start points
  const int buttonMargin = 3;

  const QPointF deltaRight(boxWidth + buttonMargin, boxHeight * 0.5);

  const QPointF modelDefinition_rightPoint(deltaRight);
//   const QPointF doeCreation_rightPoint = doeCreationProxy->pos() + deltaRight;
  const QPointF probaModel_rightPoint = probaModelProxy->pos() + deltaRight;

  // arrows ending points
  const QPointF deltaLeft(0, boxHeight * 0.5);
  const QPointF modelEval_leftPoint = modelEvaluationProxy->pos() + deltaLeft;
//   const QPointF doeCreation_leftPoint = doeCreationProxy->pos() + deltaLeft;
  const QPointF probaModel_leftPoint = probaModelProxy->pos() + deltaLeft;
//   const QPointF doeEval_leftPoint = doeEvaluationProxy->pos() + deltaLeft;
  const QPointF centralTendency_leftPoint = centralTendencyProxy->pos() + deltaLeft;

  // -- Arrows --

  // arrow model definition -> model evaluation
  Arrow * modelDef_modelEval = new Arrow(modelDefinition_rightPoint,
                                         modelEval_leftPoint);
  scene->addItem(modelDef_modelEval);
  connect(modelEvaluationButton, SIGNAL(enabledChanged(bool)), modelDef_modelEval, SLOT(setValidity(bool)));

  // arrow model definition -> doe creation
//   Arrow * modelDef_doeCreation = new Arrow(modelDefinition_rightPoint,
//       doeCreation_leftPoint);
//   scene->addItem(modelDef_doeCreation);
//   connect(doeCreationButton, SIGNAL(enabledChanged(bool)), modelDef_doeCreation, SLOT(setValidity(bool)));

  // arrow model definition -> proba model definition
  Arrow * modelDef_probaModel = new Arrow(modelDefinition_rightPoint,
                                          probaModel_leftPoint);
  scene->addItem(modelDef_probaModel);
  connect(probaModelButton, SIGNAL(enabledChanged(bool)), modelDef_probaModel, SLOT(setValidity(bool)));

  // arrow doe creation -> doe evaluation
//   Arrow * doeCreation_doeEval = new Arrow(doeCreation_rightPoint,
//                                           doeEval_leftPoint);
//   scene->addItem(doeCreation_doeEval);
//   connect(doeEvaluationButton, SIGNAL(enabledChanged(bool)), doeCreation_doeEval, SLOT(setValidity(bool)));

  // arrow proba model definition -> central tendency
  Arrow * probaModel_centralTendency = new Arrow(probaModel_rightPoint,
      centralTendency_leftPoint);
  scene->addItem(probaModel_centralTendency);
  connect(centralTendencyButton, SIGNAL(enabledChanged(bool)), probaModel_centralTendency, SLOT(setValidity(bool)));

  // -- set scene --
  graphView->setScene(scene);
  splitter->addWidget(graphView);
  splitter->setStretchFactor(0, 3);

  // -- text area --
  QTextEdit * textArea = new QTextEdit;
  textArea->setReadOnly(true);
  connect(modelDefinitionButton, SIGNAL(messageChanged(QString)), textArea, SLOT(setHtml(QString)));
  connect(modelEvaluationButton, SIGNAL(messageChanged(QString)), textArea, SLOT(setHtml(QString)));
//   connect(doeCreationButton, SIGNAL(messageChanged(QString)), textArea, SLOT(setHtml(QString)));
  connect(probaModelButton, SIGNAL(messageChanged(QString)), textArea, SLOT(setHtml(QString)));
//   connect(doeEvaluationButton, SIGNAL(messageChanged(QString)), textArea, SLOT(setHtml(QString)));
  connect(centralTendencyButton, SIGNAL(messageChanged(QString)), textArea, SLOT(setHtml(QString)));
  splitter->addWidget(textArea);
  splitter->setStretchFactor(1, 1);

  mainLayout->addWidget(splitter);
}
}
