//                                               -*- C++ -*-
/**
 *  @brief QStyledItemDelegate used to validate variables names
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
#ifndef PERSALYS_LINEEDITWITHQVALIDATORDELEGATE_HXX
#define PERSALYS_LINEEDITWITHQVALIDATORDELEGATE_HXX

#include <QStyledItemDelegate>
#include "persalys/PersalysPrivate.hxx"

namespace PERSALYS
{
class PERSALYS_UTILS_API LineEditWithQValidatorDelegate : public QStyledItemDelegate
{
  Q_OBJECT

public:
  LineEditWithQValidatorDelegate(QObject *parent = nullptr);
  LineEditWithQValidatorDelegate(const bool offsetForCheckBox, QObject *parent = nullptr);
  LineEditWithQValidatorDelegate(const QString & regex, QObject *parent = nullptr);

  QWidget * createEditor(QWidget * parent, const QStyleOptionViewItem & option, const QModelIndex & index) const;
  void setEditorData(QWidget * editor, const QModelIndex & index) const;
  void setModelData(QWidget * editor, QAbstractItemModel * model, const QModelIndex & index) const;
  void updateEditorGeometry(QWidget * editor, const QStyleOptionViewItem & option, const QModelIndex & index) const;

protected:
  QString regex_;
  bool offsetForCheckBox_;
};
}
#endif