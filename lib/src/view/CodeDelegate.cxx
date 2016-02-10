//                                               -*- C++ -*-
/**
 *  @brief QItemDelegate to define Python script
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
#include "otgui/CodeDelegate.hxx"

#include <QTextEdit>
#include <QApplication>
#include <QPainter>

namespace OTGUI {

CodeDelegate::CodeDelegate(QObject * parent)
  : QItemDelegate(parent)
{
}


QWidget *CodeDelegate::createEditor(QWidget * parent, const QStyleOptionViewItem & option, const QModelIndex & index) const
{
  QTextEdit * textEdit = new QTextEdit(parent);
#ifndef _WIN32
  QFont font("Monospace");
#else
  QFont font("Courier");
#endif
  font.setPointSize(9);
  font.setFixedPitch(true);
  textEdit->setFont(font);
  return textEdit;
}


void CodeDelegate::setEditorData(QWidget * editor, const QModelIndex & index) const
{
  QTextEdit * textEdit = dynamic_cast<QTextEdit*>(editor);
  textEdit->setPlainText(index.model()->data(index, Qt::DisplayRole).toString());
}


void CodeDelegate::setModelData(QWidget * editor, QAbstractItemModel * model, const QModelIndex & index) const
{
  QTextEdit * textEdit = dynamic_cast<QTextEdit*>(editor);
  model->setData(index, textEdit->toPlainText(), Qt::EditRole);
}


void CodeDelegate::paint(QPainter *painter, const QStyleOptionViewItem & option, const QModelIndex & index) const
{
  QVariant value = index.data(Qt::DisplayRole);
  if (value.isValid() && !value.isNull())
  {
    QTextDocument document;
#ifndef _WIN32
  QFont font("Monospace");
#else
  QFont font("Courier");
#endif
  font.setPointSize(9);
  font.setFixedPitch(true);
    document.setDefaultFont(font);
    document.setDocumentMargin(2);
    document.setPlainText(value.toString());
    painter->translate(option.rect.topLeft());
    document.drawContents(painter);
    painter->translate(-option.rect.topLeft());
  }
}

}