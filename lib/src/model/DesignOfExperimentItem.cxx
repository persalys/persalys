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
#include "otgui/DesignOfExperimentItem.hxx"

#include "otgui/FunctionalChaosAnalysis.hxx"
#include "otgui/OTStudyItem.hxx"

using namespace OT;

namespace OTGUI {

DesignOfExperimentItem::DesignOfExperimentItem(const DesignOfExperiment& designOfExperiment, const OT::String observerType)
  : OTguiItem(QString::fromUtf8(designOfExperiment.getName().c_str()), "DesignOfExperiment")
  , Observer(observerType)
  , designOfExperiment_(designOfExperiment)
{
  designOfExperiment_.addObserver(this);
}


DesignOfExperiment DesignOfExperimentItem::getDesignOfExperiment() const
{
  return designOfExperiment_;
}


void DesignOfExperimentItem::appendAnalysisItem(Analysis& analysis)
{
  AnalysisItem * newItem = new AnalysisItem(analysis);
  connect(newItem, SIGNAL(analysisStatusChanged(bool)), this, SLOT(setAnalysisInProgress(bool)));
  if (getParentOTStudyItem())
    connect(newItem, SIGNAL(analysisStatusChanged(bool)), getParentOTStudyItem(), SLOT(setAnalysisInProgress(bool)));
  appendRow(newItem);

  emit newAnalysisItemCreated(newItem);
}


void DesignOfExperimentItem::updateDesignOfExperiment(const DesignOfExperiment & designOfExperiment)
{
  designOfExperiment_.getImplementation().get()->removeObserver(this);
  // TODO ? remove observer evaluation
  designOfExperiment_ = designOfExperiment;
  designOfExperiment_.addObserver(this);

  // update the implementation of the design of experiment stored in OTStudy
  if (designOfExperiment.getImplementation()->getClassName() == "DataModel")
    getParentOTStudyItem()->getOTStudy().getDataModelByName(designOfExperiment.getName()).setImplementationAsPersistentObject(designOfExperiment.getImplementation());
  else
    getParentOTStudyItem()->getOTStudy().getDesignOfExperimentByName(designOfExperiment.getName()).setImplementationAsPersistentObject(designOfExperiment.getImplementation());

  emit updateDiagram(designOfExperiment_);
}


void DesignOfExperimentItem::update(Observable* source, const String & message)
{
  if (message == "analysisFinished")
  {
    emit analysisFinished();
  }
  if (message == "analysisBadlyFinished")
  {
    emit analysisBadlyFinished(designOfExperiment_.getErrorMessage().c_str());
  }
  else if (message == "designOfExperimentRemoved")
  {
    emit removeRequested(row());
  }
}


bool DesignOfExperimentItem::designOfExperimentValid()
{
  if (!designOfExperiment_.getSample().getSize())
  {
    emit emitErrorMessageRequested(tr("The sample is empty."));
    return false;
  }
  return true;
}


void DesignOfExperimentItem::createNewMetaModel()
{
  if (!designOfExperiment_.getOutputSample().getSize())
  {
    emit emitErrorMessageRequested(tr("The model must have at least one output."));
    return;
  }

  FunctionalChaosAnalysis analysis(getParentOTStudyItem()->getOTStudy().getAvailableAnalysisName("metaModel_"), designOfExperiment_);
  emit analysisRequested(this, analysis);
}


void DesignOfExperimentItem::removeDesignOfExperiment()
{
  if (getParentOTStudyItem())
  {
    if (analysisInProgress_)
    {
      emit emitErrorMessageRequested(tr("Can not remove a design of experiment when an analysis is running."));
      return;
    }
    getParentOTStudyItem()->getOTStudy().remove(DesignOfExperiment(designOfExperiment_));
  }
}
}