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
  connect(modelEvaluationButton, SIGNAL(clicked(bool)), physicalModelDiagramItem->newModelEvaluation_, SIGNAL(triggered()));
  connect(physicalModelDiagramItem, SIGNAL(physicalModelValidityChanged(bool, QString)), modelEvaluationButton, SLOT(setEnabled(bool, QString)));

  appendButton(modelEvaluationButton, 0, modelDefinitionButton);

  DiagramPushButton * probaModelButton = new DiagramPushButton;
  probaModelButton->setText(tr("Probabilistic\nmodel\ndefinition"));
  probaModelButton->setWhatsThis(tr("Define the stochastic input variables and the dependence"));
  connect(probaModelButton, SIGNAL(clicked(bool)), physicalModelDiagramItem, SLOT(appendProbabilisticModelItem()));
  connect(physicalModelDiagramItem, SIGNAL(inputNumberValidityChanged(bool, QString)), probaModelButton, SLOT(setEnabled(bool, QString)));
  appendButton(probaModelButton, 1, modelDefinitionButton);

  DiagramPushButton * centralTendencyButton = new DiagramPushButton;
  centralTendencyButton->setText(tr("Central\ntendency"));
  centralTendencyButton->setWhatsThis(tr("Two methods : Monte Carlo and Taylor expansion"));
  connect(centralTendencyButton, SIGNAL(clicked(bool)), physicalModelDiagramItem->newCentralTendency_, SIGNAL(triggered()));
  connect(physicalModelDiagramItem, SIGNAL(probabilisticModelValidityChanged(bool, QString)), centralTendencyButton, SLOT(setEnabled(bool, QString)));
  appendButton(centralTendencyButton, 1, probaModelButton);

  updateDiagram();
}
}
