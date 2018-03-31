//                                               -*- C++ -*-
/**
 *  @brief QStandardItem, observer of data models
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
  connect(newDataAnalysis_, SIGNAL(triggered()), this, SLOT(createDataAnalysis()));

  newInferenceAnalysis_ = new QAction(tr("Marginals inference"), this);
  newInferenceAnalysis_->setStatusTip(tr("Marginals inference"));
  connect(newInferenceAnalysis_, SIGNAL(triggered()), this, SLOT(createInferenceAnalysis()));

  newCopulaInferenceAnalysis_ = new QAction(tr("Dependence inference"), this);
  newCopulaInferenceAnalysis_->setStatusTip(tr("Dependence inference"));
  connect(newCopulaInferenceAnalysis_, SIGNAL(triggered()), this, SLOT(createCopulaInferenceAnalysis()));

  newMetaModel_ = new QAction(tr("Metamodel"), this);
  newMetaModel_->setStatusTip(tr("Create a new metamodel"));
  connect(newMetaModel_, SIGNAL(triggered()), this, SLOT(createMetaModel()));

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
    emit showErrorMessageRequested(tr("The sample is empty."));
    return false;
  }
  return true;
}


void DataModelDefinitionItem::createDataAnalysis()
{
  // check
  if (!designOfExperimentValid())
    return;

  // new analysis
  const String analysisName(getParentOTStudyItem()->getOTStudy().getAvailableAnalysisName(tr("dataAnalysis_").toStdString()));
  DataAnalysis analysis(analysisName, designOfExperiment_);
  getParentOTStudyItem()->getOTStudy().add(analysis);
}


void DataModelDefinitionItem::createInferenceAnalysis()
{
  // check
  if (!designOfExperimentValid())
    return;

  // new analysis
  const String analysisName(getParentOTStudyItem()->getOTStudy().getAvailableAnalysisName(tr("marginalsInference_").toStdString()));
  InferenceAnalysis analysis(analysisName, designOfExperiment_);
  // emit signal to StudyTreeView to open a wizard
  emit analysisRequested(this, analysis);
}


void DataModelDefinitionItem::createCopulaInferenceAnalysis()
{
  // check
  if (!designOfExperimentValid())
    return;

  if (designOfExperiment_.getSample().getDimension() < 2)
  {
    emit showErrorMessageRequested(tr("The model must contain at least two variables."));
    return;
  }

  // new analysis
  const String analysisName(getParentOTStudyItem()->getOTStudy().getAvailableAnalysisName(tr("dependenceInference_").toStdString()));
  CopulaInferenceAnalysis analysis(analysisName, designOfExperiment_);
  // emit signal to StudyTreeView to open a wizard
  emit analysisRequested(this, analysis);
}


void DataModelDefinitionItem::createMetaModel()
{
  // check
  if (!designOfExperiment_.getOutputSample().getSize() || !designOfExperiment_.getInputSample().getSize())
  {
    emit showErrorMessageRequested(tr("The model must have at least one output and one input."));
    return;
  }

  // new analysis
  const String analysisName(getParentOTStudyItem()->getOTStudy().getAvailableAnalysisName(tr("metaModel_").toStdString()));
  FunctionalChaosAnalysis analysis(analysisName, designOfExperiment_);
  // emit signal to StudyTreeView to open a wizard
  emit analysisRequested(this, analysis);
}
}
