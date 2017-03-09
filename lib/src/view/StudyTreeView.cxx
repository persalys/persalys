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

#include "otgui/Controller.hxx"
#include "otgui/ControllerWidget.hxx"
#include "otgui/OTStudyWindow.hxx"
#include "otgui/DataModel.hxx"
#include "otgui/DataAnalysis.hxx"
#include "otgui/DesignOfExperimentEvaluation.hxx"
#include "otgui/CopulaInferenceAnalysis.hxx"
#include "otgui/DataAnalysisResultWindow.hxx"
#include "otgui/InferenceWizard.hxx"
#include "otgui/AnalyticalPhysicalModel.hxx"
#include "otgui/PythonPhysicalModel.hxx"
#ifdef OTGUI_HAVE_YACS
#include "otgui/YACSPhysicalModel.hxx"
#include "otgui/YACSPhysicalModelWindow.hxx"
#endif
#include "otgui/AnalyticalPhysicalModelWindow.hxx"
#include "otgui/PythonPhysicalModelWindow.hxx"
#include "otgui/MetaModelWindow.hxx"
#include "otgui/ProbabilisticModelWindow.hxx"
#include "otgui/LimitStateWindow.hxx"
#include "otgui/DataModelWizard.hxx"
#include "otgui/DataModelWindow.hxx"
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
#include "otgui/SimulationReliabilityResultWindow.hxx"
#include "otgui/ApproximationResultWindow.hxx"
#include "otgui/FunctionalChaosResultWindow.hxx"
#include "otgui/KrigingResultWindow.hxx"
#include "otgui/InferenceResultWindow.hxx"
#include "otgui/MetaModelAnalysisWizard.hxx"
#include "otgui/LineEditWithQValidatorDelegate.hxx"
#include "otgui/CopulaInferenceResultWindow.hxx"

#include <iostream>

using namespace OT;

