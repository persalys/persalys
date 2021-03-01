//                                               -*- C++ -*-
/**
 *  @brief QMdiSubWindow to define the diagram of the physical model
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
#include "persalys/PhysicalModelDiagramWindow.hxx"

namespace PERSALYS
{

PhysicalModelDiagramWindow::PhysicalModelDiagramWindow(PhysicalModelDiagramItem * physicalModelDiagramItem, QWidget * parent)
  : DiagramWindow(physicalModelDiagramItem, parent)
{
  titleLabel_->setDocLink("user_manual/graphical_interface/physical_model/user_manual_physical_model.html#vectmodeldiagram");

  int row = 0;

  DiagramPushButton * modelDefinitionButton = new DiagramPushButton;
  modelDefinitionButton->setText(tr("Model\ndefinition"));
  modelDefinitionButton->setWhatsThis(tr("Define the input and output variables"));
  connect(modelDefinitionButton, SIGNAL(clicked(bool)), physicalModelDiagramItem, SLOT(appendPhysicalModelItem()));

  appendButton(modelDefinitionButton, row);

  DiagramPushButton * modelEvaluationButton = new DiagramPushButton;
  modelEvaluationButton->setText(tr("Model\nevaluation"));
  modelEvaluationButton->setWhatsThis(tr("Evaluate the model at a given point"));
  modelEvaluationButton->setErrorMessage(tr("Define input and output variables in the model"));
  connect(modelEvaluationButton, SIGNAL(clicked(bool)), physicalModelDiagramItem->newModelEvaluation_, SIGNAL(triggered()));
  connect(physicalModelDiagramItem, SIGNAL(physicalModelValidityChanged(bool)), modelEvaluationButton, SLOT(setEnabled(bool)));

  appendButton(modelEvaluationButton, row, modelDefinitionButton);

#ifdef PERSALYS_HAVE_OTMORRIS
  DiagramPushButton * screeningButton = new DiagramPushButton;
  screeningButton->setText(tr("Screening"));
  screeningButton->setWhatsThis(tr("One screening method : Morris"));
  screeningButton->setErrorMessage(tr("Define at least two input variables and an output variable in the model"));
  connect(screeningButton, SIGNAL(clicked(bool)), physicalModelDiagramItem->newScreening_, SIGNAL(triggered()));
  connect(physicalModelDiagramItem, SIGNAL(twoInputsValidityChanged(bool)), screeningButton, SLOT(setEnabled(bool)));

  appendButton(screeningButton, ++row, modelDefinitionButton);
#endif

  DiagramPushButton * optimizationCreationButton = new DiagramPushButton;
  optimizationCreationButton->setText(tr("Optimization"));
  optimizationCreationButton->setWhatsThis(tr("Optimization of the model function"));
  optimizationCreationButton->setErrorMessage(tr("Define input and output variables in the model"));
  connect(optimizationCreationButton, SIGNAL(clicked(bool)), physicalModelDiagramItem->newOptimization_, SIGNAL(triggered()));
  connect(physicalModelDiagramItem, SIGNAL(physicalModelValidityChanged(bool)), optimizationCreationButton, SLOT(setEnabled(bool)));

  appendButton(optimizationCreationButton, ++row, modelDefinitionButton);

  DiagramPushButton * observationButton = new DiagramPushButton;
  observationButton->setText(tr("Observations"));
  observationButton->setWhatsThis(tr("Define observations of variables"));
  observationButton->setErrorMessage(tr("Define at least two input variables and an output variable in the model"));
  connect(observationButton, SIGNAL(clicked(bool)), physicalModelDiagramItem->newObservations_, SIGNAL(triggered()));
  connect(physicalModelDiagramItem, SIGNAL(twoInputsValidityChanged(bool)), observationButton, SLOT(setEnabled(bool)));

  appendButton(observationButton, ++row, modelDefinitionButton);

  DiagramPushButton * calibrationButton = new DiagramPushButton;
  calibrationButton->setText(tr("Calibration"));
  calibrationButton->setWhatsThis(tr("Calibrate the model"));
  calibrationButton->setErrorMessage(tr("Define observations of the model"));
  connect(calibrationButton, SIGNAL(clicked(bool)), physicalModelDiagramItem, SLOT(requestCalibrationCreation()));
  connect(physicalModelDiagramItem, SIGNAL(observationsNumberValidityChanged(bool)), calibrationButton, SLOT(setEnabled(bool)));

  appendButton(calibrationButton, row, observationButton);

  DiagramPushButton * doeCreationButton = new DiagramPushButton;
  doeCreationButton->setText(tr("Design of\nexperiments\ncreation"));
  doeCreationButton->setWhatsThis(tr("Create manually a design of experiments or import one"));
  doeCreationButton->setErrorMessage(tr("Define at least one input variable"));
  connect(doeCreationButton, SIGNAL(clicked(bool)), physicalModelDiagramItem->newDesignOfExperiment_, SIGNAL(triggered()));
  connect(physicalModelDiagramItem, SIGNAL(inputNumberValidityChanged(bool)), doeCreationButton, SLOT(setEnabled(bool)));

  appendButton(doeCreationButton, ++row, modelDefinitionButton);

  DiagramPushButton * doeEvaluationButton = new DiagramPushButton;
  doeEvaluationButton->setText(tr("Design of\nexperiments\nevaluation"));
  doeEvaluationButton->setWhatsThis(tr("Evaluate the model at the points of the design of experiments"));
  doeEvaluationButton->setErrorMessage(tr("Create at least one design of experiments and define output variables in the model"));
  connect(doeEvaluationButton, SIGNAL(clicked(bool)), physicalModelDiagramItem, SLOT(requestDesignOfExperimentEvaluation()));
  connect(physicalModelDiagramItem, SIGNAL(doeNumberValidityChanged(bool)), doeEvaluationButton, SLOT(setEnabled(bool)));

  appendButton(doeEvaluationButton, row, doeCreationButton);

  DiagramPushButton * metamodelButton = new DiagramPushButton;
  metamodelButton->setText(tr("Metamodel\ncreation"));
  metamodelButton->setWhatsThis(tr("Two methods : Kriging and Functional chaos"));
  metamodelButton->setErrorMessage(tr("Define at least one design of experiments which contains output values"));
  connect(physicalModelDiagramItem, SIGNAL(doeEvaluationNumberValidityChanged(bool)), metamodelButton, SLOT(setEnabled(bool)));
  connect(metamodelButton, SIGNAL(clicked(bool)), physicalModelDiagramItem, SLOT(requestMetaModelCreation()));

  appendButton(metamodelButton, row, doeEvaluationButton);

  DiagramPushButton * metamodelExportButton = new DiagramPushButton;
  metamodelExportButton->setText(tr("Export\nas model"));
  metamodelExportButton->setWhatsThis(tr("Export metamodel as model"));
  metamodelExportButton->setErrorMessage(tr("Metamodel must first be created"));
  connect(physicalModelDiagramItem, SIGNAL(metamodelNumberValidityChanged(bool)), metamodelExportButton, SLOT(setEnabled(bool)));
  connect(metamodelExportButton, SIGNAL(clicked(bool)), physicalModelDiagramItem, SLOT(requestMetaModelExport()));

  appendButton(metamodelExportButton, row, metamodelButton);

  DiagramPushButton * probaModelButton = new DiagramPushButton;
  probaModelButton->setText(tr("Probabilistic\nmodel\ndefinition"));
  probaModelButton->setWhatsThis(tr("Define the stochastic input variables and the dependence"));
  probaModelButton->setErrorMessage(tr("Define at least one input variable"));
  connect(probaModelButton, SIGNAL(clicked(bool)), physicalModelDiagramItem, SLOT(appendProbabilisticModelItem()));
  connect(physicalModelDiagramItem, SIGNAL(inputNumberValidityChanged(bool)), probaModelButton, SLOT(setEnabled(bool)));

  appendButton(probaModelButton, ++row, modelDefinitionButton);

  DiagramPushButton * sensitivityButton = new DiagramPushButton;
  sensitivityButton->setText(tr("Sensitivity"));
  sensitivityButton->setWhatsThis(tr("Two methods : Sobol and SRC"));
  sensitivityButton->setErrorMessage(tr("Define independent stochastic input variables and output variables in the model"));
  connect(sensitivityButton, SIGNAL(clicked(bool)), physicalModelDiagramItem->newSensitivityAnalysis_, SIGNAL(triggered()));
  connect(physicalModelDiagramItem, SIGNAL(dependenceValidityChanged(bool)), sensitivityButton, SLOT(setEnabled(bool)));

  appendButton(sensitivityButton, row, probaModelButton);

  DiagramPushButton * centralTendencyButton = new DiagramPushButton;
  centralTendencyButton->setText(tr("Central\ntendency"));
  centralTendencyButton->setWhatsThis(tr("Two methods : Monte Carlo and Taylor expansion"));
  centralTendencyButton->setErrorMessage(tr("Define stochastic input variables and output variables in the model"));
  connect(centralTendencyButton, SIGNAL(clicked(bool)), physicalModelDiagramItem->newCentralTendency_, SIGNAL(triggered()));
  connect(physicalModelDiagramItem, SIGNAL(probabilisticModelValidityChanged(bool)), centralTendencyButton, SLOT(setEnabled(bool)));

  appendButton(centralTendencyButton, ++row, probaModelButton);

  DiagramPushButton * limitStateButton = new DiagramPushButton;
  limitStateButton->setText(tr("Limit state\ndefinition"));
  limitStateButton->setWhatsThis(tr("Define a limit state to perform threshold exceedance analysis"));
  limitStateButton->setErrorMessage(tr("Define input and output variables in the model"));
  connect(limitStateButton, SIGNAL(clicked(bool)), physicalModelDiagramItem->newLimitState_, SIGNAL(triggered()));
  connect(physicalModelDiagramItem, SIGNAL(probabilisticModelValidityChanged(bool)), limitStateButton, SLOT(setEnabled(bool)));

  appendButton(limitStateButton, ++row, probaModelButton);

  DiagramPushButton * reliabilityButton = new DiagramPushButton;
  reliabilityButton->setText(tr("Reliability"));
  reliabilityButton->setWhatsThis(tr("Four methods : Monte Carlo, FORM-Importance Sampling, FORM, SORM"));
  reliabilityButton->setErrorMessage(tr("Define output variables in the model and at least one limit state"));
  connect(physicalModelDiagramItem, SIGNAL(limitStateNumberValidityChanged(bool)), reliabilityButton, SLOT(setEnabled(bool)));
  connect(reliabilityButton, SIGNAL(clicked(bool)), physicalModelDiagramItem, SLOT(requestReliabilityCreation()));

  appendButton(reliabilityButton, row, limitStateButton);

  updateDiagram();
}
}
