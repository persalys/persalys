//                                               -*- C++ -*-
/**
 *  @brief QWidget used in the QWizard for inference
 *
 *  Copyright 2015-2024 EDF-Phimeca
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
#include "persalys/DistributionsForInferenceWidget.hxx"

#include "persalys/TranslationManager.hxx"

#include <QHeaderView>
#include <QPushButton>
#include <QVBoxLayout>

using namespace OT;

namespace PERSALYS
{

DistributionsForInferenceWidget::DistributionsForInferenceWidget(const QStringList &distributions, const Description &variables, QWidget *parent)
  : QWidget(parent)
  , variables_(variables)
  , distributions_(distributions)
  , tableView_(0)
  , tableModel_(0)
  , addComboBox_(0)
{
  if (variables_.getSize() == 1)
    allDistributions_ = TranslationManager::GetTranslatedContinuousDistributions();
  else if (variables_.getSize() > 2)
    allDistributions_ << TranslationManager::GetTranslatedCopulaName("Normal");
  else
    allDistributions_ = TranslationManager::GetAvailableCopulas();

  Q_ASSERT(variables_.getSize() > 0);
  buildInterface();
}


void DistributionsForInferenceWidget::buildInterface()
{
  QVBoxLayout * mainLayout = new QVBoxLayout(this);

  // list distributions
  QStringList notUsedDistributions;
  notUsedDistributions << tr("All");

  for (int i = 0; i < allDistributions_.size(); ++i)
  {
    if (!distributions_.contains(allDistributions_[i]))
      notUsedDistributions << allDistributions_[i];
  }

  // distributions table
  // - table view
  tableView_ = new QTableView;
  tableView_->horizontalHeader()->setStretchLastSection(true);
  tableView_->verticalHeader()->hide();
  tableView_->setSelectionBehavior(QAbstractItemView::SelectRows);
  tableView_->setSelectionMode(QAbstractItemView::ExtendedSelection);
  mainLayout->addWidget(tableView_);

  // - table model
  tableModel_ = new DistributionsTableModel(distributions_, allDistributions_, variables_.getSize() > 1, tableView_);
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
#if QT_VERSION >= QT_VERSION_CHECK(5, 14, 0)
  connect(addComboBox_, SIGNAL(textActivated(QString)), tableModel_, SLOT(appendDistribution(QString)));
#else
  connect(addComboBox_, SIGNAL(activated(QString)), tableModel_, SLOT(appendDistribution(QString)));
#endif
  connect(addComboBox_, SIGNAL(currentIndexChanged(int)), this, SLOT(addSelectedDistribution(int)));

  // Remove button
  QPushButton * removeButton = new QPushButton(tr("Remove"));
  removeButton->setIcon(QIcon(":/images/list-remove.png"));
  removeButton->setEnabled(variables_.getSize() <= 2);
  connect(removeButton, SIGNAL(pressed()), this, SLOT(removeSelectedDistribution()));
  buttonsLayout->addWidget(removeButton);

  mainLayout->addLayout(buttonsLayout);
}


void DistributionsForInferenceWidget::updateDistributions(const QStringList& distributions)
{
  QStringList notUsedDistributions;
  notUsedDistributions << tr("All");
  for (int i = 0; i < allDistributions_.size(); ++i)
  {
    if (!distributions.contains(allDistributions_[i]))
      notUsedDistributions << allDistributions_[i];
  }

  // update Add button items
  addComboBox_->clear();
  addComboBox_->addItems(notUsedDistributions);

  // update distributions table model
  tableModel_->updateData(distributions);
  tableView_->selectRow(0);
}


void DistributionsForInferenceWidget::removeSelectedDistribution()
{
  // update list of distributions
  const QModelIndexList indexList = tableView_->selectionModel()->selectedRows();
  QStringList distributions = tableModel_->getDistributions();
  for (int i = indexList.size() - 1; i >= 0; --i)
  {
    const QString selectedDistribution = tableModel_->data(indexList[i], Qt::DisplayRole).toString();
    distributions.removeOne(selectedDistribution);
  }
  // update widgets
  updateDistributions(distributions);

  // emit signal to parent widget
  emit distributionsListChanged(distributions);
  emit distributionsListChanged(variables_, distributions);
}


void DistributionsForInferenceWidget::addSelectedDistribution(int index)
{
  // "Distribution name" item
  if (index != 0)
  {
    addComboBox_->removeItem(index);
  }
  // "All" item
  else
  {
    addComboBox_->clear();
    addComboBox_->addItem(tr("All"));
  }
  emit distributionsListChanged(variables_, tableModel_->getDistributions());
}
}
