#include "otgui/StudyTreeView.hxx"

#include <QMenu>
#include <QHeaderView>
#include <QMdiSubWindow>
#include <QFileDialog>

#include "otgui/PhysicalModelWindow.hxx"
#include "otgui/ProbabilisticModelWindow.hxx"
#include "otgui/LimitStateWindow.hxx"
#include "otgui/DesignOfExperimentWizard.hxx"
#include "otgui/DesignOfExperimentWindow.hxx"
#include "otgui/ModelEvaluationWizard.hxx"
#include "otgui/ModelEvaluationResultWindow.hxx"
#include "otgui/CentralTendencyWizard.hxx"
#include "otgui/MonteCarloResultWindow.hxx"
#include "otgui/QuadraticCumulResultWindow.hxx"
#include "otgui/SensitivityAnalysisWizard.hxx"
#include "otgui/SobolResultWindow.hxx"
#include "otgui/SRCResultWindow.hxx"
#include "otgui/ReliabilityAnalysisWizard.hxx"
#include "otgui/MonteCarloReliabilityResultWindow.hxx"

#include <iostream>

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

  buildActions();

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
  newPhysicalModel_ = new QAction(tr("New physical model"), this);
  newPhysicalModel_->setStatusTip(tr("Create a new physical model"));
  connect(newPhysicalModel_, SIGNAL(triggered()), this, SLOT(createNewPhysicalModel()));

  newProbabilisticModel_ = new QAction(tr("New probabilistic model"), this);
  newProbabilisticModel_->setStatusTip(tr("Create a new probabilistic model"));
  connect(newProbabilisticModel_, SIGNAL(triggered()), this, SLOT(createNewProbabilisticModel()));

  newDesignOfExperiment_ = new QAction(tr("New design of experiment"), this);
  newDesignOfExperiment_->setStatusTip(tr("Create a new design of experiment"));
  connect(newDesignOfExperiment_, SIGNAL(triggered()), this, SLOT(createNewDesignOfExperiment()));

  newLimitState_ = new QAction(tr("New limit state"), this);
  newLimitState_->setStatusTip(tr("Create a new limit state"));
  connect(newLimitState_, SIGNAL(triggered()), this, SLOT(createNewLimitState()));

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

  runDesignOfExperiment_ = new QAction(tr("Run"), this);
  runDesignOfExperiment_->setStatusTip(tr("Run the design of experiment"));
  connect(runDesignOfExperiment_, SIGNAL(triggered()), this, SLOT(runDesignOfExperiment()));

  runModelEvaluation_ = new QAction(tr("Run"), this);
  runModelEvaluation_->setStatusTip(tr("Run the model evaluation"));
  connect(runModelEvaluation_, SIGNAL(triggered()), this, SLOT(runModelEvaluation()));

  runCentralTendency_ = new QAction(tr("Run"), this);
  runCentralTendency_->setStatusTip(tr("Run the central tendency"));
  connect(runCentralTendency_, SIGNAL(triggered()), this, SLOT(runCentralTendency()));

  runSensitivityAnalysis_ = new QAction(tr("Run"), this);
  runSensitivityAnalysis_->setStatusTip(tr("Run the sensitivity analysis"));
  connect(runSensitivityAnalysis_, SIGNAL(triggered()), this, SLOT(runSensitivityAnalysis()));

  runReliabilityAnalysis_ = new QAction(tr("Run"), this);
  runReliabilityAnalysis_->setStatusTip(tr("Run the reliability analysis"));
  connect(runReliabilityAnalysis_, SIGNAL(triggered()), this, SLOT(runReliabilityAnalysis()));

  dumpOTStudy_ = new QAction(tr("Dump"), this);
  dumpOTStudy_->setStatusTip(tr("Dump the OTStudy"));
  connect(dumpOTStudy_, SIGNAL(triggered()), this, SLOT(dumpOTStudy()));
}


