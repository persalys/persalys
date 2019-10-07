//                                               -*- C++ -*-
/**
 *  @brief QStackedWidget to define marginals parameters
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
#include "persalys/DependenciesWidget.hxx"

#include "persalys/CopulaWidget.hxx"
#include "persalys/DependenciesTableModel.hxx"
#include "persalys/TranslationManager.hxx"
#include "persalys/ComboBoxDelegate.hxx"
#include "persalys/VariablesSelectionTableModel.hxx"
#include "persalys/CheckableHeaderView.hxx"
#include "persalys/CopulaInferenceResultWizard.hxx"
#include "persalys/StudyItem.hxx"
#include "persalys/QtTools.hxx"

#include <openturns/NormalCopula.hxx>

#include <QSplitter>
#include <QHeaderView>
#include <QPushButton>
#include <QMessageBox>
#include <QToolButton>
#include <QMessageBox>

using namespace OT;

namespace PERSALYS
{

DependenciesWidget::DependenciesWidget(PhysicalModelItem * item, QWidget *parent)
  : QWidget(parent)
  , failSoftMode_(false)
  , study_(item->getParentStudyItem()->getStudy())
  , physicalModel_(item->getPhysicalModel())
{
  connect(item, SIGNAL(copulaChanged()), this, SLOT(updateWidgets()));
  buildInterface();
}


DependenciesWidget::DependenciesWidget(const PhysicalModel& model, QWidget *parent)
  : QWidget(parent)
  , failSoftMode_(true)
  , study_()
  , physicalModel_(model)
{
  buildInterface();
}


void DependenciesWidget::buildInterface()
{
  QVBoxLayout * mainLayout = new QVBoxLayout(this);

  QSplitter * mainSplitter = new QSplitter;

  QWidget * leftWidget = new QWidget;
  QGridLayout * leftWidgetLayout = new QGridLayout(leftWidget);

  // table view for variables
  QTableView * varTableView = new QTableView;
  varTableView->setSelectionMode(QAbstractItemView::NoSelection);
  varTableView->verticalHeader()->hide();
  varTableView->setShowGrid(false);

  // - model
  varTableModel_ = new VariablesSelectionTableModel(physicalModel_.getStochasticInputNames(), Interval::BoolCollection(physicalModel_.getStochasticInputNames().getSize()), varTableView);
  varTableView->setModel(varTableModel_);
  // - horizontal header
  CheckableHeaderView * headerView = new CheckableHeaderView;
  headerView->setStretchLastSection(true);
  varTableView->setHorizontalHeader(headerView);
  varTableView->resizeColumnsToContents();
  leftWidgetLayout->addWidget(varTableView, 0, 0, 2, 1);

  // add button
  QToolButton * arrowButton = new QToolButton;
  arrowButton->setArrowType(Qt::RightArrow);
  arrowButton->setToolTip(tr("Add a group of variables"));
  leftWidgetLayout->addWidget(arrowButton, 0, 1, Qt::AlignBottom);
  connect(arrowButton, SIGNAL(pressed()), this, SLOT(addCopula()));

  // remove button
  arrowButton = new QToolButton;
  arrowButton->setArrowType(Qt::LeftArrow);
  arrowButton->setToolTip(tr("Remove the selected group of variables"));
  leftWidgetLayout->addWidget(arrowButton, 1, 1, Qt::AlignTop);
  connect(arrowButton, SIGNAL(pressed()), this, SLOT(removeCopula()));

  mainSplitter->addWidget(leftWidget);
  mainSplitter->setStretchFactor(0, 2);

  QSplitter * hSplitter = new QSplitter;
  // left side
  QWidget * fakeWidget = new QWidget;
  QHBoxLayout * fakeLayout = new QHBoxLayout(fakeWidget);
  // table view for groups of variables
  tableView_ = new QTableView;
  fakeLayout->addWidget(tableView_);
  tableView_->setSelectionMode(QAbstractItemView::SingleSelection);
  tableView_->setSelectionBehavior(QAbstractItemView::SelectRows);
  tableView_->horizontalHeader()->setStretchLastSection(true);
  tableView_->verticalHeader()->hide();
  tableView_->setStyleSheet("QTableView::item:selected{background-color: " + ApplicationColor["lightColor"] + ";color: doubledarkgray;}");

  // - table model
  tableModel_ = new DependenciesTableModel(physicalModel_, failSoftMode_, tableView_);
  tableView_->setModel(tableModel_);

  connect(this, SIGNAL(removeTableLine(QModelIndex)), tableModel_, SLOT(removeLine(QModelIndex)));
  connect(tableModel_, SIGNAL(dataUpdated(int, OT::Copula)), this, SLOT(updateCopulaWidget(int, OT::Copula)));
  connect(tableModel_, SIGNAL(inferenceResultRequested(const QModelIndex&)), this, SLOT(openWizardToChooseInferenceResult(const QModelIndex&)));
  connect(tableView_->selectionModel(), SIGNAL(currentRowChanged(QModelIndex, QModelIndex)), this, SLOT(selectedItemChanged(QModelIndex, QModelIndex)));

  // - ComboBoxDelegate
  if (!failSoftMode_)
  {
    ComboBoxDelegate * delegate = new ComboBoxDelegate(tableView_);
    delegate->setNoWheelEvent(true);
    tableView_->setItemDelegateForColumn(1, delegate);
  }

  hSplitter->addWidget(fakeWidget);

  // right side
  rightSideOfSplitterStackedWidget_ = new ResizableStackedWidget;
  rightSideOfSplitterStackedWidget_->setContentsMargins(0, 0, 0, 0);

  // 1- If physical model has not dependent variables: use a dummy widget
  QWidget * dummyWidget = new QWidget;
  QVBoxLayout * dummyWidgetLayout = new QVBoxLayout(dummyWidget);
  QLabel * messLabel = new QLabel(tr("To define dependence, the model must have at least two independent stochastic variables."));
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

  mainSplitter->addWidget(hSplitter);
  mainSplitter->setStretchFactor(1, 4);
  mainLayout->addWidget(mainSplitter, 1);

  // QLabel for temporary error message
  errorMessageLabel_ = new TemporaryLabel;
  mainLayout->addWidget(errorMessageLabel_, 0, Qt::AlignBottom);

  // update widgets
  updateWidgets();
}


void DependenciesWidget::updateWidgets()
{
  // update copulas widgets
  // - remove all widgets
  const int nbWidgets = copulaStackedWidget_->count();
  for (int i = 0; i < nbWidgets; ++i)
  {
    QWidget * widget = copulaStackedWidget_->currentWidget();
    widget->disconnect();
    copulaStackedWidget_->removeWidget(widget);
    widget->deleteLater();
  }
  // - get copulas list
  Collection<Copula> coll(physicalModel_.getCopulaCollection());
  Collection<Copula>::iterator iter = coll.begin();
  while (iter != coll.end())
  {
    if ((*iter).getImplementation()->getClassName() == "IndependentCopula")
    {
      iter = coll.erase(iter);
    }
    else
    {
      ++iter;
    }
  }
  // - create copulas widgets
  for (UnsignedInteger i = 0; i < coll.getSize(); ++i)
  {
    CopulaWidget * newWidget = new CopulaWidget(physicalModel_, coll[i], this);
    copulaStackedWidget_->addWidget(newWidget);
    connect(newWidget, SIGNAL(emitErrorMessage(QString)), errorMessageLabel_, SLOT(setTemporaryErrorMessage(QString)));
  }

  // update table
  tableModel_->updateData();
  if (!failSoftMode_)
  {
    for (int i = 0; i < tableModel_->rowCount(); ++i)
    {
      tableView_->openPersistentEditor(tableModel_->index(i, 1));
    }
  }
  tableView_->selectRow(0);

  if (!tableView_->model()->rowCount())
    rightSideOfSplitterStackedWidget_->setCurrentIndex(0);

  // update variables list
  updateVariablesList();
}


void DependenciesWidget::updateCopulaWidget(const int index, const Copula &copula)
{
  CopulaWidget * copulaWidget = static_cast<CopulaWidget*>(copulaStackedWidget_->widget(index));
  Q_ASSERT(copulaWidget);
  copulaWidget->setCopula(copula);
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


void DependenciesWidget::removeCopula()
{
  if (!tableView_->selectionModel()->hasSelection())
    return;

  QModelIndex index = tableView_->selectionModel()->currentIndex();
  int previousRow = index.row() - 1;

  copulaStackedWidget_->removeWidget(copulaStackedWidget_->widget(index.row()));

  // update table
  emit removeTableLine(index);
  tableView_->selectRow(previousRow < 0 ? 0 : previousRow);

  if (!tableView_->model()->rowCount())
    rightSideOfSplitterStackedWidget_->setCurrentIndex(0);

  // update variables list
  updateVariablesList();
}


void DependenciesWidget::addCopula()
{
  const Description selectedVars(varTableModel_->getSelectedVariables());

  // check if at least two variables
  if (selectedVars.getSize() < 2)
  {
    errorMessageLabel_->setTemporaryErrorMessage(tr("Define at least one group of two variables"));
    return;
  }
  // check the variables are not used elsewhere
  Collection<Copula> coll(physicalModel_.getCopulaCollection());
  Description varsInCopula;
  for (UnsignedInteger i = 0; i < coll.getSize(); ++i)
  {
    if (coll[i].getImplementation()->getClassName() != "IndependentCopula")
      varsInCopula.add(coll[i].getDescription());
  }
  for (UnsignedInteger i = 0; i < selectedVars.getSize(); ++i)
  {
    if (varsInCopula.contains(selectedVars[i]))
    {
      const QString errorMessage(tr("The variable %1 is already used to define a copula").arg(selectedVars[i].c_str()));
      errorMessageLabel_->setTemporaryErrorMessage(errorMessage);
      return;
    }
  }

  // new copula
  NormalCopula newCopula(selectedVars.getSize());
  newCopula.setDescription(selectedVars);

  // set copula
  physicalModel_.blockNotification("ProbabilisticModelItem");
  physicalModel_.setCopula(selectedVars, newCopula);
  physicalModel_.blockNotification();

  // add a copula widget
  CopulaWidget * newWidget = new CopulaWidget(physicalModel_, newCopula, this);
  connect(newWidget, SIGNAL(emitErrorMessage(QString)), errorMessageLabel_, SLOT(setTemporaryErrorMessage(QString)));
  copulaStackedWidget_->addWidget(newWidget);

  // update table
  tableModel_->updateData();

  const int lastRow = tableView_->model()->rowCount() - 1;
  tableView_->selectRow(lastRow);
  if (!failSoftMode_)
  {
    for (int i = 0; i < tableView_->model()->rowCount(); ++i)
    {
      tableView_->openPersistentEditor(tableView_->model()->index(i, 1));
    }
  }
  // update variables list
  updateVariablesList();
}


void DependenciesWidget::updateVariablesList()
{
  // - get copulas list
  Collection<Copula> coll(physicalModel_.getCopulaCollection());
  Description independentVars;
  for (UnsignedInteger i = 0; i < coll.getSize(); ++i)
  {
    if (coll[i].getImplementation()->getClassName() == "IndependentCopula")
    {
      independentVars.add(coll[i].getDescription());
    }
  }
  Interval::BoolCollection varEnabled(physicalModel_.getStochasticInputNames().getSize());
  for (UnsignedInteger i = 0; i < physicalModel_.getStochasticInputNames().getSize(); ++i)
    varEnabled[i] = independentVars.contains(physicalModel_.getStochasticInputNames()[i]);
  varTableModel_->updateData(physicalModel_.getStochasticInputNames(), varEnabled);
}


void DependenciesWidget::openWizardToChooseInferenceResult(const QModelIndex& inputIndex)
{
  bool studyHasInferenceResults = false;
  // we need at least one copula inference analysis result to open the wizard
  for (UnsignedInteger i = 0; i < study_.getAnalyses().getSize(); ++i)
  {
    if (study_.getAnalyses()[i].getImplementation()->getClassName() == "CopulaInferenceAnalysis"
        && study_.getAnalyses()[i].hasValidResult())
    {
      studyHasInferenceResults = true;
      break;
    }
  }
  // error message if no inference analyses
  if (!studyHasInferenceResults)
  {
    QMessageBox::critical(this, tr("Error"), tr("The current study has no dependence inference result."));
    return;
  }
  // open a wizard to choose a result
  tableView_->setCurrentIndex(inputIndex);
  const Description currentGroup(tableModel_->getGroup(tableView_->currentIndex()));
  CopulaInferenceResultWizard wizard(study_, currentGroup, this);
  if (wizard.exec())
  {
    // get new copula
    Copula copula(wizard.getCopula());
    copula.setDescription(currentGroup);

    // update the copula
    physicalModel_.blockNotification("ProbabilisticModelItem");
    physicalModel_.setCopula(currentGroup, copula);
    physicalModel_.blockNotification();

    // update widget
    const int currentRow = tableView_->currentIndex().row();
    updateCopulaWidget(currentRow, copula);

    // update table
    for (int i = 0; i < tableView_->model()->rowCount(); ++i)
    {
      tableView_->openPersistentEditor(tableView_->model()->index(i, 1));
    }
  }
}
}
