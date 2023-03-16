//                                               -*- C++ -*-
/**
 *  @brief QStandardItem, observer of analysis
 *
 *  Copyright 2015-2023 EDF-Phimeca
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
#include "persalys/AnalysisItem.hxx"

#include "persalys/DesignOfExperimentEvaluation.hxx"
#include "persalys/FunctionalChaosAnalysis.hxx"
#include "persalys/KrigingAnalysis.hxx"
#include "persalys/PolynomialRegressionAnalysis.hxx"
#include "persalys/StudyItem.hxx"
#include "persalys/TranslationManager.hxx"
#include "persalys/MonteCarloAnalysis.hxx"


#include <QDebug>

using namespace OT;

namespace PERSALYS
{

AnalysisItem::AnalysisItem(const Analysis &analysis)
  : Item(QString::fromUtf8(analysis.getName().c_str()), analysis.getImplementation()->getClassName().c_str())
  , Observer("AnalysisItem")
  , analysis_(analysis)
{
  analysis_.addObserver(this);

  buildActions();
}


AnalysisItem::AnalysisItem(const Analysis & analysis, const String &typeName)
  : Item(QString::fromUtf8(analysis.getName().c_str()), typeName.c_str())
  , Observer(typeName)
  , analysis_(analysis)
{
  analysis_.addObserver(this);
}


void AnalysisItem::buildActions()
{
  const QString analysisType(analysis_.getImplementation()->getClassName().c_str());
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
      analysisType == "KrigingAnalysis" ||
      analysisType == "PolynomialRegressionAnalysis")
  {
    convertAction_ = new QAction(tr("Convert metamodel into physical model"), this);
    convertAction_->setStatusTip(tr("Add the metamodel in the study tree"));
    connect(convertAction_, SIGNAL(triggered()), this, SLOT(appendMetaModelItem()));
    convertAction_->setEnabled(analysis_.getImplementation()->hasValidResult());

    appendAction(convertAction_);
  }
  else if (analysisType == "FieldMonteCarloAnalysis")
  {
    extractDataAction_ = new QAction(tr("Extract data at nodes"), this);
    connect(extractDataAction_, SIGNAL(triggered()), this, SLOT(extractData()));
    appendAction(extractDataAction_);
    if (!analysis_.hasValidResult())
      extractDataAction_->setEnabled(false);
  }
  else if (analysisType.contains("DesignOfExperiment") ||
           analysisType == "MonteCarloAnalysis")
  {
    convertAction_ = new QAction(tr("Convert into data model"), this);
    convertAction_->setStatusTip(tr("Add a data model in the study tree"));
    connect(convertAction_, SIGNAL(triggered()), this, SLOT(appendDataModelItem()));
    convertAction_->setEnabled(analysis_.getImplementation()->hasValidResult());
    appendAction(convertAction_);
    if(analysisType != "MonteCarloAnalysis")
      return; // no remove action for these analyses
  }

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
    if (analysis_.isRunning())
      return QIcon(":/images/green-arrow-right.png");
    else if (analysis_.hasValidResult())
      return QIcon(":/images/dialog-ok-apply.png");
    else if (!analysis_.getErrorMessage().empty())
      return QIcon(":/images/edit-delete.png");
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
  std::vector<Observer*> copyObservers(analysis_.getImplementation()->getObservers());
  // update analysis_
  analysis_ = analysis;
  // set the observers
  for (auto obs : copyObservers)
    analysis_.addObserver(obs);

  // update analysis type
  setData(analysis_.getImplementation()->getClassName().c_str(), Qt::UserRole);

  // update the implementation of the analysis stored in Study
  getParentStudyItem()->getStudy().getAnalysisByName(analysis.getName()).setImplementationAsPersistentObject(analysis.getImplementation());

  // the analysis has not result: disable convertAction_ action and extractDataAction_
  if (convertAction_) {
    convertAction_->setEnabled(analysis_.hasValidResult());
    emit numberMetamodelChanged(analysis_.hasValidResult());
  }
  if (extractDataAction_)
    extractDataAction_->setEnabled(analysis_.hasValidResult());

  emit windowRequested(this, false);
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
  String errorMessage = "";
  if (!analysis_.canBeLaunched(errorMessage))
  {
    emit showErrorMessageRequested(TranslationManager::GetTranslatedErrorMessage(errorMessage));
    return;
  }

  // TODO check limitstate?

  const QString analysisType(data(Qt::UserRole).toString());
  if (analysisType.contains("DesignOfExperiment") && analysisType != "DesignOfExperimentDefinitionItem")
  {
    emit doeEvaluationWizardRequested(getAnalysis());
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
  PolynomialRegressionAnalysis * regression = dynamic_cast<PolynomialRegressionAnalysis*>(analysis_.getImplementation().get());
  if (chaos)
  {
    getParentStudyItem()->appendMetaModelItem(chaos->getResult().getMetaModel());
  }
  else if (kriging)
  {
    getParentStudyItem()->appendMetaModelItem(kriging->getResult().getMetaModel());
  }
  else if (regression)
  {
    getParentStudyItem()->appendMetaModelItem(regression->getResult().getMetaModel());
  }
}


void AnalysisItem::appendDataModelItem()
{
  if (!getParentStudyItem())
    return;

  DesignOfExperimentEvaluation * doeEval = dynamic_cast<DesignOfExperimentEvaluation*>(analysis_.getImplementation().get());
  DataAnalysisResult result;
  if(doeEval) {
    result = doeEval->getResult();
  } else {
    MonteCarloAnalysis * analysis  = dynamic_cast<MonteCarloAnalysis*>(analysis_.getImplementation().get());
    if(analysis) {
      result = analysis->getResult();
    } else
      return;
  }

  // create the data model
  const String newName = getParentStudyItem()->getStudy().getAvailableDataModelName((QString(result.getName().c_str()) + "_").toStdString());
  DataModel * newModel = new DataModel(newName, result.getDesignOfExperiment());
  getParentStudyItem()->getStudy().add(newModel);
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


void AnalysisItem::processStatusChanged()
{
  // change icon
  emitDataChanged();
}


void AnalysisItem::update(Observable* /*source*/, const String& message)
{
  if (message == "analysisFinished")
  {
    // if MetaModelAnalysis : enable convertAction_ action
    if (convertAction_) {
      convertAction_->setEnabled(true);
      emit numberMetamodelChanged(true);}
    if (extractDataAction_)
      extractDataAction_->setEnabled(true);
  }
  if (message == "analysisFinished" || message == "analysisBadlyFinished")
  {
    // emit signal to the StudyTreeView to create a window
    emit windowRequested(this, false);
  }
  else if (message == "informationMessageUpdated")
  {
    // emit signal to AnalysisWindow to update the information message
    emit messageChanged(analysis_.getInformationMessage().c_str());
  }
  else if (message == "progressValueChanged")
  {
    // emit signal to AnalysisWindow to update the progress bar
    emit progressValueChanged(analysis_.getProgressValue());
  }
  else if (message == "doeChanged" && convertAction_)
  {
    convertAction_->setEnabled(analysis_.hasValidResult());
  }
  else if (message == "objectRemoved")
  {
    if (hasChildren())
      qDebug() << "AnalysisItem::update(objectRemoved) has not to contain child\n";
    if (convertAction_) {
      emit numberMetamodelChanged(false);
    }
    emit removeRequested(row());
  }
}


void AnalysisItem::extractData()
{
  emit dataExtractionWizardRequested(getParentStudyItem(), analysis_);
}
}
