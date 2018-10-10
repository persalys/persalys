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
#include "otgui/PhysicalModelDiagramItem.hxx"

#include "otgui/OTStudyItem.hxx"
#include "otgui/MonteCarloReliabilityAnalysis.hxx"
#include "otgui/FunctionalChaosAnalysis.hxx"
#include "otgui/DesignOfExperimentEvaluation.hxx"

#include <QDebug>

using namespace OT;

namespace OTGUI
{

PhysicalModelDiagramItem::PhysicalModelDiagramItem(const PhysicalModel & physicalModel)
  : PhysicalModelItem(physicalModel, "PhysicalModelDiagram")
  , defineAction_(0)
  , removeAction_(0)
  , limitStateCounter_(0)
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

  // remove physical model action
  removeAction_ = new QAction(QIcon(":/images/window-close.png"), tr("Remove"), this);
  removeAction_->setStatusTip(tr("Remove the physical model"));
  connect(removeAction_, SIGNAL(triggered()), this, SLOT(removePhysicalModel()));

  // add actions
  appendAction(defineAction_);
  appendSeparator();
  appendAction(removeAction_);
}


void PhysicalModelDiagramItem::setData(const QVariant & value, int role)
{
  // rename
  if (role == Qt::EditRole)
    physicalModel_.getImplementation()->setName(value.toString().toLocal8Bit().data());

  QStandardItem::setData(value, role);
}


void PhysicalModelDiagramItem::update(Observable* source, const String & message)
{
  // emit signals to PhysicalModelDiagramWindow
  // to update the diagram (arrow color and button availability)

  if (message == "inputNumberChanged")
  {
    if (!hasChildren()) // if modification from Python console
    {
      fill();
      return;
    }
    emit inputNumberValidityChanged(physicalModel_.getInputDimension());
    emit twoInputsValidityChanged(physicalModel_.isValid() && physicalModel_.getInputDimension() > 1);
    emit physicalModelValidityChanged(physicalModel_.isValid());
    emit probabilisticModelValidityChanged(physicalModel_.isValid() && physicalModel_.hasStochasticInputs());
    emit dependenceValidityChanged(physicalModel_.isValid() && physicalModel_.hasStochasticInputs() && physicalModel_.getCopula().hasIndependentCopula());
    emit doeNumberValidityChanged(physicalModel_.isValid() && doeCounter_[0] > 0);
    emit doeEvaluationNumberValidityChanged(physicalModel_.isValid() && doeCounter_[1] > 0);
    emit limitStateNumberValidityChanged(physicalModel_.isValid() && physicalModel_.hasStochasticInputs() && limitStateCounter_ > 0);
  }
  else if (message == "outputNumberChanged")
  {
    if (!hasChildren()) // if modification from Python console
    {
      fill();
      return;
    }
    emit physicalModelValidityChanged(physicalModel_.isValid());
    emit twoInputsValidityChanged(physicalModel_.isValid() && physicalModel_.getInputDimension() > 1);
    emit probabilisticModelValidityChanged(physicalModel_.isValid() && physicalModel_.hasStochasticInputs());
    emit dependenceValidityChanged(physicalModel_.isValid() && physicalModel_.hasStochasticInputs() && physicalModel_.getCopula().hasIndependentCopula());
    emit doeNumberValidityChanged(physicalModel_.isValid() && doeCounter_[0] > 0);
    emit doeEvaluationNumberValidityChanged(physicalModel_.isValid() && doeCounter_[1] > 0);
    emit limitStateNumberValidityChanged(physicalModel_.isValid() && physicalModel_.hasStochasticInputs() && limitStateCounter_ > 0);
  }
  else if (message == "inputDistributionChanged")
  {
    appendProbabilisticModelItem(); // if modification from Python console
    emit probabilisticModelValidityChanged(physicalModel_.isValid() && physicalModel_.hasStochasticInputs());
    emit limitStateNumberValidityChanged(physicalModel_.isValid() && physicalModel_.hasStochasticInputs() && limitStateCounter_ > 0);
  }
  else if (message == "copulaChanged")
  {
    emit dependenceValidityChanged(physicalModel_.isValid() && physicalModel_.hasStochasticInputs() && physicalModel_.getCopula().hasIndependentCopula());
  }
  else if (message == "physicalModelRemoved")
  {
    if (hasChildren())
      qDebug() << "PhysicalModelDiagramItem::update(physicalModelRemoved) has not to contain child\n";
    emit removeRequested(row());
  }
}


