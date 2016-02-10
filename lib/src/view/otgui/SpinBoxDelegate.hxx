//                                               -*- C++ -*-
/**
 *  @brief QStyledItemDelegate used to define Correlations
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
#ifndef OTGUI_SPINBOXDELEGATE_HXX
#define OTGUI_SPINBOXDELEGATE_HXX

#include <QStyledItemDelegate>

namespace OTGUI {
class SpinBoxDelegate : public QStyledItemDelegate
{
  Q_OBJECT

public:
  SpinBoxDelegate(QObject * parent = 0);
 
  QWidget * createEditor(QWidget * parent, const QStyleOptionViewItem & option, const QModelIndex & index) const;
  void setEditorData(QWidget * editor, const QModelIndex & index) const;
  void setModelData(QWidget * editor, QAbstractItemModel * model, const QModelIndex & index) const;
  void updateEditorGeometry(QWidget * editor, const QStyleOptionViewItem & option, const QModelIndex & index) const;
};
}
#endif