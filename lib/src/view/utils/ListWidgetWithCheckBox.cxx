//                                               -*- C++ -*-
/**
 *  @brief QListWidget with checkable items
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
#include "otgui/ListWidgetWithCheckBox.hxx"

#include <QCheckBox>

namespace OTGUI
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
  QListWidgetItem * item_0 = new QListWidgetItem;
  item_0->setData(Qt::DisplayRole, title_);
  item_0->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
  addItem(item_0);

  // other items
  for (int i = 0; i < itemNames_.size(); ++i)
  {
    QListWidgetItem * item = new QListWidgetItem;
    item->setData(Qt::DisplayRole, itemNames_[i]);
    item->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
    addItem(item);
  }
  setCheckedNames(checkedItemNames_);
  connect(this, SIGNAL(itemChanged(QListWidgetItem *)), this, SLOT(updateCheckState(QListWidgetItem *)));
}


void ListWidgetWithCheckBox::setCheckedNames(const QStringList &selectedItemNames)
{
  for (int i = 0; i < itemNames_.size(); ++ i)
  {
    item(i + 1)->setData(Qt::CheckStateRole, selectedItemNames.contains(itemNames_[i]) ? Qt::Checked : Qt::Unchecked);
  }
  checkedItemNames_ = selectedItemNames;
  updateTitleItem();

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
  QListWidgetItem * item_0 = item(0);
  item_0->setData(Qt::CheckStateRole, itemNames_.size() == checkedItemNames_.size() ? Qt::Checked : Qt::Unchecked);
  item_0->setData(Qt::ToolTipRole, itemNames_.size() == checkedItemNames_.size() ? tr("Deselect all") : tr("Select all"));
  blockSignals(false);
}


void ListWidgetWithCheckBox::updateCheckState(QListWidgetItem * currentItem)
{
  const int row = this->row(currentItem);

  if (row == 0)
  {
    setCheckedNames(currentItem->data(Qt::CheckStateRole).toInt() == Qt::Checked ? itemNames_ : QStringList());
  }
  else
  {
    if (currentItem->data(Qt::CheckStateRole).toInt() == Qt::Checked)
    {
      if (checkedItemNames_.contains(currentItem->data(Qt::DisplayRole).toString()))
        return;
      checkedItemNames_.append(currentItem->data(Qt::DisplayRole).toString());
    }
    else
    {
      if (!checkedItemNames_.contains(currentItem->data(Qt::DisplayRole).toString()))
        return;
      checkedItemNames_.removeAt(checkedItemNames_.indexOf(currentItem->data(Qt::DisplayRole).toString()));
    }
    updateTitleItem();
    emit checkedItemsChanged(checkedItemNames_);
  }
}
}
