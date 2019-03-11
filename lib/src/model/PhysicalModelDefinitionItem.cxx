//                                               -*- C++ -*-
/**
 *  @brief QStandardItem, observer of a physical model
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
#include "otgui/PhysicalModelDefinitionItem.hxx"

#include "otgui/ModelEvaluation.hxx"
#include "otgui/FieldModelEvaluation.hxx"
#include "otgui/GridDesignOfExperiment.hxx"
#include "otgui/StudyItem.hxx"
#ifdef OTGUI_HAVE_OTMORRIS
#include "otgui/MorrisAnalysis.hxx"
#endif
#include "otgui/OptimizationAnalysis.hxx"

#include <QDebug>

using namespace OT;

namespace OTGUI
{

PhysicalModelDefinitionItem::PhysicalModelDefinitionItem(const PhysicalModel & physicalModel)
  : PhysicalModelItem(physicalModel, "PhysicalModelDefinition")
  , newProbabilisticModel_(0)
  , newModelEvaluation_(0)
  , newScreening_(0)
  , newOptimization_(0)
  , newDesignOfExperiment_(0)
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
  if (!physicalModel_.hasMesh())
  {
    newDesignOfExperiment_ = new QAction(QIcon(":/images/designOfExperiment.png"), tr("Design of experiments"), this);
    newDesignOfExperiment_->setStatusTip(tr("Create a new design of experiments"));
    connect(newDesignOfExperiment_, SIGNAL(triggered()), this, SLOT(createDesignOfExperiment()));
  }

  // new evaluation action
  newModelEvaluation_ = new QAction(QIcon(":/images/modelEvaluation.png"), tr("Evaluation"), this);
  newModelEvaluation_->setStatusTip(tr("Create a new model evaluation"));
  connect(newModelEvaluation_, SIGNAL(triggered()), this, SLOT(createModelEvaluation()));

  if (!physicalModel_.hasMesh())
  {
#ifdef OTGUI_HAVE_OTMORRIS
    newScreening_ = new QAction(QIcon(":/images/sensitivity.png"), tr("Screening"), this);
    newScreening_->setStatusTip(tr("Create a new screening"));
    connect(newScreening_, SIGNAL(triggered()), this, SLOT(createScreening()));
#endif
    // new optimization action
    newOptimization_ = new QAction(QIcon(":/images/optimize.png"), tr("Optimization"), this);
    newOptimization_->setStatusTip(tr("Create a new model optimization"));
    connect(newOptimization_, SIGNAL(triggered()), this, SLOT(createOptimization()));
  }

  // add actions
  appendAction(newProbabilisticModel_);
  appendAction(newDesignOfExperiment_);
  appendSeparator(tr("Analysis"));
  appendAction(newModelEvaluation_);
  appendAction(newScreening_);
  appendAction(newOptimization_);
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
  else if (message == "parallelizationStatusChanged")
  {
    emit parallelizationStatusChanged();
  }
  else if (message == "meshChanged")
  {
    emit meshChanged();
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
    emit showErrorMessageRequested(tr("The physical model must have inputs."));
    return;
  }
  // emit signal to StudyManager to create a window
  emit probabilisticModelRequested(this);
}


void PhysicalModelDefinitionItem::createModelEvaluation()
{
  // check
  if (!physicalModel_.isValid())
  {
    emit showErrorMessageRequested(tr("The physical model must have inputs AND at least one selected output."));
    return;
  }

  // new analysis
  const String analysisName(getParentStudyItem()->getStudy().getAvailableAnalysisName(tr("evaluation_").toStdString()));
  if (!physicalModel_.hasMesh())
  {
    ModelEvaluation evaluation(analysisName, physicalModel_);
    // emit signal to StudyManager to open a wizard
    emit analysisRequested(this, evaluation);
  }
  else
  {
    FieldModelEvaluation evaluation(analysisName, physicalModel_);
    // emit signal to StudyManager to open a wizard
    emit analysisRequested(this, evaluation);
  }
}


#ifdef OTGUI_HAVE_OTMORRIS
void PhysicalModelDefinitionItem::createScreening()
{
  // check
  if (!physicalModel_.isValid() || physicalModel_.getInputDimension() < 2)
  {
    emit showErrorMessageRequested(tr("The physical model must have at least two inputs and at least one selected output."));
    return;
  }

  // new analysis
  const String analysisName(getParentStudyItem()->getStudy().getAvailableAnalysisName(tr("screening_").toStdString()));
  MorrisAnalysis analysis(analysisName, physicalModel_);
  // emit signal to StudyManager to open a wizard
  emit analysisRequested(this, analysis);
}
#endif


void PhysicalModelDefinitionItem::createOptimization()
{
  // check
  if (!physicalModel_.isValid())
  {
    emit showErrorMessageRequested(tr("The physical model must have inputs AND at least one selected output."));
    return;
  }

  // new analysis
  OptimizationAnalysis optimization(getParentStudyItem()->getStudy().getAvailableAnalysisName(tr("optimization_").toStdString()), physicalModel_);
  // emit signal to StudyTreeView to open a wizard
  emit analysisRequested(this, optimization);
}


void PhysicalModelDefinitionItem::createDesignOfExperiment()
{
  // check
  if (!physicalModel_.getInputDimension())
  {
    emit showErrorMessageRequested(tr("The physical model must have inputs."));
    return;
  }

  // new design
  const String doeName(getParentStudyItem()->getStudy().getAvailableAnalysisName(tr("design_").toStdString()));
  GridDesignOfExperiment design(doeName, physicalModel_);
  // emit signal to StudyManager to open a wizard
  emit analysisRequested(this, design);
}


void PhysicalModelDefinitionItem::updateProbaActionAvailability()
{
  // disable the new proba model action
  newProbabilisticModel_->setDisabled(true);
}
}
