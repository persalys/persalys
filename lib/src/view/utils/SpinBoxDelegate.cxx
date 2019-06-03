//                                               -*- C++ -*-
/**
 *  @brief QStyledItemDelegate using a QSpinBox
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
#include "persalys/SpinBoxDelegate.hxx"

#include "persalys/LogDoubleSpinBox.hxx"
#include "persalys/AppliException.hxx"
#include "persalys/StudyTreeViewModel.hxx"

namespace PERSALYS
{

SpinBoxDelegate::SpinBoxDelegate(QObject *parent)
  : QStyledItemDelegate(parent)
  , type_(SpinBoxDelegate::noType)
  , offsetForCheckBox_(false)
{
}


SpinBoxDelegate::SpinBoxDelegate(const bool offsetForCheckBox, QObject *parent)
  : QStyledItemDelegate(parent)
  , type_(SpinBoxDelegate::noType)
  , offsetForCheckBox_(offsetForCheckBox)
{
}


QWidget* SpinBoxDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &, const QModelIndex &) const
{
  switch(type_)
  {
    case SpinBoxDelegate::noType:
    {
      QSpinBox * editor = new QSpinBox(parent);
      editor->setFrame(false);
      editor->setMaximum(std::numeric_limits<int>::max());
      // fix transparency of the spinbox
      editor->setStyleSheet("background-color: white;");
      return editor;
    }
    case SpinBoxDelegate::doubleValue:
    {
      DoubleSpinBox * editor = new DoubleSpinBox(parent);
      editor->setFrame(false);
      editor->setDecimals(15);
      // fix transparency of the spinbox in GridDesignPage
      editor->setStyleSheet("QDoubleSpinBox { background-color: white; }");
      return editor;
    }
    case SpinBoxDelegate::correlation:
    {
      DoubleSpinBox * editor = new DoubleSpinBox(parent);
      editor->setFrame(false);
      editor->setMinimum(-1.0);
      editor->setMaximum(1.0);
      editor->setSingleStep(0.05);
      editor->setDecimals(15);
      return editor;
    }
    case SpinBoxDelegate::differentiationStep:
    {
      LogDoubleSpinBox * editor = new LogDoubleSpinBox(parent);
      editor->setFrame(false);
      editor->setDecimals(15);
      editor->setApplyToAllInContextMenu(true);
      connect(editor, SIGNAL(applyToAllRequested(double)), this, SIGNAL(applyToAllRequested(double)));
      return editor;
    }
    default:
      throw InvalidValueException(HERE) << "Invalid type";
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
    model->setData(index, QString::number(spinBox->value(), 'g', StudyTreeViewModel::DefaultSignificantDigits), Qt::DisplayRole);
  }
}


void SpinBoxDelegate::updateEditorGeometry(QWidget * editor, const QStyleOptionViewItem & option, const QModelIndex &) const
{
  if (!editor)
    return;
  if (offsetForCheckBox_)
  {
    QStyleOptionButton checkBoxStyleOption;
    QRect checkBoxRect = editor->style()->subElementRect(QStyle::SE_CheckBoxIndicator, &checkBoxStyleOption);
    QPoint checkBoxPoint(option.rect.x() + 3 + checkBoxRect.width(), option.rect.y());

    editor->setGeometry(QRect(checkBoxPoint, QSize(option.rect.width() - 3 - checkBoxRect.width(), option.rect.height())));
  }
  else
  {
    editor->setGeometry(option.rect);
  }
}


void SpinBoxDelegate::setSpinBoxType(const SpinBoxDelegate::spinboxType type)
{
  type_ = type;
}
}