void PhysicalModelDiagramItem::requestDesignOfExperimentEvaluation()
{
  // check if there is at least a design of experiments
  QModelIndexList listIndexes = model()->match(this->index(), Qt::UserRole, "DesignsOfExperimentTitle", 1, Qt::MatchRecursive);
  if (listIndexes.size() < 1)
  {
    emit showErrorMessageRequested(tr("There is no design of experiments."));
    return;
  }
  // get an analysis to launch signal
  QStandardItem * doeTitleItem = this->model()->itemFromIndex(listIndexes[0]);
  DesignOfExperimentDefinitionItem * analysisItem = dynamic_cast<DesignOfExperimentDefinitionItem*>(doeTitleItem->child(0));
  emit designOfExperimentEvaluationRequested(analysisItem->getAnalysis(), true);
}


void PhysicalModelDiagramItem::requestMetaModelCreation()
{
  // check if there is at least a design of experiments
  QModelIndexList listIndexes = model()->match(this->index(), Qt::UserRole, "DesignsOfExperimentTitle", 1, Qt::MatchRecursive);
  if (listIndexes.size() < 1)
  {
    emit showErrorMessageRequested(tr("There is no design of experiments."));
    return;
  }
  // check if there is already an Evaluation item
  QStandardItem * doeTitleItem = this->model()->itemFromIndex(listIndexes[0]);
  for (int i = 0; i < doeTitleItem->rowCount(); ++i)
  {
    if (doeTitleItem->child(i)->data(Qt::UserRole).toString() == "DesignOfExperimentDefinitionItem")
    {
      DesignOfExperimentDefinitionItem * analysisItem = dynamic_cast<DesignOfExperimentDefinitionItem*>(doeTitleItem->child(i));
      if (analysisItem->getAnalysis().hasValidResult())
      {
        // new analysis
        DesignOfExperimentEvaluation * doeEval = dynamic_cast<DesignOfExperimentEvaluation*>(analysisItem->getAnalysis().getImplementation().get());
        const String analysisName(getParentOTStudyItem()->getOTStudy().getAvailableAnalysisName(tr("metamodel_").toStdString()));
        FunctionalChaosAnalysis analysis(analysisName, *doeEval);
        // emit signal to StudyManager to open a 'general' wizard (with a list of designs of experiments)
        emit analysisRequested(this, analysis, true);
        return;
      }
    }
  }

  // emit error message
  emit showErrorMessageRequested(tr("We have not found a design of experiments with an output sample.\n"));
}


void PhysicalModelDiagramItem::requestReliabilityCreation()
{
  LimitState limitState;
  for (UnsignedInteger i = 0; i < getParentOTStudyItem()->getOTStudy().getLimitStates().getSize(); ++i)
  {
    if (getParentOTStudyItem()->getOTStudy().getLimitStates()[i].getPhysicalModel() == physicalModel_)
    {
      limitState = getParentOTStudyItem()->getOTStudy().getLimitStates()[i];
      break;
    }
  }

  // new analysis
  const String analysisName(getParentOTStudyItem()->getOTStudy().getAvailableAnalysisName(tr("reliability_").toStdString()));
  MonteCarloReliabilityAnalysis analysis(analysisName, limitState);
  // emit signal to StudyManager to open a 'general' wizard (with a list of limit states)
  emit analysisRequested(this, analysis, true);
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
  if (getParentOTStudyItem())
    getParentOTStudyItem()->getOTStudy().remove(PhysicalModel(physicalModel_));
}


