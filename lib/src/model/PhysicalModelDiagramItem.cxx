//                                               -*- C++ -*-
/**
 *  @brief QStandardItem, observer of a physical model
 *
 *  Copyright 2015-2024 EDF-Phimeca
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
#include "persalys/PhysicalModelDiagramItem.hxx"

#include "persalys/ModelEvaluation.hxx"
#include "persalys/StudyItem.hxx"
#include "persalys/Observations.hxx"
#include "persalys/MeshItem.hxx"
#include "persalys/ProbabilisticModelItem.hxx"
#include "persalys/LimitStateItem.hxx"
#include "persalys/DesignOfExperimentDefinitionItem.hxx"
#include "persalys/ObservationsItem.hxx"
#include "persalys/FunctionalChaosAnalysis.hxx"
#include "persalys/KrigingAnalysis.hxx"

#include <openturns/PlatformInfo.hxx>

#include <QDebug>

using namespace OT;

namespace PERSALYS
{

PhysicalModelDiagramItem::PhysicalModelDiagramItem(const PhysicalModel & physicalModel)
  : PhysicalModelItem(physicalModel, "PhysicalModelDiagramItem")
  , limitStateCounter_(0)
  , observationsCounter_(0)
  , metamodelCounter_(0)
  , doeCounter_(Indices(2))
{
  setData(QString::fromUtf8(physicalModel.getName().c_str()), Qt::DisplayRole);

  buildActions();
}


void PhysicalModelDiagramItem::buildActions()
{
  // define physical model action
  defineAction_ = new QAction(tr("Define the model"), this);
  defineAction_->setStatusTip(tr("Define the physical model"));
  connect(defineAction_, SIGNAL(triggered()), this, SLOT(appendPhysicalModelItem()));

  // duplicate physical model action
  duplicateAction_ = new QAction(tr("Duplicate the model"), this);
  duplicateAction_->setStatusTip(tr("Duplicate the physical model"));
  connect(duplicateAction_, SIGNAL(triggered()), this, SLOT(duplicatePhysicalModel()));

  // new probabilistic model action
  newProbabilisticModel_ = new QAction(tr("Probabilistic model"), this);
  newProbabilisticModel_->setStatusTip(tr("Create a new probabilistic model"));
  connect(newProbabilisticModel_, SIGNAL(triggered()), this, SLOT(appendProbabilisticModelItem()));

  // new analysis actions
  newModelEvaluation_ = createAction("ModelEvaluation", getPhysicalModel());
  newCentralTendency_ = createAction("CentralTendency", getPhysicalModel());

  if (!physicalModel_.hasMesh())
  {
    newDesignOfExperiment_ = createAction("DesignOfExperiment", getPhysicalModel());
    newScreening_ = createAction("Screening", getPhysicalModel());
    newOptimization_ = createAction("Optimization", getPhysicalModel());
    if (PlatformInfo::HasFeature("pagmo"))
      newMoOptimization_ = createAction("MultiObjectiveOptimization", getPhysicalModel());
    newSensitivityAnalysis_ = createAction("Sensitivity", getPhysicalModel());

    // new limit state action
    newLimitState_ = new QAction(QIcon(":/images/limitstate.png"), tr("Limit state"), this);
    newLimitState_->setStatusTip(tr("Create a new limit state"));
    connect(newLimitState_, SIGNAL(triggered()), this, SLOT(newLimitState()));

    // new observations action
    newObservations_ = new QAction(tr("Observations"), this);
    newObservations_->setStatusTip(tr("Define observations of the model"));
    connect(newObservations_, SIGNAL(triggered()), this, SLOT(newObservations()));
  }

  // remove physical model action
  removeAction_ = new QAction(QIcon(":/images/window-close.png"), tr("Remove"), this);
  removeAction_->setStatusTip(tr("Remove the physical model"));
  connect(removeAction_, SIGNAL(triggered()), this, SLOT(removePhysicalModel()));

  // add actions
  appendAction(defineAction_);
  appendAction(duplicateAction_);
  appendSeparator();
  appendAction(removeAction_);
}


void PhysicalModelDiagramItem::setData(const QVariant & value, int role)
{
  // rename
  if (role == Qt::EditRole)
    physicalModel_.getImplementation()->setName(value.toString().toUtf8().data());

  QStandardItem::setData(value, role);
}


void PhysicalModelDiagramItem::update(Observable* /*source*/, const String & message)
{
  if (message == "outputNumberChanged" || message == "inputNumberChanged")
  {
    if (!hasChildren()) // if modification from Python console
    {
      fill();
      return;
    }
    updateDiagramBoxesValidity();
  }
  else if (message == "inputDistributionChanged")
  {
    appendProbabilisticModelItem(); // if modification from Python console
    updateDiagramBoxesValidity();
  }
  else if (message == "copulaChanged")
  {
    updateDiagramBoxesValidity();
  }
  else if (message == "analysisLaunched")
  {
    analysisInProgress_ = true;
  }
  else if (message == "analysisFinished" || message == "analysisBadlyFinished")
  {
    analysisInProgress_ = false;
  }
  else if (message == "objectRemoved")
  {
    if (hasChildren())
      qDebug() << "PhysicalModelDiagramItem::update(objectRemoved) has not to contain child\n";
    emit removeRequested(row());
  }
}


