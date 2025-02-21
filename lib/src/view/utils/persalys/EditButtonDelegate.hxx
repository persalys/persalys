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
#ifndef PERSALYS_EDITBUTTONDELEGATE_HXX
#define PERSALYS_EDITBUTTONDELEGATE_HXX

#include "persalys/ExperimentTableModel.hxx"

#include <QStyledItemDelegate>
#include <QPushButton>

namespace PERSALYS
{

class PERSALYS_UTILS_API EditButtonDelegate : public QStyledItemDelegate
{
  Q_OBJECT

public:
  EditButtonDelegate(QObject *parent = nullptr);

  QWidget * createEditor(QWidget * parent, const QStyleOptionViewItem & option, const QModelIndex & index) const;
  void setEditorData(QWidget * editor, const QModelIndex & index) const;
  void setModelData(QWidget * editor, QAbstractItemModel * model, const QModelIndex & index) const;
  void updateEditorGeometry(QWidget * editor, const QStyleOptionViewItem & option, const QModelIndex & index) const;

public slots:
  void editRequest();
};


class PERSALYS_UTILS_API EditValuesButton : public QPushButton
{
  Q_OBJECT

public:
  EditValuesButton(const QString &text, QWidget *parent = nullptr)
    : QPushButton(text, parent)
    , opened_(false)
  {};

  void setValues(const OT::Point &values)
  {
    values_ = values;
  }
  OT::Point values() const
  {
    return values_;
  }

  void setParameterName(const OT::String &parameterName)
  {
    parameterName_ = parameterName;
  }
  OT::String parameterName()
  {
    return parameterName_;
  }

  OT::Bool isOpened() const
  {
    return opened_;
  }
  void setOpened(const OT::Bool opened)
  {
    opened_ = opened;
  }

private:
  OT::Bool opened_;
  OT::Point values_;
  OT::String parameterName_;
};
}
#endif
