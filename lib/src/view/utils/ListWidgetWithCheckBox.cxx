//                                               -*- C++ -*-
/**
 *  @brief QListWidget with checkable items
 *
 *  Copyright 2015-2021 EDF-Phimeca
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
#include "persalys/ListWidgetWithCheckBox.hxx"

#include <QCheckBox>
#include <iostream>

namespace PERSALYS
{

ListWidgetWithCheckBox::ListWidgetWithCheckBox(const QString &title,
    const QStringList &itemNames,
    QWidget * parent)
  : QListWidget(parent)
  , title_(title)
  , itemNames_(itemNames)
  , checkedItemNames_(itemNames)
{
  buildInterface();
}


ListWidgetWithCheckBox::ListWidgetWithCheckBox(const QString &title,
    const QStringList &itemNames,
    const QStringList &selectedItemNames,
    QWidget * parent)
  : QListWidget(parent)
  , title_(title)
  , itemNames_(itemNames)
  , checkedItemNames_(selectedItemNames)
{
  buildInterface();
}


void ListWidgetWithCheckBox::buildInterface()
{
  setViewMode(QListWidget::ListMode);
  setSelectionMode(QAbstractItemView::NoSelection);

  // first item
  QListWidgetItem * item_0 = new QListWidgetItem(title_, this);
  item_0->setFlags(Qt::ItemIsEnabled);
  addItem(item_0);

  // other items
  for (int i = 0; i < itemNames_.size(); ++i)
  {
    QListWidgetItem * item = new QListWidgetItem(itemNames_[i], this);
    item->setFlags(Qt::ItemIsEnabled);
    addItem(item);
  }
  setCheckedNames(checkedItemNames_);
  connect(this, SIGNAL(itemPressed(QListWidgetItem *)), this, SLOT(updateCheckState(QListWidgetItem *)));
}


void ListWidgetWithCheckBox::setCheckedNames(const QStringList &selectedItemNames)
{
  blockSignals(true);
  for (int i = 0; i < itemNames_.size(); ++ i)
  {
    const bool isChecked = selectedItemNames.contains(itemNames_[i]);
    item(i + 1)->setCheckState(isChecked ? Qt::Checked : Qt::Unchecked);
  }
  checkedItemNames_ = selectedItemNames;
  updateTitleItem();
  blockSignals(false);

  emit checkedItemsChanged(checkedItemNames_);
}


QStringList ListWidgetWithCheckBox::getCheckedItemNames() const
{
  return checkedItemNames_;
}


QStringList ListWidgetWithCheckBox::getItemNames() const
{
  return itemNames_;
}


void ListWidgetWithCheckBox::updateTitleItem()
{
  blockSignals(true);
  item(0)->setCheckState(itemNames_.size() == checkedItemNames_.size() ? Qt::Checked : Qt::Unchecked);
  item(0)->setToolTip(itemNames_.size() == checkedItemNames_.size() ? tr("Deselect all") : tr("Select all"));
  blockSignals(false);
}


void ListWidgetWithCheckBox::updateCheckState(QListWidgetItem * currentItem)
{
  const int row = this->row(currentItem);

  currentItem->setCheckState(currentItem->checkState() == Qt::Checked ? Qt::Unchecked : Qt::Checked);

  if (row == 0)
  {
    setCheckedNames(currentItem->checkState() == Qt::Checked ? itemNames_ : QStringList());
  }
  else
  {
    if (currentItem->checkState() == Qt::Checked)
    {
      if (!checkedItemNames_.contains(currentItem->text()))
        checkedItemNames_.append(currentItem->text());
    }
    else
    {
      if (checkedItemNames_.contains(currentItem->text()))
        checkedItemNames_.removeAt(checkedItemNames_.indexOf(currentItem->text()));
    }
    updateTitleItem();
    emit checkedItemsChanged(checkedItemNames_);
  }
  QList<int> checkedItemIndices;
  for (int i = 0; i < itemNames_.size(); ++ i)
  {
    if (item(i + 1)->checkState())
      checkedItemIndices.append(i);
  }
  emit checkedItemsChanged(checkedItemIndices);
}
}