void PhysicalModelDiagramItem::updateDiagramBoxesValidity()
{
  // emit signals to PhysicalModelDiagramWindow
  // to update diagram (arrow color and button availability)
  emit inputNumberValidityChanged(physicalModel_.getInputDimension());
  emit twoInputsValidityChanged(physicalModel_.isValid() && physicalModel_.getInputDimension() > 1);
  emit outputNumberValidityChanged(physicalModel_.isValid() && physicalModel_.getOutputDimension() > 1 );
  emit physicalModelValidityChanged(physicalModel_.isValid());
  emit probabilisticModelValidityChanged(physicalModel_.isValid() && physicalModel_.hasStochasticInputs());
  emit dependenceValidityChanged(physicalModel_.isValid() && physicalModel_.hasStochasticInputs() && physicalModel_.getCopula().hasIndependentCopula());
  emit doeNumberValidityChanged(physicalModel_.isValid() && doeCounter_[0] > 0);
  emit doeEvaluationNumberValidityChanged(physicalModel_.isValid() && doeCounter_[1] > 0);
  emit limitStateNumberValidityChanged(physicalModel_.isValid() && physicalModel_.hasStochasticInputs() && limitStateCounter_ > 0);
  emit observationsNumberValidityChanged(physicalModel_.isValid() && physicalModel_.getInputDimension() > 1 && observationsCounter_ > 0);
  emit metamodelNumberValidityChanged(physicalModel_.isValid() && metamodelCounter_ > 0);
}


void PhysicalModelDiagramItem::newLimitState()
{
  // check
  if (!physicalModel_.isValid() || !physicalModel_.hasStochasticInputs())
  {
    emit showErrorMessageRequested(tr("The physical model must have at least a stochastic input and at least one selected output."));
    return;
  }
  // new limit state
  LimitState newLimitState(getParentStudyItem()->getStudy().getAvailableLimitStateName(tr("limitState_").toStdString()), physicalModel_, physicalModel_.getSelectedOutputsNames()[0], OT::Less(), 0.);
  getParentStudyItem()->getStudy().add(newLimitState);
}


void PhysicalModelDiagramItem::newObservations()
{
  // check
  if (!physicalModel_.isValid() || physicalModel_.getInputDimension() < 2)
  {
    emit showErrorMessageRequested(tr("The physical model must have at least two inputs and at least one selected output."));
    return;
  }
  // new observations
  const String obsName(getParentStudyItem()->getStudy().getAvailableDataModelName(tr("observations_").toStdString()));
  Observations obs(obsName, physicalModel_);
  // emit signal to StudyManager to open a wizard
  emit wizardRequested(getParentStudyItem(), obs);
}