void PhysicalModelDiagramItem::fill()
{
  // model definition item
  if (physicalModel_.getInputDimension() || physicalModel_.getOutputDimension())
    appendPhysicalModelItem();
  // probabilistic model item
  if (physicalModel_.hasStochasticInputs())
    appendProbabilisticModelItem();

  // update diagram (arrow color and button availability)
  emit inputNumberValidityChanged(physicalModel_.getInputDimension());
  emit twoInputsValidityChanged(physicalModel_.isValid() && physicalModel_.getInputDimension() > 1);
  emit physicalModelValidityChanged(physicalModel_.isValid());
  emit probabilisticModelValidityChanged(physicalModel_.isValid() && physicalModel_.hasStochasticInputs());
  emit dependenceValidityChanged(physicalModel_.isValid() && physicalModel_.hasStochasticInputs() && physicalModel_.getCopula().hasIndependentCopula());
  emit doeNumberValidityChanged(physicalModel_.isValid() && doeCounter_[0] > 0);
  emit doeEvaluationNumberValidityChanged(physicalModel_.isValid() && doeCounter_[1] > 0);
  emit limitStateNumberValidityChanged(physicalModel_.isValid() && physicalModel_.hasStochasticInputs() && limitStateCounter_ > 0);
}


void PhysicalModelDiagramItem::appendPhysicalModelItem()
{
  if (hasChildren())
  {
    emit changeCurrentItemRequested(child(0)->index());
    return;
  }

  // new item
  PhysicalModelDefinitionItem * pmItem = new PhysicalModelDefinitionItem(getPhysicalModel());

  // connections
  connect(pmItem, SIGNAL(probabilisticModelRequested(PhysicalModelItem*)), this, SLOT(appendProbabilisticModelItem()));
  connect(this, SIGNAL(evaluationModelRequested()), pmItem, SLOT(createModelEvaluation()));
#ifdef OTGUI_HAVE_OTMORRIS
  connect(this, SIGNAL(screeningRequested()), pmItem, SLOT(createScreening()));
#endif
  connect(this, SIGNAL(designOfExperimentRequested()), pmItem, SLOT(createDesignOfExperiment()));
  connect(this, SIGNAL(probabilisticModelItemCreated(ProbabilisticModelItem*)), pmItem, SLOT(updateProbaActionAvailability()));
  connect(this, SIGNAL(optimizationRequested()), pmItem, SLOT(createOptimization()));

  // append item
  appendRow(pmItem);

  // emit signal to the StudyManager to create a window
  emit modelDefinitionWindowRequested(pmItem);

  // disable the definition action
  defineAction_->setDisabled(true);
}


void PhysicalModelDiagramItem::appendProbabilisticModelItem()
{
  // do nothing if the item already exists
  QModelIndexList listIndexes = model()->match(this->index(), Qt::UserRole, "ProbabilisticModel", 1, Qt::MatchRecursive);
  if (listIndexes.size() == 1 && listIndexes[0].parent() == this->index())
  {
    // emit signal to the study tree to display the window
    emit changeCurrentItemRequested(listIndexes[0]);
    return;
  }

  // new item
  ProbabilisticModelItem * probaItem = new ProbabilisticModelItem(getPhysicalModel());

  // insert the item after the model definition item
  insertRow(1, probaItem);

  // connections
  connect(probaItem, SIGNAL(designOfExperimentRequested()), this, SIGNAL(designOfExperimentRequested()));
  connect(this, SIGNAL(limitStateRequested()), probaItem, SLOT(createLimitState()));
  connect(this, SIGNAL(centralTendencyRequested()), probaItem, SLOT(createCentralTendency()));
  connect(this, SIGNAL(sensitivityRequested()), probaItem, SLOT(createSensitivityAnalysis()));

  // emit signal to the StudyManager to create a window
  emit probabilisticModelItemCreated(probaItem);
}


