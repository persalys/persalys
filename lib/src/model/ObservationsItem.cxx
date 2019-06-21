//                                               -*- C++ -*-
/**
 *  @brief QStandardItem, observer of observations
 *
 *  Copyright 2015-2019 EDF-Phimeca
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
#include "persalys/ObservationsItem.hxx"

#include "persalys/CalibrationAnalysis.hxx"
#include "persalys/StudyItem.hxx"

#include <QDebug>

using namespace OT;

namespace PERSALYS
{

ObservationsItem::ObservationsItem(const DesignOfExperiment& designOfExperiment)
  : DesignOfExperimentItem(designOfExperiment, "Observations")
  , newCalibration_(0)
{
  buildActions();
}


void ObservationsItem::buildActions()
{
  // new calibration action
  newCalibration_ = new QAction(QIcon(":/images/calibration.png"), tr("Calibration"), this);
  newCalibration_->setStatusTip(tr("Create a new calibration"));
  connect(newCalibration_, SIGNAL(triggered()), this, SLOT(createCalibration()));

  // remove observations action
  removeAction_ = new QAction(QIcon(":/images/window-close.png"), tr("Remove"), this);
  removeAction_->setStatusTip(tr("Remove the observations"));
  connect(removeAction_, SIGNAL(triggered()), this, SLOT(removeObservations()));

  // add actions
  appendAction(newCalibration_);
  appendSeparator();
  appendAction(removeAction_);
}


void ObservationsItem::update(Observable * source, const String & message)
{
  if (message == "observationsRemoved")
  {
    if (hasChildren())
      qDebug() << "ObservationsItem::update(observationsRemoved) has not to contain child\n";
    emit removeRequested(row());
  }
}


void ObservationsItem::appendItem(Analysis& analysis)
{
  // new item
  AnalysisItem * newItem = new AnalysisItem(analysis);

  // connections
  connect(newItem, SIGNAL(analysisInProgressStatusChanged(bool)), this, SLOT(setAnalysisInProgress(bool)));
  // - signal for the PhysicalModelDiagramItem
  connect(newItem, SIGNAL(analysisInProgressStatusChanged(bool)), this, SIGNAL(analysisInProgressStatusChanged(bool)));
  if (getParentStudyItem())
    connect(newItem, SIGNAL(analysisInProgressStatusChanged(bool)), getParentStudyItem(), SLOT(setAnalysisInProgress(bool)));

  // append item
  appendRow(newItem);

  // emit signal to StudyTreeView to create a window
  emit analysisItemCreated(newItem);
}


void ObservationsItem::createCalibration()
{
  // check
  if (!getDesignOfExperiment().getInputSample().getDimension() || !getDesignOfExperiment().getOutputSample().getDimension())
  {
    emit showErrorMessageRequested(tr("Observations must be defined for at least an input and an output."));
    return;
  }
  // new analysis
  const String analysisName(getParentStudyItem()->getStudy().getAvailableAnalysisName(tr("calibration_").toStdString()));
  CalibrationAnalysis analysis(analysisName, getDesignOfExperiment());
  // emit signal to StudyManager to open a wizard
  emit analysisRequested(this, analysis);
}


void ObservationsItem::removeObservations()
{
  // check
  if (analysisInProgress_)
  {
    emit showErrorMessageRequested(tr("Can not remove observations when an analysis is running."));
    return;
  }
  // remove
  if (getParentStudyItem())
    getParentStudyItem()->getStudy().remove(DesignOfExperiment(designOfExperiment_));
}
}