void PhysicalModelDiagramItem::requestDesignOfExperimentEvaluation()
{
  // find a DesignOfExperimentEvaluation
  Study study(getParentStudyItem()->getStudy());
  for (UnsignedInteger i = 0; i < study.getAnalyses().getSize(); ++i)
  {
    DesignOfExperimentEvaluation * doeEval = dynamic_cast<DesignOfExperimentEvaluation *>(study.getAnalyses()[i].getImplementation().get());
    ModelEvaluation * modelEval = dynamic_cast<ModelEvaluation *>(study.getAnalyses()[i].getImplementation().get());
    if (doeEval && !modelEval && doeEval->getPhysicalModel() == getPhysicalModel())
    {
      emit doeEvaluationWizardRequested(study.getAnalyses()[i], true);
      return;
    }
  }
  // emit error message
  emit showErrorMessageRequested(tr("We have not found a design of experiments.\n"));
}


void PhysicalModelDiagramItem::requestMetaModelCreation()
{
  // find a DesignOfExperimentEvaluation with results
  Study study(getParentStudyItem()->getStudy());
  for (UnsignedInteger i = 0; i < study.getAnalyses().getSize(); ++i)
  {
    DesignOfExperimentEvaluation * doeEval = dynamic_cast<DesignOfExperimentEvaluation *>(study.getAnalyses()[i].getImplementation().get());
    ModelEvaluation * modelEval = dynamic_cast<ModelEvaluation *>(study.getAnalyses()[i].getImplementation().get());
    if (doeEval && !modelEval && doeEval->getPhysicalModel() == getPhysicalModel() && doeEval->hasValidResult())
    {
      newAnalysis("Metamodel", study.getAnalyses()[i], true);
      return;
    }
  }
  // emit error message
  emit showErrorMessageRequested(tr("We have not found a design of experiments with an output sample.\n"));
}

void PhysicalModelDiagramItem::requestMetaModelExport()
{
  Study study(getParentStudyItem()->getStudy());
  for (UnsignedInteger i = 0; i < study.getAnalyses().getSize(); ++i) {
    const MetaModelAnalysis * analysis = dynamic_cast<const MetaModelAnalysis*>(study.getAnalyses()[i].getImplementation().get());
    if (analysis && analysis->hasValidResult()) {
      emit mmExportWizardRequested(getParentStudyItem(), study.getAnalyses()[i], true);
      return;
    }
  }
}

void PhysicalModelDiagramItem::requestReliabilityCreation()
{
  for (UnsignedInteger i = 0; i < getParentStudyItem()->getStudy().getLimitStates().getSize(); ++i)
  {
    if (getParentStudyItem()->getStudy().getLimitStates()[i].getPhysicalModel() == physicalModel_)
    {
      newAnalysis("ThresholdExceedance", getParentStudyItem()->getStudy().getLimitStates()[i], true);
      return;
    }
  }
}


void PhysicalModelDiagramItem::requestCalibrationCreation()
{
  for (UnsignedInteger i = 0; i < getParentStudyItem()->getStudy().getDataModels().getSize(); ++i)
  {
    if (getParentStudyItem()->getStudy().getDataModels()[i].getPhysicalModel() == physicalModel_)
    {
      newAnalysis("Calibration", getParentStudyItem()->getStudy().getDataModels()[i], true);
      return;
    }
  }
}


void PhysicalModelDiagramItem::duplicatePhysicalModel()
{
  if (!getParentStudyItem())
    return;
  // duplicate
  PhysicalModelImplementation * newModel = physicalModel_.getImplementation()->clone();
  String newName = getParentStudyItem()->getStudy().getAvailablePhysicalModelName((QString(physicalModel_.getName().c_str()) + "_" + tr("copy")).toStdString());
  newModel->setName(newName);
  getParentStudyItem()->getStudy().add(newModel);
}


void PhysicalModelDiagramItem::removePhysicalModel()
{
  // check
  if (analysisInProgress_)
  {
    emit showErrorMessageRequested(tr("Can not remove a physical model when an analysis is running."));
    return;
  }
  // remove
  if (getParentStudyItem())
    getParentStudyItem()->getStudy().remove(PhysicalModel(physicalModel_));
}


