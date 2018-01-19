//                                               -*- C++ -*-
/**
 *  @brief QStackedWidget to define marginals parameters
 *
 *  Copyright 2015-2017 EDF-Phimeca
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
#include "otgui/DependenciesWidget.hxx"

#include "otgui/CopulaWidget.hxx"
#include "otgui/CopulaDefinitionWizard.hxx"
#include "otgui/DependenciesTableModel.hxx"

#include <QVBoxLayout>
#include <QSplitter>
#include <QHeaderView>
#include <QPushButton>
#include <QMessageBox>

using namespace OT;

namespace OTGUI
{

DependenciesWidget::DependenciesWidget(ProbabilisticModelItem * item, QWidget *parent)
  : QWidget(parent)
  , physicalModel_(item->getPhysicalModel())
{
  connect(item, SIGNAL(copulaChanged()), this, SIGNAL(updateTable()));
  buildInterface();
}


void DependenciesWidget::buildInterface()
{
  QVBoxLayout * mainLayout = new QVBoxLayout(this);

  QSplitter * hSplitter = new QSplitter;

  // left side
  QWidget * leftSideWidget = new QWidget;
  QVBoxLayout * leftSideLayout = new QVBoxLayout(leftSideWidget);

  // - table view
  tableView_ = new QTableView;
  tableView_->setSelectionMode(QAbstractItemView::SingleSelection);
  tableView_->setSelectionBehavior(QAbstractItemView::SelectRows);
  tableView_->horizontalHeader()->setStretchLastSection(true);

  // - table model
  DependenciesTableModel * tableModel = new DependenciesTableModel(physicalModel_, tableView_);
  tableView_->setModel(tableModel);

  connect(tableModel, SIGNAL(dataUpdated(OT::ComposedCopula)), this, SLOT(updateCopulaWidgets(OT::ComposedCopula)));
  connect(this, SIGNAL(addTableLine(OT::Copula)), tableModel, SLOT(addLine(OT::Copula)));
  connect(this, SIGNAL(updateTable()), tableModel, SLOT(updateData()));
  connect(tableView_->selectionModel(), SIGNAL(currentRowChanged(QModelIndex, QModelIndex)), this, SLOT(selectedItemChanged(QModelIndex, QModelIndex)));

  leftSideLayout->addWidget(tableView_);

  // - add button
  QPushButton * addButton = new QPushButton(QIcon(":/images/list-add.png"), tr("Add"));
  addButton->setToolTip(tr("Add a copula"));
  connect(addButton, SIGNAL(clicked(bool)), this, SLOT(openCopulaDefinitionWizard()));

  // - remove button
  QPushButton * removeButton = new QPushButton(QIcon(":/images/list-remove.png"), tr("Remove"));
  removeButton->setToolTip(tr("Remove the selected copula"));
  connect(removeButton, SIGNAL(clicked(bool)), this, SLOT(removeCopula()));
  connect(this, SIGNAL(removeTableLine(QModelIndex)), tableModel, SLOT(removeLine(QModelIndex)));

  QHBoxLayout * buttonsLayout = new QHBoxLayout;
  buttonsLayout->addStretch();
  buttonsLayout->addWidget(addButton);
  buttonsLayout->addWidget(removeButton);
  leftSideLayout->addLayout(buttonsLayout);

  hSplitter->addWidget(leftSideWidget);

  // right side
  rightSideOfSplitterStackedWidget_ = new ResizableStackedWidget;

  // 1- If physical model has not dependent variables: use a dummy widget
  QWidget * dummyWidget = new QWidget;
  QVBoxLayout * dummyWidgetLayout = new QVBoxLayout(dummyWidget);
  QLabel * messLabel = new QLabel(tr("All the input variables are deterministic or independent. To add dependency, when the model has stochastic variables, click on the Add button below the table"));
  messLabel->setWordWrap(true);
  dummyWidgetLayout->addWidget(messLabel);
  dummyWidgetLayout->addStretch();
  rightSideOfSplitterStackedWidget_->addWidget(dummyWidget);

  // 2- If physical model has dependent variables
  copulaStackedWidget_ = new ResizableStackedWidget;
  rightSideOfSplitterStackedWidget_->addWidget(copulaStackedWidget_);
  hSplitter->addWidget(rightSideOfSplitterStackedWidget_);
  hSplitter->setStretchFactor(1, 3);

  rightSideOfSplitterStackedWidget_->setCurrentIndex(1);

  mainLayout->addWidget(hSplitter);

  // update widgets
  tableModel->updateData();
  tableView_->selectRow(0);
}


void DependenciesWidget::removeCopula()
{
  if (tableView_->selectionModel()->hasSelection())
  {
    QModelIndex index = tableView_->selectionModel()->currentIndex();
    int previousRow = index.row() - 1;


    copulaStackedWidget_->removeWidget(copulaStackedWidget_->widget(index.row()));
    emit removeTableLine(index);
    tableView_->selectRow(previousRow < 0 ? 0 : previousRow);
    copulaStackedWidget_->setCurrentIndex(previousRow < 0 ? 0 : previousRow);

    if (!tableView_->model()->rowCount())
      rightSideOfSplitterStackedWidget_->setCurrentIndex(0);
  }
}


void DependenciesWidget::updateCopulaWidgets(const ComposedCopula &copula)
{
  const int nbWidgets = copulaStackedWidget_->count();
  for (int i = 0; i < nbWidgets; ++i)
  {
    QWidget * widget = copulaStackedWidget_->currentWidget();
    copulaStackedWidget_->removeWidget(widget);
    widget->deleteLater();
  }
  if (copula.hasIndependentCopula())
  {
    rightSideOfSplitterStackedWidget_->setCurrentIndex(0);
    return;
  }
  for (UnsignedInteger i = 0; i < copula.getCopulaCollection().getSize(); ++i)
  {
    CopulaWidget * newWidget = new CopulaWidget(physicalModel_, copula.getCopulaCollection()[i], this);
    copulaStackedWidget_->addWidget(newWidget);
  }
  tableView_->selectRow(0);
}


void DependenciesWidget::selectedItemChanged(const QModelIndex &current, const QModelIndex &previous)
{
  if (!current.isValid())
  {
    rightSideOfSplitterStackedWidget_->setCurrentIndex(0);
    return;
  }
  rightSideOfSplitterStackedWidget_->setCurrentIndex(1);
  copulaStackedWidget_->setCurrentIndex(current.row());
}


void DependenciesWidget::openCopulaDefinitionWizard()
{
  // check
  Collection<Copula> coll(physicalModel_.getCopulaCollection());
  Description independentVar;
  for (UnsignedInteger i = 0; i < coll.getSize(); ++i)
  {
    if (coll[i].getImplementation()->getClassName() == "IndependentCopula")
      independentVar.add(coll[i].getDescription());
  }
  if (independentVar.getSize() < 2)
  {
    QMessageBox::critical(this,
                          tr("Error"),
                          tr("There is %1 independent variable. But a copula is defined with at least two variables.").arg(independentVar.getSize()));
    return;
  }
  // open a wizard to define a new copula
  CopulaDefinitionWizard wizard(physicalModel_, this);
  if (wizard.exec())
  {
    const Copula newCopula(wizard.getCopula());
    emit addTableLine(newCopula);
    CopulaWidget * newWidget = new CopulaWidget(physicalModel_, newCopula, this);
    copulaStackedWidget_->addWidget(newWidget);
    tableView_->selectRow(tableView_->model()->rowCount() - 1);
    copulaStackedWidget_->setCurrentIndex(tableView_->model()->rowCount() - 1);
  }
}
}
