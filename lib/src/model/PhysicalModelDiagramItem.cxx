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
#include "otgui/PhysicalModelDiagramItem.hxx"

#include "otgui/OTStudyItem.hxx"
#include "otgui/MonteCarloReliabilityAnalysis.hxx"
#include "otgui/FunctionalChaosAnalysis.hxx"

#include <QDebug>

using namespace OT;

namespace OTGUI {

PhysicalModelDiagramItem::PhysicalModelDiagramItem(const PhysicalModel & physicalModel)
  : PhysicalModelItem(physicalModel, "PhysicalModelDiagram")
  , definePhysicalModel_(0)
  , removePhysicalModel_(0)
  , limitStateCounter_(0)
  , doeCounter_(Indices(2))
{
  setData(QString::fromUtf8(physicalModel.getName().c_str()), Qt::DisplayRole);

  buildActions();
}


void PhysicalModelDiagramItem::buildActions()
{
  // define physical model action
  definePhysicalModel_ = new QAction(tr("Define the model"), this);
  definePhysicalModel_->setStatusTip(tr("Define the physical model"));
  connect(definePhysicalModel_, SIGNAL(triggered()), this, SLOT(appendPhysicalModelItem()));

  // remove physical model action
  removePhysicalModel_ = new QAction(QIcon(":/images/window-close.png"), tr("Remove"), this);
  removePhysicalModel_->setStatusTip(tr("Remove the physical model"));
  connect(removePhysicalModel_, SIGNAL(triggered()), this, SLOT(removePhysicalModel()));

  // add actions
  appendAction(definePhysicalModel_);
  appendAction(removePhysicalModel_);
}


void PhysicalModelDiagramItem::setData(const QVariant & value, int role)
{
  if (role == Qt::EditRole)
    physicalModel_.getImplementation()->setName(value.toString().toLocal8Bit().data());

  QStandardItem::setData(value, role);
}


void PhysicalModelDiagramItem::update(Observable* source, const String & message)
{
  if (message == "inputNumberChanged")
  {
    emit inputNumberValidityChanged(physicalModel_.getInputs().getSize());
    emit physicalModelValidityChanged(physicalModel_.isValid());
    emit probabilisticModelValidityChanged(physicalModel_.isValid() && physicalModel_.hasStochasticInputs());
    emit dependencyValidityChanged(physicalModel_.isValid() && physicalModel_.hasStochasticInputs() && physicalModel_.getComposedDistribution().hasIndependentCopula());
    emit designOfExperimentNumberValidityChanged(physicalModel_.isValid() && doeCounter_[0] > 0);
    emit designOfExperimentEvaluationNumberValidityChanged(physicalModel_.isValid() && doeCounter_[1] > 0);
    emit limitStateNumberValidityChanged(physicalModel_.isValid() && physicalModel_.hasStochasticInputs() && limitStateCounter_ > 0);
  }
  else if (message == "outputNumberChanged")
  {
    emit physicalModelValidityChanged(physicalModel_.isValid());
    emit probabilisticModelValidityChanged(physicalModel_.isValid() && physicalModel_.hasStochasticInputs());
    emit dependencyValidityChanged(physicalModel_.isValid() && physicalModel_.hasStochasticInputs() && physicalModel_.getComposedDistribution().hasIndependentCopula());
    emit designOfExperimentNumberValidityChanged(physicalModel_.isValid() && doeCounter_[0] > 0);
    emit designOfExperimentEvaluationNumberValidityChanged(physicalModel_.isValid() && doeCounter_[1] > 0);
    emit limitStateNumberValidityChanged(physicalModel_.isValid() && physicalModel_.hasStochasticInputs() && limitStateCounter_ > 0);
  }
  else if (message == "inputDistributionChanged")
  {
    emit probabilisticModelValidityChanged(physicalModel_.isValid() && physicalModel_.hasStochasticInputs());
    emit limitStateNumberValidityChanged(physicalModel_.isValid() && physicalModel_.hasStochasticInputs() && limitStateCounter_ > 0);
  }
  else if (message == "copulaChanged")
  {
    emit dependencyValidityChanged(physicalModel_.isValid() && physicalModel_.hasStochasticInputs() && physicalModel_.getComposedDistribution().hasIndependentCopula());
  }
  else if (message == "physicalModelRemoved")
  {
    emit removeRequested(row());
  }
}


void PhysicalModelDiagramItem::requestDesignOfExperimentEvaluation()
{
  if (!getParentOTStudyItem())
  {
    qDebug() << "In PhysicalModelDiagramItem::requestDesignOfExperimentEvaluation: no OTStudyItem.\n";
    return;
  }
  QModelIndexList listIndexes = model()->match(this->index(), Qt::UserRole, "DesignsOfExperimentTitle", 2, Qt::MatchRecursive);
  if (listIndexes.size() != 1)
  {
    emit emitErrorMessageRequested(tr("There is no design of experiment."));
    return;
  }
  QList<QStandardItem*> listDOEItems;
  QStandardItem * doeTitleItem = model()->itemFromIndex(listIndexes[0]);
  for (int i=0; i<doeTitleItem->rowCount(); ++i)
  {
    if (!doeTitleItem->child(i)->hasChildren())
    {
      QStandardItem * item = new QStandardItem(doeTitleItem->child(i)->text());
      DesignOfExperimentDefinitionItem * doeItem = dynamic_cast<DesignOfExperimentDefinitionItem*>(doeTitleItem->child(i));
      if (!doeItem)
      {
        qDebug() << "In PhysicalModelDiagramItem::requestDesignOfExperimentEvaluation: can not convert item into DesignOfExperimentDefinitionItem.\n";
        emit emitErrorMessageRequested(tr("Internal error. Impossible to evaluate the design of experiment"));
        return;
      }
      item->setData(qVariantFromValue(doeItem));
      listDOEItems.append(item);
				}
		}

  if (listDOEItems.isEmpty())
    emit emitErrorMessageRequested(tr("All the designs of experiment have already been evaluated."));
  else
    emit designOfExperimentEvaluationRequested(listDOEItems);
}


void PhysicalModelDiagramItem::requestMetaModelCreation()
{
  DesignOfExperiment design;
  for (UnsignedInteger i=0; i<getParentOTStudyItem()->getOTStudy().getDesignOfExperiments().getSize(); ++i)
  {
    if (getParentOTStudyItem()->getOTStudy().getDesignOfExperiments()[i].getPhysicalModel().getImplementation().get() == physicalModel_.getImplementation().get() &&
        getParentOTStudyItem()->getOTStudy().getDesignOfExperiments()[i].getOutputSample().getSize()
       )
    {
      design = getParentOTStudyItem()->getOTStudy().getDesignOfExperiments()[i];
      break;
    }
  }
  if (!design.getOutputSample().getSize())
  {
    qDebug() << "In PhysicalModelDiagramItem::requestMetaModelCreation: We have not found a design of experiment with an output sample.\n";
    return;
  }

  FunctionalChaosAnalysis analysis(getParentOTStudyItem()->getOTStudy().getAvailableAnalysisName("metamodel_"), design);
  emit analysisRequested(this, analysis, true);
}


void PhysicalModelDiagramItem::requestReliabilityCreation()
{
  LimitState limitState;
  for (UnsignedInteger i=0; i<getParentOTStudyItem()->getOTStudy().getLimitStates().getSize(); ++i)
  {
    if (getParentOTStudyItem()->getOTStudy().getLimitStates()[i].getPhysicalModel().getImplementation().get() == physicalModel_.getImplementation().get())
    {
      limitState = getParentOTStudyItem()->getOTStudy().getLimitStates()[i];
      break;
    }
  }

  MonteCarloReliabilityAnalysis analysis(getParentOTStudyItem()->getOTStudy().getAvailableAnalysisName("reliability_"), limitState);
  emit analysisRequested(this, analysis, true);
}


void PhysicalModelDiagramItem::removePhysicalModel()
{
  if (getParentOTStudyItem())
  {
    if (analysisInProgress_)
    {
      emit emitErrorMessageRequested(tr("Can not remove a physical model when an analysis is running."));
      return;
    }
    getParentOTStudyItem()->getOTStudy().remove(PhysicalModel(physicalModel_));
  }
}


void PhysicalModelDiagramItem::fill()
{
  // model definition item
  if (physicalModel_.getInputs().getSize() || physicalModel_.getOutputs().getSize())
    appendPhysicalModelItem();
  // probabilistic model item
  if (physicalModel_.hasStochasticInputs())
    appendProbabilisticModelItem();

  // update diagram (arrow color and button availability)
  emit inputNumberValidityChanged(physicalModel_.getInputs().getSize());
  emit physicalModelValidityChanged(physicalModel_.isValid());
  emit probabilisticModelValidityChanged(physicalModel_.isValid() && physicalModel_.hasStochasticInputs());
  emit dependencyValidityChanged(physicalModel_.isValid() && physicalModel_.hasStochasticInputs() && physicalModel_.getComposedDistribution().hasIndependentCopula());
  emit designOfExperimentNumberValidityChanged(physicalModel_.isValid() && doeCounter_[0] > 0);
  emit designOfExperimentEvaluationNumberValidityChanged(physicalModel_.isValid() && doeCounter_[1] > 0);
  emit limitStateNumberValidityChanged(physicalModel_.isValid() && physicalModel_.hasStochasticInputs() && limitStateCounter_ > 0);
}


void PhysicalModelDiagramItem::appendPhysicalModelItem()
{
  if (hasChildren())
    return;

  PhysicalModelDefinitionItem * pmItem = new PhysicalModelDefinitionItem(getPhysicalModel());
  connect(pmItem, SIGNAL(probabilisticModelRequested(PhysicalModelItem*)), this, SLOT(appendProbabilisticModelItem()));
  appendRow(pmItem);

  connect(this, SIGNAL(evaluationModelRequested()), pmItem, SLOT(createNewModelEvaluation()));
  connect(this, SIGNAL(designOfExperimentRequested()), pmItem, SLOT(createNewDesignOfExperiment()));
  connect(this, SIGNAL(newProbabilisticModelItemCreated(ProbabilisticModelItem*)), pmItem, SLOT(updateProbaActionAvailability()));

  emit modelDefinitionWindowRequested(pmItem);

  // disable the definition action
  definePhysicalModel_->setDisabled(true);
}


void PhysicalModelDiagramItem::appendProbabilisticModelItem()
{
  // do nothing if the item already exists
  if (model()->match(this->index(), Qt::UserRole, "ProbabilisticModel", -1, Qt::MatchRecursive).size() == 1)
    return;

  ProbabilisticModelItem * probaItem = new ProbabilisticModelItem(getPhysicalModel());
  // insert the item after the model definition item
  insertRow(1, probaItem);

  connect(this, SIGNAL(limitStateRequested()), probaItem, SLOT(createNewLimitState()));
  connect(this, SIGNAL(centralTendencyRequested()), probaItem, SLOT(createNewCentralTendency()));
  connect(this, SIGNAL(sensitivityRequested()), probaItem, SLOT(createNewSensitivityAnalysis()));

  emit newProbabilisticModelItemCreated(probaItem);
}


void PhysicalModelDiagramItem::appendAnalysisItem(Analysis& analysis)
{
  // if reliability analysis: limit state item takes care of adding an item for the analysis
  if (dynamic_cast<ReliabilityAnalysis*>(analysis.getImplementation().get()))
  {
    LimitState limitState(dynamic_cast<ReliabilityAnalysis*>(analysis.getImplementation().get())->getLimitState());
    LimitStateItem * lsItem = dynamic_cast<LimitStateItem*>(limitState.getImplementation().get()->getObserver("LimitState"));
    if (!lsItem)
    {
      qDebug() << "In PhysicalModelDiagramItem::appendAnalysisItem: No item added for the analysis named " << analysis.getName().data() << ". Limit state item not found.\n";
      return;
    }
    lsItem->appendAnalysisItem(analysis);
    return;
  }

  // parent item of the new analysis item
  OTguiItem * analysisTypeItem = 0;

  const String analysisName = analysis.getImplementation()->getClassName();
  // Evaluation
  if (analysisName == "ModelEvaluation")
  {
    analysisTypeItem = getTitleItemNamed("ModelEvaluationTitle", tr("Evaluation"));

    if (!analysisTypeItem->getActions().size())
    {
      // context menu actions
      QAction * newEvaluation = new QAction(QIcon(":/images/modelEvaluation.png"), tr("New model evaluation"), this);
      newEvaluation->setStatusTip(tr("Create a new model evaluation"));
      connect(newEvaluation, SIGNAL(triggered()), this, SIGNAL(evaluationModelRequested()));
      analysisTypeItem->appendAction(newEvaluation);
    }
  }
  // Central tendency
  else if (analysisName == "MonteCarloAnalysis" || analysisName == "TaylorExpansionMomentsAnalysis")
  {
    analysisTypeItem = getTitleItemNamed("CentralTendencyTitle", tr("Central tendency"));

    if (!analysisTypeItem->getActions().size())
    {
      // context menu actions
      QAction * newCentralTendency = new QAction(QIcon(":/images/centralTendency.png"), tr("New central tendency"), this);
      newCentralTendency->setStatusTip(tr("Create a new central tendency"));
      connect(newCentralTendency, SIGNAL(triggered()), this, SIGNAL(centralTendencyRequested()));
      analysisTypeItem->appendAction(newCentralTendency);
    }
  }
  // Sensitivity
  else if (analysisName == "SobolAnalysis" || analysisName == "SRCAnalysis")
  {
    analysisTypeItem = getTitleItemNamed("SensitivityTitle", tr("Sensitivity"));

    if (!analysisTypeItem->getActions().size())
    {
      // context menu actions
      QAction * newSensitivityAnalysis = new QAction(QIcon(":/images/sensitivity.png"), tr("New sensitivity analysis"), this);
      newSensitivityAnalysis->setStatusTip(tr("Create a new sensitivity analysis"));
      connect(newSensitivityAnalysis, SIGNAL(triggered()), this, SIGNAL(sensitivityRequested()));
      analysisTypeItem->appendAction(newSensitivityAnalysis);
    }
  }

  if (!analysisTypeItem)
  {
    qDebug() << "In PhysicalModelDiagramItem::appendAnalysisItem: No item added for the analysis named " << analysis.getName().data() << ".\n";
    return;
  }

  // new analysis item
  AnalysisItem * newItem = new AnalysisItem(analysis);
  connect(newItem, SIGNAL(analysisStatusChanged(bool)), this, SLOT(setAnalysisInProgress(bool)));
  if (getParentOTStudyItem())
    connect(newItem, SIGNAL(analysisStatusChanged(bool)), getParentOTStudyItem(), SLOT(setAnalysisInProgress(bool)));
  analysisTypeItem->appendRow(newItem);

  emit newAnalysisItemCreated(newItem);
}


void PhysicalModelDiagramItem::appendLimitStateItem(const LimitState& limitState)
{
  // parent item of the new limit state item
  OTguiItem * analysisTypeItem = getTitleItemNamed("ReliabilityTitle", tr("Reliability"));

  if (!analysisTypeItem->getActions().size())
  {
    // context menu actions
    QAction * newLimitState = new QAction(QIcon(":/images/limitstate.png"), tr("New limit state"), this);
    newLimitState->setStatusTip(tr("Create a new limit state"));
    connect(newLimitState, SIGNAL(triggered()), this, SIGNAL(limitStateRequested()));
    analysisTypeItem->appendAction(newLimitState);
  }

  // limit state item
  LimitStateItem * newItem = new LimitStateItem(limitState);
  connect(newItem, SIGNAL(removeRequested(int)), this, SLOT(requestLimitStateRemoval()));
  connect(newItem, SIGNAL(analysisStatusChanged(bool)), this, SLOT(setAnalysisInProgress(bool)));
  analysisTypeItem->appendRow(newItem);

  // signal for StudyTreeView : create a window
  emit newLimitStateCreated(newItem);

  // signal for diagram window : update diagram
  ++limitStateCounter_;
  emit limitStateNumberValidityChanged(physicalModel_.isValid() && physicalModel_.hasStochasticInputs() && limitStateCounter_ > 0);
}


void PhysicalModelDiagramItem::appendDesignOfExperimentItem(const DesignOfExperiment& designOfExperiment)
{
  // parent item of the new doe item
  OTguiItem * typeItem = getTitleItemNamed("DesignsOfExperimentTitle", tr("Designs of experiment"));

  // context menu actions
  if (!typeItem->getActions().size())
  {
    QAction * newDesignOfExperiment = new QAction(QIcon(":/images/designOfExperiment.png"), tr("New design of experiment"), this);
    newDesignOfExperiment->setStatusTip(tr("Create a new design of experiment"));
    connect(newDesignOfExperiment, SIGNAL(triggered()), this, SIGNAL(designOfExperimentRequested()));
    typeItem->appendAction(newDesignOfExperiment);
  }

  // doe item
  DesignOfExperimentDefinitionItem * newItem = new DesignOfExperimentDefinitionItem(designOfExperiment);
  connect(newItem, SIGNAL(numberDesignEvaluationChanged(bool)),this, SLOT(requestDesignOfExperimentRemoval(bool)));
  connect(newItem, SIGNAL(designEvaluationAppended()), this , SLOT(incrementDesignEvaluationCounter()));
  connect(newItem, SIGNAL(analysisStatusChanged(bool)), this, SLOT(setAnalysisInProgress(bool)));
  typeItem->appendRow(newItem);

  // signal for StudyTreeView : create a window
  emit newDesignOfExperimentCreated(newItem);

  // signal for diagram window : update diagram
  ++doeCounter_[0];
  if (designOfExperiment.getOutputSample().getSize())
    ++doeCounter_[1];

  emit designOfExperimentNumberValidityChanged(physicalModel_.isValid() && doeCounter_[0] > 0);
  emit designOfExperimentEvaluationNumberValidityChanged(physicalModel_.isValid() && doeCounter_[1] > 0);

  // Add sub items
  newItem->fill();
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
  emit designOfExperimentEvaluationNumberValidityChanged(physicalModel_.isValid() && doeCounter_[1] > 0);
}


void PhysicalModelDiagramItem::requestDesignOfExperimentRemoval(bool isValid)
{
  // signal for diagram window : update diagram
  --doeCounter_[0];
  if (isValid)
    --doeCounter_[1];

  emit designOfExperimentNumberValidityChanged(physicalModel_.isValid() && doeCounter_[0] > 0);
  emit designOfExperimentEvaluationNumberValidityChanged(physicalModel_.isValid() && doeCounter_[1] > 0);
}
}