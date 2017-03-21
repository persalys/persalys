//                                               -*- C++ -*-
/**
 *  @brief QStyledItemDelegate used to validate variables names
 *
 *  Copyright 2015-2017 EDF-Phimeca
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
#ifndef OTGUI_LINEEDITWITHQVALIDATORDELEGATE_HXX
#define OTGUI_LINEEDITWITHQVALIDATORDELEGATE_HXX

#include <QStyledItemDelegate>
#include "otgui/OTGuiprivate.hxx"

namespace OTGUI {
class OTGUI_API LineEditWithQValidatorDelegate : public QStyledItemDelegate
{
  Q_OBJECT

public:
  LineEditWithQValidatorDelegate(const bool offsetForCheckBox = false, const QString & regex = "[a-zA-Z_][a-zA-Z_0-9]*", QObject * parent = 0);

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