//                                               -*- C++ -*-
/**
 *  @brief QStandardItem, observer of data model diagram
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
#include "otgui/DataModelDiagramItem.hxx"

#include "otgui/OTStudyItem.hxx"

#include <QDebug>

using namespace OT;

namespace OTGUI
{

DataModelDiagramItem::DataModelDiagramItem(const DesignOfExperiment& designOfExperiment)
  : DesignOfExperimentItem(designOfExperiment, "DataModelDiagram")
  , defineAction_(0)
  , removeAction_(0)
{
  buildActions();
}


void DataModelDiagramItem::buildActions()
{
  // define data model action
  defineAction_ = new QAction(tr("Define the model"), this);
  defineAction_->setStatusTip(tr("Define the data model"));
  connect(defineAction_, SIGNAL(triggered(bool)), this, SLOT(appendDataModelItem()));

  // remove data model
  removeAction_ = new QAction(QIcon(":/images/window-close.png"), tr("Remove"), this);
  removeAction_->setStatusTip(tr("Remove the data model"));
  connect(removeAction_, SIGNAL(triggered()), this, SLOT(removeDesignOfExperiment()));

  // add actions
  appendAction(defineAction_);
  appendSeparator();
  appendAction(removeAction_);
}


void DataModelDiagramItem::update(Observable* source, const String& message)
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
  else if (message == "designOfExperimentRemoved")
  {
    if (hasChildren())
      qDebug() << "DataModelDiagramItem::update(designOfExperimentRemoved) has not to contain child\n";
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
  DataModelDefinitionItem * dmItem = new DataModelDefinitionItem(getDesignOfExperiment());

  // connections
  connect(this, SIGNAL(dataAnalysisRequested()), dmItem, SLOT(createDataAnalysis()));
  connect(this, SIGNAL(inferenceRequested()), dmItem, SLOT(createInferenceAnalysis()));
  connect(this, SIGNAL(copulaInferenceRequested()), dmItem, SLOT(createCopulaInferenceAnalysis()));
  connect(this, SIGNAL(metaModelRequested()), dmItem, SLOT(createMetaModel()));

  // append item
  appendRow(dmItem);

  // emit signal to the StudyTreeView to create a window
  emit modelDefinitionWindowRequested(dmItem);

  // disable the definition action
  defineAction_->setDisabled(true);
}


void DataModelDiagramItem::appendItem(Analysis& analysis)
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


void DataModelDiagramItem::removeDesignOfExperiment()
{
  // check
  if (analysisInProgress_)
  {
    emit showErrorMessageRequested(tr("Can not remove a design of experiments when an analysis is running."));
    return;
  }
  // remove
  if (getParentOTStudyItem())
    getParentOTStudyItem()->getOTStudy().remove(DesignOfExperiment(designOfExperiment_));
}
}
