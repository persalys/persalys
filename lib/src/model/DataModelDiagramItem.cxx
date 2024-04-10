//                                               -*- C++ -*-
/**
 *  @brief QStandardItem, observer of data model diagram
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
#include "persalys/DataModelDiagramItem.hxx"

#include "persalys/StudyItem.hxx"

#include <QDebug>

using namespace OT;

namespace PERSALYS
{

DataModelDiagramItem::DataModelDiagramItem(const DesignOfExperiment& designOfExperiment)
  : DesignOfExperimentItem(designOfExperiment, "DataModelDiagramItem")
{
  buildActions();
}


void DataModelDiagramItem::buildActions()
{
  // define data model action
  defineAction_ = new QAction(tr("Define the model"), this);
  defineAction_->setStatusTip(tr("Define the data model"));
  connect(defineAction_, SIGNAL(triggered(bool)), this, SLOT(appendDataModelItem()));

  // new analyses
  newDataAnalysis_ = createAction("DataAnalysis", getDesignOfExperiment());
  newInferenceAnalysis_ = createAction("Inference", getDesignOfExperiment());
  newCopulaInferenceAnalysis_ = createAction("CopulaInference", getDesignOfExperiment());
  newMetaModel_ = createAction("Metamodel", getDesignOfExperiment());

  // remove data model
  removeAction_ = new QAction(QIcon(":/images/window-close.png"), tr("Remove"), this);
  removeAction_->setStatusTip(tr("Remove the data model"));
  connect(removeAction_, SIGNAL(triggered()), this, SLOT(removeDesignOfExperiment()));

  // add actions
  appendAction(defineAction_);
  appendSeparator();
  appendAction(removeAction_);
}


void DataModelDiagramItem::update(Observable* /*source*/, const String& message)
{
  if (message == "variablesChanged")
  {
    if (!hasChildren()) // if modification from Python console
    {
      fill();
      return;
    }
    // emit signals to DataModelDiagramWindow
    // to update the diagram (arrow color and button availability)
    const bool validDOE = designOfExperiment_.isValid();
    emit dataModelValidityChanged(designOfExperiment_.getSample().getSize() > 0 && validDOE);
    emit dependenciesValidityChanged(designOfExperiment_.getSample().getDimension() > 1 && validDOE);
    emit metaModelValidityChanged(designOfExperiment_.getInputSample().getSize() && designOfExperiment_.getOutputSample().getSize() && validDOE);
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
      qDebug() << "DataModelDiagramItem::update(objectRemoved) has not to contain child\n";
    emit removeRequested(row());
  }
}


void DataModelDiagramItem::fill()
{
  // model definition item
  if (designOfExperiment_.getSample().getSize())
    appendDataModelItem();

  // update diagram (arrow color and button availability)
  const bool validDOE = designOfExperiment_.isValid();
  emit dataModelValidityChanged(designOfExperiment_.getSample().getSize() > 0 && validDOE);
  emit dependenciesValidityChanged(designOfExperiment_.getSample().getDimension() > 1 && validDOE);
  emit metaModelValidityChanged(designOfExperiment_.getInputSample().getSize() && designOfExperiment_.getOutputSample().getSize() && validDOE);
}


void DataModelDiagramItem::appendDataModelItem()
{
  if (hasChildren())
  {
    emit changeCurrentItemRequested(child(0)->index());
    return;
  }

  // new item
  DataModelDefinitionItem * dmItem = new DataModelDefinitionItem(getDesignOfExperiment(), this);
  appendRow(dmItem);

  // emit signal to the StudyTreeView to create a window
  emit windowRequested(dmItem);

  // disable the definition action
  defineAction_->setDisabled(true);
}


void DataModelDiagramItem::appendItem(const Analysis& analysis)
{
  appendAnalysisItem(analysis);
  analysis.getImplementation().get()->addObserver(this);
  analysis.getImplementation().get()->addObserver(getParentStudyItem());
}


void DataModelDiagramItem::removeDesignOfExperiment()
{
  // check
  if (analysisInProgress_)
  {
    emit showErrorMessageRequested(tr("Can not remove a design of experiments when an analysis is running."));
    return;
  }
  // remove
  if (getParentStudyItem())
    getParentStudyItem()->getStudy().remove(DesignOfExperiment(designOfExperiment_));
}
}
