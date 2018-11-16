//                                               -*- C++ -*-
/**
 *  @brief QMdiSubWindow to define the diagram of the physical model
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
#include "otgui/PhysicalModelDiagramWindow.hxx"

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

PhysicalModelDiagramWindow::PhysicalModelDiagramWindow(PhysicalModelDiagramItem * physicalModelDiagramItem, QWidget * parent)
  : OTguiSubWindow(physicalModelDiagramItem, parent)
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

  DiagramPushButton * screeningButton = 0;
  QGraphicsProxyWidget * screeningProxy = 0;
#ifdef OTGUI_HAVE_OTMORRIS
  screeningButton = new DiagramPushButton;
  screeningButton->setText(tr("Screening"));
  screeningButton->setWhatsThis(tr("One screening method : Morris"));
  screeningButton->setErrorMessage(tr("Define at least two input variables and an output variable in the model"));
  screeningProxy = new QGraphicsProxyWidget;
  screeningProxy->setWidget(screeningButton);
  scene->addItem(screeningProxy);
  connect(screeningButton, SIGNAL(clicked(bool)), physicalModelDiagramItem, SIGNAL(screeningRequested()));
  connect(physicalModelDiagramItem, SIGNAL(twoInputsValidityChanged(bool)), screeningButton, SLOT(setEnabled(bool)));
  boxWidth = std::max(boxWidth, screeningButton->width());
  boxHeight = std::max(boxHeight, screeningButton->height());
#endif

  DiagramPushButton * doeCreationButton = new DiagramPushButton;
  doeCreationButton->setText(tr("Design of\nexperiments\ncreation"));
  doeCreationButton->setWhatsThis(tr("Create manually a design of experiments or import one"));
  doeCreationButton->setErrorMessage(tr("Define at least one input variable"));
  QGraphicsProxyWidget * doeCreationProxy = new QGraphicsProxyWidget;
  doeCreationProxy->setWidget(doeCreationButton);
  scene->addItem(doeCreationProxy);
  connect(doeCreationButton, SIGNAL(clicked(bool)), physicalModelDiagramItem, SIGNAL(designOfExperimentRequested()));
  connect(physicalModelDiagramItem, SIGNAL(inputNumberValidityChanged(bool)), doeCreationButton, SLOT(setEnabled(bool)));
  boxWidth = std::max(boxWidth, doeCreationButton->width());
  boxHeight = std::max(boxHeight, doeCreationButton->height());

  DiagramPushButton * optimizationCreationButton = new DiagramPushButton;
  optimizationCreationButton->setText(tr("Optimization"));
  optimizationCreationButton->setWhatsThis(tr("Optimization of the model function"));
  optimizationCreationButton->setErrorMessage(tr("Define input and output variables in the model"));
  QGraphicsProxyWidget * optimizationCreationProxy = new QGraphicsProxyWidget;
  optimizationCreationProxy->setWidget(optimizationCreationButton);
  scene->addItem(optimizationCreationProxy);
  connect(optimizationCreationButton, SIGNAL(clicked(bool)), physicalModelDiagramItem, SIGNAL(optimizationRequested()));
  connect(physicalModelDiagramItem, SIGNAL(physicalModelValidityChanged(bool)), optimizationCreationButton, SLOT(setEnabled(bool)));
  boxWidth = std::max(boxWidth, optimizationCreationButton->width());
  boxHeight = std::max(boxHeight, optimizationCreationButton->height());

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

  DiagramPushButton * doeEvaluationButton = new DiagramPushButton;
  doeEvaluationButton->setText(tr("Design of\nexperiments\nevaluation"));
  doeEvaluationButton->setWhatsThis(tr("Evaluate the model at the points of the design of experiments"));
  doeEvaluationButton->setErrorMessage(tr("Create at least one design of experiments and define output variables in the model"));
  QGraphicsProxyWidget * doeEvaluationProxy = new QGraphicsProxyWidget;
  doeEvaluationProxy->setWidget(doeEvaluationButton);
  scene->addItem(doeEvaluationProxy);
  connect(doeEvaluationButton, SIGNAL(clicked(bool)), physicalModelDiagramItem, SLOT(requestDesignOfExperimentEvaluation()));
  connect(physicalModelDiagramItem, SIGNAL(doeNumberValidityChanged(bool)), doeEvaluationButton, SLOT(setEnabled(bool)));
  boxWidth = std::max(boxWidth, doeEvaluationButton->width());
  boxHeight = std::max(boxHeight, doeEvaluationButton->height());

  DiagramPushButton * sensitivityButton = new DiagramPushButton;
  sensitivityButton->setText(tr("Sensitivity"));
  sensitivityButton->setWhatsThis(tr("Two methods : Sobol and SRC"));
  sensitivityButton->setErrorMessage(tr("Define independent stochastic input variables and output variables in the model"));
  QGraphicsProxyWidget * sensitivityProxy = new QGraphicsProxyWidget;
  sensitivityProxy->setWidget(sensitivityButton);
  scene->addItem(sensitivityProxy);
  connect(sensitivityButton, SIGNAL(clicked(bool)), physicalModelDiagramItem, SIGNAL(sensitivityRequested()));
  connect(physicalModelDiagramItem, SIGNAL(dependenceValidityChanged(bool)), sensitivityButton, SLOT(setEnabled(bool)));
  boxWidth = std::max(boxWidth, sensitivityButton->width());
  boxHeight = std::max(boxHeight, sensitivityButton->height());

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

  DiagramPushButton * limitStateButton = new DiagramPushButton;
  limitStateButton->setText(tr("Limit state\ndefinition"));
  limitStateButton->setWhatsThis(tr("Define a limit state to perform threshold exceedance analysis"));
  limitStateButton->setErrorMessage(tr("Define input and output variables in the model"));
  QGraphicsProxyWidget * limitStateProxy = new QGraphicsProxyWidget;
  limitStateProxy->setWidget(limitStateButton);
  scene->addItem(limitStateProxy);
  connect(limitStateButton, SIGNAL(clicked(bool)), physicalModelDiagramItem, SIGNAL(limitStateRequested()));
  connect(physicalModelDiagramItem, SIGNAL(probabilisticModelValidityChanged(bool)), limitStateButton, SLOT(setEnabled(bool)));
  boxWidth = std::max(boxWidth, limitStateButton->width());
  boxHeight = std::max(boxHeight, limitStateButton->height());

  DiagramPushButton * metamodelButton = new DiagramPushButton;
  metamodelButton->setText(tr("Metamodel\ncreation"));
  metamodelButton->setWhatsThis(tr("Two methods : Kriging and Functional chaos"));
  metamodelButton->setErrorMessage(tr("Define at least one design of experiments which contains output values"));
  QGraphicsProxyWidget * metamodelProxy = new QGraphicsProxyWidget;
  metamodelProxy->setWidget(metamodelButton);
  scene->addItem(metamodelProxy);
  connect(physicalModelDiagramItem, SIGNAL(doeEvaluationNumberValidityChanged(bool)), metamodelButton, SLOT(setEnabled(bool)));
  connect(metamodelButton, SIGNAL(clicked(bool)), physicalModelDiagramItem, SLOT(requestMetaModelCreation()));
  boxWidth = std::max(boxWidth, metamodelButton->width());
  boxHeight = std::max(boxHeight, metamodelButton->height());

  DiagramPushButton * reliabilityButton = new DiagramPushButton;
  reliabilityButton->setText(tr("Reliability"));
  reliabilityButton->setWhatsThis(tr("Four methods : Monte Carlo, FORM-Importance Sampling, FORM, SORM"));
  reliabilityButton->setErrorMessage(tr("Define output variables in the model and at least one limit state"));
  QGraphicsProxyWidget * reliabilityProxy = new QGraphicsProxyWidget;
  reliabilityProxy->setWidget(reliabilityButton);
  scene->addItem(reliabilityProxy);
  connect(physicalModelDiagramItem, SIGNAL(limitStateNumberValidityChanged(bool)), reliabilityButton, SLOT(setEnabled(bool)));
  connect(reliabilityButton, SIGNAL(clicked(bool)), physicalModelDiagramItem, SLOT(requestReliabilityCreation()));
  boxWidth = std::max(boxWidth, reliabilityButton->width());
  boxHeight = std::max(boxHeight, reliabilityButton->height());

  // -- resize --
  // we want that all the buttons have the same dimensions
  modelDefinitionButton->resize(boxWidth, boxHeight);
  modelEvaluationButton->resize(boxWidth, boxHeight);
  if (screeningButton)
    screeningButton->resize(boxWidth, boxHeight);
  optimizationCreationButton->resize(boxWidth, boxHeight);
  doeCreationButton->resize(boxWidth, boxHeight);
  probaModelButton->resize(boxWidth, boxHeight);
  doeEvaluationButton->resize(boxWidth, boxHeight);
  sensitivityButton->resize(boxWidth, boxHeight);
  centralTendencyButton->resize(boxWidth, boxHeight);
  limitStateButton->resize(boxWidth, boxHeight);
  metamodelButton->resize(boxWidth, boxHeight);
  reliabilityButton->resize(boxWidth, boxHeight);

  // -- buttons positions --
  const int horizontalSpace = 100;
  const int verticalSpace = 20;

  const int linePos = boxHeight + verticalSpace;
  const int column1pos = boxWidth + horizontalSpace;
  const int column2pos = 2 * column1pos;
  const int column3pos = 3 * column1pos;

  // -- set buttons positions --

  int lineCounter = 0;

  // line
  modelDefinitionButtonProxy->setPos(0, 0);
  modelEvaluationProxy->setPos(column1pos, 0);

  // ++line
  if (screeningProxy)
    screeningProxy->setPos(column1pos, ++lineCounter * linePos);

  // ++line
  optimizationCreationProxy->setPos(column1pos, ++lineCounter * linePos);

  // ++line
  doeCreationProxy->setPos(column1pos, ++lineCounter * linePos);
  doeEvaluationProxy->setPos(column2pos, lineCounter * linePos);
  metamodelProxy->setPos(column3pos, lineCounter * linePos);

  // ++line
  probaModelProxy->setPos(column1pos, ++lineCounter * linePos);
  sensitivityProxy->setPos(column2pos, lineCounter * linePos);

  // ++line
  centralTendencyProxy->setPos(column2pos, ++lineCounter * linePos);

  // ++line
  limitStateProxy->setPos(column2pos, ++lineCounter * linePos);
  reliabilityProxy->setPos(column3pos, lineCounter * linePos);

  // -- arrows positions --
  // arrows start points
  const int buttonMargin = 3;

  const QPointF deltaRight(boxWidth + buttonMargin, boxHeight * 0.5);

  const QPointF modelDefinition_rightPoint(deltaRight);
  const QPointF doeCreation_rightPoint = doeCreationProxy->pos() + deltaRight;
  const QPointF doeEval_rightPoint = doeEvaluationProxy->pos() + deltaRight;
  const QPointF probaModel_rightPoint = probaModelProxy->pos() + deltaRight;
  const QPointF limitState_rightPoint = limitStateProxy->pos() + deltaRight;

  // arrows ending points
  const QPointF deltaLeft(-buttonMargin, boxHeight * 0.5);
  const QPointF modelEval_leftPoint = modelEvaluationProxy->pos() + deltaLeft;
  const QPointF optimizationCreation_leftPoint = optimizationCreationProxy->pos() + deltaLeft;
  const QPointF doeCreation_leftPoint = doeCreationProxy->pos() + deltaLeft;
  const QPointF probaModel_leftPoint = probaModelProxy->pos() + deltaLeft;
  const QPointF doeEval_leftPoint = doeEvaluationProxy->pos() + deltaLeft;
  const QPointF sensitivity_leftPoint = sensitivityProxy->pos() + deltaLeft;
  const QPointF centralTendency_leftPoint = centralTendencyProxy->pos() + deltaLeft;
  const QPointF limitState_leftPoint = limitStateProxy->pos() + deltaLeft;
  const QPointF metamodel_leftPoint = metamodelProxy->pos() + deltaLeft;
  const QPointF reliability_leftPoint = reliabilityProxy->pos() + deltaLeft;

  // -- Arrows --

  // arrow model definition -> model evaluation
  Arrow * modelDef_modelEval = new Arrow(modelDefinition_rightPoint,
                                         modelEval_leftPoint);
  scene->addItem(modelDef_modelEval);
  connect(modelEvaluationButton, SIGNAL(enabledChanged(bool)), modelDef_modelEval, SLOT(setValidity(bool)));

  if (screeningButton)
  {
    // arrow model definition -> screening
    const QPointF screening_leftPoint = screeningProxy->pos() + deltaLeft;
    Arrow * modelDef_screening = new Arrow(modelDefinition_rightPoint,
                                           screening_leftPoint);
    scene->addItem(modelDef_screening);
    connect(screeningButton, SIGNAL(enabledChanged(bool)), modelDef_screening, SLOT(setValidity(bool)));
  }

  // arrow model definition -> doe creation
  Arrow * modelDef_doeCreation = new Arrow(modelDefinition_rightPoint,
      doeCreation_leftPoint);
  scene->addItem(modelDef_doeCreation);
  connect(doeCreationButton, SIGNAL(enabledChanged(bool)), modelDef_doeCreation, SLOT(setValidity(bool)));

  // arrow model definition -> model optimization
  Arrow * modelDef_optimizationCreation = new Arrow(modelDefinition_rightPoint, optimizationCreation_leftPoint);
  scene->addItem(modelDef_optimizationCreation);
  connect(optimizationCreationButton, SIGNAL(enabledChanged(bool)), modelDef_optimizationCreation, SLOT(setValidity(bool)));

  // arrow model definition -> proba model definition
  Arrow * modelDef_probaModel = new Arrow(modelDefinition_rightPoint,
                                          probaModel_leftPoint);
  scene->addItem(modelDef_probaModel);
  connect(probaModelButton, SIGNAL(enabledChanged(bool)), modelDef_probaModel, SLOT(setValidity(bool)));

  // arrow doe creation -> doe evaluation
  Arrow * doeCreation_doeEval = new Arrow(doeCreation_rightPoint,
                                          doeEval_leftPoint);
  scene->addItem(doeCreation_doeEval);
  connect(doeEvaluationButton, SIGNAL(enabledChanged(bool)), doeCreation_doeEval, SLOT(setValidity(bool)));

  // arrow proba model definition -> sensitivity
  Arrow * probaModel_sensitivity = new Arrow(probaModel_rightPoint,
      sensitivity_leftPoint);
  scene->addItem(probaModel_sensitivity);
  connect(sensitivityButton, SIGNAL(enabledChanged(bool)), probaModel_sensitivity, SLOT(setValidity(bool)));

  // arrow proba model definition -> central tendency
  Arrow * probaModel_centralTendency = new Arrow(probaModel_rightPoint,
      centralTendency_leftPoint);
  scene->addItem(probaModel_centralTendency);
  connect(centralTendencyButton, SIGNAL(enabledChanged(bool)), probaModel_centralTendency, SLOT(setValidity(bool)));

  // arrow proba model definition -> limit state
  Arrow * probaModel_limitState = new Arrow(probaModel_rightPoint,
      limitState_leftPoint);
  scene->addItem(probaModel_limitState);
  connect(limitStateButton, SIGNAL(enabledChanged(bool)), probaModel_limitState, SLOT(setValidity(bool)));

  // arrow doe evaluation -> metamodel
  Arrow * doeEval_metamodel = new Arrow(doeEval_rightPoint,
                                        metamodel_leftPoint);
  scene->addItem(doeEval_metamodel);
  connect(metamodelButton, SIGNAL(enabledChanged(bool)), doeEval_metamodel, SLOT(setValidity(bool)));

  // arrow limit state -> reliability
  Arrow * limitState_reliability = new Arrow(limitState_rightPoint,
      reliability_leftPoint);
  scene->addItem(limitState_reliability);
  connect(reliabilityButton, SIGNAL(enabledChanged(bool)), limitState_reliability, SLOT(setValidity(bool)));

  // -- set scene --
  graphView->setScene(scene);
  graphView->ensureVisible(modelDefinitionButtonProxy);
  splitter->addWidget(graphView);
  splitter->setStretchFactor(0, 3);

  // -- text area --
  QTextEdit * textArea = new QTextEdit;
  textArea->setReadOnly(true);
  connect(modelDefinitionButton, SIGNAL(messageChanged(QString)), textArea, SLOT(setHtml(QString)));
  connect(modelEvaluationButton, SIGNAL(messageChanged(QString)), textArea, SLOT(setHtml(QString)));
  if (screeningButton)
    connect(screeningButton, SIGNAL(messageChanged(QString)), textArea, SLOT(setHtml(QString)));
  connect(optimizationCreationButton, SIGNAL(messageChanged(QString)), textArea, SLOT(setHtml(QString)));
  connect(doeCreationButton, SIGNAL(messageChanged(QString)), textArea, SLOT(setHtml(QString)));
  connect(probaModelButton, SIGNAL(messageChanged(QString)), textArea, SLOT(setHtml(QString)));
  connect(doeEvaluationButton, SIGNAL(messageChanged(QString)), textArea, SLOT(setHtml(QString)));
  connect(sensitivityButton, SIGNAL(messageChanged(QString)), textArea, SLOT(setHtml(QString)));
  connect(centralTendencyButton, SIGNAL(messageChanged(QString)), textArea, SLOT(setHtml(QString)));
  connect(limitStateButton, SIGNAL(messageChanged(QString)), textArea, SLOT(setHtml(QString)));
  connect(metamodelButton, SIGNAL(messageChanged(QString)), textArea, SLOT(setHtml(QString)));
  connect(reliabilityButton, SIGNAL(messageChanged(QString)), textArea, SLOT(setHtml(QString)));
  splitter->addWidget(textArea);
  splitter->setStretchFactor(1, 1);

  mainLayout->addWidget(splitter);
}
}
