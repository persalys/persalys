#include "otgui/StudyTreeView.hxx"

#include <QModelIndex>
#include <QMenu>
#include <QHeaderView>
#include <QMdiSubWindow>
#include <QFileDialog>

#include "otgui/PhysicalModelWindow.hxx"
#include "otgui/ParametricAnalysisWizard.hxx"
#include "otgui/ParametricAnalysisResultWindow.hxx"
#include "otgui/CentralTendencyWizard.hxx"
#include "otgui/MonteCarloResultWindow.hxx"
#include "otgui/QuadraticCumulResultWindow.hxx"
#include "otgui/SensitivityAnalysisWizard.hxx"
#include "otgui/SobolResultWindow.hxx"
#include "otgui/SRCResultWindow.hxx"

#include <iostream>

namespace OTGUI {

StudyTreeView::StudyTreeView(QWidget * parent)
  : QTreeView(parent)
  , treeViewModel_(new StudyTreeViewModel())
{
  OTStudy::SetInstanceObserver(treeViewModel_);
  setModel(treeViewModel_);
  connect(treeViewModel_, SIGNAL(newPhysicalModelCreated(PhysicalModelItem*)), this, SLOT(createNewPhysicalModelWindow(PhysicalModelItem*)));
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

  if (index.isValid())
  {
    QMenu * contextMenu = new QMenu(this);
    QString data = treeViewModel_->itemFromIndex(index)->data(Qt::UserRole).toString();
    if (data=="Study")
    {
      contextMenu->addAction(newPhysicalModelAction_);
      contextMenu->addAction(dumpStudy_);
    }
    if (data=="PhysicalModel")
    {
      contextMenu->addAction(newParametricAnalysis_);
      contextMenu->addAction(newCentralTendency_);
      contextMenu->addAction(newSensitivityAnalysis_);
    }
    if (data=="ParametricAnalysis")
    {
      contextMenu->addAction(runParametricAnalysis_);
    }
    if (data=="CentralTendency")
    {
      contextMenu->addAction(runCentralTendency_);
    }
    if (data=="SensitivityAnalysis")
    {
      contextMenu->addAction(runSensitivityAnalysis_);
    }
    contextMenu->exec(mapToGlobal(point));
  }    
}


void StudyTreeView::buildActions()
{
  newPhysicalModelAction_ = new QAction(tr("New physical model"), this);
  newPhysicalModelAction_->setStatusTip(tr("Create a new physical model"));
  connect(newPhysicalModelAction_, SIGNAL(triggered()), this, SLOT(createNewPhysicalModel()));

  newParametricAnalysis_ = new QAction(tr("New parametric analysis"), this);
  newParametricAnalysis_->setStatusTip(tr("Create a new parametric analysis"));
  connect(newParametricAnalysis_, SIGNAL(triggered()), this, SLOT(createNewParametricAnalysis()));

  newCentralTendency_ = new QAction(tr("New central tendency"), this);
  newCentralTendency_->setStatusTip(tr("Create a new central tendency"));
  connect(newCentralTendency_, SIGNAL(triggered()), this, SLOT(createNewCentralTendency()));

  newSensitivityAnalysis_ = new QAction(tr("New sensitivity analysis"), this);
  newSensitivityAnalysis_->setStatusTip(tr("Create a new sensitivity analysis"));
  connect(newSensitivityAnalysis_, SIGNAL(triggered()), this, SLOT(createNewSensitivityAnalysis()));

  runParametricAnalysis_ = new QAction(tr("Run"), this);
  runParametricAnalysis_->setStatusTip(tr("Run the parametric analysis"));
  connect(runParametricAnalysis_, SIGNAL(triggered()), this, SLOT(runParametricAnalysis()));

  runCentralTendency_ = new QAction(tr("Run"), this);
  runCentralTendency_->setStatusTip(tr("Run the central tendency"));
  connect(runCentralTendency_, SIGNAL(triggered()), this, SLOT(runCentralTendency()));

  runSensitivityAnalysis_ = new QAction(tr("Run"), this);
  runSensitivityAnalysis_->setStatusTip(tr("Run the sensitivity analysis"));
  connect(runSensitivityAnalysis_, SIGNAL(triggered()), this, SLOT(runSensitivityAnalysis()));

  dumpStudy_ = new QAction(tr("Dump"), this);
  dumpStudy_->setStatusTip(tr("Dump the study"));
  connect(dumpStudy_, SIGNAL(triggered()), this, SLOT(dumpStudy()));
}


void StudyTreeView::createNewStudy()
{
  treeViewModel_->createNewStudy();
}


void StudyTreeView::createNewPhysicalModel()
{
  QModelIndex parentIndex = selectionModel()->currentIndex();
  treeViewModel_->addPhysicalModelItem(parentIndex);
  setExpanded(parentIndex, true);
}


void StudyTreeView::createNewPhysicalModelWindow(PhysicalModelItem * item)
{
  PhysicalModelWindow * window = new PhysicalModelWindow(item);
  emit showWindow(window);
}


void StudyTreeView::createNewParametricAnalysis()
{
  QModelIndex physicalModelIndex = selectionModel()->currentIndex();
  PhysicalModelItem * physicalModelItem = static_cast<PhysicalModelItem*>(treeViewModel_->itemFromIndex(physicalModelIndex));
  StudyItem * studyItem = static_cast<StudyItem*>(physicalModelItem->QStandardItem::parent());
  ParametricAnalysisWizard * wizard = new ParametricAnalysisWizard(studyItem->getStudy(), physicalModelItem->getPhysicalModel());

  if (wizard->exec())
  {
    wizard->validate();
    setExpanded(physicalModelIndex, true);
  }
}


void StudyTreeView::createNewCentralTendency()
{
  QModelIndex physicalModelIndex = selectionModel()->currentIndex();
  PhysicalModelItem * physicalModelItem = static_cast<PhysicalModelItem*>(treeViewModel_->itemFromIndex(physicalModelIndex));
  StudyItem * studyItem = static_cast<StudyItem*>(physicalModelItem->QStandardItem::parent());
  CentralTendencyWizard * wizard = new CentralTendencyWizard(studyItem->getStudy(), physicalModelItem->getPhysicalModel());

  if (wizard->exec())
  {
    wizard->validate();
    setExpanded(physicalModelIndex, true);
  }
}


void StudyTreeView::createNewSensitivityAnalysis()
{
  QModelIndex physicalModelIndex = selectionModel()->currentIndex();
  PhysicalModelItem * physicalModelItem = static_cast<PhysicalModelItem*>(treeViewModel_->itemFromIndex(physicalModelIndex));
  StudyItem * studyItem = static_cast<StudyItem*>(physicalModelItem->QStandardItem::parent());
  SensitivityAnalysisWizard * wizard = new SensitivityAnalysisWizard(studyItem->getStudy(), physicalModelItem->getPhysicalModel());

  if (wizard->exec())
  {
    wizard->validate();
    setExpanded(physicalModelIndex, true);
  }
}


void StudyTreeView::createAnalysisConnection(AnalysisItem * item)
{
  connect(item, SIGNAL(analysisFinished(AnalysisItem *)), this, SIGNAL(checkIfWindowResultExists(AnalysisItem *)));
  QString data = item->data(Qt::UserRole).toString();
  if (data == "ParametricAnalysis")
    connect(item, SIGNAL(analysisFinished(AnalysisItem *)), this, SLOT(createParametricAnalysisResult(AnalysisItem *)));
  else if (data == "CentralTendency")
    connect(item, SIGNAL(analysisFinished(AnalysisItem *)), this, SLOT(createCentralTendencyResult(AnalysisItem *)));
  else if (data == "SensitivityAnalysis")
  connect(item, SIGNAL(analysisFinished(AnalysisItem *)), this, SLOT(createSensitivityAnalysisResult(AnalysisItem*)));
}


void StudyTreeView::runParametricAnalysis()
{
  QModelIndex index = selectionModel()->currentIndex();
  QStandardItem * selectedItem = treeViewModel_->itemFromIndex(index);

  ParametricAnalysisItem * item = static_cast<ParametricAnalysisItem*>(selectedItem);

  ParametricAnalysisWizard * wizard = new ParametricAnalysisWizard(item->getAnalysis());
  if (wizard->exec())
  {
    item->getAnalysis().run();
  }
}


void StudyTreeView::runCentralTendency()
{
  QModelIndex index = selectionModel()->currentIndex();
  QStandardItem * selectedItem = treeViewModel_->itemFromIndex(index);

  CentralTendencyItem * item = static_cast<CentralTendencyItem*>(selectedItem);

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

  SensitivityAnalysisItem * item = static_cast<SensitivityAnalysisItem*>(selectedItem);

  SensitivityAnalysisWizard * wizard = new SensitivityAnalysisWizard(item->getAnalysis());
  connect(wizard, SIGNAL(analysisChanged(Analysis)), item, SLOT(updateAnalysis(Analysis)));
  if (wizard->exec())
  {
    item->getAnalysis().run();
  }
}


void StudyTreeView::createParametricAnalysisResult(AnalysisItem * item)
{
  ParametricAnalysisResultWindow * window = new ParametricAnalysisResultWindow(static_cast<ParametricAnalysisItem*>(item));
  emit showWindow(window);
}


void StudyTreeView::createCentralTendencyResult(AnalysisItem * item)
{
  if (item->getAnalysis().getImplementation()->getClassName() == "MonteCarloAnalysis")
  {
    MonteCarloResultWindow * window = new MonteCarloResultWindow(static_cast<CentralTendencyItem*>(item));
    emit showWindow(window);
  }
  else
  {
    QuadraticCumulResultWindow * window = new QuadraticCumulResultWindow(static_cast<CentralTendencyItem*>(item));
    emit showWindow(window);
  }
}


void StudyTreeView::createSensitivityAnalysisResult(AnalysisItem * item)
{
  if (item->getAnalysis().getImplementation()->getClassName() == "SobolAnalysis")
  {
    SobolResultWindow * window = new SobolResultWindow(static_cast<SensitivityAnalysisItem*>(item));
    emit showWindow(window);
  }
  else
  {
    SRCResultWindow * window = new SRCResultWindow(static_cast<SensitivityAnalysisItem*>(item));
    emit showWindow(window);
  }
}


void StudyTreeView::dumpStudy()
{
  QModelIndex index = selectionModel()->currentIndex();
  QStandardItem * selectedItem = treeViewModel_->itemFromIndex(index);
  StudyItem * item = static_cast<StudyItem*>(selectedItem);

  QString fileName = QFileDialog::getSaveFileName(this, tr("Dump study..."),
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
      script = item->dumpStudy();
      out << script;
      file.setPermissions(QFile::ReadUser|QFile::WriteUser|QFile::ExeUser|QFile::ReadGroup|QFile::ExeGroup|QFile::ReadOther|QFile::ExeOther);
      file.close();
    }
  }
}


void StudyTreeView::loadStudy()
{
  QString fileName = QFileDialog::getOpenFileName(this, tr("Load study..."),
                     QDir::homePath(),
                     tr("Python source files (*.py)"));

  if ( ! fileName.isEmpty() )
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
