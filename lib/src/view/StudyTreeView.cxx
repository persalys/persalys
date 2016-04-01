//                                               -*- C++ -*-
/**
 *  @brief QTreeView to list studies
 *
 *  Copyright 2015-2016 EDF-Phimeca
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
#include "otgui/StudyTreeView.hxx"

#include <QMenu>
#include <QHeaderView>
#include <QMdiSubWindow>
#include <QFileDialog>
#include <QFileInfo>
#include <QApplication>
#include <QMessageBox>
#include <QSettings>

#include "otgui/AnalyticalPhysicalModel.hxx"
#include "otgui/PythonPhysicalModel.hxx"
#ifdef OTGUI_HAVE_YACS
#include "otgui/YACSPhysicalModel.hxx"
#include "otgui/YACSPhysicalModelWindow.hxx"
#endif
#include "otgui/AnalyticalPhysicalModelWindow.hxx"
#include "otgui/PythonPhysicalModelWindow.hxx"
#include "otgui/ProbabilisticModelWindow.hxx"
#include "otgui/LimitStateWindow.hxx"
#include "otgui/DesignOfExperimentWizard.hxx"
#include "otgui/DesignOfExperimentWindow.hxx"
#include "otgui/AnalysisExecutionFailedWindow.hxx"
#include "otgui/ModelEvaluationWizard.hxx"
#include "otgui/ModelEvaluationResultWindow.hxx"
#include "otgui/CentralTendencyWizard.hxx"
#include "otgui/MonteCarloResultWindow.hxx"
#include "otgui/TaylorExpansionMomentsResultWindow.hxx"
#include "otgui/SensitivityAnalysisWizard.hxx"
#include "otgui/SobolResultWindow.hxx"
#include "otgui/SRCResultWindow.hxx"
#include "otgui/ReliabilityAnalysisWizard.hxx"
#include "otgui/MonteCarloReliabilityResultWindow.hxx"
#include "otgui/LineEditWithQValidatorDelegate.hxx"

#include <iostream>

using namespace OT;

namespace OTGUI {

StudyTreeView::StudyTreeView(QWidget * parent)
  : QTreeView(parent)
  , treeViewModel_(new StudyTreeViewModel())
{
  OTStudy::SetInstanceObserver(treeViewModel_);
  setModel(treeViewModel_);
  connect(treeViewModel_, SIGNAL(newPhysicalModelCreated(PhysicalModelItem*)), this, SLOT(createNewPhysicalModelWindow(PhysicalModelItem*)));
  connect(treeViewModel_, SIGNAL(newProbabilisticModelCreated(ProbabilisticModelItem*)), this, SLOT(createNewProbabilisticModelWindow(ProbabilisticModelItem*)));
  connect(treeViewModel_, SIGNAL(newDesignOfExperimentCreated(DesignOfExperimentItem*)), this, SLOT(createNewDesignOfExperimentWindow(DesignOfExperimentItem*)));
  connect(treeViewModel_, SIGNAL(newLimitStateCreated(LimitStateItem*)), this, SLOT(createNewLimitStateWindow(LimitStateItem*)));
  connect(treeViewModel_, SIGNAL(newAnalysisCreated(AnalysisItem*)), this, SLOT(createAnalysisConnection(AnalysisItem*)));
  connect(treeViewModel_, SIGNAL(itemRemoved(QStandardItem*)), this, SIGNAL(removeSubWindow(QStandardItem*)));

  buildActions();

  // Forbid the user to define not valid item's name
  LineEditWithQValidatorDelegate * delegate = new LineEditWithQValidatorDelegate;
  setItemDelegateForColumn(0, delegate);

  setContextMenuPolicy(Qt::CustomContextMenu);
  connect(this, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(onCustomContextMenu(const QPoint &)));

  header()->hide();

  connect(this, SIGNAL(clicked(const QModelIndex &)), this, SLOT(selectedItemChanged(const QModelIndex &)));
}


void StudyTreeView::onCustomContextMenu(const QPoint &point)
{
  QModelIndex index = indexAt(point);

  if (!index.isValid())
    return;

  QMenu * contextMenu = new QMenu(this);
  QString dataType = treeViewModel_->itemFromIndex(index)->data(Qt::UserRole).toString();
  if (dataType == "ProbabilisticStudy" && treeViewModel_->itemFromIndex(index)->hasChildren())
    return;
  contextMenu->addActions(getActions(dataType));
  contextMenu->exec(mapToGlobal(point));
}


void StudyTreeView::buildActions()
{
  // close otstudy action
  closeOTStudy_ = new QAction(QIcon(":/images/window-close.png"), tr("Close"), this);
  closeOTStudy_->setStatusTip(tr("Close the OTStudy"));
  connect(closeOTStudy_, SIGNAL(triggered()), this, SLOT(closeOTStudy()));
  
  // new physical model actions
  newAnalyticalPhysicalModel_ = new QAction(tr("New analytical physical model"), this);
  newAnalyticalPhysicalModel_->setStatusTip(tr("Create a new analytical physical model"));
  connect(newAnalyticalPhysicalModel_, SIGNAL(triggered()), this, SLOT(createNewAnalyticalPhysicalModel()));

  newPythonPhysicalModel_ = new QAction(tr("New Python physical model"), this);
  newPythonPhysicalModel_->setStatusTip(tr("Create a new Python physical model"));
  connect(newPythonPhysicalModel_, SIGNAL(triggered()), this, SLOT(createNewPythonPhysicalModel()));

#ifdef OTGUI_HAVE_YACS
  newYACSPhysicalModel_ = new QAction(tr("New YACS physical model"), this);
  newYACSPhysicalModel_->setStatusTip(tr("Create a new YACS physical model"));
  connect(newYACSPhysicalModel_, SIGNAL(triggered()), this, SLOT(createNewYACSPhysicalModel()));
#endif

  // remove physical model action
  removePhysicalModel_ = new QAction(QIcon(":/images/window-close.png"), tr("Remove"), this);
  removePhysicalModel_->setStatusTip(tr("Remove the physical model"));
  connect(removePhysicalModel_, SIGNAL(triggered()), this, SLOT(removePhysicalModel()));

  // new probabilistic model action
  newProbabilisticModel_ = new QAction(tr("New probabilistic model"), this);
  newProbabilisticModel_->setStatusTip(tr("Create a new probabilistic model"));
  connect(newProbabilisticModel_, SIGNAL(triggered()), this, SLOT(createNewProbabilisticModel()));

  // new design of experiment action
  newDesignOfExperiment_ = new QAction(tr("New design of experiment"), this);
  newDesignOfExperiment_->setStatusTip(tr("Create a new design of experiment"));
  connect(newDesignOfExperiment_, SIGNAL(triggered()), this, SLOT(createNewDesignOfExperiment()));

  // new limit state action
  newLimitState_ = new QAction(tr("New limit state"), this);
  newLimitState_->setStatusTip(tr("Create a new limit state"));
  connect(newLimitState_, SIGNAL(triggered()), this, SLOT(createNewLimitState()));

  // remove limit state action
  removeLimitState_ = new QAction(QIcon(":/images/window-close.png"), tr("Remove"), this);
  removeLimitState_->setStatusTip(tr("Remove the limit state"));
  connect(removeLimitState_, SIGNAL(triggered()), this, SLOT(removeLimitState()));

  // new analysis actions
  newModelEvaluation_ = new QAction(tr("New model evaluation"), this);
  newModelEvaluation_->setStatusTip(tr("Create a new model evaluation"));
  connect(newModelEvaluation_, SIGNAL(triggered()), this, SLOT(createNewModelEvaluation()));

  newCentralTendency_ = new QAction(tr("New central tendency"), this);
  newCentralTendency_->setStatusTip(tr("Create a new central tendency"));
  connect(newCentralTendency_, SIGNAL(triggered()), this, SLOT(createNewCentralTendency()));

  newSensitivityAnalysis_ = new QAction(tr("New sensitivity analysis"), this);
  newSensitivityAnalysis_->setStatusTip(tr("Create a new sensitivity analysis"));
  connect(newSensitivityAnalysis_, SIGNAL(triggered()), this, SLOT(createNewSensitivityAnalysis()));

  newThresholdExceedance_ = new QAction(tr("New threshold exceedance"), this);
  newThresholdExceedance_->setStatusTip(tr("Create a new threshold exceedance"));
  connect(newThresholdExceedance_, SIGNAL(triggered()), this, SLOT(createNewThresholdExceedance()));

  // run design of experiment action
  runDesignOfExperiment_ = new QAction(QIcon(":/images/run-build.png"), tr("Run"), this);
  runDesignOfExperiment_->setStatusTip(tr("Run the design of experiment"));
  connect(runDesignOfExperiment_, SIGNAL(triggered()), this, SLOT(runDesignOfExperiment()));

  // remove design of experiment action
  removeDesignOfExperiment_ = new QAction(QIcon(":/images/window-close.png"), tr("Remove"), this);
  removeDesignOfExperiment_->setStatusTip(tr("Remove the design of experiment"));
  connect(removeDesignOfExperiment_, SIGNAL(triggered()), this, SLOT(removeDesignOfExperiment()));

  // run analysis action
  runAnalysis_ = new QAction(QIcon(":/images/run-build.png"), tr("Run"), this);
  runAnalysis_->setStatusTip(tr("Run the analysis"));
  connect(runAnalysis_, SIGNAL(triggered()), this, SLOT(runAnalysis()));

  // remove analysis action
  removeAnalysis_ = new QAction(QIcon(":/images/window-close.png"), tr("Remove"), this);
  removeAnalysis_->setStatusTip(tr("Remove the model evaluation"));
  connect(removeAnalysis_, SIGNAL(triggered()), this, SLOT(removeAnalysis()));

  // save the current OTStudy action
  saveOTStudy_ = new QAction(QIcon(":/images/document-save.png"), tr("Save"), this);
  saveOTStudy_->setStatusTip(tr("Save the OTStudy"));
  connect(saveOTStudy_, SIGNAL(triggered()), this, SLOT(saveOTStudy()));
}


QList<QAction* > StudyTreeView::getActions(const QString & dataType)
{
  QList<QAction* > actions;

  if (dataType == "OTStudy")
  {
    actions.append(newAnalyticalPhysicalModel_);
    actions.append(newPythonPhysicalModel_);
#ifdef OTGUI_HAVE_YACS
    actions.append(newYACSPhysicalModel_);
#endif
    actions.append(saveOTStudy_);
    actions.append(closeOTStudy_);
  }
  else if (dataType == "PhysicalModel")
  {
    actions.append(removePhysicalModel_);
  }
  else if (dataType == "DeterministicStudy")
  {
    actions.append(newModelEvaluation_);
  }
  else if (dataType == "ProbabilisticStudy")
  {
    actions.append(newProbabilisticModel_);
  }
  else if (dataType == "ProbabilisticModel")
  {
    actions.append(newLimitState_);
    actions.append(newCentralTendency_);
    actions.append(newSensitivityAnalysis_);
  }
  else if (dataType == "DesignOfExperimentList")
  {
    actions.append(newDesignOfExperiment_);
  }
  else if (dataType == "DesignOfExperiment")
  {
    actions.append(runDesignOfExperiment_);
    actions.append(removeDesignOfExperiment_);
  }
  else if (dataType == "LimitState")
  {
    actions.append(newThresholdExceedance_);
    actions.append(removeLimitState_);
  }
  else if (dataType == "ModelEvaluation" || dataType == "MonteCarloAnalysis" ||
           dataType == "TaylorExpansionMomentsAnalysis" || dataType == "SobolAnalysis" ||
           dataType == "SRCAnalysis" || dataType == "MonteCarloReliabilityAnalysis")
  {
    actions.append(runAnalysis_);
    actions.append(removeAnalysis_);
  }
  return actions;
}


bool StudyTreeView::isPhysicalModelValid(const QModelIndex & currentIndex)
{
  PhysicalModelItem * physicalModelItem = treeViewModel_->getPhysicalModelItem(currentIndex);
  if (!physicalModelItem)
    return false;
  if (!physicalModelItem->getPhysicalModel().isValid())
  {
    QMessageBox::critical(this, tr("Error"), tr("The physical model has no inputs and/or no outputs."));
    return false;
  }
  return true;
}


bool StudyTreeView::hasPhysicalModelInputs(const QModelIndex & currentIndex)
{
  PhysicalModelItem * physicalModelItem = treeViewModel_->getPhysicalModelItem(currentIndex);
  if (!physicalModelItem)
    return false;
  if (!physicalModelItem->getPhysicalModel().getInputs().getSize())
  {
    QMessageBox::critical(this, tr("Error"), tr("The physical model has no inputs."));
    return false;
  }
  return true;
}


bool StudyTreeView::isProbabilisticModelValid(const QModelIndex & currentIndex)
{
  PhysicalModelItem * physicalModelItem = treeViewModel_->getPhysicalModelItem(currentIndex);
  if (!physicalModelItem)
    return false;
  if (!physicalModelItem->getPhysicalModel().hasStochasticInputs())
  {
    QMessageBox::critical(this, tr("Error"), tr("The physical model has no stochastic inputs."));
    return false;
  }
  return true;
}


bool StudyTreeView::isLimitStateValid(const QModelIndex & currentIndex)
{
  LimitStateItem * limitStateItem = treeViewModel_->getLimitStateItem(currentIndex);
  if (!limitStateItem)
    return false;
  if (!limitStateItem->getLimitState().isValid())
  {
    QMessageBox::critical(this, tr("Error"), tr("The limit state is not valid."));
    return false;
  }
  return true;
}


void StudyTreeView::createNewOTStudy()
{
  treeViewModel_->createNewOTStudy();
}


void StudyTreeView::createNewAnalyticalPhysicalModel()
{
  QModelIndex studyIndex = selectionModel()->currentIndex();
  OTStudyItem * studyItem = static_cast<OTStudyItem*>(treeViewModel_->itemFromIndex(studyIndex));
  AnalyticalPhysicalModel newPhysicalModel(studyItem->getOTStudy()->getAvailablePhysicalModelName());
  studyItem->getOTStudy()->add(newPhysicalModel);
}


void StudyTreeView::createNewPythonPhysicalModel()
{
  QModelIndex studyIndex = selectionModel()->currentIndex();
  OTStudyItem * studyItem = static_cast<OTStudyItem*>(treeViewModel_->itemFromIndex(studyIndex));
  PythonPhysicalModel newPhysicalModel(studyItem->getOTStudy()->getAvailablePhysicalModelName());
  studyItem->getOTStudy()->add(newPhysicalModel);
}


#ifdef OTGUI_HAVE_YACS
void StudyTreeView::createNewYACSPhysicalModel()
{
  QModelIndex studyIndex = selectionModel()->currentIndex();
  OTStudyItem * studyItem = static_cast<OTStudyItem*>(treeViewModel_->itemFromIndex(studyIndex));
  YACSPhysicalModel newPhysicalModel(studyItem->getOTStudy()->getAvailablePhysicalModelName());
  studyItem->getOTStudy()->add(newPhysicalModel);
}
#endif


void StudyTreeView::removePhysicalModel()
{
  QModelIndex index = selectionModel()->currentIndex();
  QStandardItem * selectedItem = treeViewModel_->itemFromIndex(index);

  if (selectedItem->child(1)->rowCount())
    emit removeSubWindow(selectedItem->child(1)->child(0));
  treeViewModel_->getOTStudyItem(index)->getOTStudy()->remove(dynamic_cast<PhysicalModelItem*>(selectedItem)->getPhysicalModel());
}


void StudyTreeView::createNewProbabilisticModel()
{
  QModelIndex probabilisticStudyIndex = selectionModel()->currentIndex();
  if (!hasPhysicalModelInputs(probabilisticStudyIndex))
    return;
  treeViewModel_->addProbabilisticModelItem(probabilisticStudyIndex);
  setExpanded(probabilisticStudyIndex, true);
}


void StudyTreeView::createNewDesignOfExperiment()
{
  QModelIndex DesignOfExperimentListIndex = selectionModel()->currentIndex();
  PhysicalModelItem * physicalModelItem = treeViewModel_->getPhysicalModelItem(DesignOfExperimentListIndex);
  if (!hasPhysicalModelInputs(DesignOfExperimentListIndex))
    return;
  OTStudyItem * otStudyItem = dynamic_cast<OTStudyItem*>(physicalModelItem->QStandardItem::parent());
  DesignOfExperimentWizard * wizard = new DesignOfExperimentWizard(otStudyItem->getOTStudy(), physicalModelItem->getPhysicalModel());

  if (wizard->exec())
  {
    wizard->validate();
  }
}


void StudyTreeView::createNewLimitState()
{
  QModelIndex probabilisticStudyIndex = selectionModel()->currentIndex().parent();
  if (!isPhysicalModelValid(probabilisticStudyIndex) || !isProbabilisticModelValid(probabilisticStudyIndex))
    return;
  treeViewModel_->addLimitStateItem(probabilisticStudyIndex);
  setExpanded(probabilisticStudyIndex, true);
}


void StudyTreeView::removeLimitState()
{
  QModelIndex index = selectionModel()->currentIndex();
  QStandardItem * selectedItem = treeViewModel_->itemFromIndex(index);

  treeViewModel_->getOTStudyItem(index)->getOTStudy()->remove(dynamic_cast<LimitStateItem*>(selectedItem)->getLimitState());
}


void StudyTreeView::createNewPhysicalModelWindow(PhysicalModelItem * item)
{
  OTguiSubWindow * window = 0;
  String physicalModelType = item->getPhysicalModel().getImplementation()->getClassName();
  if (physicalModelType == "AnalyticalPhysicalModel")
    window = new AnalyticalPhysicalModelWindow(item);
  else if (physicalModelType == "PythonPhysicalModel")
    window = new PythonPhysicalModelWindow(item);
#ifdef OTGUI_HAVE_YACS
  else if (physicalModelType == "YACSPhysicalModel")
    window = new YACSPhysicalModelWindow(item);
#endif
  if (window)
  {
    connect(window, SIGNAL(errorMessageChanged(QString)), this, SIGNAL(errorMessageEmitted(QString)));
    emit showWindow(window);
    setCurrentIndex(item->index());
    setExpanded(item->index(), true);
  }
}


void StudyTreeView::createNewProbabilisticModelWindow(ProbabilisticModelItem * item)
{
  ProbabilisticModelWindow * window = new ProbabilisticModelWindow(item);
  connect(window, SIGNAL(errorMessageChanged(QString)), this, SIGNAL(errorMessageEmitted(QString)));
  connect(window, SIGNAL(graphWindowActivated(QWidget*)), this, SIGNAL(graphWindowActivated(QWidget*)));
  connect(window, SIGNAL(graphWindowDeactivated(QWidget*)), this, SIGNAL(graphWindowDeactivated(QWidget*)));
  emit showWindow(window);
  setCurrentIndex(item->index());
}


void StudyTreeView::createNewLimitStateWindow(LimitStateItem* item)
{
  LimitStateWindow * window = new LimitStateWindow(item);
  connect(window, SIGNAL(errorMessageChanged(QString)), this, SIGNAL(errorMessageEmitted(QString)));
  emit showWindow(window);
  setCurrentIndex(item->index());
}


void StudyTreeView::createNewDesignOfExperimentWindow(DesignOfExperimentItem * item)
{
  DesignOfExperimentWindow * window = new DesignOfExperimentWindow(item);
  connect(window, SIGNAL(errorMessageChanged(QString)), this, SIGNAL(errorMessageEmitted(QString)));
  connect(window, SIGNAL(graphWindowActivated(QWidget*)), this, SIGNAL(graphWindowActivated(QWidget*)));
  connect(window, SIGNAL(graphWindowDeactivated(QWidget*)), this, SIGNAL(graphWindowDeactivated(QWidget*)));
  connect(item, SIGNAL(analysisFinished()), window, SLOT(updateWindowForOutputs()));
  emit showWindow(window);
  setExpanded(item->index().parent(), true);
  setCurrentIndex(item->index());
}


void StudyTreeView::createNewModelEvaluation()
{
  QModelIndex deterministicStudyIndex = selectionModel()->currentIndex();
  if (!isPhysicalModelValid(deterministicStudyIndex))
    return;
  PhysicalModelItem * physicalModelItem = treeViewModel_->getPhysicalModelItem(deterministicStudyIndex);
  OTStudyItem * otStudyItem = dynamic_cast<OTStudyItem*>(physicalModelItem->QStandardItem::parent());
  ModelEvaluationWizard * wizard = new ModelEvaluationWizard(otStudyItem->getOTStudy(), physicalModelItem->getPhysicalModel());

  if (wizard->exec())
  {
    wizard->validate();
    findAnalysisItemAndLaunchExecution(otStudyItem, wizard->getAnalysisName());
  }
}


void StudyTreeView::createNewCentralTendency()
{
  QModelIndex probabilisticStudyIndex = selectionModel()->currentIndex().parent();
  if (!isPhysicalModelValid(probabilisticStudyIndex) || !isProbabilisticModelValid(probabilisticStudyIndex))
    return;
  PhysicalModelItem * physicalModelItem = treeViewModel_->getPhysicalModelItem(probabilisticStudyIndex);
  OTStudyItem * otStudyItem = dynamic_cast<OTStudyItem*>(physicalModelItem->QStandardItem::parent());
  CentralTendencyWizard * wizard = new CentralTendencyWizard(otStudyItem->getOTStudy(), physicalModelItem->getPhysicalModel());

  if (wizard->exec())
  {
    wizard->validate();
    findAnalysisItemAndLaunchExecution(otStudyItem, wizard->getAnalysisName());
  }
}


void StudyTreeView::createNewSensitivityAnalysis()
{
  QModelIndex probabilisticStudyIndex = selectionModel()->currentIndex().parent();
  if (!isPhysicalModelValid(probabilisticStudyIndex) || !isProbabilisticModelValid(probabilisticStudyIndex))
    return;
  PhysicalModelItem * physicalModelItem = treeViewModel_->getPhysicalModelItem(probabilisticStudyIndex);
  OTStudyItem * otStudyItem = dynamic_cast<OTStudyItem*>(physicalModelItem->QStandardItem::parent());
  SensitivityAnalysisWizard * wizard = new SensitivityAnalysisWizard(otStudyItem->getOTStudy(), physicalModelItem->getPhysicalModel());

  if (wizard->exec())
  {
    wizard->validate();
    findAnalysisItemAndLaunchExecution(otStudyItem, wizard->getAnalysisName());
  }
}


void StudyTreeView::createNewThresholdExceedance()
{
  QModelIndex limitStateIndex = selectionModel()->currentIndex();
  if (!isPhysicalModelValid(limitStateIndex) || !isProbabilisticModelValid(limitStateIndex))
    return;
  if (!isLimitStateValid(limitStateIndex))
    return;
  LimitStateItem * limitStateItem = dynamic_cast<LimitStateItem*>(treeViewModel_->itemFromIndex(limitStateIndex));
  OTStudyItem * otStudyItem = treeViewModel_->getOTStudyItem(limitStateIndex);
  ReliabilityAnalysisWizard * wizard = new ReliabilityAnalysisWizard(otStudyItem->getOTStudy(), limitStateItem->getLimitState());

  if (wizard->exec())
  {
    wizard->validate();
    findAnalysisItemAndLaunchExecution(otStudyItem, wizard->getAnalysisName());
  }
}


void StudyTreeView::createAnalysisConnection(AnalysisItem * item)
{
  connect(item, SIGNAL(analysisFinished(AnalysisItem *)), this, SLOT(createAnalysisResultWindow(AnalysisItem*)));
}


void StudyTreeView::runDesignOfExperiment()
{
  QModelIndex index = selectionModel()->currentIndex();
  QStandardItem * selectedItem = treeViewModel_->itemFromIndex(index);

  DesignOfExperimentItem * item = dynamic_cast<DesignOfExperimentItem*>(selectedItem);

  DesignOfExperimentWizard * wizard = new DesignOfExperimentWizard(item->getDesignOfExperiment());
  if (wizard->exec())
  {
    emit removeSubWindow(item);
    item->getDesignOfExperiment().clearResult();
    createNewDesignOfExperimentWindow(item);
  }
}


void StudyTreeView::removeDesignOfExperiment()
{
  QModelIndex index = selectionModel()->currentIndex();
  QStandardItem * selectedItem = treeViewModel_->itemFromIndex(index);

  treeViewModel_->getOTStudyItem(index)->getOTStudy()->remove(dynamic_cast<DesignOfExperimentItem*>(selectedItem)->getDesignOfExperiment());
}


void StudyTreeView::findAnalysisItemAndLaunchExecution(OTStudyItem * otStudyItem, const QString & analysisName)
{
  AnalysisItem * analysisItem = treeViewModel_->getAnalysisItem(otStudyItem, analysisName);
  if (analysisItem)
  {
    try
    {
      analysisItem->getAnalysis().run();
    }
    catch (std::exception & ex)
    {
      createAnalysisExecutionFailedWindow(analysisItem, ex.what());
      setExpanded(analysisItem->index(), true);
    }
  }
}


void StudyTreeView::runAnalysis()
{
  QStandardItem * selectedItem = treeViewModel_->itemFromIndex(selectionModel()->currentIndex());
  AnalysisItem * item = dynamic_cast<AnalysisItem*>(selectedItem);

  OTguiWizard * wizard = 0;
  QString analysisType = item->data(Qt::UserRole).toString();
  if (!isPhysicalModelValid(selectionModel()->currentIndex()))
    return;
  if (analysisType == "ModelEvaluation")
  {
    wizard = new ModelEvaluationWizard(item->getAnalysis());
  }
  else
  {
    if (!isProbabilisticModelValid(selectionModel()->currentIndex()))
      return;

    if (analysisType == "MonteCarloAnalysis" || analysisType == "TaylorExpansionMomentsAnalysis")
    {
      wizard = new CentralTendencyWizard(item->getAnalysis());
    }
    else if (analysisType == "SobolAnalysis" || analysisType == "SRCAnalysis")
    {
      wizard = new SensitivityAnalysisWizard(item->getAnalysis());
    }
    else if (analysisType == "MonteCarloReliabilityAnalysis")
    {
      if (!isLimitStateValid(selectionModel()->currentIndex()))
        return;
      wizard = new ReliabilityAnalysisWizard(item->getAnalysis());
    }
    else
    {
      throw InvalidArgumentException(HERE) << "analysisType " << analysisType.toStdString() << " not recognized.";
    }
  }

  if (wizard)
  {
    connect(wizard, SIGNAL(analysisChanged(Analysis)), item, SLOT(updateAnalysis(Analysis)));
    if (wizard->exec())
    {
      try
      {
        item->getAnalysis().run();
      }
      catch (std::exception & ex)
      {
        createAnalysisExecutionFailedWindow(item, ex.what());
      }
    }
  }
}


void StudyTreeView::removeAnalysis()
{
  QModelIndex index = selectionModel()->currentIndex();
  QStandardItem * selectedItem = treeViewModel_->itemFromIndex(index);

  treeViewModel_->getOTStudyItem(index)->getOTStudy()->remove(dynamic_cast<AnalysisItem*>(selectedItem)->getAnalysis());
}


void StudyTreeView::createAnalysisResultWindow(AnalysisItem* item)
{
  emit removeSubWindow(item);

  OTguiSubWindow * resultWindow = 0;
  QString analysisType = item->data(Qt::UserRole).toString();
  if (analysisType == "ModelEvaluation")
    resultWindow = new ModelEvaluationResultWindow(item);
  else if (analysisType == "MonteCarloAnalysis")
    resultWindow = new MonteCarloResultWindow(item);
  else if (analysisType == "TaylorExpansionMomentsAnalysis")
    resultWindow = new TaylorExpansionMomentsResultWindow(item);
  else if (analysisType == "SobolAnalysis")
    resultWindow = new SobolResultWindow(item);
  else if (analysisType == "SRCAnalysis")
    resultWindow = new SRCResultWindow(item);
  else if (analysisType == "MonteCarloReliabilityAnalysis")
    resultWindow = new MonteCarloReliabilityResultWindow(item);
  else
    throw InvalidArgumentException(HERE) << "analysisType " << analysisType.toStdString() << " not recognized.";

  if (resultWindow)
  {
    connect(resultWindow, SIGNAL(errorMessageChanged(QString)), this, SIGNAL(errorMessageEmitted(QString)));
    connect(resultWindow, SIGNAL(graphWindowActivated(QWidget*)), this, SIGNAL(graphWindowActivated(QWidget*)));
    connect(resultWindow, SIGNAL(graphWindowDeactivated(QWidget*)), this, SIGNAL(graphWindowDeactivated(QWidget*)));
    emit showWindow(resultWindow);
    setCurrentIndex(item->index());
  }
}


void StudyTreeView::createAnalysisExecutionFailedWindow(AnalysisItem * item, const QString & errorMessage)
{
  emit removeSubWindow(item);
  AnalysisExecutionFailedWindow * window = new AnalysisExecutionFailedWindow(item, errorMessage);
  connect(window, SIGNAL(errorMessageChanged(QString)), this, SIGNAL(errorMessageEmitted(QString)));
  emit showWindow(window);
  setCurrentIndex(item->index());
}


void StudyTreeView::exportPython()
{
  OTStudyItem * item = treeViewModel_->getOTStudyItem(selectionModel()->currentIndex());

  QSettings settings;
  QString currentDir = settings.value("currentDir").toString();
  if (currentDir.isEmpty())
    currentDir = QDir::homePath();
  QString defaultFileName = QDir::separator() + QFileInfo(QString::fromUtf8(item->getOTStudy()->getFileName().c_str())).baseName();
  QString fileName = QFileDialog::getSaveFileName(this, tr("Export Python..."),
                     currentDir + defaultFileName,
                     tr("Python source files (*.py)"));

  if (!fileName.isEmpty())
  {
    if (!fileName.endsWith(".py"))
      fileName += ".py";

    QFile file(fileName);
    settings.setValue("currentDir", QFileInfo(fileName).absolutePath());

    // check
    if (!file.open(QFile::WriteOnly))
    {
      std::cout << "cannot open" << file.fileName().toStdString() << std::endl;
      QMessageBox::warning(this, tr("Warning"),
                           tr("Cannot read file %1:\n%2").arg(fileName).arg(file.errorString()));
    }
    // fill
    else
    {
      QTextStream out(&file);
      out.setCodec("UTF-8");
      out << QString::fromUtf8(item->getOTStudy()->getPythonScript().c_str());
      file.setPermissions(QFile::ReadUser|QFile::WriteUser|QFile::ExeUser|QFile::ReadGroup|QFile::ExeGroup|QFile::ReadOther|QFile::ExeOther);
      file.close();
    }
  }
}


void StudyTreeView::importPython()
{
  if (treeViewModel_->invisibleRootItem()->hasChildren())
  {
    int ret = QMessageBox::warning(this, tr("Warning"),
                           tr("Cannot import a Python script when other studies are opened.\nDo you want to continue and close the other studies?"),
                           QMessageBox::Cancel | QMessageBox::Ok,
                           QMessageBox::Ok);
    if (ret == QMessageBox::Ok)
    {
      bool allStudiesClosed = closeAllOTStudies();
      if (!allStudiesClosed)
        return;
    }
    else
      return;
  }

  QSettings settings;
  QString currentDir = settings.value("currentDir").toString();
  if (currentDir.isEmpty())
    currentDir = QDir::homePath();
  QString fileName = QFileDialog::getOpenFileName(this, tr("Import Python..."),
                     currentDir,
                     tr("Python source files (*.py)"));

  if (!fileName.isEmpty())
  {
    QFile file(fileName);
    settings.setValue("currentDir", QFileInfo(fileName).absolutePath());

    // check
    if (!file.open(QFile::ReadOnly))
    {
      QMessageBox::warning(this, tr("Warning"),
                           tr("Cannot read file %1:\n%2").arg(fileName).arg(file.errorString()));
    }
    // load
    else
      emit importPythonScript(fileName);
  }
}


bool StudyTreeView::saveOTStudy()
{
  OTStudyItem * item = treeViewModel_->getOTStudyItem(selectionModel()->currentIndex());

  if (QFileInfo(QString::fromUtf8(item->getOTStudy()->getFileName().c_str())).exists())
  {
    QApplication::setOverrideCursor(Qt::WaitCursor);
    item->getOTStudy()->save(item->getOTStudy()->getFileName());
    QApplication::restoreOverrideCursor();
    return true;
  }
  else
  {
    return saveAsOTStudy();
  }
}


bool StudyTreeView::saveAsOTStudy()
{
  OTStudyItem * item = treeViewModel_->getOTStudyItem(selectionModel()->currentIndex());

  QSettings settings;
  QString currentDir = settings.value("currentDir").toString();
  if (currentDir.isEmpty())
    currentDir = QDir::homePath();
  QString fileName = QFileDialog::getSaveFileName(this, tr("Save OTStudy..."),
                     currentDir,
                     tr("XML files (*.xml)"));

  if (!fileName.isEmpty())
  {
    if (!fileName.endsWith(".xml"))
      fileName += ".xml";

    QFile file(fileName);
    settings.setValue("currentDir", QFileInfo(fileName).absolutePath());

    // check
    if (!file.open(QFile::WriteOnly))
    {
      std::cout << "cannot open" << file.fileName().toStdString() << std::endl;
      QMessageBox::warning(this, tr("Warning"),
                           tr("Cannot save file %1:\n%2").arg(fileName).arg(file.errorString()));
      return false;
    }
    // fill
    else
    {
      QApplication::setOverrideCursor(Qt::WaitCursor);
      item->getOTStudy()->save(fileName.toUtf8().constData());
      QApplication::restoreOverrideCursor();
      emit recentFilesListChanged(fileName);
      return true;
    }
  }
  return false;
}


void StudyTreeView::openOTStudy()
{
  QSettings settings;
  QString currentDir = settings.value("currentDir").toString();
  if (currentDir.isEmpty())
    currentDir = QDir::homePath();
  QString fileName = QFileDialog::getOpenFileName(this, tr("Open an existing OTStudy"),
                     currentDir,
                     tr("XML files (*.xml)"));

  if (!fileName.isEmpty())
  {
    openOTStudy(fileName);
  }
}


void StudyTreeView::openOTStudy(const QString & fileName)
{
  QFileInfo file(fileName);
  QSettings settings;
  settings.setValue("currentDir", file.absolutePath());

  if (file.exists() && !OTStudy::GetFileNames().__contains__(file.absoluteFilePath().toUtf8().constData()))
  {
    try
    {
      QApplication::setOverrideCursor(Qt::WaitCursor);
      OTStudy::Open(fileName.toUtf8().constData());
      QApplication::restoreOverrideCursor();
      emit recentFilesListChanged(fileName);
    }
    catch (std::exception & ex)
    {
      QApplication::restoreOverrideCursor();
      QString message;
      message = tr("Impossible to read the file '%1'. \n").arg(fileName);
      QMessageBox::warning(this, tr("Warning"), message+ex.what());
    }
  }
  else
  {
    // put this in OTStudy::OpenOTStudy
    QString message;
    if (!file.exists())
    {
      message = tr("The file '%1' does not exist.").arg(fileName);
    }
    else if (OTStudy::GetFileNames().__contains__(file.absoluteFilePath().toUtf8().constData()))
    {
      message = tr("The file '%1' is already opened.").arg(fileName);
    }
    QMessageBox::warning(this, tr("Warning"), message);
  }
}


bool StudyTreeView::closeOTStudy()
{
  OTStudyItem * item = treeViewModel_->getOTStudyItem(selectionModel()->currentIndex());
  if (QFileInfo(QString::fromUtf8(item->getOTStudy()->getFileName().c_str())).exists())
  {
    QApplication::setOverrideCursor(Qt::WaitCursor);
    item->getOTStudy()->save(item->getOTStudy()->getFileName());
    QApplication::restoreOverrideCursor();
    OTStudy::Remove(item->getOTStudy());
    return true;
  }
  else
  {
    int ret = QMessageBox::warning(this, tr("Warning"),
                               tr("Do you want to save the OTStudy '%1' [%2]?").arg(item->getOTStudy()->getName().c_str()).arg(item->getOTStudy()->getFileName().c_str()),
                               QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel,
                               QMessageBox::Save);

    if (ret == QMessageBox::Discard)
    {
      OTStudy::Remove(item->getOTStudy());
      return true;
    }
    else if (ret == QMessageBox::Save)
    {
      bool isSaved = saveOTStudy();
      if (isSaved)
      {
        OTStudy::Remove(item->getOTStudy());
        return true;
      }
    }
  }
  return false;
}


bool StudyTreeView::closeAllOTStudies()
{
  QStandardItem * rootItem = treeViewModel_->invisibleRootItem();
  if (!rootItem->hasChildren())
    return true;
  while (rootItem->hasChildren())
  {
    selectionModel()->setCurrentIndex(rootItem->child(0)->index(), QItemSelectionModel::Select);
    int ret = closeOTStudy();
    if (!ret)
      return false;
  }
  return true;
}


void StudyTreeView::selectedItemChanged(const QModelIndex & index)
{
  QStandardItem * selectedItem = treeViewModel_->itemFromIndex(index);
  if (selectedItem->data(Qt::UserRole).toString() != "Study")
    emit itemSelected(selectedItem);
}
}