namespace OTGUI {

StudyTreeView::StudyTreeView(QWidget * parent)
  : QTreeView(parent)
  , treeViewModel_(new StudyTreeViewModel(this))
  , runningAnalysisItem_(0)
{
  OTStudy::SetInstanceObserver(treeViewModel_);
  setModel(treeViewModel_);
  connect(treeViewModel_, SIGNAL(newOTStudyCreated(OTStudyItem*)), this, SLOT(createNewOTStudyWindow(OTStudyItem*)));
  connect(treeViewModel_, SIGNAL(newDataModelCreated(DesignOfExperimentItem*)), this, SLOT(createNewDataModelWindow(DesignOfExperimentItem*)));
  connect(treeViewModel_, SIGNAL(newPhysicalModelCreated(PhysicalModelItem*)), this, SLOT(createNewPhysicalModelWindow(PhysicalModelItem*)));
  connect(treeViewModel_, SIGNAL(newProbabilisticModelCreated(ProbabilisticModelItem*)), this, SLOT(createNewProbabilisticModelWindow(ProbabilisticModelItem*)));
  connect(treeViewModel_, SIGNAL(newDesignOfExperimentCreated(DesignOfExperimentItem*)), this, SLOT(createNewDesignOfExperimentWindow(DesignOfExperimentItem*)));
  connect(treeViewModel_, SIGNAL(newLimitStateCreated(LimitStateItem*)), this, SLOT(createNewLimitStateWindow(LimitStateItem*)));
  connect(treeViewModel_, SIGNAL(newAnalysisCreated(AnalysisItem*)), this, SLOT(createAnalysisConnection(AnalysisItem*)));
  connect(treeViewModel_, SIGNAL(itemRemoved(QStandardItem*)), this, SIGNAL(removeSubWindow(QStandardItem*)));

  buildActions();

  // Forbid the user to define not valid item's name
  LineEditWithQValidatorDelegate * delegate = new LineEditWithQValidatorDelegate(false, "[a-zA-Z_][a-zA-Z_0-9]*", this);
  setItemDelegateForColumn(0, delegate);

  setContextMenuPolicy(Qt::CustomContextMenu);
  connect(this, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(onCustomContextMenu(const QPoint &)));

  header()->hide();

  connect(this, SIGNAL(clicked(QModelIndex)), this, SLOT(setCurrentIndex(QModelIndex)));
  connect(this->selectionModel(), SIGNAL(currentChanged(QModelIndex,QModelIndex)), this, SLOT(selectedItemChanged(QModelIndex, QModelIndex)));
}


StudyTreeView::~StudyTreeView()
{
  runningAnalysisItem_ = 0;
}


void StudyTreeView::onCustomContextMenu(const QPoint &point)
{
  QModelIndex index = indexAt(point);

  if (!index.isValid())
    return;

  QStandardItem * currentItem = treeViewModel_->itemFromIndex(index);
  QString dataType = currentItem->data(Qt::UserRole).toString();

  if (dataType == "ProbabilisticStudy" && currentItem->hasChildren())
    return;

  if (dataType.contains("DesignOfExperiment"))
  {
    DesignOfExperimentItem * DOEItem = dynamic_cast<DesignOfExperimentItem*>(currentItem);
    if (DOEItem)
    {
      if (DOEItem->getDesignOfExperiment().getOutputSample().getSize())
        evaluateDesignOfExperiment_->setEnabled(false);
      else
        if (!runningAnalysisItem_)
          evaluateDesignOfExperiment_->setEnabled(true);
    }
  }

  QMenu * contextMenu = new QMenu(this);
  contextMenu->addActions(getActions(dataType));
  contextMenu->exec(mapToGlobal(point));
}


void StudyTreeView::buildActions()
{
  // close otstudy action
  closeOTStudy_ = new QAction(QIcon(":/images/window-close.png"), tr("Close"), this);
  closeOTStudy_->setStatusTip(tr("Close the OTStudy"));
  connect(closeOTStudy_, SIGNAL(triggered()), this, SLOT(closeOTStudy()));

  // new data model
  newDataModel_ = new QAction(tr("New data model"), this);
  newDataModel_->setStatusTip(tr("Create a new data model"));
  connect(newDataModel_, SIGNAL(triggered()), this, SLOT(createNewDataModel()));

  // modify data model action
  modifyDataModel_ = new QAction(tr("Modify"), this);
  modifyDataModel_->setStatusTip(tr("Modify the data model"));
  connect(modifyDataModel_, SIGNAL(triggered(bool)), this, SLOT(modifyDataModel()));

  // remove data model
  removeDataModel_ = new QAction(QIcon(":/images/window-close.png"), tr("Remove"), this);
  removeDataModel_->setStatusTip(tr("Remove the data model"));
  connect(removeDataModel_, SIGNAL(triggered()), this, SLOT(removeDesignOfExperiment()));

  // new data analyses
  newDataAnalysis_ = new QAction(tr("New data analysis"), this);
  newDataAnalysis_->setStatusTip(tr("Analyse the data sample"));
  connect(newDataAnalysis_, SIGNAL(triggered()), this, SLOT(createNewDataAnalysis()));

  newInferenceAnalysis_ = new QAction(tr("New inference analysis"), this);
  newInferenceAnalysis_->setStatusTip(tr("Inference"));
  connect(newInferenceAnalysis_, SIGNAL(triggered()), this, SLOT(createNewInferenceAnalysis()));

  newCopulaInferenceAnalysis_ = new QAction(tr("Dependencies inference analysis"), this);
  newCopulaInferenceAnalysis_->setStatusTip(tr("Dependencies inference"));
  connect(newCopulaInferenceAnalysis_, SIGNAL(triggered()), this, SLOT(createNewCopulaInferenceAnalysis()));

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

  newMetaModel_ = new QAction(tr("New metamodel"), this);
  newMetaModel_->setStatusTip(tr("Create a new metamodel"));
  connect(newMetaModel_, SIGNAL(triggered()), this, SLOT(createNewMetaModel()));

  // run design of experiment action
  runDesignOfExperiment_ = new QAction(tr("Modify"), this);
  runDesignOfExperiment_->setStatusTip(tr("Modify the design of experiment"));
  connect(runDesignOfExperiment_, SIGNAL(triggered()), this, SLOT(runDesignOfExperiment()));

  evaluateDesignOfExperiment_ = new QAction(QIcon(":/images/run-build.png"), tr("Evaluate"), this);
  evaluateDesignOfExperiment_->setStatusTip(tr("Evaluate the design of experiment"));
  connect(evaluateDesignOfExperiment_, SIGNAL(triggered()), this, SLOT(evaluateDesignOfExperiment()));

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
  removeAnalysis_->setStatusTip(tr("Remove the analysis"));
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
    actions.append(newDataModel_);
    actions.append(saveOTStudy_);
    actions.append(closeOTStudy_);
  }
  else if (dataType == "DataModel")
  {
    actions.append(modifyDataModel_);
    actions.append(newDataAnalysis_);
    actions.append(newMetaModel_);
    actions.append(newInferenceAnalysis_);
    actions.append(newCopulaInferenceAnalysis_);
    actions.append(removeDataModel_);
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
  else if (dataType.contains("DesignOfExperiment"))
  {
    actions.append(runDesignOfExperiment_);
    actions.append(evaluateDesignOfExperiment_);
    actions.append(newMetaModel_);
    actions.append(removeDesignOfExperiment_);
  }
  else if (dataType == "LimitState")
  {
    actions.append(newThresholdExceedance_);
    actions.append(removeLimitState_);
  }
  else if (dataType.contains("Analysis") || dataType == "ModelEvaluation")
  {
    if (dataType != "DataAnalysis" && dataType != "CopulaInferenceAnalysis" &&
        dataType != "ImportanceSamplingAnalysis") // there is no wizard associated with these analyses
      actions.append(runAnalysis_);

    actions.append(removeAnalysis_);
  }
  return actions;
}


void StudyTreeView::changeActionsAvailability(const bool availability)
{
  newCentralTendency_->setEnabled(availability);
  newSensitivityAnalysis_->setEnabled(availability);
  newThresholdExceedance_->setEnabled(availability);
  newModelEvaluation_->setEnabled(availability);
  newDataAnalysis_->setEnabled(availability);
  newMetaModel_->setEnabled(availability);
  newInferenceAnalysis_->setEnabled(availability);
  newDesignOfExperiment_->setEnabled(availability);
  runAnalysis_->setEnabled(availability);
  runDesignOfExperiment_->setEnabled(availability);
  evaluateDesignOfExperiment_->setEnabled(availability);
  emit actionsAvailabilityChanged(availability);
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
  OTStudy otstudy = OTStudy(OTStudy::GetAvailableName());
  OTStudy::Add(otstudy);
}


void StudyTreeView::createNewDataModel()
{
  QModelIndex studyIndex = selectionModel()->currentIndex();
  OTStudyItem * studyItem = static_cast<OTStudyItem*>(treeViewModel_->itemFromIndex(studyIndex));
  DataModel newDataModel(studyItem->getOTStudy().getAvailableDataModelName());
  QSharedPointer<DataModelWizard> wizard = QSharedPointer<DataModelWizard>(new DataModelWizard(newDataModel, this));

  if (wizard->exec())
    studyItem->getOTStudy().add(wizard->getDataModel());
}


void StudyTreeView::modifyDataModel()
{
  QModelIndex index = selectionModel()->currentIndex();
  QStandardItem * selectedItem = treeViewModel_->itemFromIndex(index);
  DesignOfExperimentItem * item = dynamic_cast<DesignOfExperimentItem*>(selectedItem);
  QSharedPointer<DataModelWizard> wizard = QSharedPointer<DataModelWizard>(new DataModelWizard(item->getDesignOfExperiment(), this));

  if (wizard->exec())
  {
    emit removeSubWindow(item);
    item->updateDesignOfExperiment(wizard->getDataModel());
    createNewDataModelWindow(item);
  }
}


void StudyTreeView::createNewDataAnalysis()
{
  QModelIndex index = selectionModel()->currentIndex();
  QStandardItem * selectedItem = treeViewModel_->itemFromIndex(index);
  DesignOfExperimentItem * item = dynamic_cast<DesignOfExperimentItem*>(selectedItem);
  OTStudyItem * otStudyItem = dynamic_cast<OTStudyItem*>(item->QStandardItem::parent());
  if (!dynamic_cast<const DataModel*>(&*item->getDesignOfExperiment().getImplementation()))
    throw InvalidValueException(HERE) << "StudyTreeView::createNewDataAnalysis: The design of experiment is not a datamodel";
  DataAnalysis * analysis = new DataAnalysis(otStudyItem->getOTStudy().getAvailableAnalysisName("DataAnalysis_"), *dynamic_cast<const DataModel*>(&*item->getDesignOfExperiment().getImplementation()));

  otStudyItem->getOTStudy().add(analysis);
  findAnalysisItemAndLaunchExecution(otStudyItem, analysis->getName().c_str());
}


void StudyTreeView::createNewAnalyticalPhysicalModel()
{
  QModelIndex studyIndex = selectionModel()->currentIndex();
  OTStudyItem * studyItem = static_cast<OTStudyItem*>(treeViewModel_->itemFromIndex(studyIndex));
  AnalyticalPhysicalModel newPhysicalModel(studyItem->getOTStudy().getAvailablePhysicalModelName());
  studyItem->getOTStudy().add(newPhysicalModel);
}


void StudyTreeView::createNewPythonPhysicalModel()
{
  QModelIndex studyIndex = selectionModel()->currentIndex();
  OTStudyItem * studyItem = static_cast<OTStudyItem*>(treeViewModel_->itemFromIndex(studyIndex));
  PythonPhysicalModel newPhysicalModel(studyItem->getOTStudy().getAvailablePhysicalModelName());
  studyItem->getOTStudy().add(newPhysicalModel);
}


#ifdef OTGUI_HAVE_YACS
void StudyTreeView::createNewYACSPhysicalModel()
{
  QModelIndex studyIndex = selectionModel()->currentIndex();
  OTStudyItem * studyItem = static_cast<OTStudyItem*>(treeViewModel_->itemFromIndex(studyIndex));
  YACSPhysicalModel newPhysicalModel(studyItem->getOTStudy().getAvailablePhysicalModelName());
  studyItem->getOTStudy().add(newPhysicalModel);
}
#endif


void StudyTreeView::removePhysicalModel()
{
  const QModelIndex index = selectionModel()->currentIndex();
  QStandardItem * selectedItem = treeViewModel_->itemFromIndex(index);

  if (dynamic_cast<PhysicalModelItem*>(selectedItem))
    treeViewModel_->getOTStudyItem(index)->getOTStudy().remove(dynamic_cast<PhysicalModelItem*>(selectedItem)->getPhysicalModel());
}


void StudyTreeView::createNewProbabilisticModel()
{
  const QModelIndex probabilisticStudyIndex = selectionModel()->currentIndex();
  if (!hasPhysicalModelInputs(probabilisticStudyIndex))
    return;
  treeViewModel_->addProbabilisticModelItem(probabilisticStudyIndex);
  setExpanded(probabilisticStudyIndex, true);
}


void StudyTreeView::createNewDesignOfExperiment()
{
  const QModelIndex DesignOfExperimentListIndex = selectionModel()->currentIndex();
  PhysicalModelItem * physicalModelItem = treeViewModel_->getPhysicalModelItem(DesignOfExperimentListIndex);
  if (!physicalModelItem)
    return;

  if (!hasPhysicalModelInputs(DesignOfExperimentListIndex))
    return;
  OTStudyItem * otStudyItem = dynamic_cast<OTStudyItem*>(physicalModelItem->QStandardItem::parent());
  QSharedPointer<DesignOfExperimentWizard> wizard = QSharedPointer<DesignOfExperimentWizard>(new DesignOfExperimentWizard(otStudyItem->getOTStudy(), physicalModelItem->getPhysicalModel(), this));

  if (wizard->exec())
    otStudyItem->getOTStudy().add(wizard->getDesignOfExperiment());
}


void StudyTreeView::createNewLimitState()
{
  const QModelIndex probabilisticStudyIndex = selectionModel()->currentIndex().parent();
  if (!isPhysicalModelValid(probabilisticStudyIndex) || !isProbabilisticModelValid(probabilisticStudyIndex))
    return;
  treeViewModel_->addLimitStateItem(probabilisticStudyIndex);
  setExpanded(probabilisticStudyIndex, true);
}


void StudyTreeView::removeLimitState()
{
  const QModelIndex index = selectionModel()->currentIndex();
  QStandardItem * selectedItem = treeViewModel_->itemFromIndex(index);

  if (dynamic_cast<LimitStateItem*>(selectedItem))
    treeViewModel_->getOTStudyItem(index)->getOTStudy().remove(dynamic_cast<LimitStateItem*>(selectedItem)->getLimitState());
}


void StudyTreeView::createNewOTStudyWindow(OTStudyItem* item)
{
  OTStudyWindow * window = new OTStudyWindow(item);
  connect(window, SIGNAL(createNewAnalyticalPhysicalModel()), this, SLOT(createNewAnalyticalPhysicalModel()));
  connect(window, SIGNAL(createNewPythonPhysicalModel()), this, SLOT(createNewPythonPhysicalModel()));
#ifdef OTGUI_HAVE_YACS
  connect(window, SIGNAL(createNewYACSPhysicalModel()), this, SLOT(createNewYACSPhysicalModel()));
#endif
  connect(window, SIGNAL(createNewDataModel()), this, SLOT(createNewDataModel()));
  emit showWindow(window);
  setCurrentIndex(item->index());
}


void StudyTreeView::createNewDataModelWindow(DesignOfExperimentItem * item)
{
  DataModelWindow * window = new DataModelWindow(item);
  emit showWindow(window);
  setExpanded(item->index().parent(), true);
  setCurrentIndex(item->index());
}


void StudyTreeView::createNewPhysicalModelWindow(PhysicalModelItem * item)
{
  OTguiSubWindow * window = 0;
  String physicalModelType = item->getPhysicalModel().getImplementation()->getClassName();
  if (physicalModelType == "AnalyticalPhysicalModel")
    window = new AnalyticalPhysicalModelWindow(item);
  else if (physicalModelType == "PythonPhysicalModel")
    window = new PythonPhysicalModelWindow(item);
  else if (physicalModelType == "MetaModel")
    window = new MetaModelWindow(item);
#ifdef OTGUI_HAVE_YACS
  else if (physicalModelType == "YACSPhysicalModel")
    window = new YACSPhysicalModelWindow(item);
#endif
  if (window)
  {
    emit showWindow(window);
    setCurrentIndex(item->index());
    setExpanded(item->index(), true);
  }
}


void StudyTreeView::createNewProbabilisticModelWindow(ProbabilisticModelItem * item)
{
  OTStudyItem * otStudyItem = treeViewModel_->getOTStudyItem(item->index());
  if (!otStudyItem)
    return;
  ProbabilisticModelWindow * window = new ProbabilisticModelWindow(otStudyItem->getOTStudy(), item);
  connect(window, SIGNAL(graphWindowActivated(QWidget*)), this, SIGNAL(graphWindowActivated(QWidget*)));
  connect(window, SIGNAL(graphWindowDeactivated()), this, SIGNAL(graphWindowDeactivated()));
  emit showWindow(window);
  setCurrentIndex(item->index());
}


void StudyTreeView::createNewLimitStateWindow(LimitStateItem* item)
{
  LimitStateWindow * window = new LimitStateWindow(item);
  emit showWindow(window);
  setCurrentIndex(item->index());
}


void StudyTreeView::createNewDesignOfExperimentWindow(DesignOfExperimentItem * item)
{
  DesignOfExperimentWindow * window = new DesignOfExperimentWindow(item);

  connect(window, SIGNAL(graphWindowActivated(QWidget*)), this, SIGNAL(graphWindowActivated(QWidget*)));
  connect(window, SIGNAL(graphWindowDeactivated()), this, SIGNAL(graphWindowDeactivated()));
  connect(item, SIGNAL(analysisFinished()), window, SLOT(updateTable()));
  connect(item, SIGNAL(analysisBadlyFinished(QString)), window, SLOT(setTemporaryErrorMessage(QString)));

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
  QSharedPointer<ModelEvaluationWizard> wizard = QSharedPointer<ModelEvaluationWizard>(new ModelEvaluationWizard(otStudyItem->getOTStudy(), physicalModelItem->getPhysicalModel(), this));

  if (wizard->exec())
  {
    otStudyItem->getOTStudy().add(wizard->getAnalysis());
    findAnalysisItemAndLaunchExecution(otStudyItem, wizard->getAnalysis().getName().c_str());
  }
}


void StudyTreeView::createNewCentralTendency()
{
  QModelIndex probabilisticStudyIndex = selectionModel()->currentIndex().parent();
  if (!isPhysicalModelValid(probabilisticStudyIndex) || !isProbabilisticModelValid(probabilisticStudyIndex))
    return;
  PhysicalModelItem * physicalModelItem = treeViewModel_->getPhysicalModelItem(probabilisticStudyIndex);
  OTStudyItem * otStudyItem = dynamic_cast<OTStudyItem*>(physicalModelItem->QStandardItem::parent());
  QSharedPointer<CentralTendencyWizard> wizard = QSharedPointer<CentralTendencyWizard>(new CentralTendencyWizard(otStudyItem->getOTStudy(), physicalModelItem->getPhysicalModel(), this));

  if (wizard->exec())
  {
    otStudyItem->getOTStudy().add(wizard->getAnalysis());
    findAnalysisItemAndLaunchExecution(otStudyItem, wizard->getAnalysis().getName().c_str());
  }
}


void StudyTreeView::createNewSensitivityAnalysis()
{
  QModelIndex probabilisticStudyIndex = selectionModel()->currentIndex().parent();
  if (!isPhysicalModelValid(probabilisticStudyIndex) || !isProbabilisticModelValid(probabilisticStudyIndex))
    return;
  PhysicalModelItem * physicalModelItem = treeViewModel_->getPhysicalModelItem(probabilisticStudyIndex);
  // check if the model has an independent copula
  if (!physicalModelItem->getPhysicalModel().getComposedDistribution().hasIndependentCopula())
  {
    QMessageBox::critical(this, tr("Error"), tr("The model must have an independent copula to compute a sensitivity analysis but inputs are correlated."));
    return;
  }

  OTStudyItem * otStudyItem = dynamic_cast<OTStudyItem*>(physicalModelItem->QStandardItem::parent());
  QSharedPointer<SensitivityAnalysisWizard> wizard = QSharedPointer<SensitivityAnalysisWizard>(new SensitivityAnalysisWizard(otStudyItem->getOTStudy(), physicalModelItem->getPhysicalModel(), this));

  if (wizard->exec())
  {
    otStudyItem->getOTStudy().add(wizard->getAnalysis());
    findAnalysisItemAndLaunchExecution(otStudyItem, wizard->getAnalysis().getName().c_str());
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
  QSharedPointer<ReliabilityAnalysisWizard> wizard = QSharedPointer<ReliabilityAnalysisWizard>(new ReliabilityAnalysisWizard(otStudyItem->getOTStudy(), limitStateItem->getLimitState(), this));

  if (wizard->exec())
  {
    otStudyItem->getOTStudy().add(wizard->getAnalysis());
    findAnalysisItemAndLaunchExecution(otStudyItem, wizard->getAnalysis().getName().c_str());
  }
}


void StudyTreeView::createNewMetaModel()
{
  QModelIndex index = selectionModel()->currentIndex();
  QStandardItem * selectedItem = treeViewModel_->itemFromIndex(index);
  DesignOfExperimentItem * item = dynamic_cast<DesignOfExperimentItem*>(selectedItem);
  if (!item->getDesignOfExperiment().getOutputSample().getSize())
  {
    QMessageBox::critical(this, tr("Error"), tr("The model must have at least one output."));
    return;
  }
  OTStudyItem * otStudyItem = treeViewModel_->getOTStudyItem(index);
  QSharedPointer<MetaModelAnalysisWizard> wizard = QSharedPointer<MetaModelAnalysisWizard>(new MetaModelAnalysisWizard(otStudyItem->getOTStudy(), item->getDesignOfExperiment(), this));

  if (wizard->exec())
  {
    otStudyItem->getOTStudy().add(wizard->getAnalysis());
    findAnalysisItemAndLaunchExecution(otStudyItem, wizard->getAnalysis().getName().c_str());
  }
}


void StudyTreeView::createNewInferenceAnalysis()
{
  QModelIndex index = selectionModel()->currentIndex();
  QStandardItem * selectedItem = treeViewModel_->itemFromIndex(index);
  DesignOfExperimentItem * item = dynamic_cast<DesignOfExperimentItem*>(selectedItem);
  OTStudyItem * otStudyItem = treeViewModel_->getOTStudyItem(index);
  QSharedPointer<InferenceWizard> wizard = QSharedPointer<InferenceWizard>(new InferenceWizard(otStudyItem->getOTStudy(), item->getDesignOfExperiment(), this));

  if (wizard->exec())
  {
    otStudyItem->getOTStudy().add(wizard->getAnalysis());
    findAnalysisItemAndLaunchExecution(otStudyItem, wizard->getAnalysis().getName().c_str());
  }
}


void StudyTreeView::createNewCopulaInferenceAnalysis()
{
  QModelIndex index = selectionModel()->currentIndex();
  QStandardItem * selectedItem = treeViewModel_->itemFromIndex(index);
  DesignOfExperimentItem * item = dynamic_cast<DesignOfExperimentItem*>(selectedItem);
  OTStudyItem * otStudyItem = dynamic_cast<OTStudyItem*>(item->QStandardItem::parent());
  if (!dynamic_cast<const DataModel*>(&*item->getDesignOfExperiment().getImplementation()))
    throw InvalidValueException(HERE) << "StudyTreeView::createNewCopulaInferenceAnalysis: The design of experiment is not a datamodel";

  CopulaInferenceAnalysis * analysis = new CopulaInferenceAnalysis(otStudyItem->getOTStudy().getAvailableAnalysisName("DependenciesInference_"), *dynamic_cast<const DataModel*>(&*item->getDesignOfExperiment().getImplementation()));

  otStudyItem->getOTStudy().add(analysis);
  findAnalysisItemAndLaunchExecution(otStudyItem, analysis->getName().c_str());
}


void StudyTreeView::createAnalysisConnection(AnalysisItem * item)
{
  connect(item, SIGNAL(analysisFinished(AnalysisItem *)), this, SLOT(createAnalysisResultWindow(AnalysisItem*)));
  connect(item, SIGNAL(analysisBadlyFinished(AnalysisItem*,QString)), this, SLOT(createAnalysisExecutionFailedWindow(AnalysisItem*,QString)));
}


void StudyTreeView::runDesignOfExperiment()
{
  QModelIndex index = selectionModel()->currentIndex();
  QStandardItem * selectedItem = treeViewModel_->itemFromIndex(index);
  DesignOfExperimentItem * item = dynamic_cast<DesignOfExperimentItem*>(selectedItem);
  QSharedPointer<DesignOfExperimentWizard> wizard = QSharedPointer<DesignOfExperimentWizard>(new DesignOfExperimentWizard(item->getDesignOfExperiment(), this));

  if (wizard->exec())
  {
    emit removeSubWindow(item);
    item->updateDesignOfExperiment(wizard->getDesignOfExperiment());
    createNewDesignOfExperimentWindow(item);
  }
}


void StudyTreeView::removeDesignOfExperiment()
{
  QModelIndex index = selectionModel()->currentIndex();
  QStandardItem * selectedItem = treeViewModel_->itemFromIndex(index);

  treeViewModel_->getOTStudyItem(index)->getOTStudy().remove(dynamic_cast<DesignOfExperimentItem*>(selectedItem)->getDesignOfExperiment());
}


void StudyTreeView::launchAnalysis(AnalysisItem* analysisItem)
{
  emit removeSubWindow(analysisItem);

  ControllerWidget * controllerWidget = new ControllerWidget(analysisItem->getAnalysis().getName().c_str());
  emit showControllerWidget(controllerWidget);

  connect(analysisItem, SIGNAL(messageChanged(QString)), controllerWidget, SLOT(setMessage(QString)));
  connect(analysisItem, SIGNAL(progressValueChanged(int)), controllerWidget, SLOT(changeProgressBarValue(int)));

  connect(controllerWidget, SIGNAL(stopRunningRequested()), analysisItem, SLOT(stopAnalysis()));

  // controller/widget automatically deleted when the analysis is finished
  Controller * controller = new Controller;
  connect(controller, SIGNAL(analysisFinished()), controllerWidget, SLOT(deleteLater()));
  controller->launchAnalysis(analysisItem->getAnalysis());

  runningAnalysisItem_ = analysisItem;
  changeActionsAvailability(false);
}


void StudyTreeView::findAnalysisItemAndLaunchExecution(OTStudyItem * otStudyItem, const QString & analysisName)
{
  AnalysisItem * analysisItem = treeViewModel_->getAnalysisItem(otStudyItem, analysisName);

  if (analysisItem)
  {
    launchAnalysis(analysisItem);
  }
}


void StudyTreeView::runAnalysis()
{
  QStandardItem * selectedItem = treeViewModel_->itemFromIndex(selectionModel()->currentIndex());
  AnalysisItem * item = dynamic_cast<AnalysisItem*>(selectedItem);

  QSharedPointer<AnalysisWizard> wizard;
  QString analysisType = item->data(Qt::UserRole).toString();

  if (analysisType == "ModelEvaluation")
  {
    if (!isPhysicalModelValid(selectionModel()->currentIndex()))
    {
      qDebug() << "Error: In runAnalysis: physical model not valid for the " << analysisType << "\n";
      return;
    }
    wizard = QSharedPointer<AnalysisWizard>(new ModelEvaluationWizard(item->getAnalysis(), this));
  }
  else if (analysisType == "FunctionalChaosAnalysis" || analysisType == "KrigingAnalysis")
  {
    wizard = QSharedPointer<AnalysisWizard>(new MetaModelAnalysisWizard(item->getAnalysis(), this));
  }
  else if (analysisType == "InferenceAnalysis")
  {
    wizard = QSharedPointer<AnalysisWizard>(new InferenceWizard(item->getAnalysis(), this));
  }
  else
  {
    if (!isPhysicalModelValid(selectionModel()->currentIndex()))
    {
      qDebug() << "Error: In runAnalysis: physical model not valid for the " << analysisType << "\n";
      return;
    }
    if (!isProbabilisticModelValid(selectionModel()->currentIndex()))
    {
      qDebug() << "Error: In runAnalysis: probabilistic model not valid for the " << analysisType << "\n";
      return;
    }

    if (analysisType == "MonteCarloAnalysis" || analysisType == "TaylorExpansionMomentsAnalysis")
    {
      wizard = QSharedPointer<AnalysisWizard>(new CentralTendencyWizard(item->getAnalysis(), this));
    }
    else if (analysisType == "SobolAnalysis" || analysisType == "SRCAnalysis")
    {
      wizard = QSharedPointer<AnalysisWizard>(new SensitivityAnalysisWizard(item->getAnalysis(), this));
    }
    else if (analysisType == "MonteCarloReliabilityAnalysis" || analysisType == "FORMImportanceSamplingAnalysis" ||
             analysisType == "FORMAnalysis")
    {
      if (!isLimitStateValid(selectionModel()->currentIndex()))
      {
        qDebug() << "In runAnalysis: limit state not valid for the " << analysisType << "\n";
        return;
      }
      wizard = QSharedPointer<AnalysisWizard>(new ReliabilityAnalysisWizard(item->getAnalysis(), this));
    }
    else
    {
      qDebug() << "Error: In runAnalysis: analysisType " << analysisType << " not recognized.\n";
    }
  }

  if (wizard)
  {
    if (wizard->exec())
    {
      item->updateAnalysis(wizard->getAnalysis());
      launchAnalysis(item);
    }
  }
}


void StudyTreeView::resetTree()
{
  runningAnalysisItem_ = 0;
  changeActionsAvailability(true);
  emit analysisFinished();
}


void StudyTreeView::evaluateDesignOfExperiment()
{
  QStandardItem * selectedItem = treeViewModel_->itemFromIndex(selectionModel()->currentIndex());
  DesignOfExperimentItem * DOEItem = dynamic_cast<DesignOfExperimentItem*>(selectedItem);

  Analysis evaluation = DesignOfExperimentEvaluation(DOEItem->getDesignOfExperiment());
  AnalysisItem * item = new AnalysisItem(evaluation);

  connect(item, SIGNAL(analysisFinished(AnalysisItem*)), item, SLOT(deleteLater()));
  connect(item, SIGNAL(analysisBadlyFinished(AnalysisItem*,QString)), item, SLOT(deleteLater()));
  connect(item, SIGNAL(analysisFinished(AnalysisItem*)), this, SLOT(resetTree()));
  connect(item, SIGNAL(analysisBadlyFinished(AnalysisItem*,QString)), this, SLOT(resetTree()));

  launchAnalysis(item);
}


void StudyTreeView::removeAnalysis()
{
  QModelIndex index = selectionModel()->currentIndex();
  if (!index.isValid())
    return;

  QStandardItem * selectedItem = treeViewModel_->itemFromIndex(index);
  AnalysisItem * currentItem = dynamic_cast<AnalysisItem*>(selectedItem);

  if (!currentItem)
    return;

  if (runningAnalysisItem_ == currentItem)
  {
    QMessageBox::warning(this, tr("Warning"), tr("Cannot close a running analysis."));
    return;
  }

  treeViewModel_->getOTStudyItem(index)->getOTStudy().remove(currentItem->getAnalysis());
}


void StudyTreeView::createAnalysisResultWindow(AnalysisItem* item)
{
  resetTree();
  // do removeSubWindow if the analysis run method has been launched from a Python script
  emit removeSubWindow(item);

  ResultWindow * resultWindow = 0;
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
  else if (analysisType == "MonteCarloReliabilityAnalysis" ||
           analysisType == "FORMImportanceSamplingAnalysis" ||
           analysisType == "ImportanceSamplingAnalysis")
    resultWindow = new SimulationReliabilityResultWindow(item);
  else if (analysisType == "FORMAnalysis")
    resultWindow = new ApproximationResultWindow(item);
  else if (analysisType == "DataAnalysis")
    resultWindow = new DataAnalysisResultWindow(item);
  else if (analysisType == "FunctionalChaosAnalysis")
    resultWindow = new FunctionalChaosResultWindow(item);
  else if (analysisType == "KrigingAnalysis")
    resultWindow = new KrigingResultWindow(item);
  else if (analysisType == "InferenceAnalysis")
    resultWindow = new InferenceResultWindow(item);
  else if (analysisType == "CopulaInferenceAnalysis")
    resultWindow = new CopulaInferenceResultWindow(item);
  else
    qDebug() << "Error: In createAnalysisResultWindow: analysisType " << analysisType << " not recognized.";

  if (resultWindow)
  {
    connect(resultWindow, SIGNAL(graphWindowActivated(QWidget*)), this, SIGNAL(graphWindowActivated(QWidget*)));
    connect(resultWindow, SIGNAL(graphWindowDeactivated()), this, SIGNAL(graphWindowDeactivated()));
    emit showWindow(resultWindow);
    if (analysisType == "SobolAnalysis" || analysisType == "SRCAnalysis")
      resultWindow->showHideGraphConfigurationWidget(Qt::WindowNoState, Qt::WindowFullScreen);
    setCurrentIndex(item->index());
  }
}


void StudyTreeView::createAnalysisExecutionFailedWindow(AnalysisItem * item, const QString & errorMessage)
{
  resetTree();

  AnalysisExecutionFailedWindow * window = new AnalysisExecutionFailedWindow(item, errorMessage);
  emit showWindow(window);

  setCurrentIndex(QModelIndex()); // call it otherwise the signal currentChanged is not emitted
  setCurrentIndex(item->index());
}


void StudyTreeView::exportPython()
{
  OTStudyItem * item = treeViewModel_->getOTStudyItem(selectionModel()->currentIndex());
  if (!item)
    return ;

  QSettings settings;
  QString currentDir = settings.value("currentDir").toString();
  if (currentDir.isEmpty())
    currentDir = QDir::homePath();
  QString defaultFileName = QDir::separator() + QFileInfo(QString::fromUtf8(item->getOTStudy().getFileName().c_str())).baseName();
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
      out << QString::fromUtf8(item->getOTStudy().getPythonScript().c_str());
      file.setPermissions(QFile::ReadUser|QFile::WriteUser|QFile::ExeUser|QFile::ReadGroup|QFile::ExeGroup|QFile::ReadOther|QFile::ExeOther);
      file.close();
    }
  }
}


