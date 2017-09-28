//                                               -*- C++ -*-
/**
 *  @brief QListWidget where items are checkable
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

ListWidgetWithCheckBox::ListWidgetWithCheckBox(QString title, QStringList itemNames, QStringList selectedItemNames, QWidget * parent)
  : QListWidget(parent)
  , itemNames_(itemNames)
  , checkedItemNames_(selectedItemNames)
{
  setViewMode(QListWidget::ListMode);
  setSelectionMode(QAbstractItemView::NoSelection);

  // first item
  QListWidgetItem * item = new QListWidgetItem;
  item->setData(Qt::DisplayRole, title);
  insertItem(model()->rowCount(), item);

  // other items associated to a checkBox
  for (int i = 0; i < itemNames_.size(); ++i)
  {
    item = new QListWidgetItem;
    item->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);

    insertItem(model()->rowCount(), item);

    QCheckBox * box = new QCheckBox(itemNames_[i]);
    if (selectedItemNames.contains(itemNames_[i]))
      box->setCheckState(Qt::Checked);
    setItemWidget(item, box);

    connect(box, SIGNAL(stateChanged(int)), this, SLOT(checkStateChanged(int)));
  }
}


void ListWidgetWithCheckBox::setCheckedNames(QStringList selectedItemNames)
{
  for (int i = 0; i < itemNames_.size(); ++ i)
  {
    QListWidgetItem * item_i = item(i + 1);// skip first item
    QWidget * widget_i = itemWidget(item_i);
    QCheckBox * box = qobject_cast<QCheckBox *>(widget_i);
    box->setChecked(selectedItemNames.contains(itemNames_[i]));
  }
}

QStringList ListWidgetWithCheckBox::getCheckedItemNames() const
{
  return checkedItemNames_;
}


QStringList ListWidgetWithCheckBox::getItemNames() const
{
  return itemNames_;
}

void ListWidgetWithCheckBox::checkStateChanged(int state)
{
  QCheckBox * box = qobject_cast<QCheckBox *>(sender());

  if (state == Qt::Checked)
  {
    if (checkedItemNames_.contains(box->text()))
      return;
    else
      checkedItemNames_.append(box->text());
  }
  else
  {
    if (!checkedItemNames_.contains(box->text()))
      return;
    else
      checkedItemNames_.removeAt(checkedItemNames_.indexOf(box->text()));
  }
  emit checkedItemsChanged(checkedItemNames_);
}
}
