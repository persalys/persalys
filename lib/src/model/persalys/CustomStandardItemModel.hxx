//                                               -*- C++ -*-
/**
 *  @brief QStandardItemModel to set not editable items and export data
 *
 *  Copyright 2015-2024 EDF-Phimeca
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
#ifndef PERSALYS_CUSTOMSTANDARDITEMMODEL_HXX
#define PERSALYS_CUSTOMSTANDARDITEMMODEL_HXX

#include <QStandardItemModel>

#include "persalys/PersalysPrivate.hxx"

namespace PERSALYS
{
class PERSALYS_MODEL_API CustomStandardItemModel : public QStandardItemModel
{
  Q_OBJECT

public:
  CustomStandardItemModel(int nbRows, int nbColumns, QObject *parent = nullptr);

  void setNotEditableItem(const int row, const int column, const QString text, const QColor color = QColor());
  void setNotEditableItem(const int row, const int column, const double value, const int prec = 6);
  void setNotEditableHeaderItem(const int row, const int column, const QString text);
  QString getFormattedText() const;
};
}
#endif
