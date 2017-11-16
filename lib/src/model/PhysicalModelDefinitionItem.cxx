//                                               -*- C++ -*-
/**
 *  @brief QStandardItem, observer of a physical model
 *
 *  Copyright 2015-2017 EDF-Phimeca
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
 *  You should have received a copy of the GNU Lesser General Public
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
#include "otgui/PhysicalModelDefinitionItem.hxx"

#include "otgui/ModelEvaluation.hxx"
#include "otgui/GridDesignOfExperiment.hxx"
#include "otgui/OTStudyItem.hxx"

#include <QDebug>

using namespace OT;

namespace OTGUI
{

PhysicalModelDefinitionItem::PhysicalModelDefinitionItem(const PhysicalModel & physicalModel)
  : PhysicalModelItem(physicalModel, "PhysicalModelDefinition")
{
  setData(tr("Definition"), Qt::DisplayRole);
  QFont font;
  font.setWeight(font.weight() + 10);
  setData(font, Qt::FontRole);
  setEditable(false);

  buildActions();
}


void PhysicalModelDefinitionItem::buildActions()
{
  // new probabilistic model action
  newProbabilisticModel_ = new QAction(tr("Probabilistic model"), this);
  newProbabilisticModel_->setStatusTip(tr("Create a new probabilistic model"));
  connect(newProbabilisticModel_, SIGNAL(triggered()), this, SLOT(createProbabilisticModel()));

  // new design of experiments action
  newDesignOfExperiment_ = new QAction(QIcon(":/images/designOfExperiment.png"), tr("Design of experiments"), this);
  newDesignOfExperiment_->setStatusTip(tr("Create a new design of experiments"));
  connect(newDesignOfExperiment_, SIGNAL(triggered()), this, SLOT(createNewDesignOfExperiment()));

  // new analysis actions
  newModelEvaluation_ = new QAction(QIcon(":/images/modelEvaluation.png"), tr("Evaluation"), this);
  newModelEvaluation_->setStatusTip(tr("Create a new model evaluation"));
  connect(newModelEvaluation_, SIGNAL(triggered()), this, SLOT(createNewModelEvaluation()));

  // add actions
  appendAction(newProbabilisticModel_);
  appendAction(newModelEvaluation_);
  appendAction(newDesignOfExperiment_);
}


void PhysicalModelDefinitionItem::update(Observable* source, const String & message)
{
  // emit signals to PhysicalModelWindowWidget

  // input signals
  if (message == "inputNumberChanged")
  {
    emit numberInputsChanged();
  }
  else if (message == "inputNameChanged")
  {
    emit inputListDifferentiationChanged();
    emit inputListDefinitionChanged();
  }
  else if (message == "inputDescriptionChanged")
  {
    emit inputListDefinitionChanged();
  }
  else if (message == "inputValueChanged")
  {
    emit inputListDefinitionChanged();
    emit outputChanged();
  }
  else if (message == "inputStepChanged")
  {
    emit inputListDifferentiationChanged();
  }
  // output signals
  else if (message == "outputNumberChanged")
  {
    emit numberOutputsChanged();
  }
  else if (message == "outputNameChanged" ||
           message == "outputDescriptionChanged" ||
           message == "outputFormulaChanged" ||
           message == "outputSelectionChanged" ||
           message == "outputValueChanged"
          )
  {
    emit outputChanged();
  }
  else if (message == "codeChanged")
  {
    emit codeChanged();
  }
  else if (message == "parallelizeStatusChanged")
  {
    emit parallelizeStatusChanged();
  }
  else if (message == "wantedMachineChanged")
  {
    emit wantedMachineChanged();
  }
  else if (message == "physicalModelRemoved")
  {
    if (hasChildren())
      qDebug() << "PhysicalModelDefinitionItem::update(physicalModelRemoved) has not to contain child\n";
    emit removeRequested(row());
  }
}


void PhysicalModelDefinitionItem::createProbabilisticModel()
{
  // check
  if (!physicalModel_.getInputDimension())
  {
    emit emitErrorMessageRequested(tr("The physical model must have inputs."));
    return;
  }
  // emit signal to StudyTreeView to create a window
  emit probabilisticModelRequested(this);
}


void PhysicalModelDefinitionItem::createNewModelEvaluation()
{
  // check
  if (!physicalModel_.isValid())
  {
    emit emitErrorMessageRequested(tr("The physical model must have inputs AND at least one selected output."));
    return;
  }

  // new analysis
  ModelEvaluation evaluation(getParentOTStudyItem()->getOTStudy().getAvailableAnalysisName("evaluation_"), physicalModel_);
  // emit signal to StudyTreeView to open a wizard
  emit analysisRequested(this, evaluation);
}


void PhysicalModelDefinitionItem::createNewDesignOfExperiment()
{
  // check
  if (!physicalModel_.getInputDimension())
  {
    emit emitErrorMessageRequested(tr("The physical model must have inputs."));
    return;
  }

  // new design
  GridDesignOfExperiment design(getParentOTStudyItem()->getOTStudy().getAvailableAnalysisName("design_"), physicalModel_);
  // emit signal to StudyTreeView to open a wizard
  emit analysisRequested(this, design);
}


void PhysicalModelDefinitionItem::updateProbaActionAvailability()
{
  // disable the new proba model action
  newProbabilisticModel_->setDisabled(true);
}
}
