//                                               -*- C++ -*-
/**
 *  @brief QStandardItem, observer of data model diagram
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
#include "otgui/DataModelDiagramItem.hxx"

#include "otgui/OTStudyItem.hxx"

#include <QDebug>

using namespace OT;

namespace OTGUI
{

DataModelDiagramItem::DataModelDiagramItem(const DesignOfExperiment& designOfExperiment)
  : DesignOfExperimentItem(designOfExperiment, "DataModelDiagram")
  , defineDataModel_(0)
  , removeDataModel_(0)
{
  buildActions();
}


void DataModelDiagramItem::buildActions()
{
  // define data model action
  defineDataModel_ = new QAction(tr("Define the model"), this);
  defineDataModel_->setStatusTip(tr("Define the data model"));
  connect(defineDataModel_, SIGNAL(triggered(bool)), this, SLOT(appendDataModelItem()));

  // remove data model
  removeDataModel_ = new QAction(QIcon(":/images/window-close.png"), tr("Remove"), this);
  removeDataModel_->setStatusTip(tr("Remove the data model"));
  connect(removeDataModel_, SIGNAL(triggered()), this, SLOT(removeDesignOfExperiment()));

  // add actions
  appendAction(defineDataModel_);
  appendSeparator();
  appendAction(removeDataModel_);
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
    emit dataModelValidityChanged(designOfExperiment_.getSample().getSize() > 0);
    emit dependenciesValidityChanged(designOfExperiment_.getInputSample().getDimension() > 1);
    emit metaModelValidityChanged(designOfExperiment_.getInputSample().getSize() && designOfExperiment_.getOutputSample().getSize());
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
  emit dataModelValidityChanged(designOfExperiment_.getSample().getSize() > 0);
  emit dependenciesValidityChanged(designOfExperiment_.getInputSample().getDimension() > 1);
  emit metaModelValidityChanged(designOfExperiment_.getInputSample().getSize() && designOfExperiment_.getOutputSample().getSize());
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
  connect(this, SIGNAL(dataAnalysisRequested()), dmItem, SLOT(createNewDataAnalysis()));
  connect(this, SIGNAL(inferenceRequested()), dmItem, SLOT(createNewInferenceAnalysis()));
  connect(this, SIGNAL(copulaInferenceRequested()), dmItem, SLOT(createNewCopulaInferenceAnalysis()));
  connect(this, SIGNAL(metaModelRequested()), dmItem, SLOT(createNewMetaModel()));

  // append item
  appendRow(dmItem);

  // emit signal to the StudyTreeView to create a window
  emit modelDefinitionWindowRequested(dmItem);

  // disable the definition action
  defineDataModel_->setDisabled(true);
}


void DataModelDiagramItem::appendAnalysisItem(Analysis& analysis)
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


void DataModelDiagramItem::removeDesignOfExperiment()
{
  // check
  if (analysisInProgress_)
  {
    emit emitErrorMessageRequested(tr("Can not remove a design of experiments when an analysis is running."));
    return;
  }
  // remove
  if (getParentOTStudyItem())
    getParentOTStudyItem()->getOTStudy().remove(DesignOfExperiment(designOfExperiment_));
}
}