QList<QAction* > StudyTreeView::getActions(const QString & dataType)
{
  QList<QAction* > actions;

  if (dataType=="OTStudy")
  {
    actions.append(newPhysicalModel_);
    actions.append(dumpOTStudy_);
  }
  else if (dataType=="DeterministicStudy")
  {
    actions.append(newModelEvaluation_);
  }
  else if (dataType=="ProbabilisticStudy")
  {
    actions.append(newProbabilisticModel_);
  }
  else if (dataType=="ProbabilisticModel")
  {
    actions.append(newLimitState_);
    actions.append(newCentralTendency_);
    actions.append(newSensitivityAnalysis_);
  }
  else if (dataType=="DesignOfExperimentList")
  {
    actions.append(newDesignOfExperiment_);
  }
  else if (dataType=="DesignOfExperiment")
  {
    actions.append(runDesignOfExperiment_);
  }
  else if (dataType=="LimitState")
  {
    actions.append(newThresholdExceedance_);
  }
  else if (dataType=="ModelEvaluation")
  {
    actions.append(runModelEvaluation_);
  }
  else if (dataType=="CentralTendency")
  {
    actions.append(runCentralTendency_);
  }
  else if (dataType=="SensitivityAnalysis")
  {
    actions.append(runSensitivityAnalysis_);
  }
  else if (dataType=="ReliabilityAnalysis")
  {
    actions.append(runReliabilityAnalysis_);
  }
  return actions;
}


void StudyTreeView::createNewOTStudy()
{
  treeViewModel_->createNewOTStudy();
}


void StudyTreeView::createNewPhysicalModel()
{
  QModelIndex parentIndex = selectionModel()->currentIndex();
  treeViewModel_->addPhysicalModelItem(parentIndex);
  setExpanded(parentIndex, true);
  setExpanded(parentIndex.child(0, 0), true);
  setExpanded(parentIndex.child(1, 0), true);
}


void StudyTreeView::createNewProbabilisticModel()
{
  QModelIndex parentIndex = selectionModel()->currentIndex();
  treeViewModel_->addProbabilisticModelItem(parentIndex);
  setExpanded(parentIndex, true);
}


void StudyTreeView::createNewDesignOfExperiment()
{
  QModelIndex physicalModelIndex = selectionModel()->currentIndex().parent();
  PhysicalModelItem * physicalModelItem = dynamic_cast<PhysicalModelItem*>(treeViewModel_->itemFromIndex(physicalModelIndex));
  OTStudyItem * otStudyItem = dynamic_cast<OTStudyItem*>(physicalModelItem->QStandardItem::parent());
  DesignOfExperimentWizard * wizard = new DesignOfExperimentWizard(otStudyItem->getOTStudy(), physicalModelItem->getPhysicalModel());

  if (wizard->exec())
  {
    wizard->validate();
  }
}


void StudyTreeView::createNewLimitState()
{
  QModelIndex parentIndex = selectionModel()->currentIndex().parent();
  treeViewModel_->addLimitStateItem(parentIndex);
  setExpanded(parentIndex, true);
}


void StudyTreeView::createNewPhysicalModelWindow(PhysicalModelItem * item)
{
  PhysicalModelWindow * window = new PhysicalModelWindow(item);
  connect(window, SIGNAL(errorMessageChanged(QString)), this, SIGNAL(errorMessageEmitted(QString)));
  emit showWindow(window);
  setCurrentIndex(item->index());
  setExpanded(item->index(), true);
}


void StudyTreeView::createNewProbabilisticModelWindow(ProbabilisticModelItem * item)
{
  ProbabilisticModelWindow * window = new ProbabilisticModelWindow(item);
  connect(window, SIGNAL(errorMessageChanged(QString)), this, SIGNAL(errorMessageEmitted(QString)));
  connect(window, SIGNAL(graphWindowActivated(QTabWidget*)), this, SIGNAL(graphWindowActivated(QTabWidget*)));
  connect(window, SIGNAL(graphWindowDeactivated(QTabWidget*)), this, SIGNAL(graphWindowDeactivated(QTabWidget*)));
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
  connect(window, SIGNAL(errorMessageChanged(QString)), this, SIGNAL(errorMessageEmitted(QString)));
  connect(window, SIGNAL(graphWindowActivated(QTabWidget*)), this, SIGNAL(graphWindowActivated(QTabWidget*)));
  connect(window, SIGNAL(graphWindowDeactivated(QTabWidget*)), this, SIGNAL(graphWindowDeactivated(QTabWidget*)));
  connect(item, SIGNAL(analysisFinished()), window, SLOT(updateWindowForOutputs()));
  emit showWindow(window);
  setExpanded(item->index().parent(), true);
  setCurrentIndex(item->index());
}


