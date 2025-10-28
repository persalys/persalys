//                                               -*- C++ -*-
/**
 *  @brief QStandardItem, observer of analysis
 *
 *  Copyright 2015-2025 EDF-Phimeca
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
#include "persalys/MorrisAnalysis.hxx"
#include "persalys/DesignOfExperimentEvaluation.hxx"
#include "persalys/SimulationReliabilityAnalysis.hxx"
#include "persalys/SobolAnalysis.hxx"
#include "persalys/SRCAnalysis.hxx"
#include "persalys/MultiObjectiveOptimizationAnalysis.hxx"
#include "persalys/CalibrationAnalysis.hxx"
#include "persalys/PythonPhysicalModel.hxx"


#include <QDebug>
#include <QMessageBox>
#include <QApplication>

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
      analysisType != "ImportanceSamplingAnalysis" &&
      analysisType != "FieldKarhunenLoeveAnalysis" &&
      analysisType != "DataSensitivityAnalysis") // there is no wizard associated with these analyses <=> impossible to modify them
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
    connect(convertAction_, &QAction::triggered, [this](){
      const auto * analysis = dynamic_cast<MetaModelAnalysis*>(analysis_.getImplementation().get());
      getParentStudyItem()->appendMetaModelItem(analysis->getMetaModel());
    });
    convertAction_->setEnabled(analysis_.getImplementation()->hasValidResult());
    appendAction(convertAction_);

    convertPythonAction_ = new QAction(tr("Convert metamodel into python model"), this);
    convertPythonAction_->setStatusTip(tr("Import the metamodel inside a python model and add the model to the study"));
    connect(convertPythonAction_, &QAction::triggered, this, &AnalysisItem::addPythonMetaModel);
    convertPythonAction_->setEnabled(analysis_.getImplementation()->hasValidResult());
    appendAction(convertPythonAction_);

    exportAction_ = new QAction(tr("Export metamodel"), this);
    exportAction_->setStatusTip(tr("Export to an independent Python script"));
    connect(exportAction_, &QAction::triggered, [this](){
      const auto * analysis = dynamic_cast<MetaModelAnalysis*>(analysis_.getImplementation().get());
      emit pythonMetamodelExportRequested(analysis->getMetaModel());
    });
    exportAction_->setEnabled(analysis_.getImplementation()->hasValidResult());
    appendAction(exportAction_);
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
           analysisType == "MonteCarloAnalysis" || 
           analysisType == "MorrisAnalysis" ||
           analysisType == "CalibrationAnalysis")
  {
    convertAction_ = new QAction(tr("Convert into data model"), this);
    convertAction_->setStatusTip(tr("Add a data model in the study tree"));
    connect(convertAction_, SIGNAL(triggered()), this, SLOT(appendDataModelItem()));
    convertAction_->setEnabled(analysis_.getImplementation()->hasValidResult());
    appendAction(convertAction_);

    if(analysisType != "MonteCarloAnalysis" && analysisType != "MorrisAnalysis")
      return; // no remove action for these analyses
  }

  // DoE export
  if ((analysisType.contains("MonteCarlo") && !analysisType.contains("field", Qt::CaseInsensitive))
      || analysisType == "SobolAnalysis" || analysisType == "SRCAnalysis"
      || analysisType == "FORMImportanceSamplingAnalysis"
      || analysisType == "MorrisAnalysis"
      || analysisType == "MultiObjectiveOptimizationAnalysis"
    )
  {
    exportDoEAction_ = new QAction(tr("Export DoE"), this);
    exportDoEAction_->setStatusTip(tr("Add the underlying DoE in the study tree"));
    connect(exportDoEAction_, &QAction::triggered, this, &AnalysisItem::exportDoE);
    exportDoEAction_->setEnabled(analysis_.getImplementation()->hasValidResult());
    appendAction(exportDoEAction_);
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
  const bool wasValid = analysis_.hasValidResult();
  analysis_ = analysis;
  // set the observers
  for (auto obs : copyObservers)
    analysis_.addObserver(obs);

  // update analysis type
  setData(analysis_.getImplementation()->getClassName().c_str(), Qt::UserRole);

  // update the implementation of the analysis stored in Study
  getParentStudyItem()->getStudy().getAnalysisByName(analysis.getName()).setImplementationAsPersistentObject(analysis.getImplementation());

  // the analysis has not result: disable convertAction_ action and extractDataAction_
  if (convertAction_)
  {
    convertAction_->setEnabled(analysis_.hasValidResult());
    if (wasValid)
      emit numberMetamodelChanged(analysis_.hasValidResult() ? 0 : -1);
    else
      emit numberMetamodelChanged(analysis_.hasValidResult() ? 1 : 0);
  }
  if (extractDataAction_)
    extractDataAction_->setEnabled(analysis_.hasValidResult());
  if (exportDoEAction_)
    exportDoEAction_->setEnabled(analysis_.hasValidResult());

  emit windowRequested(this, false);
}


void AnalysisItem::stopAnalysis()
{
  analysis_.stop();
}


void AnalysisItem::detachAnalysis()
{
  analysis_.detach();
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

void AnalysisItem::addPythonMetaModel()
{
  try
  {
    const auto * analysis = dynamic_cast<MetaModelAnalysis*>(analysis_.getImplementation().get());
    getParentStudyItem()->appendPythonMetaModelItem(analysis->asPythonPhysicalModel(getParentStudyItem()->getStudy()));
  }
  catch (InvalidArgumentException &e)
  {
    QMessageBox::warning(QApplication::activeWindow(), tr("Impossible to retrieve the metamodel via Persalys Python API"), e.what());
  }
  catch (DuplicateItemException &e)
  {
    QString warningTitle(tr("Impossible to retrieve the metamodel via Persalys Python API"));
    QString warningMessage;
    if (e.getItemType() == DuplicateItemException::ItemType::Study)
      warningMessage = tr("Two or more studies are opened with name %1. Try renaming one.").arg(getParentStudyItem()->getStudy().getName().c_str());
    else if (e.getItemType() == DuplicateItemException::ItemType::Analysis)
      warningMessage = tr("Two or more analysis are named %1 inside the study. Try renaming one.").arg(analysis_.getName().c_str());
    else
      warningMessage = e.what();
    
    QMessageBox::warning(QApplication::activeWindow(), warningTitle, warningMessage);
  }
}

void AnalysisItem::appendDataModelItem()
{
  if (!getParentStudyItem())
    return;

  Study study{getParentStudyItem()->getStudy()};

  EvaluationResult result;
  AnalysisImplementation * implementation = analysis_.getImplementation().get();
  const auto * doeEval              = dynamic_cast<DesignOfExperimentEvaluation*>(implementation);
  const auto * MCAnalysis           = dynamic_cast<MonteCarloAnalysis*>(implementation);
  const auto * morrisAnalysis       = dynamic_cast<MorrisAnalysis*>(implementation);
  const auto * calibrationAnalysis  = dynamic_cast<CalibrationAnalysis*>(implementation);

  if (doeEval)
    result = doeEval->getResult();
  else if (MCAnalysis)
    result = MCAnalysis->getResult();
  else if (morrisAnalysis)
    result = morrisAnalysis->getResult();
  else if (calibrationAnalysis)
  {
    CalibrationResult calibrationResult = calibrationAnalysis->getResult().getCalibrationResult();
    const String newName{study.getAvailableDataModelName((QString(result.getName().c_str()) + "_").toStdString())};

    const DesignOfExperiment observations = calibrationAnalysis->getObservations();
    const Sample obsInput{observations.getInputSample()};
    const Sample obsOutput{observations.getOutputSample()};
    const Description obsOutputDescr{obsOutput.getDescription()};

    Sample inSample{obsInput};
    inSample.stack(obsOutput);
    Description inSampleDescription{inSample.getDescription()};
    inSample.stack(obsOutput - calibrationResult.getOutputAtPriorMean());
    for (UnsignedInteger i = 0 ; i < obsOutput.getDimension() ; ++i)
    {
      inSampleDescription.add(tr("Initial residuals for %1").arg(QString{obsOutputDescr[i].c_str()}).toStdString());
    }
    inSample.setDescription(inSampleDescription);

    Sample outSample{calibrationResult.getOutputAtPosteriorMean()};
    Description outSampleDescription;
    for (UnsignedInteger i = 0 ; i < obsOutput.getDimension() ; ++i)
    {
      outSampleDescription.add(tr("Calibrated %1").arg(QString{obsOutputDescr[i].c_str()}).toStdString());
    }
    if (calibrationResult.getOutputAtPosteriorMean().getSize())
    {
      outSample.stack(obsOutput - calibrationResult.getOutputAtPosteriorMean());
      for (UnsignedInteger i = 0 ; i < obsOutput.getDimension() ; ++i)
      {
        outSampleDescription.add(tr("Calibrated residuals for %1").arg(QString{obsOutputDescr[i].c_str()}).toStdString());
      }
    }
    outSample.setDescription(outSampleDescription);

    auto * newModel = new DataModel(newName, inSample, outSample);
    study.add(newModel);
    return;
  }
  else
    return;

  // create the data model
  const String newName{study.getAvailableDataModelName((QString(result.getName().c_str()) + "_").toStdString())};
  auto * newModel = new DataModel(newName, result.getDesignOfExperiment());
  study.add(newModel);  // implicit conversion DesignOfExperiment(DesignOfExperimentImplementation*)
}

void AnalysisItem::addDoEToStudy(const StudyItem * studyItem, const String &name, const PhysicalModel &model, const DesignOfExperiment &doe) const
{
  const DataAnalysisResult result(doe);
  const DesignOfExperimentEvaluation doeEval(name, model,result);
  const Analysis analysis(doeEval);
  studyItem->getStudy().add(analysis);
}

void AnalysisItem::exportDoE()
{
  const String name = analysis_.getName();
  const StudyItem *parentStudyItem = getParentStudyItem();

  if (!parentStudyItem)
  {
    std::cerr << "AnalysisItem::exportDoe: no parent StudyItem" << std::endl;
    return;
  }
  
  if (const auto * SRAImplementation = dynamic_cast<SimulationReliabilityAnalysis*>(analysis_.getImplementation().get()); SRAImplementation)
  {
    // MonteCarloReliability or FORMImportanceSampling
    const PhysicalModel model = SRAImplementation->getPhysicalModel();
    const SimulationReliabilityResult result = SRAImplementation->getResult();
    const DesignOfExperiment doe = result.getDesignOfExperiment();
    addDoEToStudy(parentStudyItem, name, model, doe);
    
    return;
  }

  if (const auto * MCImplementation = dynamic_cast<MonteCarloAnalysis*>(analysis_.getImplementation().get()); MCImplementation)
  {
    const PhysicalModel model = MCImplementation->getPhysicalModel();
    const DataAnalysisResult result = MCImplementation->getResult();
    const DesignOfExperimentEvaluation doeEval(name, model, result);
    const Analysis analysis(doeEval);
    parentStudyItem->getStudy().add(analysis);

    return;
  }

  if (const auto * sobolImplementation = dynamic_cast<SobolAnalysis*>(analysis_.getImplementation().get()); sobolImplementation)
  {
    const PhysicalModel model = sobolImplementation->getPhysicalModel();
    const SobolResult result = sobolImplementation->getResult();
    const DesignOfExperiment doe = result.getDesignOfExperiment();
    addDoEToStudy(parentStudyItem, name, model, doe);
    
    return;
  }

  if (const auto * SRCImplementation = dynamic_cast<SRCAnalysis*>(analysis_.getImplementation().get()); SRCImplementation)
  {
    const PhysicalModel model = SRCImplementation->getPhysicalModel();
    const SRCResult result = SRCImplementation->getResult();
    const DesignOfExperiment doe = result.getDesignOfExperiment();
    addDoEToStudy(parentStudyItem, name, model, doe);

    return;
  }

  if (const auto * morrisImplementation = dynamic_cast<MorrisAnalysis*>(analysis_.getImplementation().get()); morrisImplementation)
  {
    const PhysicalModel model = morrisImplementation->getPhysicalModel();
    const MorrisResult result = morrisImplementation->getResult();
    const DesignOfExperiment doe = result.getDesignOfExperiment();
    addDoEToStudy(parentStudyItem, name, model, doe);

    return;
  }

  if (const auto * mooImplementation = dynamic_cast<MultiObjectiveOptimizationAnalysis*>(analysis_.getImplementation().get()); mooImplementation)
  {
    const PhysicalModel model = mooImplementation->getPhysicalModel();
    const MultiObjectiveOptimizationAnalysisResult result = mooImplementation->getResult();
    const Sample sample = result.getFinalPop();
    const Description interestVariables = mooImplementation->getInterestVariables();
    DesignOfExperiment doe;
    doe.setOutputSample(sample.getMarginal(interestVariables));
    Indices inputIndices(sample.getDimension()-interestVariables.getSize()-1u);
    inputIndices.fill();
    doe.setInputSample(sample.getMarginal(inputIndices));
    addDoEToStudy(parentStudyItem, name, model, doe);
    return;
  }

  std::cerr << "AnalysisItem::exportDoE: every cast failed for analysis " << analysis_.getClassName() << std::endl;
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
    if (convertAction_)
    {
      convertAction_->setEnabled(true);
      emit numberMetamodelChanged(1);
    }
    if (convertPythonAction_)
    {
      convertPythonAction_->setEnabled(true);
    }
    if (extractDataAction_)
      extractDataAction_->setEnabled(true);
    if (exportDoEAction_)
      exportDoEAction_->setEnabled(true);
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
  else if (message == "doeChanged")
  {
    if (convertAction_)
      convertAction_->setEnabled(analysis_.hasValidResult());
    if (convertPythonAction_)
      convertPythonAction_->setEnabled(analysis_.hasValidResult());
  }
  else if (message == "metamodelAvailable" && exportAction_)
  {
    exportAction_->setEnabled(analysis_.hasValidResult());
  }
  else if (message == "objectRemoved")
  {
    if (hasChildren())
      qDebug() << "AnalysisItem::update(objectRemoved) has not to contain child\n";
    if (convertAction_)
    {
      emit numberMetamodelChanged(-1);
    }
    emit removeRequested(row());
  }
}


void AnalysisItem::extractData()
{
  emit dataExtractionWizardRequested(getParentStudyItem(), analysis_);
}
}
