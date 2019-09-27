//                                               -*- C++ -*-
/**
 *  @brief QDialog to define values
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
#include "persalys/EditValuesWizard.hxx"

#include "persalys/DoubleSpinBox.hxx"
#include "persalys/StudyTreeViewModel.hxx"

#include <QHeaderView>
#include <QHBoxLayout>
#include <QDebug>
#include <QtAlgorithms>
#include <QFileDialog>
#include <QDialogButtonBox>
#include <cfloat>

using namespace OT;

namespace PERSALYS
{
EditValuesWizard::EditValuesWizard(const QString &variableName, const Point& values, QWidget *parent)
  : QWizard(parent)
  , model_(new QStandardItemModel(0, 1, this))
  , proxy_(new QSortFilterProxyModel(this))
  , valueTable_(new QTableView(this))
  , valueNumber_(new QLabel("0"))
  , addedValueBox_(new DoubleSpinBox)
  , removeButton_(0)
  , values_(values)
{
  setWindowTitle(tr("Define values"));
  setButtonText(QWizard::FinishButton, tr("Finish"));
  setButtonText(QWizard::CancelButton, tr("Cancel"));
  setOption(QWizard::NoDefaultButton, true);
  setOption(QWizard::NoBackButtonOnStartPage, true);


  model_->setHorizontalHeaderLabels(QStringList() << variableName);
  proxy_->setSourceModel(model_);
  proxy_->setSortRole(Qt::UserRole);
  valueTable_->setModel(proxy_);
  valueTable_->horizontalHeader()->setStretchLastSection(true);
  valueTable_->verticalHeader()->hide();
  connect(valueTable_->selectionModel(), SIGNAL(selectionChanged(QItemSelection,QItemSelection)), this, SLOT(checkButtons()));

  QVBoxLayout * optionLayout = new QVBoxLayout;

  // number of values Label
  QLabel * label = new QLabel(tr("<b>Number of values:</b>"));
  QHBoxLayout* numberLayout = new QHBoxLayout;
  numberLayout->addWidget(label);
  numberLayout->addWidget(valueNumber_);
  optionLayout->addLayout(numberLayout);

  // add button
  QPushButton * addButton = new QPushButton(QIcon(":/images/list-add.png"), tr("Add"), this);
  addButton->setToolTip(tr("Add a value"));
  QHBoxLayout * addLayout = new QHBoxLayout;
  addLayout->addWidget(addedValueBox_);
  addLayout->addWidget(addButton);
  connect(addButton, SIGNAL(clicked()), this, SLOT(addValue()));
  optionLayout->addLayout(addLayout);

  // remove button
  removeButton_ = new QPushButton(QIcon(":/images/list-remove.png"), tr("Remove"));
  removeButton_->setToolTip(tr("Remove the selected values"));
  connect(removeButton_, SIGNAL(clicked()), this, SLOT(removeSelectedValues()));
  optionLayout->addWidget(removeButton_);

  optionLayout->addStretch();

  QWizardPage * page = new QWizardPage;
  QHBoxLayout * pageLayout = new QHBoxLayout(page);
  pageLayout->addWidget(valueTable_);
  pageLayout->addLayout(optionLayout);
  addPage(page);

  // sort and remove duplicates
  std::sort(values_.begin(), values_.end());
  auto last = std::unique(values_.begin(), values_.end());
  values_.erase(last, values_.end());

  // fill table
  foreach(double value, values_)
    insertAValue(value);

  // set default value of addedValueBox_
  if (values_.getSize())
  {
    std::pair<Collection<Scalar>::iterator, Collection<Scalar>::iterator> p = std::minmax_element(values_.begin(), values_.end());
    addedValueBox_->setValue((*p.second) + 1);
  }

  checkButtons();
}


void EditValuesWizard::addValue()
{
  if (!values_.contains(addedValueBox_->value()))
  {
    addAValue(addedValueBox_->value());
    addedValueBox_->setValue(addedValueBox_->value()+1);
  }
}


void EditValuesWizard::checkButtons()
{
  const QItemSelectionModel * selectionModel = valueTable_->selectionModel();
  const QItemSelection & selection = selectionModel->selection();
  removeButton_->setEnabled(!selection.empty());
}


void EditValuesWizard::addAValue(double v)
{
  if (!values_.contains(v))
  {
    values_.add(v);
    insertAValue(v);
  }
}


void EditValuesWizard::insertAValue(double v)
{
  Q_ASSERT(values_.contains(v));

  QStandardItem * currentItem = new QStandardItem(QString::number(v, 'g', StudyTreeViewModel::DefaultSignificantDigits));
  currentItem->setData(v, Qt::UserRole);
  currentItem->setEditable(false);
  model_->appendRow(currentItem);

  check();
}


void EditValuesWizard::removeSelectedValues()
{
  QList<int> selectedRow;
  // retrieve rows
  foreach(QModelIndex index, valueTable_->selectionModel()->selectedIndexes())
  {
    selectedRow << proxy_->mapToSource(index).row();
  }
  // sort
  std::sort(selectedRow.begin(), selectedRow.end(), std::greater<int>());

  // remove
  foreach(int row, selectedRow)
  {
    QList<QStandardItem*> rowItems = model_->takeRow(row);
    Q_ASSERT(rowItems.count() == 1);
    bool ok;
    const double value = rowItems.first()->data(Qt::UserRole).toDouble(&ok);
    Q_ASSERT(ok);
    values_.erase(std::remove(values_.begin(), values_.end(), value), values_.end());
  }

  check();
}


Point EditValuesWizard::getValues() const
{
  Point orderedValues(values_);
  std::sort(orderedValues.begin(), orderedValues.end());
  return orderedValues;
}


void EditValuesWizard::check()
{
  proxy_->sort(0);
  valueNumber_->setText(QString::number(values_.getSize()));
  checkButtons();
}


bool EditValuesWizard::validateCurrentPage()
{
  return values_.getSize() > 1;
}
}
