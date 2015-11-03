#include "StudyTreeView.hxx"

#include <QModelIndex>
#include <QMenu>
#include <QHeaderView>
#include <QMdiSubWindow>
#include <QFileDialog>

#include "PhysicalModelWindow.hxx"
#include "ParametricCalculusWizard.hxx"
#include "ParametricCalculusResultWindow.hxx"
#include "DistributionAnalysisWizard.hxx"
#include "MonteCarloCalculusResultWindow.hxx"
#include "QuadraticCumulCalculusResultWindow.hxx"
#include "SensitivityAnalysisWizard.hxx"
#include "SobolCalculusResultWindow.hxx"

#include <iostream>

namespace OTGUI {

StudyTreeView::StudyTreeView(QWidget * parent)
  : QTreeView(parent)
  , treeViewModel_(new StudyTreeViewModel())
{
  OTStudy::SetInstanceObserver(treeViewModel_);
  setModel(treeViewModel_);
  connect(treeViewModel_, SIGNAL(newPhysicalModelCreated(PhysicalModelItem*)), this, SLOT(createNewPhysicalModelWindow(PhysicalModelItem*)));
  connect(treeViewModel_, SIGNAL(newCalculusCreated(CalculusItem*)), this, SLOT(createCalculusConnection(CalculusItem*)));

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
      contextMenu->addAction(newParametricCalculus_);
      contextMenu->addAction(newDistributionAnalysis_);
      contextMenu->addAction(newSensitivityAnalysis_);
    }
    if (data=="ParametricCalculus")
    {
      contextMenu->addAction(runParametricCalculus_);
    }
    if (data=="DistributionAnalysis")
    {
      contextMenu->addAction(runDistributionAnalysis_);
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

  newParametricCalculus_ = new QAction(tr("New parametric calculus"), this);
  newParametricCalculus_->setStatusTip(tr("Create a new parametric calculus"));
  connect(newParametricCalculus_, SIGNAL(triggered()), this, SLOT(createNewParametricCalculus()));

  newDistributionAnalysis_ = new QAction(tr("New distribution analysis"), this);
  newDistributionAnalysis_->setStatusTip(tr("Create a new distribution analysis"));
  connect(newDistributionAnalysis_, SIGNAL(triggered()), this, SLOT(createNewDistributionAnalysis()));

  newSensitivityAnalysis_ = new QAction(tr("New sensitivity analysis"), this);
  newSensitivityAnalysis_->setStatusTip(tr("Create a new sensitivity analysis"));
  connect(newSensitivityAnalysis_, SIGNAL(triggered()), this, SLOT(createNewSensitivityAnalysis()));

  runParametricCalculus_ = new QAction(tr("Run"), this);
  runParametricCalculus_->setStatusTip(tr("Run the parametric calculus"));
  connect(runParametricCalculus_, SIGNAL(triggered()), this, SLOT(runParametricCalculus()));

  runDistributionAnalysis_ = new QAction(tr("Run"), this);
  runDistributionAnalysis_->setStatusTip(tr("Run the distribution analysis"));
  connect(runDistributionAnalysis_, SIGNAL(triggered()), this, SLOT(runDistributionAnalysis()));

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


void StudyTreeView::createNewParametricCalculus()
{
  QModelIndex physicalModelIndex = selectionModel()->currentIndex();
  PhysicalModelItem * physicalModelItem = static_cast<PhysicalModelItem*>(treeViewModel_->itemFromIndex(physicalModelIndex));
  StudyItem * studyItem = static_cast<StudyItem*>(physicalModelItem->QStandardItem::parent());
  ParametricCalculusWizard * wizard = new ParametricCalculusWizard(studyItem->getStudy(), physicalModelItem->getPhysicalModel());

  if (wizard->exec())
  {
    wizard->validate();
    setExpanded(physicalModelIndex, true);
  }
}


void StudyTreeView::createNewDistributionAnalysis()
{
  QModelIndex physicalModelIndex = selectionModel()->currentIndex();
  PhysicalModelItem * physicalModelItem = static_cast<PhysicalModelItem*>(treeViewModel_->itemFromIndex(physicalModelIndex));
  StudyItem * studyItem = static_cast<StudyItem*>(physicalModelItem->QStandardItem::parent());
  DistributionAnalysisWizard * wizard = new DistributionAnalysisWizard(studyItem->getStudy(), physicalModelItem->getPhysicalModel());

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


void StudyTreeView::createCalculusConnection(CalculusItem * item)
{
  connect(item, SIGNAL(calculusFinished(CalculusItem *)), this, SIGNAL(checkIfWindowResultExists(CalculusItem *)));
  QString data = item->data(Qt::UserRole).toString();
  if (data == "ParametricCalculus")
    connect(item, SIGNAL(calculusFinished(CalculusItem *)), this, SLOT(createParametricCalculusResult(CalculusItem *)));
  else if (data == "DistributionAnalysis")
    connect(item, SIGNAL(calculusFinished(CalculusItem *)), this, SLOT(createDistributionAnalysisResult(CalculusItem *)));
  else if (data == "SensitivityAnalysis")
  connect(item, SIGNAL(calculusFinished(CalculusItem *)), this, SLOT(createSensitivityAnalysisResult(CalculusItem*)));
}


void StudyTreeView::runParametricCalculus()
{
  QModelIndex index = selectionModel()->currentIndex();
  QStandardItem * selectedItem = treeViewModel_->itemFromIndex(index);

  ParametricCalculusItem * item = static_cast<ParametricCalculusItem*>(selectedItem);

  ParametricCalculusWizard * wizard = new ParametricCalculusWizard(item->getCalculus());
  if (wizard->exec())
  {
    item->getCalculus().run();
  }
}


void StudyTreeView::runDistributionAnalysis()
{
  QModelIndex index = selectionModel()->currentIndex();
  QStandardItem * selectedItem = treeViewModel_->itemFromIndex(index);

  DistributionAnalysisItem * item = static_cast<DistributionAnalysisItem*>(selectedItem);

  DistributionAnalysisWizard * wizard = new DistributionAnalysisWizard(item->getCalculus());
  connect(wizard, SIGNAL(calculusChanged(Calculus)), item, SLOT(updateCalculus(Calculus)));
  if (wizard->exec())
  {
    item->getCalculus().run();
  }
}


void StudyTreeView::runSensitivityAnalysis()
{
  QModelIndex index = selectionModel()->currentIndex();
  QStandardItem * selectedItem = treeViewModel_->itemFromIndex(index);

  SensitivityAnalysisItem * item = static_cast<SensitivityAnalysisItem*>(selectedItem);

  SensitivityAnalysisWizard * wizard = new SensitivityAnalysisWizard(item->getCalculus());
  connect(wizard, SIGNAL(calculusChanged(Calculus)), item, SLOT(updateCalculus(Calculus)));
  if (wizard->exec())
  {
    item->getCalculus().run();
  }
}


void StudyTreeView::createParametricCalculusResult(CalculusItem * item)
{
  ParametricCalculusResultWindow * window = new ParametricCalculusResultWindow(static_cast<ParametricCalculusItem*>(item));
  emit showWindow(window);
}


void StudyTreeView::createDistributionAnalysisResult(CalculusItem * item)
{
  if (item->getCalculus().getImplementation()->getClassName() == "MonteCarloCalculus")
  {
    MonteCarloCalculusResultWindow * window = new MonteCarloCalculusResultWindow(static_cast<DistributionAnalysisItem*>(item));
    emit showWindow(window);
  }
  else
  {
    QuadraticCumulCalculusResultWindow * window = new QuadraticCumulCalculusResultWindow(static_cast<DistributionAnalysisItem*>(item));
    emit showWindow(window);
  }
}


void StudyTreeView::createSensitivityAnalysisResult(CalculusItem * item)
{
  if (item->getCalculus().getImplementation()->getClassName() == "SobolCalculus")
  {
    SobolCalculusResultWindow * window = new SobolCalculusResultWindow(static_cast<SensitivityAnalysisItem*>(item));
    emit showWindow(window);
  }
  else
  {
//     QuadraticCumulCalculusResultWindow * window = new QuadraticCumulCalculusResultWindow(static_cast<DistributionAnalysisItem*>(item));
//     emit showWindow(window);
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
