//                                               -*- C++ -*-
/**
 *  @brief QStandardItem, observer of a design of experiments
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
#include "otgui/DesignOfExperimentDefinitionItem.hxx"

#include "otgui/DesignOfExperimentEvaluation.hxx"
#include "otgui/FunctionalChaosAnalysis.hxx"
#include "otgui/StudyItem.hxx"

#include <QDebug>

using namespace OT;

namespace OTGUI
{

DesignOfExperimentDefinitionItem::DesignOfExperimentDefinitionItem(const Analysis& analysis)
  : Item(QString::fromUtf8(analysis.getName().c_str()), "DesignOfExperimentDefinitionItem")
  , Observer("DesignOfExperimentDefinition")
  , analysis_(analysis)
  , newMetaModelAction_(0)
  , modifyAction_(0)
  , removeAction_(0)
{
  analysis_.addObserver(this);

  // this item must be an observer of the design of experiments:
  // it is used in StudyItem to know where adding a DesignOfExperimentAnalysis based
  // on this design of experiments
  const DesignOfExperimentEvaluation * doeEval = dynamic_cast<DesignOfExperimentEvaluation *>(analysis_.getImplementation().get());
  Q_ASSERT(doeEval);
  doeEval->getDesignOfExperiment().addObserver(this);

  buildActions();
}


void DesignOfExperimentDefinitionItem::setData(const QVariant & value, int role)
{
  // rename
  if (role == Qt::EditRole)
    analysis_.getImplementation()->setName(value.toString().toUtf8().data());

  QStandardItem::setData(value, role);
}


void DesignOfExperimentDefinitionItem::buildActions()
{
  modifyAction_ = new QAction(QIcon(":/images/run-build.png"), tr("Modify"), this);
  modifyAction_->setStatusTip(tr("Modify the analysis"));
  connect(modifyAction_, SIGNAL(triggered()), this, SLOT(modifyAnalysis()));

  appendAction(modifyAction_);

  exportAction_ = new QAction(QIcon(":/images/document-export.png"), tr("Export data"), this);
  exportAction_->setStatusTip(tr("Export the data in a file"));
  connect(exportAction_, SIGNAL(triggered()), this, SIGNAL(dataExportRequested()));
  appendAction(exportAction_);

  appendSeparator(tr("Analysis"));

  // evaluate design of experiments action
  evaluateAction_ = new QAction(QIcon(":/images/system-run.png"), tr("Evaluate"), this);
  evaluateAction_->setStatusTip(tr("Evaluate the design of experiments"));
  connect(evaluateAction_, SIGNAL(triggered()), this, SLOT(createEvaluation()));

  appendAction(evaluateAction_);

  // new metamodel action
  newMetaModelAction_ = new QAction(QIcon(":/images/metaModel.png"), tr("Metamodel"), this);
  newMetaModelAction_->setStatusTip(tr("Create a new metamodel"));
  connect(newMetaModelAction_, SIGNAL(triggered()), this, SLOT(createMetaModel()));

  appendSeparator();

  // remove analysis action
  removeAction_ = new QAction(QIcon(":/images/window-close.png"), tr("Remove"), this);
  removeAction_->setStatusTip(tr("Remove the analysis"));
  connect(removeAction_, SIGNAL(triggered()), this, SLOT(removeAnalysis()));

  appendAction(removeAction_);
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
    emit numberDesignEvaluationChanged(getAnalysis().hasValidResult());
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
    emit showErrorMessageRequested(tr("The input sample is empty."));
    return;
  }
  // check if there is already an Evaluation item
  for (int i = 0; i < rowCount(); ++i)
  {
    if (child(i)->data(Qt::UserRole).toString().contains("DesignOfExperiment"))
    {
      AnalysisItem * analysisItem = dynamic_cast<AnalysisItem*>(child(i));
      // emit signal to StudyTreeView to update the window bound to analysisItem
      emit updateEvaluationWindowRequested(analysisItem, false);
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
  evaluationItem->appendSeparator(tr("Analysis"));
  evaluationItem->appendAction(newMetaModelAction_);

  // connections
  // - signal for the PhysicalModelDiagramItem
  connect(evaluationItem, SIGNAL(analysisInProgressStatusChanged(bool)), this, SIGNAL(analysisInProgressStatusChanged(bool)));
  if (getParentOTStudyItem())
  {
    connect(evaluationItem, SIGNAL(analysisInProgressStatusChanged(bool)), getParentOTStudyItem(), SLOT(setAnalysisInProgress(bool)));
  }

  // insert item
  insertRow(0, evaluationItem);

  // emit signal to StudyTreeView to create a window
  emit analysisItemCreated(evaluationItem);

  // disable the evaluation action
  evaluateAction_->setDisabled(true);
}


void DesignOfExperimentDefinitionItem::appendItem(Analysis& analysis)
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
  emit analysisItemCreated(newItem);
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
  analysis_.getImplementation().get()->removeObserver("Study");

  evaluateAction_->setEnabled(true);
  // remove last observer
  analysis_.getImplementation().get()->removeObserver(this);

  // update analysis_
  analysis_ = analysis;
  analysis_.addObserver(this);
  analysis_.addObserver(getParentOTStudyItem()->getOTStudy().getImplementation().get());

  // update the implementation of the design of experiments stored in Study
  getParentOTStudyItem()->getOTStudy().getAnalysisByName(analysis.getName()).setImplementationAsPersistentObject(analysis.getImplementation());
}


void DesignOfExperimentDefinitionItem::createEvaluation()
{
  // check
  if (!getOriginalInputSample().getSize())
  {
    emit showErrorMessageRequested(tr("The input sample is empty."));
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
    emit showErrorMessageRequested(tr("Can not remove a running analysis."));
    return;
  }
  // check
  if (analysisInProgress_)
  {
    emit showErrorMessageRequested(tr("Can not remove a design of experiments when an analysis is running."));
    return;
  }

  // remove
  if (getParentOTStudyItem())
    getParentOTStudyItem()->getOTStudy().remove(Analysis(analysis_));
}


void DesignOfExperimentDefinitionItem::createMetaModel()
{
  // check
  if (!getAnalysis().hasValidResult())
  {
    emit showErrorMessageRequested(tr("The model must have at least one output. Evaluate the design of experiments"));
    return;
  }
  const DesignOfExperimentEvaluation * doeEval = dynamic_cast<DesignOfExperimentEvaluation *>(analysis_.getImplementation().get());
  Q_ASSERT(doeEval);
  if (doeEval->getDesignOfExperiment().getSample().getSize() < 2)
  {
    emit showErrorMessageRequested(tr("The design of experiments must contain at least two points."));
    return;
  }

  // new analysis
  const String analysisName(getParentOTStudyItem()->getOTStudy().getAvailableAnalysisName(tr("metaModel_").toStdString()));
  FunctionalChaosAnalysis analysis(analysisName, getAnalysis());
  // emit signal to StudyTreeView to open a wizard
  emit analysisRequested(this, analysis);
}


void DesignOfExperimentDefinitionItem::fill()
{
  // append Evaluation item
  if (getAnalysis().hasValidResult())
    appendEvaluationItem();
}
}
