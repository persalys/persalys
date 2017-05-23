//                                               -*- C++ -*-
/**
 *  @brief QStandardItem, observer of data models
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
#include "otgui/DataModelDefinitionItem.hxx"

#include "otgui/OTStudyItem.hxx"
#include "otgui/InferenceAnalysis.hxx"
#include "otgui/DataAnalysis.hxx"
#include "otgui/CopulaInferenceAnalysis.hxx"

using namespace OT;

namespace OTGUI {

DataModelDefinitionItem::DataModelDefinitionItem(const DesignOfExperiment & designOfExperiment)
  : DesignOfExperimentItem(designOfExperiment, "DataModelDefinition")
{
  setData(tr("Definition"), Qt::DisplayRole);
  QFont font;
  font.setWeight(font.weight()+10);
  setData(font, Qt::FontRole);
  setEditable(false);

  buildActions();
}


void DataModelDefinitionItem::buildActions()
{
  // new data analyses
  newDataAnalysis_ = new QAction(tr("New data analysis"), this);
  newDataAnalysis_->setStatusTip(tr("Analyse the data sample"));
  connect(newDataAnalysis_, SIGNAL(triggered()), this, SLOT(createNewDataAnalysis()));

  newInferenceAnalysis_ = new QAction(tr("New inference analysis"), this);
  newInferenceAnalysis_->setStatusTip(tr("Inference"));
  connect(newInferenceAnalysis_, SIGNAL(triggered()), this, SLOT(createNewInferenceAnalysis()));

  newCopulaInferenceAnalysis_ = new QAction(tr("Dependencies inference analysis"), this);
  newCopulaInferenceAnalysis_->setStatusTip(tr("Dependencies inference"));
  connect(newCopulaInferenceAnalysis_, SIGNAL(triggered()), this, SLOT(createNewCopulaInferenceAnalysis()));

  newMetaModel_ = new QAction(tr("New metamodel"), this);
  newMetaModel_->setStatusTip(tr("Create a new metamodel"));
  connect(newMetaModel_, SIGNAL(triggered()), this, SLOT(createNewMetaModel()));

  // remove data model
  removeDataModel_ = new QAction(QIcon(":/images/window-close.png"), tr("Remove"), this);
  removeDataModel_->setStatusTip(tr("Remove the data model"));
  connect(removeDataModel_, SIGNAL(triggered()), this, SLOT(removeDesignOfExperiment()));

  // add actions
  appendAction(newDataAnalysis_);
  appendAction(newInferenceAnalysis_);
  appendAction(newCopulaInferenceAnalysis_);
  appendAction(newMetaModel_);
  appendAction(removeDataModel_);
}


void DataModelDefinitionItem::setData(const QVariant & value, int role)
{
  if (role == Qt::EditRole)
    designOfExperiment_.getImplementation()->setName(value.toString().toLocal8Bit().data());

  QStandardItem::setData(value, role);
}


void DataModelDefinitionItem::update(Observable* source, const String& message)
{
  if (message == "variablesChanged")
  {
    emit variablesChanged();
  }
  else if (message == "designOfExperimentRemoved")
  {
    emit removeRequested(row());
  }
}


void DataModelDefinitionItem::createNewDataAnalysis()
{
  if (!designOfExperimentValid())
    return;

  DataAnalysis analysis(getParentOTStudyItem()->getOTStudy().getAvailableAnalysisName("DataAnalysis_"), designOfExperiment_);
  getParentOTStudyItem()->getOTStudy().add(analysis);
}


void DataModelDefinitionItem::createNewInferenceAnalysis()
{
  if (!designOfExperimentValid())
    return;

  InferenceAnalysis analysis(getParentOTStudyItem()->getOTStudy().getAvailableAnalysisName("inference_"), designOfExperiment_);
  emit analysisRequested(this, analysis);
}


void DataModelDefinitionItem::createNewCopulaInferenceAnalysis()
{
  if (!designOfExperimentValid())
    return;

  if (designOfExperiment_.getInputSample().getDimension() < 2)
    emit emitErrorMessageRequested(tr("The model must contain at least two inputs."));
  else
  {
    CopulaInferenceAnalysis analysis(getParentOTStudyItem()->getOTStudy().getAvailableAnalysisName("DependenciesInference_"), designOfExperiment_);
    getParentOTStudyItem()->getOTStudy().add(analysis);
  }
}
}