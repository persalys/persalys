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
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
#ifndef OTGUI_COMBOBOXDELEGATE_HXX
#define OTGUI_COMBOBOXDELEGATE_HXX

#include "otgui/OTGuiprivate.hxx"

#include <QItemDelegate>

namespace OTGUI
{
class OTGUI_API ComboBoxDelegate : public QItemDelegate
{
  Q_OBJECT

public:
  ComboBoxDelegate(QObject * parent = 0);
  ComboBoxDelegate(QPair<int, int> cell, QObject * parent = 0);

  void setNoWheelEvent(const bool noWheelEvent);

  QWidget *createEditor(QWidget * parent, const QStyleOptionViewItem & option, const QModelIndex & index) const;
  void setEditorData(QWidget * editor, const QModelIndex & index) const;
  void setModelData(QWidget * editor, QAbstractItemModel * model, const QModelIndex & index) const;
  void updateEditorGeometry(QWidget * editor, const QStyleOptionViewItem & option, const QModelIndex & index) const;

public slots:
  void emitCommitData();

private:
  QPair<int, int> cell_;
  bool noWheelEvent_;
};
}
#endif