void PhysicalModelDiagramItem::fill()
{
  // mesh definition item
  if (physicalModel_.hasMesh())
  {
    MeshItem * meshItem = new MeshItem(getPhysicalModel());
    appendRow(meshItem);
    emit windowRequested(meshItem);
  }
  // model definition item
  if (physicalModel_.getInputDimension() || physicalModel_.getOutputDimension())
    appendPhysicalModelItem();
  // probabilistic model item
  if (physicalModel_.hasStochasticInputs())
    appendProbabilisticModelItem();
  updateDiagramBoxesValidity();
}


void PhysicalModelDiagramItem::appendPhysicalModelItem()
{
  // do nothing if the item already exists
  QModelIndexList listIndexes = model()->match(this->index(), Qt::UserRole, "PhysicalModelDefinitionItem", 1, Qt::MatchRecursive);
  if (listIndexes.size() == 1 && listIndexes[0].parent() == this->index())
  {
    // emit signal to the study tree to display the window
    emit changeCurrentItemRequested(listIndexes[0]);
    return;
  }

  // new item
  PhysicalModelDefinitionItem * pmItem = new PhysicalModelDefinitionItem(getPhysicalModel(), this);
  appendRow(pmItem);

  // emit signal to the StudyManager to create a window
  emit windowRequested(pmItem);

  // disable the definition action
  defineAction_->setDisabled(true);
}


void PhysicalModelDiagramItem::appendProbabilisticModelItem()
{
  // check
  if (!physicalModel_.getInputDimension())
  {
    emit showErrorMessageRequested(tr("The physical model must have inputs."));
    return;
  }
  // do nothing if the item already exists
  QModelIndexList listIndexes = model()->match(this->index(), Qt::UserRole, "ProbabilisticModelItem", 1, Qt::MatchRecursive);
  if (listIndexes.size() == 1 && listIndexes[0].parent() == this->index())
  {
    // emit signal to the study tree to display the window
    emit changeCurrentItemRequested(listIndexes[0]);
    return;
  }

  // new item
  ProbabilisticModelItem * probaItem = new ProbabilisticModelItem(getPhysicalModel(), this);

  // insert the item after the model definition item
  listIndexes = model()->match(this->index(), Qt::UserRole, "PhysicalModelDefinitionItem", 1, Qt::MatchRecursive);
  if (listIndexes.size() == 1 && listIndexes[0].parent() == this->index())
  {
    insertRow(listIndexes[0].row() + 1, probaItem);

    // emit signal to the StudyManager to create a window
    emit windowRequested(probaItem);
    // disable the new proba model action
    newProbabilisticModel_->setDisabled(true);
    return;
  }
  qDebug() << "In PhysicalModelDiagramItem::appendProbabilisticModelItem: No item added for the proba model\n";
}


void PhysicalModelDiagramItem::appendItem(const Analysis& analysis)
{
  const QString analysisName = analysis.getImplementation()->getClassName().c_str();
  // create new analysis item

  // if DesignOfExperimentEvaluation GridDesignOfExperiment ImportedDesignOfExperiment ProbabilisticDesignOfExperiment
  if (analysisName.contains("DesignOfExperiment"))
  {
    DesignOfExperimentDefinitionItem * newItem = new DesignOfExperimentDefinitionItem(analysis);

    // connections
    connect(newItem, SIGNAL(numberDesignEvaluationChanged(bool)), this, SLOT(requestDesignOfExperimentRemoval(bool)));
    connect(newItem, SIGNAL(designEvaluationUpdated(bool)), this , SLOT(updateDesignEvaluationCounter(bool)));

    // append item
    // search title, parent item of the new item
    Item * titleItem = getTitleItemNamed(analysisName);
    Q_ASSERT(titleItem != 0);
    titleItem->appendRow(newItem);

    // emit signal to StudyManager to create a window
    emit windowRequested(newItem);

    // add Evaluation item if needed : do it AFTER emit windowRequested
    newItem->fill();

    // signal for diagram window : update diagram
    ++doeCounter_[0];
    if (newItem->getAnalysis().hasValidResult())
      ++doeCounter_[1];

    emit doeNumberValidityChanged(physicalModel_.isValid() && doeCounter_[0] > 0);
    emit doeEvaluationNumberValidityChanged(physicalModel_.isValid() && doeCounter_[1] > 0);

  } else if (analysisName == "FunctionalChaosAnalysis" ||
             analysisName == "KrigingAnalysis" ||
	     analysisName == "PolynomialRegressionAnalysis") {
    AnalysisItem * newItem = dynamic_cast<AnalysisItem*>(analysis.getImplementation().get()->getObserver("AnalysisItem"));
    // connections
    connect(newItem, SIGNAL(numberMetamodelChanged(bool)), this, SLOT(updateMetamodelCounter(bool)));

    if(newItem->getAnalysis().hasValidResult())
      ++metamodelCounter_;
    emit metamodelNumberValidityChanged(physicalModel_.isValid() && metamodelCounter_ > 0);

  } else {
    appendAnalysisItem(analysis);
  }

  analysis.getImplementation().get()->addObserver(this);
  analysis.getImplementation().get()->addObserver(getParentStudyItem());
}


