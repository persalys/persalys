//                                               -*- C++ -*-
/**
 *  @brief QStandardItem, observer of a design of experiments
 *
 *  Copyright 2015-2019 EDF-Phimeca
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
#include "persalys/DesignOfExperimentDefinitionItem.hxx"

#include "persalys/DesignOfExperimentEvaluation.hxx"
#include "persalys/FunctionalChaosAnalysis.hxx"
#include "persalys/StudyItem.hxx"

#include <QDebug>

using namespace OT;

namespace PERSALYS
{

DesignOfExperimentDefinitionItem::DesignOfExperimentDefinitionItem(const Analysis& analysis)
  : AnalysisItem(analysis, "DesignOfExperimentDefinitionItem")
{
  // this item must be an observer of the design of experiments:
  // it is used in StudyItem to know where adding a DesignOfExperimentAnalysis based
  // on this design of experiments
  const DesignOfExperimentEvaluation * doeEval = dynamic_cast<DesignOfExperimentEvaluation *>(analysis_.getImplementation().get());
  Q_ASSERT(doeEval);

  doeEval->getResult().getDesignOfExperiment().addObserver(this);

  buildActions();
}


void DesignOfExperimentDefinitionItem::buildActions()
{
  modifyAction_ = new QAction(QIcon(":/images/run-build.png"), tr("Modify"), this);
  modifyAction_->setStatusTip(tr("Modify the analysis"));
  connect(modifyAction_, SIGNAL(triggered()), this, SLOT(modifyAnalysis()));

  appendAction(modifyAction_);

  convertAction_ = new QAction(tr("Convert into data model"), this);
  convertAction_->setStatusTip(tr("Add a data model in the study tree"));
  connect(convertAction_, SIGNAL(triggered()), this, SLOT(appendDataModelItem()));
  appendAction(convertAction_);

  appendSeparator(tr("Analysis"));

  // evaluate design of experiments action
  evaluateAction_ = new QAction(QIcon(":/images/system-run.png"), tr("Evaluate"), this);
  evaluateAction_->setStatusTip(tr("Evaluate the design of experiments"));
  connect(evaluateAction_, SIGNAL(triggered()), this, SLOT(createEvaluation()));

  appendAction(evaluateAction_);

  appendSeparator();

  // remove analysis action
  removeAction_ = new QAction(QIcon(":/images/window-close.png"), tr("Remove"), this);
  removeAction_->setStatusTip(tr("Remove the analysis"));
  connect(removeAction_, SIGNAL(triggered()), this, SLOT(removeAnalysis()));

  appendAction(removeAction_);
}


QVariant DesignOfExperimentDefinitionItem::data(int role) const
{
  return Item::data(role);
}


Sample DesignOfExperimentDefinitionItem::getOriginalInputSample() const
{
  const DesignOfExperimentEvaluation * doeEval = dynamic_cast<DesignOfExperimentEvaluation *>(analysis_.getImplementation().get());
  Q_ASSERT(doeEval);
  return doeEval->getOriginalInputSample();
}


void DesignOfExperimentDefinitionItem::update(Observable* source, const String& message)
{
  if (message == "EvaluationItemRequested")
  {
    appendEvaluationItem();
  }
  else if (message == "analysisLaunched")
  {
    analysisInProgress_ = true;
  }
  else if (message == "analysisFinished")
  {
    // emit signal to PhysicalModelDiagramItem to update the diagram
    if (dynamic_cast<DesignOfExperimentEvaluation *>(source))
      emit designEvaluationUpdated(true);
    analysisInProgress_ = false;
  }
  else if (message == "analysisBadlyFinished")
  {
    analysisInProgress_ = false;
  }
  else if (message == "objectRemoved")
  {
    // emit signal to PhysicalModelDiagramItem to update the diagram
    emit numberDesignEvaluationChanged(getAnalysis().hasValidResult());
    if (hasChildren())
      qDebug() << "DesignOfExperimentDefinitionItem::update(objectRemoved) has not to contain child\n";

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
    QString dataType(child(i)->data(Qt::UserRole).toString());
    if (dataType.contains("DesignOfExperiment") && dataType != "DesignOfExperimentDefinitionItem")
    {
      AnalysisItem * analysisItem = dynamic_cast<AnalysisItem*>(child(i));
      // emit signal to StudyTreeView to update the window bound to analysisItem
      emit windowRequested(analysisItem, false);
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
  evaluationItem->appendAction(createAction("Metamodel", getAnalysis()));

  // insert item
  insertRow(0, evaluationItem);

  // emit signal to StudyTreeView to create a window
  emit windowRequested(evaluationItem);
}


void DesignOfExperimentDefinitionItem::appendItem(const Analysis& analysis)
{
  appendAnalysisItem(analysis);
  analysis.getImplementation().get()->addObserver(this);
  analysis.getImplementation().get()->addObserver(analysis_.getImplementation().get()->getObserver("PhysicalModelDiagramItem"));
  analysis.getImplementation().get()->addObserver(getParentStudyItem());
}


void DesignOfExperimentDefinitionItem::updateAnalysis(const Analysis & analysis)
{
  DesignOfExperimentEvaluation * oldDoeEval = dynamic_cast<DesignOfExperimentEvaluation*>(analysis_.getImplementation().get());
  const bool wasValid = oldDoeEval->hasValidResult();

  DesignOfExperimentEvaluation * newDoeEval = dynamic_cast<DesignOfExperimentEvaluation*>(analysis.getImplementation().get());
  newDoeEval->setDesignOfExperiment(oldDoeEval->getResult().getDesignOfExperiment());

  // remove the item named Evaluation which is an AnalysisItem
  analysis_.getImplementation().get()->notifyAndRemove("AnalysisItem");
  std::vector<Observer*> copyObservers(analysis_.getImplementation()->getObservers());
  // update analysis_
  analysis_ = analysis;
  // set the observers
  for (auto obs : copyObservers)
    analysis_.addObserver(obs);

  // update the implementation of the analysis stored in Study
  getParentStudyItem()->getStudy().getAnalysisByName(analysis.getName()).setImplementationAsPersistentObject(analysis.getImplementation());

  if (wasValid)
    emit designEvaluationUpdated(false);
  emit windowRequested(this, false);
}


void DesignOfExperimentDefinitionItem::removeAnalysis()
{
  // check if the analysis is running
  if (analysis_.isRunning())
  {
    emit showErrorMessageRequested(tr("Can not remove a running analysis."));
    return;
  }
  // check if a metamodel is running
  if (analysisInProgress_)
  {
    emit showErrorMessageRequested(tr("Can not remove a design of experiment when an analysis is running."));
    return;
  }
  // remove
  if (getParentStudyItem())
    getParentStudyItem()->getStudy().remove(Analysis(analysis_));
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
  emit doeEvaluationWizardRequested(analysis_);
}


void DesignOfExperimentDefinitionItem::fill()
{
  // append Evaluation item
  if (getAnalysis().hasValidResult() || !getAnalysis().getErrorMessage().empty())
    appendEvaluationItem();
}
}
