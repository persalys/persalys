//                                               -*- C++ -*-
/**
 *  @brief QItemDelegate to insert combobox in QTableView
 *
 *  Copyright 2015-2016 EDF
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
#include "otgui/ComboBoxDelegate.hxx"

#include <QComboBox>
#include <QApplication>
#include <QStandardItemModel>

namespace OTGUI {

ComboBoxDelegate::ComboBoxDelegate(QStringList items, QPair<int, int> cell, QObject * parent)
  : QItemDelegate(parent)
  , items_(items)
  , cell_(cell)
{
}


QWidget *ComboBoxDelegate::createEditor(QWidget * parent, const QStyleOptionViewItem & option, const QModelIndex & index) const
{
  if (cell_ != QPair<int, int>())
    if (index.row() != cell_.first || index.column() != cell_.second)
        return QItemDelegate::createEditor(parent, option, index);

  QComboBox * editor = new QComboBox(parent);
  // for probabilisticModelWindow:
  if (cell_ == QPair<int, int>())
  {
    QStandardItemModel * model = new QStandardItemModel(1, 1);
    QStandardItem * firstItem = new QStandardItem;
    firstItem->setEnabled(false);
    model->setItem(0, 0, firstItem);
    editor->setModel(model);
  }
  editor->addItems(items_);
  connect(editor, SIGNAL(activated(QString)), this, SLOT(emitCommitData()));
  return editor;
}


void ComboBoxDelegate::setEditorData(QWidget * editor, const QModelIndex & index) const
{
  if (cell_ != QPair<int, int>())
    if (index.row() != cell_.first || index.column() != cell_.second)
        return QItemDelegate::setEditorData(editor, index);

  QComboBox * comboBox = static_cast<QComboBox*>(editor);
  comboBox->setCurrentIndex(comboBox->findText(index.model()->data(index, Qt::DisplayRole).toString()));
  comboBox->setEnabled(true);
  if (comboBox->currentIndex() == -1)
    comboBox->setEnabled(false);
}


void ComboBoxDelegate::setModelData(QWidget * editor, QAbstractItemModel * model, const QModelIndex & index) const
{
  if (cell_ != QPair<int, int>())
    if (index.row() != cell_.first || index.column() != cell_.second)
        return QItemDelegate::setModelData(editor, model, index);

  QComboBox * comboBox = static_cast<QComboBox*>(editor);
  model->setData(index, comboBox->itemText(comboBox->currentIndex()), Qt::EditRole);
}


void ComboBoxDelegate::updateEditorGeometry(QWidget * editor, const QStyleOptionViewItem & option, const QModelIndex & index) const
{
  editor->setGeometry(option.rect);
}


void ComboBoxDelegate::emitCommitData()
{
  emit closeEditor(qobject_cast<QWidget *>(sender()));
}
}