void PhysicalModelDiagramItem::appendItem(const LimitState& limitState)
{
  // new limit state item
  LimitStateItem * newItem = new LimitStateItem(limitState);

  // connections
  connect(newItem, SIGNAL(removeRequested(int)), this, SLOT(requestLimitStateRemoval()));

  // append item
  Item * titleItem = getTitleItemNamed("Reliability");
  titleItem->appendRow(newItem);

  // emit signal to StudyManager to create a window
  emit windowRequested(newItem);

  // signal for diagram window : update diagram
  ++limitStateCounter_;
  emit limitStateNumberValidityChanged(physicalModel_.isValid() && physicalModel_.hasStochasticInputs() && limitStateCounter_ > 0);
}


void PhysicalModelDiagramItem::appendItem(const DesignOfExperiment &designOfExp)
{
  ObservationsItem * newItem = new ObservationsItem(designOfExp);

  // connections
  connect(newItem, SIGNAL(removeRequested(int)), this, SLOT(requestObservationsRemoval()));

  Item * titleItem = getTitleItemNamed("Calibration");
  titleItem->appendRow(newItem);

  // emit signal to StudyManager to create a window
  emit windowRequested(newItem);

  // signal for diagram window : update diagram
  ++observationsCounter_;
  emit observationsNumberValidityChanged(physicalModel_.isValid() && observationsCounter_ > 0);
}


void PhysicalModelDiagramItem::requestLimitStateRemoval()
{
  // signal for diagram window : update diagram
  --limitStateCounter_;
  emit limitStateNumberValidityChanged(physicalModel_.isValid() && physicalModel_.hasStochasticInputs() && limitStateCounter_ > 0);
}


void PhysicalModelDiagramItem::requestObservationsRemoval()
{
  // signal for diagram window : update diagram
  --observationsCounter_;
  emit observationsNumberValidityChanged(physicalModel_.isValid() && observationsCounter_ > 0);
}


void PhysicalModelDiagramItem::updateDesignEvaluationCounter(bool increment)
{
  // signal for diagram window : update diagram
  if (increment)
    ++doeCounter_[1];
  else
    --doeCounter_[1];
  emit doeEvaluationNumberValidityChanged(physicalModel_.isValid() && doeCounter_[1] > 0);
}

void PhysicalModelDiagramItem::updateMetamodelCounter(bool increment) {
  if (increment)
    ++metamodelCounter_;
  else
    --metamodelCounter_;
  emit metamodelNumberValidityChanged(physicalModel_.isValid() && metamodelCounter_ > 0);
}

void PhysicalModelDiagramItem::requestDesignOfExperimentRemoval(bool isValid)
{
  // signal for diagram window : update diagram
  --doeCounter_[0];
  if (isValid)
    --doeCounter_[1];

  emit doeNumberValidityChanged(physicalModel_.isValid() && doeCounter_[0] > 0);
  emit doeEvaluationNumberValidityChanged(physicalModel_.isValid() && doeCounter_[1] > 0);
}
}
