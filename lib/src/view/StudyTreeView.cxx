//                                               -*- C++ -*-
/**
 *  @brief QTreeView to list studies
 *
 *  Copyright 2015-2016 EDF
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

#include "otgui/PhysicalModelWindow.hxx"
#include "otgui/ProbabilisticModelWindow.hxx"
#include "otgui/LimitStateWindow.hxx"
#include "otgui/DesignOfExperimentWizard.hxx"
#include "otgui/DesignOfExperimentWindow.hxx"
#include "otgui/ModelEvaluationWizard.hxx"
#include "otgui/ModelEvaluationResultWindow.hxx"
#include "otgui/CentralTendencyWizard.hxx"
#include "otgui/MonteCarloResultWindow.hxx"
#include "otgui/TaylorExpansionsMomentsResultWindow.hxx"
#include "otgui/SensitivityAnalysisWizard.hxx"
#include "otgui/SobolResultWindow.hxx"
#include "otgui/SRCResultWindow.hxx"
#include "otgui/ReliabilityAnalysisWizard.hxx"
#include "otgui/MonteCarloReliabilityResultWindow.hxx"

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
  // new physical model action
  newPhysicalModel_ = new QAction(tr("New physical model"), this);
  newPhysicalModel_->setStatusTip(tr("Create a new physical model"));
  connect(newPhysicalModel_, SIGNAL(triggered()), this, SLOT(createNewPhysicalModel()));

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
    actions.append(newPhysicalModel_);
    actions.append(saveOTStudy_);
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
  }
  else if (dataType == "LimitState")
  {
    actions.append(newThresholdExceedance_);
  }
  else if (dataType == "ModelEvaluation" || dataType == "MonteCarloAnalysis" ||
           dataType == "TaylorExpansionsMomentsAnalysis" || dataType == "SobolAnalysis" ||
           dataType == "SRCAnalysis" || dataType == "MonteCarloReliabilityAnalysis")
  {
    actions.append(runAnalysis_);
    actions.append(removeAnalysis_);
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
  connect(window, SIGNAL(graphWindowActivated(QWidget*)), this, SIGNAL(graphWindowActivated(QWidget*)));
  connect(window, SIGNAL(graphWindowDeactivated(QWidget*)), this, SIGNAL(graphWindowDeactivated(QWidget*)));
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
  connect(window, SIGNAL(graphWindowActivated(QWidget*)), this, SIGNAL(graphWindowActivated(QWidget*)));
  connect(window, SIGNAL(graphWindowDeactivated(QWidget*)), this, SIGNAL(graphWindowDeactivated(QWidget*)));
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
    emit checkIfWindowResultExists(item);
    item->getDesignOfExperiment().clearResult();
    createNewDesignOfExperimentWindow(item);
  }
}


void StudyTreeView::runAnalysis()
{
  QModelIndex index = selectionModel()->currentIndex();
  QStandardItem * selectedItem = treeViewModel_->itemFromIndex(index);
  AnalysisItem * item = dynamic_cast<AnalysisItem*>(selectedItem);

  OTguiWizard * wizard = 0;
  if (item->data(Qt::UserRole).toString() == "ModelEvaluation")
  {
    wizard = new ModelEvaluationWizard(item->getAnalysis());
  }
  else if (item->data(Qt::UserRole).toString() == "MonteCarloAnalysis" ||
           item->data(Qt::UserRole).toString() == "TaylorExpansionsMomentsAnalysis")
  {
    wizard = new CentralTendencyWizard(item->getAnalysis());
  }
  else if (item->data(Qt::UserRole).toString() == "SobolAnalysis" ||
           item->data(Qt::UserRole).toString() == "SRCAnalysis")
  {
    wizard = new SensitivityAnalysisWizard(item->getAnalysis());
  }
  else if (item->data(Qt::UserRole).toString() == "MonteCarloReliabilityAnalysis")
  {
    wizard = new ReliabilityAnalysisWizard(item->getAnalysis());
  }

  if (wizard)
  {
    connect(wizard, SIGNAL(analysisChanged(Analysis)), item, SLOT(updateAnalysis(Analysis)));
    if (wizard->exec())
      item->getAnalysis().run();
  }
}


void StudyTreeView::removeAnalysis()
{
  QModelIndex index = selectionModel()->currentIndex();
  QStandardItem * selectedItem = treeViewModel_->itemFromIndex(index);

  treeViewModel_->getOTStudyItem(index)->getOTStudy()->removeAnalysis(dynamic_cast<AnalysisItem*>(selectedItem)->getAnalysis());
}


void StudyTreeView::createAnalysisResultWindow(AnalysisItem* item)
{
  emit checkIfWindowResultExists(item);

  OTguiSubWindow * resultWindow = 0;
  QString data = item->data(Qt::UserRole).toString();
  if (data == "ModelEvaluation")
    resultWindow = new ModelEvaluationResultWindow(item);
  else if (data == "MonteCarloAnalysis")
    resultWindow = new MonteCarloResultWindow(item);
  else if (data == "TaylorExpansionsMomentsAnalysis")
    resultWindow = new TaylorExpansionsMomentsResultWindow(item);
  else if (data == "SobolAnalysis")
    resultWindow = new SobolResultWindow(item);
  else if (data == "SRCAnalysis")
    resultWindow = new SRCResultWindow(item);
  else if (data == "MonteCarloReliabilityAnalysis")
    resultWindow = new MonteCarloReliabilityResultWindow(item);

  if (resultWindow)
  {
    connect(resultWindow, SIGNAL(graphWindowActivated(QWidget*)), this, SIGNAL(graphWindowActivated(QWidget*)));
    connect(resultWindow, SIGNAL(graphWindowDeactivated(QWidget*)), this, SIGNAL(graphWindowDeactivated(QWidget*)));
    emit showWindow(resultWindow);
    setCurrentIndex(item->index());
  }
}


void StudyTreeView::dumpOTStudy()
{
  OTStudyItem * item = treeViewModel_->getOTStudyItem(selectionModel()->currentIndex());

  QString fileName = QFileDialog::getSaveFileName(this, tr("Dump OTStudy..."),
                     QDir::homePath(),
                     tr("Python source files (*.py)"));

  if (!fileName.isEmpty())
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


void StudyTreeView::saveOTStudy()
{
  OTStudyItem * item = treeViewModel_->getOTStudyItem(selectionModel()->currentIndex());

  if (QFileInfo(item->getOTStudy()->getFileName().c_str()).exists())
  {
    QApplication::setOverrideCursor(Qt::WaitCursor);
    item->getOTStudy()->save(item->getOTStudy()->getFileName());
    QApplication::restoreOverrideCursor();
  }
  else
  {
    saveAsOTStudy();
  }
}


void StudyTreeView::saveAsOTStudy()
{
  OTStudyItem * item = treeViewModel_->getOTStudyItem(selectionModel()->currentIndex());

  QString fileName = QFileDialog::getSaveFileName(this, tr("Save OTStudy..."),
                     QDir::homePath(),
                     tr("XML files (*.xml)"));

  if (!fileName.isEmpty())
  {
    if (!fileName.endsWith(".xml"))
      fileName += ".xml";

    QFile file(fileName);

    // check
    if (!file.open(QFile::WriteOnly))
    {
      std::cout << "cannot open" << file.fileName().toStdString() << std::endl;
    }
    // fill
    else
    {
      QApplication::setOverrideCursor(Qt::WaitCursor);
      item->getOTStudy()->save(fileName.toStdString());
      QApplication::restoreOverrideCursor();
    }
  }
}


void StudyTreeView::openOTStudy()
{
  QString fileName = QFileDialog::getOpenFileName(this, tr("Open an existing OTStudy"),
                     QDir::homePath(),
                     tr("XML files (*.xml)"));

  if (!fileName.isEmpty())
  {
    QFileInfo file(fileName);
    if (file.exists() && !OTStudy::GetOTStudiesFileNames().__contains__(file.absoluteFilePath().toStdString()))
    {
      QApplication::setOverrideCursor(Qt::WaitCursor);
      OTStudy::OpenOTStudy(fileName.toStdString());
      QApplication::restoreOverrideCursor();
    }
    else
    {
      // put this in OTStudy::OpenOTStudy
      QString message;
      if (!file.exists())
      {
        message = tr("The file '%1' does not exist.").arg(file.baseName());
      }
      else if (OTStudy::GetOTStudiesFileNames().__contains__(file.absoluteFilePath().toStdString()))
      {
        message = tr("The file '%1' is already opened.").arg(file.baseName());
      }
      QMessageBox::warning(this, tr("Warning"), message);
    }
  }
}


void StudyTreeView::selectedItemChanged(const QModelIndex & index)
{
  QStandardItem * selectedItem = treeViewModel_->itemFromIndex(index);
  if (selectedItem->data(Qt::UserRole).toString() != "Study")
    emit itemSelected(selectedItem);
}
}