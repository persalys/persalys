//                                               -*- C++ -*-
/**
 *  @brief QItemDelegate to insert combobox in QTableView
 *
 *  Copyright 2015-2023 EDF-Phimeca
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
#ifndef PERSALYS_COMBOBOXDELEGATE_HXX
#define PERSALYS_COMBOBOXDELEGATE_HXX

#include "persalys/PersalysPrivate.hxx"

#include <QItemDelegate>
#include <QStandardItemModel>

namespace PERSALYS
{
class PERSALYS_UTILS_API ComboBoxDelegate : public QItemDelegate
{
  Q_OBJECT

public:
  ComboBoxDelegate(QObject * parent = 0);
  ComboBoxDelegate(QPair<int, int> cell, QObject * parent = 0);

  void setNoWheelEvent(const bool noWheelEvent);
  void addSeparatorIndex(const int index, const QString &text = "");

  QWidget *createEditor(QWidget * parent, const QStyleOptionViewItem & option, const QModelIndex & index) const;
  void setEditorData(QWidget * editor, const QModelIndex & index) const;
  void setModelData(QWidget * editor, QAbstractItemModel * model, const QModelIndex & index) const;
  void updateEditorGeometry(QWidget * editor, const QStyleOptionViewItem & option, const QModelIndex & index) const;

public slots:
  void emitCommitData();

private:
  QPair<int, int> cell_;
  bool noWheelEvent_;
  QList<int> separatorIndex_;
  QStringList separatorText_;
};


class PERSALYS_UTILS_API ComboBoxWithSeparatorDelegate : public QItemDelegate
{
public:
  ComboBoxWithSeparatorDelegate(QObject *parent);

  void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
  QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const;
};


class PERSALYS_UTILS_API ModelForComboBoxWithSeparator : public QStandardItemModel
{
public:
  ModelForComboBoxWithSeparator(const QStringList &allItems, const QList<int> separatorIndex, const QStringList &separatorText, QWidget *parent);
};
}
#endif
