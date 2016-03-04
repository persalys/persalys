//                                               -*- C++ -*-
/**
 *  @brief QStyledItemDelegate used to validate variables names
 *
 *  Copyright 2015-2016 EDF-Phimeca
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
#include "otgui/LineEditWithQValidatorDelegate.hxx"

#include <QLineEdit>

namespace OTGUI {

LineEditWithQValidatorDelegate::LineEditWithQValidatorDelegate(QObject *parent)
  : QStyledItemDelegate(parent)
{
}


QWidget* LineEditWithQValidatorDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &, const QModelIndex &) const
{
  QLineEdit * editor = new QLineEdit(parent);
  editor->setValidator(new QRegExpValidator(QRegExp("^[a-zA-Z_][a-zA-Z_0-9]*")));

  return editor;
}


void LineEditWithQValidatorDelegate::setEditorData(QWidget * editor, const QModelIndex & index) const
{
  QLineEdit * lineEdit = static_cast<QLineEdit*>(editor);
  lineEdit->setText(index.model()->data(index, Qt::EditRole).toString());
}


void LineEditWithQValidatorDelegate::setModelData(QWidget * editor, QAbstractItemModel * model, const QModelIndex & index) const
{
  QLineEdit * lineEdit = static_cast<QLineEdit*>(editor);
  model->setData(index, lineEdit->text(), Qt::EditRole);
}


void LineEditWithQValidatorDelegate::updateEditorGeometry(QWidget * editor, const QStyleOptionViewItem & option, const QModelIndex &) const
{
  editor->setGeometry(option.rect);
}
}