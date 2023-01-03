//                                               -*- C++ -*-
/**
 *  @brief QMdiSubWindow to define the diagram of the physical model
 *
 *  Copyright 2015-2023 EDF-Phimeca
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

namespace PERSALYS
{

FieldModelDiagramWindow::FieldModelDiagramWindow(PhysicalModelDiagramItem * physicalModelDiagramItem, QWidget * parent)
  : DiagramWindow(physicalModelDiagramItem, parent)
{
  titleLabel_->setDocLink("user_manual/graphical_interface/field_model/user_manual_field_model.html#fieldmodeldiagram");


  // -- Buttons --
  DiagramPushButton * modelDefinitionButton = new DiagramPushButton;
  modelDefinitionButton->setText(tr("Model\ndefinition"));
  modelDefinitionButton->setWhatsThis(tr("Define the input and output variables"));
  connect(modelDefinitionButton, SIGNAL(clicked(bool)), physicalModelDiagramItem, SLOT(appendPhysicalModelItem()));

  appendButton(modelDefinitionButton, 0);

  DiagramPushButton * modelEvaluationButton = new DiagramPushButton;
  modelEvaluationButton->setText(tr("Model\nevaluation"));
  modelEvaluationButton->setWhatsThis(tr("Evaluate the model at a given point"));
  modelEvaluationButton->setErrorMessage(tr("Define input and output variables in the model"));
  connect(modelEvaluationButton, SIGNAL(clicked(bool)), physicalModelDiagramItem->newModelEvaluation_, SIGNAL(triggered()));
  connect(physicalModelDiagramItem, SIGNAL(physicalModelValidityChanged(bool)), modelEvaluationButton, SLOT(setEnabled(bool)));

  appendButton(modelEvaluationButton, 0, modelDefinitionButton);

//   DiagramPushButton * doeCreationButton = new DiagramPushButton;
//   doeCreationButton->setText(tr("Design of\nexperiments\ncreation"));
//   doeCreationButton->setWhatsThis(tr("Create manually a design of experiments or import one"));
//   doeCreationButton->setErrorMessage(tr("Define at least one input variable"));

//   connect(doeCreationButton, SIGNAL(clicked(bool)), physicalModelDiagramItem, SIGNAL(designOfExperimentRequested()));
//   connect(physicalModelDiagramItem, SIGNAL(inputNumberValidityChanged(bool)), doeCreationButton, SLOT(setEnabled(bool)));


  DiagramPushButton * probaModelButton = new DiagramPushButton;
  probaModelButton->setText(tr("Probabilistic\nmodel\ndefinition"));
  probaModelButton->setWhatsThis(tr("Define the stochastic input variables and the dependence"));
  probaModelButton->setErrorMessage(tr("Define at least one input variable"));
  connect(probaModelButton, SIGNAL(clicked(bool)), physicalModelDiagramItem, SLOT(appendProbabilisticModelItem()));
  connect(physicalModelDiagramItem, SIGNAL(inputNumberValidityChanged(bool)), probaModelButton, SLOT(setEnabled(bool)));
  appendButton(probaModelButton, 1, modelDefinitionButton);

//   DiagramPushButton * doeEvaluationButton = new DiagramPushButton;
//   doeEvaluationButton->setText(tr("Design of\nexperiments\nevaluation"));
//   doeEvaluationButton->setWhatsThis(tr("Evaluate the model at the points of the design of experiments"));
//   doeEvaluationButton->setErrorMessage(tr("Create at least one design of experiments and define output variables in the model"));

//   connect(doeEvaluationButton, SIGNAL(clicked(bool)), physicalModelDiagramItem, SLOT(requestDesignOfExperimentEvaluation()));
//   connect(physicalModelDiagramItem, SIGNAL(doeNumberValidityChanged(bool)), doeEvaluationButton, SLOT(setEnabled(bool)));


  DiagramPushButton * centralTendencyButton = new DiagramPushButton;
  centralTendencyButton->setText(tr("Central\ntendency"));
  centralTendencyButton->setWhatsThis(tr("Two methods : Monte Carlo and Taylor expansion"));
  centralTendencyButton->setErrorMessage(tr("Define stochastic input variables and output variables in the model"));
  connect(centralTendencyButton, SIGNAL(clicked(bool)), physicalModelDiagramItem->newCentralTendency_, SIGNAL(triggered()));
  connect(physicalModelDiagramItem, SIGNAL(probabilisticModelValidityChanged(bool)), centralTendencyButton, SLOT(setEnabled(bool)));
  appendButton(centralTendencyButton, 1, probaModelButton);

  updateDiagram();
}
}
