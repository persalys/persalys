//                                               -*- C++ -*-
/**
 *  @brief QStandardItem, observer of designs of experiments
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
#include "otgui/OTStudyItem.hxx"

#include <QDebug>

using namespace OT;

namespace OTGUI {

DesignOfExperimentDefinitionItem::DesignOfExperimentDefinitionItem(const DesignOfExperiment & designOfExperiment)
  : DesignOfExperimentItem(designOfExperiment, "DesignOfExperimentDefinition")
  , modifyDesignOfExperiment_(0)
  , evaluateDesignOfExperiment_(0)
  , newMetaModel_(0)
  , removeDesignOfExperiment_(0)
{
  buildActions();
}


void DesignOfExperimentDefinitionItem::buildActions()
{
  // run design of experiment action
  modifyDesignOfExperiment_ = new QAction(QIcon(":/images/run-build.png"), tr("Modify"), this);
  modifyDesignOfExperiment_->setStatusTip(tr("Modify the design of experiment"));
  connect(modifyDesignOfExperiment_, SIGNAL(triggered()), this, SLOT(modifyDesignOfExperiment()));

  // evaluate design of experiment action
  evaluateDesignOfExperiment_ = new QAction(QIcon(":/images/system-run.png"), tr("Evaluate"), this);
  evaluateDesignOfExperiment_->setStatusTip(tr("Evaluate the design of experiment"));
  connect(evaluateDesignOfExperiment_, SIGNAL(triggered()), this, SLOT(appendEvaluationItem()));

  // new metamodel action
  newMetaModel_ = new QAction(QIcon(":/images/metaModel.png"), tr("Metamodel"), this);
  newMetaModel_->setStatusTip(tr("Create a new metamodel"));
  connect(newMetaModel_, SIGNAL(triggered()), this, SLOT(createNewMetaModel()));

  // remove design of experiment action
  removeDesignOfExperiment_ = new QAction(QIcon(":/images/window-close.png"), tr("Remove"), this);
  removeDesignOfExperiment_->setStatusTip(tr("Remove the design of experiment"));
  connect(removeDesignOfExperiment_, SIGNAL(triggered()), this, SLOT(removeDesignOfExperiment()));

  // add actions
  appendAction(modifyDesignOfExperiment_);
  appendAction(evaluateDesignOfExperiment_);
  appendAction(removeDesignOfExperiment_);
}


void DesignOfExperimentDefinitionItem::updateDesignOfExperiment(const DesignOfExperiment & doe)
{
  // remove Evaluation item
  designOfExperiment_.getImplementation().get()->notifyAndRemove("analysisRemoved", "Analysis");
  designOfExperiment_.getImplementation().get()->removeObserver("OTStudy");

  evaluateDesignOfExperiment_->setEnabled(true);
  // remove last observer
  designOfExperiment_.getImplementation().get()->removeObserver(this);

  // update designOfExperiment_
  designOfExperiment_ = doe;
  designOfExperiment_.addObserver(this);
  designOfExperiment_.addObserver(getParentOTStudyItem()->getOTStudy().getImplementation().get());

  // update the implementation of the design of experiment stored in OTStudy
  getParentOTStudyItem()->getOTStudy().getDesignOfExperimentByName(doe.getName()).setImplementationAsPersistentObject(doe.getImplementation());

  // emit signal to all analysisItem children to update the doe of the analyses
  emit designOfExperimentChanged(designOfExperiment_);
}


void DesignOfExperimentDefinitionItem::update(Observable* source, const String & message)
{
  if (message == "analysisFinished")
  {
    // emit signal to PhysicalModelDiagramItem to update the diagram
    emit designEvaluationAppended();
  }
  else if (message == "designOfExperimentRemoved")
  {
    // emit signal to PhysicalModelDiagramItem to update the diagram
    emit numberDesignEvaluationChanged(designOfExperiment_.getOutputSample().getSize() > 0);
    if (hasChildren())
      qDebug() << "DesignOfExperimentDefinitionItem::update(designOfExperimentRemoved) has not to contain child\n";

    emit removeRequested(row());
  }
}


void DesignOfExperimentDefinitionItem::modifyDesignOfExperiment()
{
  // emit signal to StudyTreeView to open a wizard
  emit modifyDesignOfExperimentRequested(this);
}


void DesignOfExperimentDefinitionItem::appendEvaluationItem()
{
  // check
  if (!designOfExperiment_.getInputSample().getSize())
  {
    emit emitErrorMessageRequested(tr("The input sample is empty."));
    return;
  }
  // new analysis
  DesignOfExperimentEvaluation * evaluation = new DesignOfExperimentEvaluation(DesignOfExperiment(designOfExperiment_));

  // new analysis item
  AnalysisItem * evaluationItem = new AnalysisItem(evaluation);

  evaluationItem->setData(tr("Evaluation"), Qt::DisplayRole);
  QFont font;
  font.setWeight(font.weight() + 10);
  evaluationItem->setData(font, Qt::FontRole);
  evaluationItem->setEditable(false);
  evaluationItem->insertAction(1, newMetaModel_);

  // the new item is an observer of designOfExperiment_
  getDesignOfExperiment().addObserver(evaluationItem);

  // insert item
  insertRow(0, evaluationItem);

  // emit signal to StudyTreeView to create a window
  emit designOfExperimentEvaluationRequested(evaluationItem);

  // disable the evaluation action
  evaluateDesignOfExperiment_->setDisabled(true);
}


void DesignOfExperimentDefinitionItem::fill()
{
  // append Evaluation item
  if (designOfExperiment_.getOutputSample().getSize())
    appendEvaluationItem();
}
}