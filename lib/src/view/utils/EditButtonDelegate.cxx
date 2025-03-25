//                                               -*- C++ -*-
/**
 *  @brief QStyledItemDelegate using a QPushButton
 *
 *  Copyright 2015-2025 EDF-Phimeca
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
#include "persalys/EditButtonDelegate.hxx"

#include "persalys/EditValuesWizard.hxx"

using namespace OT;

namespace PERSALYS
{

  EditButtonDelegate::EditButtonDelegate(QObject *parent, UnsignedInteger nMinValues, Scalar factor)
  : QStyledItemDelegate(parent)
  , nMinValues_(nMinValues)
  , factor_(factor)
{
}


QWidget* EditButtonDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &, const QModelIndex & /*index*/) const
{
  EditValuesButton * editor = new EditValuesButton("...", parent);
  connect(editor, &QPushButton::clicked, this, &EditButtonDelegate::editRequest);
  return editor;
}


void EditButtonDelegate::editRequest()
{
  EditValuesButton * editor = qobject_cast<EditValuesButton *>(sender());
  EditValuesWizard wizard(editor->parameterName().c_str(), editor->values(), nullptr, nMinValues_, factor_);
  if (wizard.exec())
  {
    editor->setValues(wizard.getValues());
    editor->setOpened(true);
    emit commitData(editor);
    editor->setOpened(false);
  }
}


void EditButtonDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
  if (index.data(Qt::UserRole).canConvert<Point>())
  {
    EditValuesButton * buttonEditor = qobject_cast<EditValuesButton *>(editor);
    buttonEditor->setValues(qvariant_cast<Point>(index.data(Qt::UserRole)));
    buttonEditor->setParameterName(index.model()->data(index.model()->index(index.row(), 0)).toString().toStdString());
  }
  else
    QStyledItemDelegate::setEditorData(editor, index);
}


void EditButtonDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
  if (!editor || !model || !index.isValid())
    return;

  if (index.data(Qt::UserRole).canConvert<Point>())
  {
    EditValuesButton * buttonEditor = qobject_cast<EditValuesButton *>(editor);
    if (buttonEditor->isOpened())
      model->setData(index, QVariant::fromValue(buttonEditor->values()), Qt::UserRole);
  }
  else
    QStyledItemDelegate::setModelData(editor, model, index);
}


void EditButtonDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &) const
{
  if (!editor)
    return;
  editor->setGeometry(option.rect);
}
}
