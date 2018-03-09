//                                               -*- C++ -*-
/**
 *  @brief QStyledItemDelegate using a QSpinBox
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
#ifndef OTGUI_SPINBOXDELEGATE_HXX
#define OTGUI_SPINBOXDELEGATE_HXX

#include <QStyledItemDelegate>
#include "otgui/OTGuiprivate.hxx"

namespace OTGUI
{
class OTGUI_API SpinBoxDelegate : public QStyledItemDelegate
{
  Q_OBJECT

public:
  enum spinboxType {noType, doubleValue, correlation, differentiationStep};

  SpinBoxDelegate(QObject * parent = 0);
  SpinBoxDelegate(const bool offsetForCheckBox, QObject * parent = 0);

  QWidget * createEditor(QWidget * parent, const QStyleOptionViewItem & option, const QModelIndex & index) const;
  void setEditorData(QWidget * editor, const QModelIndex & index) const;
  void setModelData(QWidget * editor, QAbstractItemModel * model, const QModelIndex & index) const;
  void updateEditorGeometry(QWidget * editor, const QStyleOptionViewItem & option, const QModelIndex & index) const;
  void setSpinBoxType(const spinboxType type);

signals:
  void applyToAllRequested(double);

private:
  spinboxType type_;
  bool offsetForCheckBox_;
};
}
#endif