void StudyTreeView::createNewModelEvaluation()
{
  QModelIndex physicalModelIndex = selectionModel()->currentIndex().parent();
  PhysicalModelItem * physicalModelItem = dynamic_cast<PhysicalModelItem*>(treeViewModel_->itemFromIndex(physicalModelIndex));
  OTStudyItem * otStudyItem = dynamic_cast<OTStudyItem*>(physicalModelItem->QStandardItem::parent());
  ModelEvaluationWizard * wizard = new ModelEvaluationWizard(otStudyItem->getOTStudy(), physicalModelItem->getPhysicalModel());

  if (wizard->exec())
  {
    wizard->validate();
    setExpanded(physicalModelIndex, true);
  }
}


void StudyTreeView::createNewCentralTendency()
{
  QModelIndex physicalModelIndex = selectionModel()->currentIndex().parent().parent();
  PhysicalModelItem * physicalModelItem = dynamic_cast<PhysicalModelItem*>(treeViewModel_->itemFromIndex(physicalModelIndex));
  OTStudyItem * otStudyItem = dynamic_cast<OTStudyItem*>(physicalModelItem->QStandardItem::parent());
  CentralTendencyWizard * wizard = new CentralTendencyWizard(otStudyItem->getOTStudy(), physicalModelItem->getPhysicalModel());

  if (wizard->exec())
  {
    wizard->validate();
    setExpanded(physicalModelIndex, true);
  }
}


void StudyTreeView::createNewSensitivityAnalysis()
{
  QModelIndex physicalModelIndex = selectionModel()->currentIndex().parent().parent();
  PhysicalModelItem * physicalModelItem = dynamic_cast<PhysicalModelItem*>(treeViewModel_->itemFromIndex(physicalModelIndex));
  OTStudyItem * otStudyItem = dynamic_cast<OTStudyItem*>(physicalModelItem->QStandardItem::parent());
  SensitivityAnalysisWizard * wizard = new SensitivityAnalysisWizard(otStudyItem->getOTStudy(), physicalModelItem->getPhysicalModel());

  if (wizard->exec())
  {
    wizard->validate();
    setExpanded(physicalModelIndex, true);
  }
}


void StudyTreeView::createNewThresholdExceedance()
{
  QModelIndex limitStateIndex = selectionModel()->currentIndex();
  LimitStateItem * limitStateItem = dynamic_cast<LimitStateItem*>(treeViewModel_->itemFromIndex(limitStateIndex));
  OTStudyItem * otStudyItem = treeViewModel_->getOTStudyItem(limitStateIndex);
  ReliabilityAnalysisWizard * wizard = new ReliabilityAnalysisWizard(otStudyItem->getOTStudy(), limitStateItem->getLimitState());

  if (wizard->exec())
  {
    wizard->validate();
    setExpanded(limitStateIndex, true);
  }
}


void StudyTreeView::createAnalysisConnection(AnalysisItem * item)
{
  QString data = item->data(Qt::UserRole).toString();
  if (data == "ModelEvaluation")
    connect(item, SIGNAL(analysisFinished(AnalysisItem *)), this, SLOT(createModelEvaluationResult(AnalysisItem*)));
  else if (data == "CentralTendency")
    connect(item, SIGNAL(analysisFinished(AnalysisItem *)), this, SLOT(createCentralTendencyResult(AnalysisItem *)));
  else if (data == "SensitivityAnalysis")
    connect(item, SIGNAL(analysisFinished(AnalysisItem *)), this, SLOT(createSensitivityAnalysisResult(AnalysisItem*)));
  else if (data == "ReliabilityAnalysis")
    connect(item, SIGNAL(analysisFinished(AnalysisItem *)), this, SLOT(createReliabilityAnalysisResult(AnalysisItem*)));
}