void PhysicalModelDiagramItem::appendItem(Analysis& analysis)
{
  // if reliability analysis: limit state item takes care of adding an item for the analysis
  ReliabilityAnalysis * analysis_ptr = dynamic_cast<ReliabilityAnalysis*>(analysis.getImplementation().get());
  if (analysis_ptr)
  {
    LimitState limitState(analysis_ptr->getLimitState());
    LimitStateItem * lsItem = dynamic_cast<LimitStateItem*>(limitState.getImplementation().get()->getObserver("LimitState"));
    if (!lsItem)
    {
      qDebug() << "In PhysicalModelDiagramItem::appendAnalysisItem: No item added for the analysis named " << analysis.getName().data() << ". Limit state item not found.\n";
      return;
    }
    lsItem->appendItem(analysis);
    return;
  }

  /// append a sub-item (title) if it does not exist yet

  // parent item of the new analysis item
  OTguiItem * titleItem = 0;

  const QString analysisName = analysis.getImplementation()->getClassName().c_str();

  // If a DesignOfExperimentEvaluation

  // if DesignOfExperimentEvaluation GridDesignOfExperiment ImportedDesignOfExperiment ProbabilisticDesignOfExperiment
  if (analysisName.contains("DesignOfExperiment"))
  {
    // search DesignsOfExperiment title
    // parent item of the new doe item
    titleItem = getTitleItemNamed(tr("Designs of experiments"), "DesignsOfExperimentTitle");

    // context menu actions
    if (!titleItem->getActions().size())
    {
      QAction * newDesignOfExperiment = new QAction(QIcon(":/images/designOfExperiment.png"), tr("New design of experiments"), this);
      newDesignOfExperiment->setStatusTip(tr("Create a new design of experiments"));
      connect(newDesignOfExperiment, SIGNAL(triggered()), this, SIGNAL(designOfExperimentRequested()));
      titleItem->appendAction(newDesignOfExperiment);
    }
    Q_ASSERT(titleItem);

    // new analysis item
    DesignOfExperimentDefinitionItem * newItem = new DesignOfExperimentDefinitionItem(analysis);

    // connections
    connect(newItem, SIGNAL(numberDesignEvaluationChanged(bool)), this, SLOT(requestDesignOfExperimentRemoval(bool)));
    connect(newItem, SIGNAL(designEvaluationAppended()), this , SLOT(incrementDesignEvaluationCounter()));
    connect(newItem, SIGNAL(analysisInProgressStatusChanged(bool)), this, SLOT(setAnalysisInProgress(bool)));

    // append item
    titleItem->appendRow(newItem);

    // emit signal to StudyManager to create a window
    emit doeAnalysisItemCreated(newItem);

    // signal for diagram window : update diagram
    ++doeCounter_[0];
    if (newItem->getAnalysis().hasValidResult())
      ++doeCounter_[1];

    emit doeNumberValidityChanged(physicalModel_.isValid() && doeCounter_[0] > 0);
    emit doeEvaluationNumberValidityChanged(physicalModel_.isValid() && doeCounter_[1] > 0);

    // add Evaluation item if needed
    newItem->fill();

    return;
  }

  // If not a DesignOfExperimentEvaluation

  // Evaluation title
  if (analysisName == "ModelEvaluation")
  {
    titleItem = getTitleItemNamed(tr("Evaluation"), "ModelEvaluationTitle");

    if (!titleItem->getActions().size())
    {
      // context menu actions
      QAction * newEvaluation = new QAction(QIcon(":/images/modelEvaluation.png"), tr("New evaluation"), this);
      newEvaluation->setStatusTip(tr("Create a new model evaluation"));
      connect(newEvaluation, SIGNAL(triggered()), this, SIGNAL(evaluationModelRequested()));
      titleItem->appendAction(newEvaluation);
    }
  }
  // Central tendency title
  else if (analysisName == "MonteCarloAnalysis" ||
           analysisName == "TaylorExpansionMomentsAnalysis")
  {
    titleItem = getTitleItemNamed(tr("Central tendency"), "CentralTendencyTitle");

    if (!titleItem->getActions().size())
    {
      // context menu actions
      QAction * newCentralTendency = new QAction(QIcon(":/images/centralTendency.png"), tr("New analysis"), this);
      newCentralTendency->setStatusTip(tr("Create a new central tendency"));
      connect(newCentralTendency, SIGNAL(triggered()), this, SIGNAL(centralTendencyRequested()));
      titleItem->appendAction(newCentralTendency);
    }
  }
  // Sensitivity title
  else if (analysisName == "SobolAnalysis" ||
           analysisName == "SRCAnalysis")
  {
    titleItem = getTitleItemNamed(tr("Sensitivity"), "SensitivityTitle");

    if (!titleItem->getActions().size())
    {
      // context menu actions
      QAction * newSensitivityAnalysis = new QAction(QIcon(":/images/sensitivity.png"), tr("New analysis"), this);
      newSensitivityAnalysis->setStatusTip(tr("Create a new sensitivity analysis"));
      connect(newSensitivityAnalysis, SIGNAL(triggered()), this, SIGNAL(sensitivityRequested()));
      titleItem->appendAction(newSensitivityAnalysis);
    }
  }
  // Screening title
  else if (analysisName == "MorrisAnalysis")
  {
    titleItem = getTitleItemNamed(tr("Screening"), "ScreeningTitle");

    if (!titleItem->getActions().size())
    {
      // context menu actions
      QAction * newScreeningAnalysis = new QAction(QIcon(":/images/sensitivity.png"), tr("New analysis"), this);
      newScreeningAnalysis->setStatusTip(tr("Create a new screening analysis"));
      connect(newScreeningAnalysis, SIGNAL(triggered()), this, SIGNAL(screeningRequested()));
      titleItem->appendAction(newScreeningAnalysis);
    }
  }
  // Optimization title
  else if (analysisName == "OptimizationAnalysis")
  {
    titleItem = getTitleItemNamed(tr("Optimization"), "OptimizationTitle");

    if (!titleItem->getActions().size())
    {
      // context menu actions
      QAction * newOptimizationAnalysis = new QAction(QIcon(":/images/optimize.png"), tr("New analysis"), this);
      newOptimizationAnalysis->setStatusTip(tr("Create a new optimization analysis"));
      connect(newOptimizationAnalysis, SIGNAL(triggered()), this, SIGNAL(optimizationRequested()));
      titleItem->appendAction(newOptimizationAnalysis);
    }
  }
  ///
  if (!titleItem)
  {
    qDebug() << "In PhysicalModelDiagramItem::appendAnalysisItem: No item added for the analysis named " << analysis.getName().data() << ".\n";
    return;
  }

  // new analysis item
  AnalysisItem * newItem = new AnalysisItem(analysis);

  // connections
  connect(newItem, SIGNAL(analysisInProgressStatusChanged(bool)), this, SLOT(setAnalysisInProgress(bool)));
  if (getParentOTStudyItem())
    connect(newItem, SIGNAL(analysisInProgressStatusChanged(bool)), getParentOTStudyItem(), SLOT(setAnalysisInProgress(bool)));

  // append item
  titleItem->appendRow(newItem);

  // emit signal to StudyManager to create a window
  emit analysisItemCreated(newItem);
}


