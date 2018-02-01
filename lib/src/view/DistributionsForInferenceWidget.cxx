//                                               -*- C++ -*-
/**
 *  @brief QWidget used in the QWizard for inference
 *
 *  Copyright 2015-2018 EDF-Phimeca
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
#include "otgui/DistributionsForInferenceWidget.hxx"

#include "otgui/TranslationManager.hxx"

#include <QHeaderView>
#include <QPushButton>
#include <QVBoxLayout>

using namespace OT;

namespace OTGUI
{

DistributionsForInferenceWidget::DistributionsForInferenceWidget(const QStringList &distributions, const Description &variables, QWidget *parent)
  : QWidget(parent)
  , variables_(variables)
  , distributions_(distributions)
  , tableView_(0)
  , tableModel_(0)
  , addComboBox_(0)
{
  Q_ASSERT(variables_.getSize() > 0);
  buildInterface();
}


void DistributionsForInferenceWidget::buildInterface()
{
  QVBoxLayout * mainLayout = new QVBoxLayout(this);

  // list distributions
  QStringList notUsedDistributions;
  QStringList allDistributions;
  if (variables_.getSize() == 1)
    allDistributions = TranslationManager::GetAvailableDistributions();
  else if (variables_.getSize() > 2)
    allDistributions << TranslationManager::GetTranslatedCopulaName("Normal");
  else
    allDistributions = TranslationManager::GetAvailableCopulas();

  for (int i = 0; i < allDistributions.size(); ++i)
  {
    if (!distributions_.contains(allDistributions[i]))
      notUsedDistributions << allDistributions[i];
  }
  notUsedDistributions << tr("All");

  // distributions table
  // - table view
  tableView_ = new QTableView;
  tableView_->horizontalHeader()->setStretchLastSection(true);
  tableView_->verticalHeader()->hide();
  tableView_->setSelectionBehavior(QAbstractItemView::SelectRows);
  tableView_->setSelectionMode(QAbstractItemView::ExtendedSelection);
  mainLayout->addWidget(tableView_);

  // - table model
  tableModel_ = new DistributionsTableModel(distributions_, allDistributions, tableView_);
  connect(tableModel_, SIGNAL(distributionsListChanged(QStringList)), this, SIGNAL(distributionsListChanged(QStringList)));

  tableView_->setModel(tableModel_);
  tableView_->selectRow(0);

  // Add button
  QHBoxLayout * buttonsLayout = new QHBoxLayout;
  buttonsLayout->addStretch();
  addComboBox_ = new TitledComboBox(QIcon(":/images/list-add.png"), tr("Add"));
  addComboBox_->addItems(notUsedDistributions);
  addComboBox_->setEnabled(variables_.getSize() <= 2);

  buttonsLayout->addWidget(addComboBox_);
  connect(addComboBox_, SIGNAL(textActivated(QString)), tableModel_, SLOT(appendDistribution(QString)));
  connect(addComboBox_, SIGNAL(activated(int)), this, SLOT(addSelectedDistribution(int)));

  // Remove button
  QPushButton * removeButton = new QPushButton(tr("Remove"));
  removeButton->setIcon(QIcon(":/images/list-remove.png"));
  removeButton->setEnabled(variables_.getSize() <= 2);
  connect(removeButton, SIGNAL(pressed()), this, SLOT(removeSelectedDistribution()));
  buttonsLayout->addWidget(removeButton);

  mainLayout->addLayout(buttonsLayout);
}


void DistributionsForInferenceWidget::removeSelectedDistribution()
{
  const QModelIndexList indexList = tableView_->selectionModel()->selectedRows();

  QStringList allDistributions;
  if (variables_.getSize() == 1)
    allDistributions = TranslationManager::GetAvailableDistributions();
  else
    allDistributions = TranslationManager::GetAvailableCopulas();

  // update Add button items
  addComboBox_->removeItem(addComboBox_->count() - 1);
  for (int i = indexList.size() - 1; i >= 0; --i)
  {
    QString selectedDistribution = tableModel_->data(indexList[i], Qt::DisplayRole).toString();
    if (allDistributions.contains(selectedDistribution))
      addComboBox_->addItem(selectedDistribution);
  }
  addComboBox_->model()->sort(0);
  addComboBox_->addItem(tr("All"));

  // update distributions table model
  QStringList distributions;
  for (int i = 0; i < allDistributions.size(); ++i)
    if (addComboBox_->findText(allDistributions[i]) == -1)
      distributions << allDistributions[i];

  tableModel_->updateData(distributions);
  tableView_->selectRow(0);

  // emit signal to parent widget
  emit distributionsListChanged(distributions);
  emit distributionsListChanged(variables_, distributions);
}


void DistributionsForInferenceWidget::addSelectedDistribution(int index)
{
  if (index < addComboBox_->count() - 1)
  {
    addComboBox_->removeItem(index);
  }
  else
  {
    addComboBox_->clear();
    addComboBox_->addItem(tr("All"));
  }
  emit distributionsListChanged(variables_, tableModel_->getDistributions());
}
}
