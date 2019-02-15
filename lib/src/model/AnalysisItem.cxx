//                                               -*- C++ -*-
/**
 *  @brief QStandardItem, observer of analysis
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
#include "otgui/AnalysisItem.hxx"

#include "otgui/ModelEvaluation.hxx"
#include "otgui/GridDesignOfExperiment.hxx"
#include "otgui/ProbabilisticDesignOfExperiment.hxx"
#include "otgui/ImportedDesignOfExperiment.hxx"
#include "otgui/FunctionalChaosAnalysis.hxx"
#include "otgui/KrigingAnalysis.hxx"
#include "otgui/SobolAnalysis.hxx"
#include "otgui/SRCAnalysis.hxx"
#include "otgui/FORMAnalysis.hxx"
#include "otgui/MonteCarloAnalysis.hxx"
#include "otgui/SimulationReliabilityAnalysis.hxx"
#include "otgui/MonteCarloReliabilityAnalysis.hxx"
#include "otgui/FORMImportanceSamplingAnalysis.hxx"
#include "otgui/InferenceAnalysis.hxx"
#include "otgui/TranslationManager.hxx"

#include "otgui/PhysicalModelItem.hxx"
#include "otgui/DesignOfExperimentItem.hxx"
#include "otgui/LimitStateItem.hxx"
#include "otgui/StudyItem.hxx"

#include <openturns/OTBase.hxx>

#include <QDebug>

using namespace OT;

namespace OTGUI
{

AnalysisItem::AnalysisItem(const Analysis & analysis)
  : Item(QString::fromUtf8(analysis.getName().c_str()), analysis.getImplementation()->getClassName().c_str())
  , Observer("Analysis")
  , analysis_(analysis)
  , convertAction_(0)
  , modifyAction_(0)
  , removeAction_(0)
{
  analysis_.addObserver(this);

  buildActions();
}


void AnalysisItem::buildActions()
{
  const QString analysisType = data(Qt::UserRole).toString();

  // modify analysis action
  if (analysisType != "DataAnalysis" &&
      analysisType != "ImportanceSamplingAnalysis") // there is no wizard associated with these analyses <=> impossible to modify them
  {
    modifyAction_ = new QAction(QIcon(":/images/run-build.png"), tr("Modify"), this);
    modifyAction_->setStatusTip(tr("Modify the analysis"));
    connect(modifyAction_, SIGNAL(triggered()), this, SLOT(modifyAnalysis()));

    appendAction(modifyAction_);
  }
  if (analysisType == "FunctionalChaosAnalysis" ||
      analysisType == "KrigingAnalysis")
  {
    convertAction_ = new QAction(tr("Convert metamodel into physical model"), this);
    convertAction_->setStatusTip(tr("Add the metamodel in the study tree"));
    connect(convertAction_, SIGNAL(triggered()), this, SLOT(appendMetaModelItem()));
    convertAction_->setEnabled(analysis_.getImplementation()->hasValidResult());

    appendAction(convertAction_);
  }

  // no remove action for these analyses
  if (analysisType.contains("DesignOfExperiment"))
    return;

  appendSeparator();

  // remove analysis action
  removeAction_ = new QAction(QIcon(":/images/window-close.png"), tr("Remove"), this);
  removeAction_->setStatusTip(tr("Remove the analysis"));
  connect(removeAction_, SIGNAL(triggered()), this, SLOT(removeAnalysis()));

  appendAction(removeAction_);
}


QVariant AnalysisItem::data(int role) const
{
  // set icon
  if (role == Qt::DecorationRole)
  {
    if (analysis_.hasValidResult())
      return QIcon(":/images/dialog-ok-apply.png");
    else if (!analysis_.getErrorMessage().empty())
      return QIcon(":/images/edit-delete.png");
    else if (analysis_.isRunning())
      return QIcon(":/images/green-arrow-right.png");
    else
      return QIcon(":/images/run-build.png");
  }
  else
    return Item::data(role);
}


void AnalysisItem::setData(const QVariant & value, int role)
{
  // rename
  if (role == Qt::EditRole)
    analysis_.getImplementation()->setName(value.toString().toUtf8().data());

  QStandardItem::setData(value, role);
}


Analysis AnalysisItem::getAnalysis() const
{
  return analysis_;
}


void AnalysisItem::updateAnalysis(const Analysis & analysis)
{
  // update analysis_
  analysis_.getImplementation().get()->removeObserver("Study");
  analysis_.getImplementation().get()->removeObserver(this);
  analysis_ = analysis;
  if (!analysis_.getImplementation().get()->getObservers().size())
    analysis_.addObserver(this);
  analysis_.addObserver(getParentStudyItem()->getStudy().getImplementation().get());

  // update analysis type
  setData(analysis_.getImplementation()->getClassName().c_str(), Qt::UserRole);

  // update the implementation of the analysis stored in Study
  getParentStudyItem()->getStudy().getAnalysisByName(analysis.getName()).setImplementationAsPersistentObject(analysis.getImplementation());

  // the analysis has not result: disable convertAction_ action
  if (convertAction_)
    convertAction_->setEnabled(analysis_.hasValidResult());
}


void AnalysisItem::stopAnalysis()
{
  analysis_.stop();
}


void AnalysisItem::modifyAnalysis()
{
  // check if the analysis is running
  if (analysis_.isRunning())
  {
    emit showErrorMessageRequested(tr("Can not modify a running analysis."));
    return;
  }

  const QString analysisType = data(Qt::UserRole).toString();

  // -- IF PhysicalModelAnalysis

  // check physical model
  PhysicalModelAnalysis * pmAnalysis_ptr = dynamic_cast<PhysicalModelAnalysis*>(analysis_.getImplementation().get());
  if (pmAnalysis_ptr)
  {
    // must have in/outputs
    if (!pmAnalysis_ptr->getPhysicalModel().isValid())
    {
      emit showErrorMessageRequested(tr("The physical model must have inputs AND at least one selected output."));
      return;
    }
    if (analysisType == "MorrisAnalysis" && pmAnalysis_ptr->getPhysicalModel().getInputDimension() < 2)
    {
      emit showErrorMessageRequested(tr("The physical model must have at least two inputs"));
      return;
    }
    // if proba analysis
    if (analysisType != "ModelEvaluation" &&
        analysisType != "MorrisAnalysis" &&
        analysisType != "OptimizationAnalysis" &&
        !analysisType.contains("DesignOfExperiment"))
    {
      // must have stochastic variables
      if (!pmAnalysis_ptr->getPhysicalModel().hasStochasticInputs())
      {
        emit showErrorMessageRequested(tr("The physical model must have stochastic inputs."));
        return;
      }
      // if sensitivity analysis
      if (analysisType == "SobolAnalysis" || analysisType == "SRCAnalysis")
      {
        // must have an independent copula
        if (!pmAnalysis_ptr->getPhysicalModel().getCopula().hasIndependentCopula())
        {
          emit showErrorMessageRequested(tr("The model must have an independent copula to compute a sensitivity analysis but here inputs are dependent."));
          return;
        }
      }
    }
    // TODO check limitstate?
  }

  // -- IF DesignOfExperimentAnalysis

  // check data model
  DesignOfExperimentAnalysis * dmAnalysis_ptr = dynamic_cast<DesignOfExperimentAnalysis*>(analysis_.getImplementation().get());
  if (dmAnalysis_ptr)
  {
    // if meta model analysis
    if (analysisType == "FunctionalChaosAnalysis" || analysisType == "KrigingAnalysis")
    {
      // must have output data
      if (!dmAnalysis_ptr->getDesignOfExperiment().getOutputSample().getSize())
      {
        emit showErrorMessageRequested(tr("The sample must not be empty and must contain output values."));
        return;
      }
    }
    // must have at least a point
    if (!dmAnalysis_ptr->getDesignOfExperiment().getSample().getSize())
    {
      emit showErrorMessageRequested(tr("The sample is empty."));
      return;
    }
    if (!dmAnalysis_ptr->getDesignOfExperiment().isValid())
    {
      emit showErrorMessageRequested(tr("The sample contains invalid values."));
      return;
    }
  }

  if (analysisType.contains("DesignOfExperiment"))
  {
    emit modifyDesignOfExperimentEvaluation(getAnalysis());
  }
  else
  {
    // emit signal to StudyTreeView to open the wizard
    emit modifyAnalysisRequested(this);
  }
}


void AnalysisItem::appendMetaModelItem()
{
  FunctionalChaosAnalysis * chaos = dynamic_cast<FunctionalChaosAnalysis*>(analysis_.getImplementation().get());
  KrigingAnalysis * kriging = dynamic_cast<KrigingAnalysis*>(analysis_.getImplementation().get());
  if (chaos)
  {
    getParentStudyItem()->appendMetaModelItem(chaos->getResult().getMetaModel());
  }
  else if (kriging)
  {
    getParentStudyItem()->appendMetaModelItem(kriging->getResult().getMetaModel());
  }
}


void AnalysisItem::removeAnalysis()
{
  // check if the analysis is running
  if (analysis_.isRunning())
  {
    emit showErrorMessageRequested(tr("Can not remove a running analysis."));
    return;
  }

  // remove
  if (getParentStudyItem())
    getParentStudyItem()->getStudy().remove(Analysis(analysis_));
}


void AnalysisItem::processLaunched()
{
  // change icon
  emitDataChanged();

  // emit signal to disable run analysis/close study/import script...
  // warn the other objects that an analysis is running
  emit analysisInProgressStatusChanged(true);
}


void AnalysisItem::processFinished()
{
  // change icon
  emitDataChanged();

  // emit signal to enable run analysis/close study/import script...
  // warn the other objects that an analysis is finished
  emit analysisInProgressStatusChanged(false);
}


void AnalysisItem::update(Observable* source, const String& message)
{
  if (message == "analysisFinished")
  {
    // emit signal to the StudyTreeView to create a window
    emit analysisFinished(this, false);

    // if MetaModelAnalysis : enable convertAction_ action
    if (convertAction_)
      convertAction_->setEnabled(true);
  }
  else if (message == "analysisBadlyFinished")
  {
    // emit signal to the StudyTreeView to create a window
    emit analysisFinished(this, false);
  }
  else if (message == "informationMessageUpdated")
  {
    // emit signal to AnalysisWindow to update the information message
    emit messageChanged(analysis_.getInformationMessage().c_str());
  }
  else if (message == "progressValueChanged")
  {
    // emit signal to AnalysisWindow to upate the progress bar
    emit progressValueChanged(analysis_.getProgressValue());
  }
  else if (message == "analysisRemoved")
  {
    if (hasChildren())
      qDebug() << "AnalysisItem::update(analysisRemoved) has not to contain child\n";
    emit removeRequested(row());
  }
}
}
