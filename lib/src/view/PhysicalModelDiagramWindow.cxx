//                                               -*- C++ -*-
/**
 *  @brief QMdiSubWindow to define the diagram of the physical model
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
#include "persalys/PhysicalModelDiagramWindow.hxx"

#include <openturns/PlatformInfo.hxx>

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
  connect(modelEvaluationButton, SIGNAL(clicked(bool)), physicalModelDiagramItem->newModelEvaluation_, SIGNAL(triggered()));
  connect(physicalModelDiagramItem, SIGNAL(physicalModelValidityChanged(bool, QString)), modelEvaluationButton, SLOT(setEnabled(bool, QString)));

  appendButton(modelEvaluationButton, row, modelDefinitionButton);

#ifdef PERSALYS_HAVE_OTMORRIS
  DiagramPushButton * screeningButton = new DiagramPushButton;
  screeningButton->setText(tr("Screening"));
  screeningButton->setWhatsThis(tr("One screening method : Morris"));
  connect(screeningButton, SIGNAL(clicked(bool)), physicalModelDiagramItem->newScreening_, SIGNAL(triggered()));
  connect(physicalModelDiagramItem, SIGNAL(twoInputsValidityChanged(bool, QString)), screeningButton, SLOT(setEnabled(bool, QString)));

  appendButton(screeningButton, ++row, modelDefinitionButton);
#endif

  DiagramPushButton * optimizationCreationButton = new DiagramPushButton;
  optimizationCreationButton->setText(tr("Optimization"));
  optimizationCreationButton->setWhatsThis(tr("Optimization of the model function"));
  connect(optimizationCreationButton, SIGNAL(clicked(bool)), physicalModelDiagramItem->newOptimization_, SIGNAL(triggered()));
  connect(physicalModelDiagramItem, SIGNAL(physicalModelValidityChanged(bool, QString)), optimizationCreationButton, SLOT(setEnabled(bool, QString)));

  appendButton(optimizationCreationButton, ++row, modelDefinitionButton);

  if (OT::PlatformInfo::HasFeature("pagmo"))
  {
    DiagramPushButton * moOptimizationCreationButton = new DiagramPushButton;
    moOptimizationCreationButton->setText(tr("Multi-objective\noptimization"));
    moOptimizationCreationButton->setWhatsThis(tr("Multi-objective optimization of the model function"));
    connect(moOptimizationCreationButton, SIGNAL(clicked(bool)), physicalModelDiagramItem->newMoOptimization_, SIGNAL(triggered()));
    connect(physicalModelDiagramItem, SIGNAL(outputNumberValidityChanged(bool, QString)), moOptimizationCreationButton, SLOT(setEnabled(bool, QString)));

    appendButton(moOptimizationCreationButton, ++row, modelDefinitionButton);
  }

  DiagramPushButton * observationButton = new DiagramPushButton;
  observationButton->setText(tr("Observations"));
  observationButton->setWhatsThis(tr("Define observations of variables"));
  connect(observationButton, SIGNAL(clicked(bool)), physicalModelDiagramItem->newObservations_, SIGNAL(triggered()));
  connect(physicalModelDiagramItem, SIGNAL(twoInputsValidityChanged(bool, QString)), observationButton, SLOT(setEnabled(bool, QString)));

  appendButton(observationButton, ++row, modelDefinitionButton);

  DiagramPushButton * calibrationButton = new DiagramPushButton;
  calibrationButton->setText(tr("Calibration"));
  calibrationButton->setWhatsThis(tr("Calibrate the model"));
  connect(calibrationButton, SIGNAL(clicked(bool)), physicalModelDiagramItem, SLOT(requestCalibrationCreation()));
  connect(physicalModelDiagramItem, SIGNAL(observationsNumberValidityChanged(bool, QString)), calibrationButton, SLOT(setEnabled(bool, QString)));

  appendButton(calibrationButton, row, observationButton);

  DiagramPushButton * doeCreationButton = new DiagramPushButton;
  doeCreationButton->setText(tr("Design of\nexperiments\ncreation"));
  doeCreationButton->setWhatsThis(tr("Create manually a design of experiments or import one"));
  connect(doeCreationButton, SIGNAL(clicked(bool)), physicalModelDiagramItem->newDesignOfExperiment_, SIGNAL(triggered()));
  connect(physicalModelDiagramItem, SIGNAL(inputNumberValidityChanged(bool, QString)), doeCreationButton, SLOT(setEnabled(bool, QString)));

  appendButton(doeCreationButton, ++row, modelDefinitionButton);

  DiagramPushButton * doeEvaluationButton = new DiagramPushButton;
  doeEvaluationButton->setText(tr("Design of\nexperiments\nevaluation"));
  doeEvaluationButton->setWhatsThis(tr("Evaluate the model at the points of the design of experiments"));
  connect(doeEvaluationButton, SIGNAL(clicked(bool)), physicalModelDiagramItem, SLOT(requestDesignOfExperimentEvaluation()));
  connect(physicalModelDiagramItem, SIGNAL(doeNumberValidityChanged(bool, QString)), doeEvaluationButton, SLOT(setEnabled(bool, QString)));

  appendButton(doeEvaluationButton, row, doeCreationButton);

  DiagramPushButton * metamodelButton = new DiagramPushButton;
  metamodelButton->setText(tr("Metamodel\ncreation"));
  metamodelButton->setWhatsThis(tr("Build a computationally cheap approximation model from input-output data"));
  connect(physicalModelDiagramItem, SIGNAL(doeEvaluationNumberValidityChanged(bool, QString)), metamodelButton, SLOT(setEnabled(bool, QString)));
  connect(metamodelButton, SIGNAL(clicked(bool)), physicalModelDiagramItem, SLOT(requestMetaModelCreation()));

  appendButton(metamodelButton, row, doeEvaluationButton);

  DiagramPushButton * metamodelExportButton = new DiagramPushButton;
  metamodelExportButton->setText(tr("Export\nas model"));
  metamodelExportButton->setWhatsThis(tr("Export metamodel as model"));
  connect(physicalModelDiagramItem, SIGNAL(metamodelNumberValidityChanged(bool, QString)), metamodelExportButton, SLOT(setEnabled(bool, QString)));
  connect(metamodelExportButton, SIGNAL(clicked(bool)), physicalModelDiagramItem, SLOT(requestMetaModelExport()));

  appendButton(metamodelExportButton, row, metamodelButton);

  DiagramPushButton * probaModelButton = new DiagramPushButton;
  probaModelButton->setText(tr("Probabilistic\nmodel\ndefinition"));
  probaModelButton->setWhatsThis(tr("Define the stochastic input variables and the dependence"));
  connect(probaModelButton, SIGNAL(clicked(bool)), physicalModelDiagramItem, SLOT(appendProbabilisticModelItem()));
  connect(physicalModelDiagramItem, SIGNAL(inputNumberValidityChanged(bool, QString)), probaModelButton, SLOT(setEnabled(bool, QString)));

  appendButton(probaModelButton, ++row, modelDefinitionButton);

  DiagramPushButton * sensitivityButton = new DiagramPushButton;
  sensitivityButton->setText(tr("Sensitivity"));
  sensitivityButton->setWhatsThis(tr("Two methods : Sobol and SRC"));
  connect(sensitivityButton, SIGNAL(clicked(bool)), physicalModelDiagramItem->newSensitivityAnalysis_, SIGNAL(triggered()));
  connect(physicalModelDiagramItem, SIGNAL(dependenceValidityChanged(bool, QString)), sensitivityButton, SLOT(setEnabled(bool, QString)));

  appendButton(sensitivityButton, row, probaModelButton);

  DiagramPushButton * centralTendencyButton = new DiagramPushButton;
  centralTendencyButton->setText(tr("Central\ntendency"));
  centralTendencyButton->setWhatsThis(tr("Two methods : Monte Carlo and Taylor expansion"));
  connect(centralTendencyButton, SIGNAL(clicked(bool)), physicalModelDiagramItem->newCentralTendency_, SIGNAL(triggered()));
  connect(physicalModelDiagramItem, SIGNAL(probabilisticModelValidityChanged(bool, QString)), centralTendencyButton, SLOT(setEnabled(bool, QString)));

  appendButton(centralTendencyButton, ++row, probaModelButton);

  DiagramPushButton * limitStateButton = new DiagramPushButton;
  limitStateButton->setText(tr("Limit state\ndefinition"));
  limitStateButton->setWhatsThis(tr("Define a limit state to perform threshold exceedance analysis"));
  connect(limitStateButton, SIGNAL(clicked(bool)), physicalModelDiagramItem->newLimitState_, SIGNAL(triggered()));
  connect(physicalModelDiagramItem, SIGNAL(probabilisticModelValidityChanged(bool, QString)), limitStateButton, SLOT(setEnabled(bool, QString)));

  appendButton(limitStateButton, ++row, probaModelButton);

  DiagramPushButton * reliabilityButton = new DiagramPushButton;
  reliabilityButton->setText(tr("Reliability"));
  reliabilityButton->setWhatsThis(tr("Four methods : Monte Carlo, FORM-Importance Sampling, FORM, SORM"));
  connect(physicalModelDiagramItem, SIGNAL(limitStateNumberValidityChanged(bool, QString)), reliabilityButton, SLOT(setEnabled(bool, QString)));
  connect(reliabilityButton, SIGNAL(clicked(bool)), physicalModelDiagramItem, SLOT(requestReliabilityCreation()));

  appendButton(reliabilityButton, row, limitStateButton);

  updateDiagram();
}
}
