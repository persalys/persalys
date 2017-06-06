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

using namespace OT;

namespace OTGUI {

DesignOfExperimentDefinitionItem::DesignOfExperimentDefinitionItem(const DesignOfExperiment & designOfExperiment)
  : DesignOfExperimentItem(designOfExperiment, "DesignOfExperimentDefinition")
{
  buildActions();
}


void DesignOfExperimentDefinitionItem::buildActions()
{
  // run design of experiment action
  modifyDesignOfExperiment_ = new QAction(QIcon(":/images/run-build.png"), tr("Modify"), this);
  modifyDesignOfExperiment_->setStatusTip(tr("Modify the design of experiment"));
  connect(modifyDesignOfExperiment_, SIGNAL(triggered()), this, SLOT(modifyDesignOfExperiment()));

  evaluateDesignOfExperiment_ = new QAction(QIcon(":/images/system-run.png"), tr("Evaluate"), this);
  evaluateDesignOfExperiment_->setStatusTip(tr("Evaluate the design of experiment"));
  connect(evaluateDesignOfExperiment_, SIGNAL(triggered()), this, SLOT(appendEvaluationItem()));
  if (designOfExperiment_.getOutputSample().getSize())
    evaluateDesignOfExperiment_->setEnabled(false);

  newMetaModel_ = new QAction(QIcon(":/images/metaModel.png"), tr("New metamodel"), this);
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


void DesignOfExperimentDefinitionItem::setData(const QVariant & value, int role)
{
  if (role == Qt::EditRole)
    designOfExperiment_.getImplementation()->setName(value.toString().toLocal8Bit().data());

  QStandardItem::setData(value, role);
}


void DesignOfExperimentDefinitionItem::update(Observable* source, const String & message)
{
  if (message == "analysisFinished")
  {
    evaluateDesignOfExperiment_->setEnabled(false);
    emit designEvaluationAppended();
    emit analysisFinished();
  }
  if (message == "analysisBadlyFinished")
  {
    evaluateDesignOfExperiment_->setEnabled(false);
    emit analysisBadlyFinished(designOfExperiment_.getErrorMessage().c_str());
  }
  else if (message == "designOfExperimentRemoved")
  {
    emit numberDesignEvaluationChanged(designOfExperiment_.getOutputSample().getSize() > 0);
    emit removeRequested(row());
  }
}


void DesignOfExperimentDefinitionItem::modifyDesignOfExperiment()
{
  emit modifyDesignOfExperimentRequested(this);
}


void DesignOfExperimentDefinitionItem::appendEvaluationItem()
{
  if (!designOfExperiment_.getInputSample().getSize())
  {
    emit emitErrorMessageRequested(tr("The input sample is empty."));
    return;
  }
  DesignOfExperimentEvaluation * evaluation = new DesignOfExperimentEvaluation(DesignOfExperiment(designOfExperiment_));
  // new analysis item
  AnalysisItem * evaluationItem = new AnalysisItem(evaluation);
  evaluationItem->setData(tr("Evaluation"), Qt::DisplayRole);
  QFont font;
  font.setWeight(font.weight()+10);
  evaluationItem->setData(font, Qt::FontRole);
  evaluationItem->setEditable(false);
  evaluationItem->insertAction(1, newMetaModel_);

  getDesignOfExperiment().addObserver(evaluationItem);

  appendRow(evaluationItem);
  emit designOfExperimentEvaluationRequested(evaluationItem);
}


void DesignOfExperimentDefinitionItem::fill()
{
  if (designOfExperiment_.getOutputSample().getSize())
    appendEvaluationItem();
}
}