bool StudyTreeView::saveOTStudy()
{
  OTStudyItem * item = treeViewModel_->getOTStudyItem(selectionModel()->currentIndex());
  if (!item)
    return false;

  if (QFileInfo(QString::fromUtf8(item->getOTStudy().getFileName().c_str())).exists())
  {
    QApplication::setOverrideCursor(Qt::WaitCursor);
    item->getOTStudy().save(item->getOTStudy().getFileName());
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
  if (!item)
    return false;

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
      item->getOTStudy().save(fileName.toUtf8().constData());
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

  if (file.exists() && !OTStudy::GetFileNames().contains(file.absoluteFilePath().toUtf8().constData()))
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
      message = tr("A problem appeared when reading the file '%1'. \nMaybe objects are not opened.\n").arg(fileName);
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
    else if (OTStudy::GetFileNames().contains(file.absoluteFilePath().toUtf8().constData()))
    {
      message = tr("The file '%1' is already opened.").arg(fileName);
    }
    QMessageBox::warning(this, tr("Warning"), message);
  }
}


bool StudyTreeView::closeOTStudy()
{
  if (runningAnalysisItem_)
  {
    QMessageBox::warning(this, tr("Warning"), tr("Cannot close studies when an analysis is running."));
    return false;
  }

  OTStudyItem * item = treeViewModel_->getOTStudyItem(selectionModel()->currentIndex());
  if (!item)
    return true;
  if (QFileInfo(QString::fromUtf8(item->getOTStudy().getFileName().c_str())).exists())
  {
    QApplication::setOverrideCursor(Qt::WaitCursor);
    item->getOTStudy().save(item->getOTStudy().getFileName());
    QApplication::restoreOverrideCursor();
    OTStudy::Remove(item->getOTStudy());
    return true;
  }
  else
  {
    int ret = QMessageBox::warning(this, tr("Warning"),
                               tr("Do you want to save the OTStudy '%1' [%2]?").arg(item->getOTStudy().getName().c_str()).arg(item->getOTStudy().getFileName().c_str()),
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
  if (runningAnalysisItem_)
  {
    QMessageBox::warning(this, tr("Warning"), tr("Cannot close studies when an analysis is running."));
    return false;
  }
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


void StudyTreeView::selectedItemChanged(const QModelIndex & currentIndex, const QModelIndex & previousIndex)
{
  if (!currentIndex.isValid())
    return;
  QStandardItem * selectedItem = treeViewModel_->itemFromIndex(currentIndex);
  emit itemSelected(selectedItem);
}
}
