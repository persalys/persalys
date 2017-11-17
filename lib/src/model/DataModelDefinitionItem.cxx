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
#include "otgui/FunctionalChaosAnalysis.hxx"

#include <QDebug>

using namespace OT;

namespace OTGUI
{

DataModelDefinitionItem::DataModelDefinitionItem(const DesignOfExperiment & designOfExperiment)
  : DesignOfExperimentItem(designOfExperiment, "DataModelDefinition")
{
  setData(tr("Definition"), Qt::DisplayRole);
  QFont font;
  font.setWeight(font.weight() + 10);
  setData(font, Qt::FontRole);
  setEditable(false);

  buildActions();
}


void DataModelDefinitionItem::buildActions()
{
  // new data analyses
  newDataAnalysis_ = new QAction(tr("Data analysis"), this);
  newDataAnalysis_->setStatusTip(tr("Analyse the data sample"));
  connect(newDataAnalysis_, SIGNAL(triggered()), this, SLOT(createNewDataAnalysis()));

  newInferenceAnalysis_ = new QAction(tr("Inference"), this);
  newInferenceAnalysis_->setStatusTip(tr("Inference"));
  connect(newInferenceAnalysis_, SIGNAL(triggered()), this, SLOT(createNewInferenceAnalysis()));

  newCopulaInferenceAnalysis_ = new QAction(tr("Dependencies inference"), this);
  newCopulaInferenceAnalysis_->setStatusTip(tr("Dependencies inference"));
  connect(newCopulaInferenceAnalysis_, SIGNAL(triggered()), this, SLOT(createNewCopulaInferenceAnalysis()));

  newMetaModel_ = new QAction(tr("Metamodel"), this);
  newMetaModel_->setStatusTip(tr("Create a new metamodel"));
  connect(newMetaModel_, SIGNAL(triggered()), this, SLOT(createNewMetaModel()));

  // add actions
  appendSeparator(tr("Analysis"));
  appendAction(newDataAnalysis_);
  appendAction(newInferenceAnalysis_);
  appendAction(newCopulaInferenceAnalysis_);
  appendAction(newMetaModel_);
}


void DataModelDefinitionItem::update(Observable* source, const String& message)
{
  if (message == "variablesChanged")
  {
    // emit signal to DataModelWindow
    emit variablesChanged();
  }
  else if (message == "designOfExperimentRemoved")
  {
    if (hasChildren())
      qDebug() << "DataModelDefinitionItem::update(designOfExperimentRemoved) has not to contain child\n";
    emit removeRequested(row());
  }
}


bool DataModelDefinitionItem::designOfExperimentValid()
{
  if (!designOfExperiment_.getSample().getSize())
  {
    emit emitErrorMessageRequested(tr("The sample is empty."));
    return false;
  }
  return true;
}


void DataModelDefinitionItem::createNewDataAnalysis()
{
  // check
  if (!designOfExperimentValid())
    return;

  // new analysis
  DataAnalysis analysis(getParentOTStudyItem()->getOTStudy().getAvailableAnalysisName("DataAnalysis_"), designOfExperiment_);
  getParentOTStudyItem()->getOTStudy().add(analysis);
}


void DataModelDefinitionItem::createNewInferenceAnalysis()
{
  // check
  if (!designOfExperimentValid())
    return;

  // new analysis
  InferenceAnalysis analysis(getParentOTStudyItem()->getOTStudy().getAvailableAnalysisName("inference_"), designOfExperiment_);
  // emit signal to StudyTreeView to open a wizard
  emit analysisRequested(this, analysis);
}


void DataModelDefinitionItem::createNewCopulaInferenceAnalysis()
{
  // check
  if (!designOfExperimentValid())
    return;

  if (designOfExperiment_.getInputSample().getDimension() < 2)
  {
    emit emitErrorMessageRequested(tr("The model must contain at least two inputs."));
    return;
  }

  // new analysis
  CopulaInferenceAnalysis analysis(getParentOTStudyItem()->getOTStudy().getAvailableAnalysisName("DependenciesInference_"), designOfExperiment_);
  getParentOTStudyItem()->getOTStudy().add(analysis);
}


void DataModelDefinitionItem::createNewMetaModel()
{
  // check
  if (!designOfExperiment_.getOutputSample().getSize())
  {
    emit emitErrorMessageRequested(tr("The model must have at least one output."));
    return;
  }

  // new analysis
  FunctionalChaosAnalysis analysis(getParentOTStudyItem()->getOTStudy().getAvailableAnalysisName("metaModel_"), designOfExperiment_);
  // emit signal to StudyTreeView to open a wizard
  emit analysisRequested(this, analysis);
}
}