void PhysicalModelDiagramItem::appendItem(const LimitState& limitState)
{
  // parent item of the new limit state item
  OTguiItem * titleItem = getTitleItemNamed(tr("Reliability"), "ReliabilityTitle");

  if (!titleItem->getActions().size())
  {
    // context menu actions
    QAction * newLimitState = new QAction(QIcon(":/images/limitstate.png"), tr("New limit state"), this);
    newLimitState->setStatusTip(tr("Create a new limit state"));
    connect(newLimitState, SIGNAL(triggered()), this, SIGNAL(limitStateRequested()));
    titleItem->appendAction(newLimitState);
  }

  // new limit state item
  LimitStateItem * newItem = new LimitStateItem(limitState);

  // connections
  connect(newItem, SIGNAL(removeRequested(int)), this, SLOT(requestLimitStateRemoval()));
  connect(newItem, SIGNAL(analysisInProgressStatusChanged(bool)), this, SLOT(setAnalysisInProgress(bool)));

  // append item
  titleItem->appendRow(newItem);

  // emit signal to StudyManager to create a window
  emit limitStateCreated(newItem);

  // signal for diagram window : update diagram
  ++limitStateCounter_;
  emit limitStateNumberValidityChanged(physicalModel_.isValid() && physicalModel_.hasStochasticInputs() && limitStateCounter_ > 0);
}


void PhysicalModelDiagramItem::requestLimitStateRemoval()
{
  // signal for diagram window : update diagram
  --limitStateCounter_;
  emit limitStateNumberValidityChanged(physicalModel_.isValid() && physicalModel_.hasStochasticInputs() && limitStateCounter_ > 0);
}


void PhysicalModelDiagramItem::incrementDesignEvaluationCounter()
{
  // signal for diagram window : update diagram
  ++doeCounter_[1];
  emit doeEvaluationNumberValidityChanged(physicalModel_.isValid() && doeCounter_[1] > 0);
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
