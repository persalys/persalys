//                                               -*- C++ -*-
/**
 *  @brief QObject
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
#include "persalys/ItemFactory.hxx"

#include "persalys/DesignOfExperimentEvaluation.hxx"
#include "persalys/FunctionalChaosAnalysis.hxx"
#include "persalys/CalibrationAnalysis.hxx"
#include "persalys/InferenceAnalysis.hxx"
#include "persalys/CopulaInferenceAnalysis.hxx"
#include "persalys/DataAnalysis.hxx"
#include "persalys/FieldModelEvaluation.hxx"
#include "persalys/FieldMonteCarloAnalysis.hxx"
#include "persalys/MonteCarloAnalysis.hxx"
#include "persalys/SobolAnalysis.hxx"
#include "persalys/MonteCarloReliabilityAnalysis.hxx"
#include "persalys/OptimizationAnalysis.hxx"
#ifdef PERSALYS_HAVE_OTMORRIS
#include "persalys/MorrisAnalysis.hxx"
#endif

#include "persalys/StudyItem.hxx"

#include <QDebug>

using namespace OT;

namespace PERSALYS
{

ItemFactory::ItemFactory()
  : QObject()
  , parentStudyItem_(0)
{
}


ItemFactory::~ItemFactory()
{
  parentStudyItem_ = 0;
}


QString ItemFactory::getParentTitleType(const QString &objectName) const
{
  // models
  if (objectName.contains("Model") && !objectName.contains("ModelEvaluation"))
  {
    if (objectName == "MetaModel")
      return "MetaModelsTitle";
    else if (objectName == "DataModel")
      return "DataModelsTitle";
    else
      return "PhysicalModelsTitle";
  }
  // analyses
  else if (objectName.contains("ModelEvaluation"))
    return "ModelEvaluationTitle";
#ifdef PERSALYS_HAVE_OTMORRIS
  else if (objectName == "MorrisAnalysis")
    return "ScreeningTitle";
#endif
  else if (objectName == "OptimizationAnalysis")
    return "OptimizationTitle";
  else if (objectName.contains("DesignOfExperiment"))
    return "DesignsOfExperimentTitle";
  else if (objectName == "MonteCarloAnalysis" ||
           objectName == "TaylorExpansionMomentsAnalysis" ||
           objectName == "FieldMonteCarloAnalysis")
    return "CentralTendencyTitle";
  else if (objectName == "SobolAnalysis" ||
           objectName == "SRCAnalysis")
    return "SensitivityTitle";
  else if (objectName == "Reliability")
    return "ReliabilityTitle";
  else if (objectName == "Calibration")
    return "CalibrationTitle";
  return "";
}


Item * ItemFactory::getTitleItem(const QString &objectName)
{
  Item * item = 0;

  // models
  if (objectName.contains("Model") && !objectName.contains("ModelEvaluation"))
  {
    if (objectName == "MetaModel")
    {
      item = new Item(tr("Metamodels"), "MetaModelsTitle");
      item->setData(QIcon(":/images/metaModel.png"), Qt::DecorationRole);
    }
    else if (objectName == "DataModel")
    {
      item = new Item(tr("Data models"), "DataModelsTitle");
      item->setData(QIcon(":/images/dataModel.png"), Qt::DecorationRole);
      item->appendAction(newDataModel_);
    }
    else
    {
      item = new Item(tr("Physical models"), "PhysicalModelsTitle");
      item->setData(QIcon(":/images/physicalModel.png"), Qt::DecorationRole);
      item->appendAction(newSymbolicModel_);
      item->appendAction(newPythonModel_);
      item->appendAction(newCouplingModel_);
#ifdef PERSALYS_HAVE_YACS
      item->appendAction(newYACSModel_);
#endif
#ifdef PERSALYS_HAVE_OTFMI
      item->appendAction(newFMIModel_);
#endif
    }
  }
  // analyses
  else if (objectName.contains("ModelEvaluation"))
  {
    item = new Item(tr("Evaluation"), "ModelEvaluationTitle");
    item->setData(QIcon(":/images/modelEvaluation.png"), Qt::DecorationRole);
    item->appendAction(newModelEvaluation_);
  }
#ifdef PERSALYS_HAVE_OTMORRIS
  else if (objectName == "MorrisAnalysis")
  {
    item = new Item(tr("Screening"), "ScreeningTitle");
    item->setData(QIcon(":/images/sensitivity.png"), Qt::DecorationRole);
    item->appendAction(newScreening_);
  }
#endif
  else if (objectName == "OptimizationAnalysis")
  {
    item = new Item(tr("Optimization"), "OptimizationTitle");
    item->setData(QIcon(":/images/optimize.png"), Qt::DecorationRole);
    item->appendAction(newOptimization_);
  }
  else if (objectName.contains("DesignOfExperiment"))
  {
    item = new Item(tr("Designs of experiments"), "DesignsOfExperimentTitle");
    item->setData(QIcon(":/images/designOfExperiment.png"), Qt::DecorationRole);
    item->appendAction(newDesignOfExperiment_);
  }
  else if (objectName == "MonteCarloAnalysis" ||
           objectName == "TaylorExpansionMomentsAnalysis" ||
           objectName == "FieldMonteCarloAnalysis")
  {
    item = new Item(tr("Central tendency"), "CentralTendencyTitle");
    item->setData(QIcon(":/images/centralTendency.png"), Qt::DecorationRole);
    item->appendAction(newCentralTendency_);
  }
  else if (objectName == "SobolAnalysis" ||
           objectName == "SRCAnalysis")
  {
    item = new Item(tr("Sensitivity"), "SensitivityTitle");
    item->setData(QIcon(":/images/sensitivity.png"), Qt::DecorationRole);
    item->appendAction(newSensitivityAnalysis_);
  }
  else if (objectName == "Reliability")
  {
    item = new Item(tr("Reliability"), "ReliabilityTitle");
    item->setData(QIcon(":/images/limitstate.png"), Qt::DecorationRole);
    item->appendAction(newLimitState_);
  }
  else if (objectName == "Calibration")
  {
    item = new Item(tr("Calibration"), "CalibrationTitle");
    item->setData(QIcon(":/images/calibration.png"), Qt::DecorationRole);
    item->appendAction(newObservations_);
  }

  return item;
}


Analysis ItemFactory::createAnalysis(const QString &analysisName, const DesignOfExperiment &doe)
{
  if (analysisName == "Metamodel")
    return new FunctionalChaosAnalysis(availableAnalysisName(tr("metaModel_")), doe);
  else if (analysisName == "Calibration")
    return new CalibrationAnalysis(availableAnalysisName(tr("calibration_")), doe);
  qDebug() << "Error: In createAnalysis: analysisName " << analysisName << " not recognized.\n";
  return 0;
}


Analysis ItemFactory::createAnalysis(const QString &analysisName, const Analysis &inAnalysis)
{
  if (analysisName == "Metamodel")
    return new FunctionalChaosAnalysis(availableAnalysisName(tr("metaModel_")), inAnalysis);
  qDebug() << "Error: In createAnalysis: analysisName " << analysisName << " not recognized.\n";
  return 0;
}


Analysis ItemFactory::createAnalysis(const QString &analysisName, const LimitState &limitState)
{
  if (analysisName == "ThresholdExceedance")
    return new MonteCarloReliabilityAnalysis(availableAnalysisName(tr("reliability_")), limitState);
  qDebug() << "Error: In createAnalysis: analysisName " << analysisName << " not recognized.\n";
  return 0;
}


QAction * ItemFactory::createAction(const QString &analysisName, const PhysicalModel &model)
{
  QAction * action = 0;
  if (analysisName == "ModelEvaluation")
  {
    action = new QAction(QIcon(":/images/modelEvaluation.png"), tr("Evaluation"), this);
    action->setStatusTip(tr("Create a new model evaluation"));
    if (!model.hasMesh())
      connect(action, &QAction::triggered, [=]() {
      emit wizardRequested(getParentStudyItem(),
                           ModelEvaluation(availableAnalysisName(tr("evaluation_")), model));});
    else
      connect(action, &QAction::triggered, [=]() {
      emit wizardRequested(getParentStudyItem(),
                           FieldModelEvaluation(availableAnalysisName(tr("evaluation_")), model));});
  }
  else if (analysisName == "DesignOfExperiment")
  {
    action = new QAction(QIcon(":/images/designOfExperiment.png"), tr("Design of experiments"), this);
    action->setStatusTip(tr("Create a new design of experiments"));
    connect(action, &QAction::triggered, [=]() {
      emit wizardRequested(getParentStudyItem(),
                           GridDesignOfExperiment(availableAnalysisName(tr("design_")), model));});
  }
#ifdef PERSALYS_HAVE_OTMORRIS
  else if (analysisName == "Screening")
  {
    action = new QAction(QIcon(":/images/sensitivity.png"), tr("Screening"), this);
    action->setStatusTip(tr("Create a new screening"));
    connect(action, &QAction::triggered, [=]() {
      emit wizardRequested(getParentStudyItem(),
                           MorrisAnalysis(availableAnalysisName(tr("screening_")), model));});
  }
#endif
  else if (analysisName == "Optimization")
  {
    action = new QAction(QIcon(":/images/optimize.png"), tr("Optimization"), this);
    action->setStatusTip(tr("Create a new model optimization"));
    connect(action, &QAction::triggered, [=]() {
      emit wizardRequested(getParentStudyItem(),
                           OptimizationAnalysis(availableAnalysisName(tr("optimization_")), model));});
  }
  else if (analysisName == "Sensitivity")
  {
    action = new QAction(QIcon(":/images/sensitivity.png"), tr("Sensitivity"), this);
    action->setStatusTip(tr("Create a new sensitivity analysis"));
    connect(action, &QAction::triggered, [=]() {
      emit wizardRequested(getParentStudyItem(),
                           SobolAnalysis(availableAnalysisName(tr("sensitivity_")), model));});
  }
  else if (analysisName == "CentralTendency")
  {
    action = new QAction(QIcon(":/images/centralTendency.png"), tr("Central tendency"), this);
    action->setStatusTip(tr("Create a new central tendency"));
    if (!model.hasMesh())
      connect(action, &QAction::triggered, [=]() {
      emit wizardRequested(getParentStudyItem(),
                           MonteCarloAnalysis(availableAnalysisName(tr("centralTendency_")), model));});
    else
      connect(action, &QAction::triggered, [=]() {
      emit wizardRequested(getParentStudyItem(),
                           FieldMonteCarloAnalysis(availableAnalysisName(tr("centralTendency_")), model));});
  }
  return action;
}


QAction * ItemFactory::createAction(const QString &analysisName, const DesignOfExperiment &doe)
{
  QAction * action = 0;

  if (analysisName == "DataAnalysis")
  {
    action = new QAction(tr("Data analysis"), this);
    action->setStatusTip(tr("Analyse the data sample"));
    connect(action, &QAction::triggered, [=]() {
      getParentStudyItem()->getStudy().add(DataAnalysis(availableAnalysisName(tr("dataAnalysis_")), doe));});
  }
  else if (analysisName == "Metamodel")
  {
    action = new QAction(tr("Metamodel"), this);
    action->setStatusTip(tr("Create a new metamodel"));
    connect(action, &QAction::triggered, [=]() {newAnalysis("Metamodel", doe);});
  }
  else if (analysisName == "Inference")
  {
    action = new QAction(tr("Marginals inference"), this);
    action->setStatusTip(tr("Marginals inference"));
    connect(action, &QAction::triggered, [=]() {
      emit wizardRequested(getParentStudyItem(),
                             InferenceAnalysis(availableAnalysisName(tr("marginalsInference_")), doe));});
  }
  else if (analysisName == "CopulaInference")
  {
    action = new QAction(tr("Dependence inference"), this);
    action->setStatusTip(tr("Dependence inference"));
    connect(action, &QAction::triggered, [=]() {
      emit wizardRequested(getParentStudyItem(),
                             CopulaInferenceAnalysis(availableAnalysisName(tr("dependenceInference_")), doe));});
  }
  else if (analysisName == "Calibration")
  {
    action = new QAction(QIcon(":/images/calibration.png"), tr("Calibration"), this);
    action->setStatusTip(tr("Create a new calibration"));
    connect(action, &QAction::triggered, [=]() {newAnalysis("Calibration", doe);});
  }
  else
  {
    qDebug() << "Error: In createAction: analysisName " << analysisName << " not recognized.\n";
  }
  return action;
}


QAction * ItemFactory::createAction(const QString &analysisName, const LimitState &limitState)
{
  if (analysisName == "ThresholdExceedance")
  {
    QAction * action = new QAction(tr("Threshold exceedance"), this);
    action->setStatusTip(tr("Create a new threshold exceedance"));
    connect(action, &QAction::triggered, [=]() {newAnalysis("ThresholdExceedance", limitState);});
    return action;
  }
  qDebug() << "Error: In createAction: analysisName " << analysisName << " not recognized.\n";
  return 0;
}


QAction * ItemFactory::createAction(const QString &analysisName, const Analysis &analysis)
{
  if (analysisName == "Metamodel")
  {
    QAction * action = new QAction(tr("Metamodel"), this);
    action->setStatusTip(tr("Create a new metamodel"));
    connect(action, &QAction::triggered, [=]() {newAnalysis("Metamodel", analysis);});
    return action;
  }
  qDebug() << "Error: In createAction: analysisName " << analysisName << " not recognized.\n";
  return 0;
}


String ItemFactory::availableAnalysisName(const QString &baseName)
{
  return parentStudyItem_->getStudy().getAvailableAnalysisName(baseName.toStdString());
}
}
