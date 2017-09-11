//                                               -*- C++ -*-
/**
 *  @brief QStandardItem, observer of a design of experiment
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
#include "otgui/DesignOfExperimentDefinitionItem.hxx"

#include "otgui/DesignOfExperimentEvaluation.hxx"
#include "otgui/FunctionalChaosAnalysis.hxx"
#include "otgui/OTStudyItem.hxx"

#include <QDebug>

using namespace OT;

namespace OTGUI {

DesignOfExperimentDefinitionItem::DesignOfExperimentDefinitionItem(const Analysis& analysis)
  : OTguiItem(QString::fromUtf8(analysis.getName().c_str()), "DesignOfExperimentDefinitionItem")
  , Observer("DesignOfExperimentDefinition")
  , analysis_(analysis)
  , newMetaModel_(0)
  , modifyAnalysis_(0)
  , removeAnalysis_(0)
{
  analysis_.addObserver(this);

  // this item must be an observer of the design of experiment:
  // it is used in OTStudyItem to know where adding a DesignOfExperimentAnalysis based
  // on this design of experiment
  const DesignOfExperimentEvaluation * doeEval = dynamic_cast<DesignOfExperimentEvaluation *>(analysis_.getImplementation().get());
  Q_ASSERT(doeEval);
  doeEval->getDesignOfExperiment().addObserver(this);

  buildActions();
}


void DesignOfExperimentDefinitionItem::buildActions()
{
  modifyAnalysis_ = new QAction(QIcon(":/images/run-build.png"), tr("Modify"), this);
  modifyAnalysis_->setStatusTip(tr("Modify the analysis"));
  connect(modifyAnalysis_, SIGNAL(triggered()), this, SLOT(modifyAnalysis()));

  appendAction(modifyAnalysis_);

  // evaluate design of experiment action
  evaluateDesignOfExperiment_ = new QAction(QIcon(":/images/system-run.png"), tr("Evaluate"), this);
  evaluateDesignOfExperiment_->setStatusTip(tr("Evaluate the design of experiment"));
  connect(evaluateDesignOfExperiment_, SIGNAL(triggered()), this, SLOT(createNewEvaluation()));

  appendAction(evaluateDesignOfExperiment_);

  // new metamodel action
  newMetaModel_ = new QAction(QIcon(":/images/metaModel.png"), tr("Metamodel"), this);
  newMetaModel_->setStatusTip(tr("Create a new metamodel"));
  connect(newMetaModel_, SIGNAL(triggered()), this, SLOT(createNewMetaModel()));

  // remove analysis action
  removeAnalysis_ = new QAction(QIcon(":/images/window-close.png"), tr("Remove"), this);
  removeAnalysis_->setStatusTip(tr("Remove the analysis"));
  connect(removeAnalysis_, SIGNAL(triggered()), this, SLOT(removeAnalysis()));

  appendAction(removeAnalysis_);
}


Analysis DesignOfExperimentDefinitionItem::getAnalysis() const
{
  return analysis_;
}


Sample DesignOfExperimentDefinitionItem::getOriginalInputSample() const
{
  const DesignOfExperimentEvaluation * doeEval = dynamic_cast<DesignOfExperimentEvaluation *>(analysis_.getImplementation().get());
  Q_ASSERT(doeEval);
  return doeEval->getOriginalInputSample();
}


void DesignOfExperimentDefinitionItem::update(Observable* source, const String& message)
{
  if (message == "analysisFinished")
  {
    // emit signal to PhysicalModelDiagramItem to update the diagram
    emit designEvaluationAppended();
  }
  else if (message == "analysisRemoved")
  {
    // emit signal to PhysicalModelDiagramItem to update the diagram
    emit numberDesignEvaluationChanged(getAnalysis().analysisLaunched());
    if (hasChildren())
      qDebug() << "DesignOfExperimentDefinitionItem::update(analysisRemoved) has not to contain child\n";

    emit removeRequested(row());
  }
}


void DesignOfExperimentDefinitionItem::appendEvaluationItem()
{
  // check
  if (!getOriginalInputSample().getSize())
  {
    emit emitErrorMessageRequested(tr("The input sample is empty."));
    return;
  }
  // check if there is already an Evaluation item
  for (int i = 0; i < rowCount(); ++i)
  {
    if (child(i)->data(Qt::UserRole).toString().contains("DesignOfExperiment"))
    {
      AnalysisItem * analysisItem = dynamic_cast<AnalysisItem*>(child(i));
      // emit signal to StudyTreeView to update the window bound to analysisItem
      emit updateEvaluationWindowRequested(analysisItem);
      return;
    }
  }

  // new analysis item
  AnalysisItem * evaluationItem = new AnalysisItem(analysis_);

  evaluationItem->setData(tr("Evaluation"), Qt::DisplayRole);
  QFont font;
  font.setWeight(font.weight() + 10);
  evaluationItem->setData(font, Qt::FontRole);
  evaluationItem->setEditable(false);
  evaluationItem->appendAction(newMetaModel_);

  // insert item
  insertRow(0, evaluationItem);

  // emit signal to StudyTreeView to create a window
  emit newAnalysisItemCreated(evaluationItem);

  // disable the evaluation action
  evaluateDesignOfExperiment_->setDisabled(true);
}


void DesignOfExperimentDefinitionItem::appendAnalysisItem(Analysis& analysis)
{
  // new item
  AnalysisItem * newItem = new AnalysisItem(analysis);

  // connections
  connect(newItem, SIGNAL(analysisInProgressStatusChanged(bool)), this, SLOT(setAnalysisInProgress(bool)));
  // - signal for the PhysicalModelDiagramItem
  connect(newItem, SIGNAL(analysisInProgressStatusChanged(bool)), this, SIGNAL(analysisInProgressStatusChanged(bool)));
  if (getParentOTStudyItem())
  {
    connect(newItem, SIGNAL(analysisInProgressStatusChanged(bool)), getParentOTStudyItem(), SLOT(setAnalysisInProgress(bool)));
  }

  // append item
  appendRow(newItem);

  // emit signal to StudyTreeView to create a window
  emit newAnalysisItemCreated(newItem);
}


void DesignOfExperimentDefinitionItem::modifyAnalysis()
{
  // emit signal to StudyTreeView to open the wizard
  emit modifyAnalysisRequested(this);
}


void DesignOfExperimentDefinitionItem::updateAnalysis(const Analysis & analysis)
{
  // remove Evaluation item
  analysis_.getImplementation().get()->notifyAndRemove("analysisRemoved", "Analysis");
  analysis_.getImplementation().get()->removeObserver("OTStudy");

  evaluateDesignOfExperiment_->setEnabled(true);
  // remove last observer
  analysis_.getImplementation().get()->removeObserver(this);

  // update analysis_
  analysis_ = analysis;
  analysis_.addObserver(this);
  analysis_.addObserver(getParentOTStudyItem()->getOTStudy().getImplementation().get());

  // update the implementation of the design of experiment stored in OTStudy
  getParentOTStudyItem()->getOTStudy().getAnalysisByName(analysis.getName()).setImplementationAsPersistentObject(analysis.getImplementation());
}


void DesignOfExperimentDefinitionItem::createNewEvaluation()
{
  // check
  if (!getOriginalInputSample().getSize())
  {
    emit emitErrorMessageRequested(tr("The input sample is empty."));
    return;
  }

  // emit signal to StudyTreeView to open a wizard
  emit DOEEvaluationRequested(analysis_);
}


void DesignOfExperimentDefinitionItem::removeAnalysis()
{
  // check if the analysis is running
  if (analysis_.isRunning())
  {
    emit emitErrorMessageRequested(tr("Can not remove a running analysis."));
    return;
  }

  // remove
  if (getParentOTStudyItem())
    getParentOTStudyItem()->getOTStudy().remove(Analysis(analysis_));
}


void DesignOfExperimentDefinitionItem::createNewMetaModel()
{
  // check
  if (!getAnalysis().analysisLaunched())
  {
    emit emitErrorMessageRequested(tr("The model must have at least one output. Evaluate the design of experiment"));
    return;
  }

  // new analysis
  FunctionalChaosAnalysis analysis(getParentOTStudyItem()->getOTStudy().getAvailableAnalysisName("metaModel_"), getAnalysis());
  // emit signal to StudyTreeView to open a wizard
  emit analysisRequested(this, analysis);
}


void DesignOfExperimentDefinitionItem::fill()
{
  // append Evaluation item
  if (getAnalysis().analysisLaunched())
    appendEvaluationItem();
}
}
