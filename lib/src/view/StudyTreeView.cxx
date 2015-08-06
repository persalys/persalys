#include "StudyTreeView.hxx"

#include <QModelIndex>
#include <QMenu>
#include <QHeaderView>
#include <QMdiSubWindow>
#include <QFileDialog>

#include "PhysicalModelWindow.hxx"
#include "ParametricCalculusWizard.hxx"
#include "ParametricCalculusResultWindow.hxx"

#include <iostream>

namespace OTGUI {

StudyTreeView::StudyTreeView(QWidget * parent)
  : QTreeView(parent)
  , treeViewModel_(new StudyTreeViewModel())
{
  OTStudy::SetInstanceObserver(treeViewModel_);
  setModel(treeViewModel_);
  connect(treeViewModel_, SIGNAL(newPhysicalModelCreated(PhysicalModelItem*)), this, SLOT(createNewPhysicalModelWindow(PhysicalModelItem*)));
  connect(treeViewModel_, SIGNAL(newParametricCalculusCreated(ParametricCalculusItem*)), this, SLOT(createNewParametricCalculusWizard(ParametricCalculusItem*)));

  buildActions();

  setContextMenuPolicy(Qt::CustomContextMenu);
  connect(this, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(onCustomContextMenu(const QPoint &)));

  header()->hide();

  connect(this, SIGNAL(clicked(const QModelIndex &)), this, SLOT(selectedItemChanged(const QModelIndex &)));

}


StudyTreeView::~StudyTreeView()
{
  
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
    }
    if (data=="ParametricCalculus")
    {
      contextMenu->addAction(runCalculus_);
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

  runCalculus_ = new QAction(tr("Run"), this);
  runCalculus_->setStatusTip(tr("Run the parametric calculus"));
  connect(runCalculus_, SIGNAL(triggered()), this, SLOT(runCalculus()));

  dumpStudy_ = new QAction(tr("Dump"), this);
  dumpStudy_->setStatusTip(tr("Dump the study"));
  connect(dumpStudy_, SIGNAL(triggered()), this, SLOT(dumpStudy()));
}


void StudyTreeView::buildContextMenus()
{
  
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
  QModelIndex parentIndex = selectionModel()->currentIndex();
  treeViewModel_->addParametricCalculusItem(parentIndex);
  setExpanded(parentIndex, true);
}


void StudyTreeView::createNewParametricCalculusWizard(ParametricCalculusItem * item)
{
  ParametricCalculusWizard * wizard = new ParametricCalculusWizard(item);

  if (wizard->exec())
    connect(item, SIGNAL(calculusFinished()), this, SLOT(createParametricCalculusResult()));
  else
    item->QStandardItem::parent()->removeRow(item->row());
}


void StudyTreeView::runCalculus()
{
  QModelIndex index = selectionModel()->currentIndex();
  QStandardItem * selectedItem = treeViewModel_->itemFromIndex(index);

  ParametricCalculusItem * item = static_cast<ParametricCalculusItem*>(selectedItem);

  item->runCalculus();
}


void StudyTreeView::createParametricCalculusResult()
{
  QModelIndex index = selectionModel()->currentIndex();
  QStandardItem * selectedItem = treeViewModel_->itemFromIndex(index);
  ParametricCalculusItem * item = static_cast<ParametricCalculusItem*>(selectedItem);

  ParametricCalculusResultWindow * window = new ParametricCalculusResultWindow(item);

  emit showWindow(window);
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