void StudyTreeView::runDesignOfExperiment()
{
  QModelIndex index = selectionModel()->currentIndex();
  QStandardItem * selectedItem = treeViewModel_->itemFromIndex(index);

  DesignOfExperimentItem * item = dynamic_cast<DesignOfExperimentItem*>(selectedItem);

  DesignOfExperimentWizard * wizard = new DesignOfExperimentWizard(item->getDesignOfExperiment());
  if (wizard->exec())
  {
    emit checkIfWindowResultExists(item);
    item->getDesignOfExperiment().clearResult();
    createNewDesignOfExperimentWindow(item);
  }
}


void StudyTreeView::runModelEvaluation()
{
  QModelIndex index = selectionModel()->currentIndex();
  QStandardItem * selectedItem = treeViewModel_->itemFromIndex(index);

  ModelEvaluationItem * item = dynamic_cast<ModelEvaluationItem*>(selectedItem);

  ModelEvaluationWizard * wizard = new ModelEvaluationWizard(item->getAnalysis());
  if (wizard->exec())
  {
    item->getAnalysis().run();
  }
}


void StudyTreeView::runCentralTendency()
{
  QModelIndex index = selectionModel()->currentIndex();
  QStandardItem * selectedItem = treeViewModel_->itemFromIndex(index);

  CentralTendencyItem * item = dynamic_cast<CentralTendencyItem*>(selectedItem);

  CentralTendencyWizard * wizard = new CentralTendencyWizard(item->getAnalysis());
  connect(wizard, SIGNAL(analysisChanged(Analysis)), item, SLOT(updateAnalysis(Analysis)));
  if (wizard->exec())
  {
    item->getAnalysis().run();
  }
}


void StudyTreeView::runSensitivityAnalysis()
{
  QModelIndex index = selectionModel()->currentIndex();
  QStandardItem * selectedItem = treeViewModel_->itemFromIndex(index);

  SensitivityAnalysisItem * item = dynamic_cast<SensitivityAnalysisItem*>(selectedItem);

  SensitivityAnalysisWizard * wizard = new SensitivityAnalysisWizard(item->getAnalysis());
  connect(wizard, SIGNAL(analysisChanged(Analysis)), item, SLOT(updateAnalysis(Analysis)));
  if (wizard->exec())
  {
    item->getAnalysis().run();
  }
}


void StudyTreeView::runReliabilityAnalysis()
{
  QModelIndex index = selectionModel()->currentIndex();
  QStandardItem * selectedItem = treeViewModel_->itemFromIndex(index);

  ReliabilityAnalysisItem * item = dynamic_cast<ReliabilityAnalysisItem*>(selectedItem);

  ReliabilityAnalysisWizard * wizard = new ReliabilityAnalysisWizard(item->getAnalysis());
  connect(wizard, SIGNAL(analysisChanged(Analysis)), item, SLOT(updateAnalysis(Analysis)));
  if (wizard->exec())
  {
    item->getAnalysis().run();
  }
}


void StudyTreeView::createModelEvaluationResult(AnalysisItem* item)
{
  emit checkIfWindowResultExists(item);
  ModelEvaluationResultWindow * window = new ModelEvaluationResultWindow(dynamic_cast<ModelEvaluationItem*>(item));
  emit showWindow(window);
  setCurrentIndex(item->index());
}


void StudyTreeView::createCentralTendencyResult(AnalysisItem * item)
{
  emit checkIfWindowResultExists(item);
  if (item->getAnalysis().getImplementation()->getClassName() == "MonteCarloAnalysis")
  {
    MonteCarloResultWindow * window = new MonteCarloResultWindow(dynamic_cast<CentralTendencyItem*>(item));
    connect(window, SIGNAL(graphWindowActivated(QTabWidget*)), this, SIGNAL(graphWindowActivated(QTabWidget*)));
    connect(window, SIGNAL(graphWindowDeactivated(QTabWidget*)), this, SIGNAL(graphWindowDeactivated(QTabWidget*)));
    emit showWindow(window);
  setCurrentIndex(item->index());
  }
  else
  {
    QuadraticCumulResultWindow * window = new QuadraticCumulResultWindow(dynamic_cast<CentralTendencyItem*>(item));
    emit showWindow(window);
  }
  setCurrentIndex(item->index());
}


