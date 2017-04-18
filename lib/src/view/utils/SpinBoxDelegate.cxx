//                                               -*- C++ -*-
/**
 *  @brief QStyledItemDelegate using a QSpinBox
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
#include "otgui/SpinBoxDelegate.hxx"

#include "otgui/LogDoubleSpinBox.hxx"

namespace OTGUI {

SpinBoxDelegate::SpinBoxDelegate(QObject *parent)
  : QStyledItemDelegate(parent)
  , type_(SpinBoxDelegate::noType)
{
}


QWidget* SpinBoxDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &, const QModelIndex &) const
{
  if (type_ == SpinBoxDelegate::noType)
  {
    return new QSpinBox(parent);
  }
  else if (type_ == SpinBoxDelegate::correlation)
  {
    DoubleSpinBox * editor = new DoubleSpinBox(parent);
    editor->setFrame(false);
    editor->setMinimum(-1.0);
    editor->setMaximum(1.0);
    editor->setSingleStep(0.05);
    editor->setDecimals(15);
    return editor;
  }
  else if (type_ == SpinBoxDelegate::differentiationStep)
  {
    LogDoubleSpinBox * editor = new LogDoubleSpinBox(parent);
    editor->setFrame(false);
    editor->setDecimals(15);
    editor->setApplyToAllInContextMenu(true);
    connect(editor, SIGNAL(applyToAllRequested(double)), this, SIGNAL(applyToAllRequested(double)));
    return editor;
  }
}


void SpinBoxDelegate::setEditorData(QWidget * editor, const QModelIndex & index) const
{
  if (type_ == SpinBoxDelegate::noType)
  {
    QSpinBox * spinBox = static_cast<QSpinBox*>(editor);
    spinBox->setValue(index.model()->data(index, Qt::EditRole).toInt());
  }
  else
  {
    DoubleSpinBox * spinBox = static_cast<DoubleSpinBox*>(editor);
    spinBox->setValue(index.model()->data(index, Qt::EditRole).toDouble());
  }
}


void SpinBoxDelegate::setModelData(QWidget * editor, QAbstractItemModel * model, const QModelIndex & index) const
{
  if (!editor || !model || !index.isValid())
    return;

  if (type_ == SpinBoxDelegate::noType)
  {
    QSpinBox * spinBox = static_cast<QSpinBox*>(editor);
    spinBox->interpretText();
    model->setData(index, spinBox->value(), Qt::EditRole);
  }
  else
  {
    DoubleSpinBox * spinBox = static_cast<DoubleSpinBox*>(editor);
    spinBox->interpretText();
    model->setData(index, spinBox->value(), Qt::EditRole);
  }
}


void SpinBoxDelegate::updateEditorGeometry(QWidget * editor, const QStyleOptionViewItem & option, const QModelIndex &) const
{
  if (!editor)
    return;
  editor->setGeometry(option.rect);
}


void SpinBoxDelegate::setSpinBoxType(const SpinBoxDelegate::spinboxType type)
{
  type_ = type;
}
}