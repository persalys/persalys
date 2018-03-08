//                                               -*- C++ -*-
/**
 *  @brief QItemDelegate to insert combobox in QTableView
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
#include "otgui/ComboBoxDelegate.hxx"

#include <otgui/NoWheelEventComboBox.hxx>

#include <QStandardItemModel>
#include <QWheelEvent>

namespace OTGUI
{

// CustomComboBox with no wheel event if it has no focus
class CustomComboBox : public QComboBox
{
public:
  CustomComboBox(QWidget *parent)
  : QComboBox(parent) {}

  virtual void wheelEvent(QWheelEvent *e)
  {
    // if no focus : no signal currentIndexChanged emitted -> wrong behavior
    // so if no focus : do nothing
    if (hasFocus())
      QComboBox::wheelEvent(e);
  }
};


ComboBoxDelegate::ComboBoxDelegate(QObject * parent)
  : QItemDelegate(parent)
  , cell_()
  , noWheelEvent_(false)
{
}


ComboBoxDelegate::ComboBoxDelegate(QPair<int, int> cell, QObject * parent)
  : QItemDelegate(parent)
  , cell_(cell)
  , noWheelEvent_(false)
{
}


void ComboBoxDelegate::setNoWheelEvent(const bool noWheelEvent)
{
  noWheelEvent_ = noWheelEvent;
}


QWidget *ComboBoxDelegate::createEditor(QWidget * parent, const QStyleOptionViewItem & option, const QModelIndex & index) const
{
  if (cell_ != QPair<int, int>())
    if (index.row() != cell_.first || index.column() != cell_.second)
      return QItemDelegate::createEditor(parent, option, index);

  QComboBox * editor;
  if (noWheelEvent_)
    editor = new NoWheelEventComboBox(parent);
  else
    editor = new CustomComboBox(parent);
  const QStringList items(index.model()->data(index, Qt::UserRole + 1).toStringList());
  editor->addItems(items);
  editor->setEnabled(items.size() > 0);

  connect(editor, SIGNAL(activated(QString)), this, SLOT(emitCommitData()));
  return editor;
}


void ComboBoxDelegate::setEditorData(QWidget * editor, const QModelIndex & index) const
{
  if (cell_ != QPair<int, int>())
    if (index.row() != cell_.first || index.column() != cell_.second)
      return QItemDelegate::setEditorData(editor, index);

  QComboBox * comboBox = static_cast<QComboBox*>(editor);
  const QStringList items(index.model()->data(index, Qt::UserRole + 1).toStringList());
  comboBox->clear();
  comboBox->addItems(items);
  comboBox->setEnabled(items.size() > 0);
  comboBox->setCurrentIndex(comboBox->findText(index.model()->data(index, Qt::DisplayRole).toString()));
  comboBox->setEnabled(comboBox->currentIndex() != -1);
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
