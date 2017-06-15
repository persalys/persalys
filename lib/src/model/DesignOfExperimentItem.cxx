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


void DesignOfExperimentItem::setData(const QVariant & value, int role)
{
  // rename
  if (role == Qt::EditRole)
    designOfExperiment_.getImplementation()->setName(value.toString().toLocal8Bit().data());

  QStandardItem::setData(value, role);
}


void DesignOfExperimentItem::appendAnalysisItem(Analysis& analysis)
{
  // new item
  AnalysisItem * newItem = new AnalysisItem(analysis);

  // connections
  connect(newItem, SIGNAL(analysisInProgressStatusChanged(bool)), this, SLOT(setAnalysisInProgress(bool)));
  // - signal for the PhysicalModelDiagramItem
  connect(newItem, SIGNAL(analysisInProgressStatusChanged(bool)), this, SIGNAL(analysisInProgressStatusChanged(bool)));
  if (getParentOTStudyItem())
  {
    connect(newItem, SIGNAL(metaModelCreated(PhysicalModel)), getParentOTStudyItem(), SLOT(addMetaModelItem(PhysicalModel)));
    connect(newItem, SIGNAL(analysisInProgressStatusChanged(bool)), getParentOTStudyItem(), SLOT(setAnalysisInProgress(bool)));
  }
  connect(this, SIGNAL(designOfExperimentChanged(DesignOfExperiment)), newItem, SLOT(setDesignOfExperiment(DesignOfExperiment)));

  // append item
  appendRow(newItem);

  // emit signal to StudyTreeView to create a window
  emit newAnalysisItemCreated(newItem);
}


void DesignOfExperimentItem::createNewMetaModel()
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


void DesignOfExperimentItem::removeDesignOfExperiment()
{
  // check
  if (analysisInProgress_)
  {
    emit emitErrorMessageRequested(tr("Can not remove a design of experiment when an analysis is running."));
    return;
  }
  // remove
  if (getParentOTStudyItem())
    getParentOTStudyItem()->getOTStudy().remove(DesignOfExperiment(designOfExperiment_));
}
}