void StudyTreeView::createSensitivityAnalysisResult(AnalysisItem * item)
{
  emit checkIfWindowResultExists(item);
  if (item->getAnalysis().getImplementation()->getClassName() == "SobolAnalysis")
  {
    SobolResultWindow * window = new SobolResultWindow(dynamic_cast<SensitivityAnalysisItem*>(item));
    connect(window, SIGNAL(graphWindowActivated(QTabWidget*)), this, SIGNAL(graphWindowActivated(QTabWidget*)));
    connect(window, SIGNAL(graphWindowDeactivated(QTabWidget*)), this, SIGNAL(graphWindowDeactivated(QTabWidget*)));
    emit showWindow(window);
  }
  else
  {
    SRCResultWindow * window = new SRCResultWindow(dynamic_cast<SensitivityAnalysisItem*>(item));
    connect(window, SIGNAL(graphWindowActivated(QTabWidget*)), this, SIGNAL(graphWindowActivated(QTabWidget*)));
    connect(window, SIGNAL(graphWindowDeactivated(QTabWidget*)), this, SIGNAL(graphWindowDeactivated(QTabWidget*)));
    emit showWindow(window);
  }
  setCurrentIndex(item->index());
}


void StudyTreeView::createReliabilityAnalysisResult(AnalysisItem * item)
{
  emit checkIfWindowResultExists(item);
  MonteCarloReliabilityResultWindow * window = new MonteCarloReliabilityResultWindow(dynamic_cast<ReliabilityAnalysisItem*>(item));
  emit showWindow(window);
  setCurrentIndex(item->index());
}


void StudyTreeView::dumpOTStudy()
{
  QModelIndex index = selectionModel()->currentIndex();
  QStandardItem * selectedItem = treeViewModel_->itemFromIndex(index);
  OTStudyItem * item = dynamic_cast<OTStudyItem*>(selectedItem);

  QString fileName = QFileDialog::getSaveFileName(this, tr("Dump OTStudy..."),
                     QDir::homePath(),
                     tr("Python source files (*.py)"));

  if ( ! fileName.isEmpty() )
  {
    if (!fileName.endsWith(".py"))
      fileName += ".py";

    QFile file(fileName);

    // check
    if (!file.open(QFile::WriteOnly))
    {
      std::cout << "cannot open" << file.fileName().toStdString() << std::endl;
    }
    // fill
    else
    {
      QTextStream out(&file);
      QString script("#! /usr/bin/env python\n\nfrom __future__ import print_function\nimport openturns as ot\nimport otguibase\n\n");
      out << script;
      script = item->dumpOTStudy();
      out << script;
      file.setPermissions(QFile::ReadUser|QFile::WriteUser|QFile::ExeUser|QFile::ReadGroup|QFile::ExeGroup|QFile::ReadOther|QFile::ExeOther);
      file.close();
    }
  }
}


void StudyTreeView::loadOTStudy()
{
  QString fileName = QFileDialog::getOpenFileName(this, tr("Load OTStudy..."),
                     QDir::homePath(),
                     tr("Python source files (*.py)"));

  if (!fileName.isEmpty())
  {
    QFile file(fileName);

    // check
    if (!file.open(QFile::ReadOnly))
    {
    //     "cannot open"<<file.fileName();
//     QMessageBox::warning(QApplication::activeWindow(), QObject::tr("otgui"),
//                          QObject::tr("Cannot read file %1:\n%2")
//                          .arg(fileName)
//                          .arg(file.errorString()));
    }
    // load
    else
      emit loadPythonScript(fileName);
  }
}


void StudyTreeView::selectedItemChanged(const QModelIndex & index)
{
  QStandardItem * selectedItem = treeViewModel_->itemFromIndex(index);
  if (selectedItem->data(Qt::UserRole).toString()!="Study")
    emit itemSelected(selectedItem);
}
}