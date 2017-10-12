//                                               -*- C++ -*-
/**
 *  @brief QWidget used in the QWizard for inference
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
#include "otgui/DistributionsForInferenceWidget.hxx"

#include "otgui/TranslationManager.hxx"

#include <QTableView>
#include <QHeaderView>
#include <QPushButton>
#include <QVBoxLayout>

namespace OTGUI
{

DistributionsForInferenceWidget::DistributionsForInferenceWidget(const QStringList & distributions, QWidget* parent)
  : QWidget(parent)
  , distributions_(distributions)
{
  buildInterface();
}


void DistributionsForInferenceWidget::buildInterface()
{
  QVBoxLayout * mainLayout = new QVBoxLayout(this);

  // list distributions
  QStringList notUsedDistributions;
  const QStringList allDistributions = TranslationManager::GetAvailableDistributions();
  for (int i = 0; i < allDistributions.size(); ++i)
  {
    if (!distributions_.contains(allDistributions[i]))
      notUsedDistributions << allDistributions[i];
  }
  notUsedDistributions << tr("All");

  distributionsTableView_ = new QTableView;
#if QT_VERSION >= 0x050000
  distributionsTableView_->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
#else
  distributionsTableView_->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
#endif
  distributionsTableView_->setSelectionBehavior(QAbstractItemView::SelectRows);
  distributionsTableView_->setSelectionMode(QAbstractItemView::ExtendedSelection);
  distributionsTableView_->verticalHeader()->hide();

  tableModel_ = new DistributionsTableModel(distributions_, distributionsTableView_);
  tableModel_->setHeaderData(0, Qt::Vertical, "aName", Qt::UserRole);
  connect(tableModel_, SIGNAL(distributionsListChanged(QStringList)), this, SIGNAL(distributionsListChanged(QStringList)));

  distributionsTableView_->setModel(tableModel_);
  distributionsTableView_->selectRow(0);
  mainLayout->addWidget(distributionsTableView_);

  QHBoxLayout * buttonsLayout = new QHBoxLayout;
  buttonsLayout->addStretch();
  addDistributionCombox_ = new TitledComboBox(QIcon(":/images/list-add.png"), tr("Add"));
  addDistributionCombox_->addItems(notUsedDistributions);

  buttonsLayout->addWidget(addDistributionCombox_);
  connect(addDistributionCombox_, SIGNAL(textActivated(QString)), tableModel_, SLOT(appendDistribution(QString)));
  connect(addDistributionCombox_, SIGNAL(activated(int)), this, SLOT(addSelectedDistribution(int)));

  QPushButton * removeDistributionButton = new QPushButton(tr("Remove"));
  removeDistributionButton->setIcon(QIcon(":/images/list-remove.png"));
  connect(removeDistributionButton, SIGNAL(pressed()), this, SLOT(removeSelectedDistribution()));
  buttonsLayout->addWidget(removeDistributionButton);

  mainLayout->addLayout(buttonsLayout);
}


void DistributionsForInferenceWidget::removeSelectedDistribution()
{
  QModelIndexList indexList = distributionsTableView_->selectionModel()->selectedRows();

  const QStringList allDistributions = TranslationManager::GetAvailableDistributions();

  addDistributionCombox_->removeItem(addDistributionCombox_->count() - 1);
  for (int i = indexList.size() - 1; i >= 0; --i)
  {
    QString selectedDistribution = tableModel_->data(indexList[i], Qt::DisplayRole).toString();
    if (allDistributions.contains(selectedDistribution))
      addDistributionCombox_->addItem(selectedDistribution);
  }
  addDistributionCombox_->model()->sort(0);
  addDistributionCombox_->addItem(tr("All"));

  QStringList distributions;
  for (int i = 0; i < allDistributions.size(); ++i)
    if (addDistributionCombox_->findText(allDistributions[i]) == -1)
      distributions << allDistributions[i];

  delete tableModel_;
  tableModel_ = new DistributionsTableModel(distributions, distributionsTableView_);
  tableModel_->setHeaderData(0, Qt::Vertical, "aName", Qt::UserRole);
  connect(tableModel_, SIGNAL(distributionsListChanged(QStringList)), this, SIGNAL(distributionsListChanged(QStringList)));
  connect(addDistributionCombox_, SIGNAL(textActivated(QString)), tableModel_, SLOT(appendDistribution(QString)));
  distributionsTableView_->setModel(tableModel_);
  distributionsTableView_->selectRow(0);
  emit distributionsListChanged(distributions);
}


void DistributionsForInferenceWidget::addSelectedDistribution(int index)
{
  if (index < addDistributionCombox_->count() - 1)
  {
    addDistributionCombox_->removeItem(index);
  }
  else
  {
    addDistributionCombox_->clear();
    addDistributionCombox_->addItem(tr("All"));
  }
}
}
