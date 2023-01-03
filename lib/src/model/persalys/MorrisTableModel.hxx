//                                               -*- C++ -*-
/**
 *  @brief QAbstractTableModel to list inputs
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
#ifndef PERSALYS_MORRISTABLEMODEL_HXX
#define PERSALYS_MORRISTABLEMODEL_HXX

#include "persalys/MorrisAnalysis.hxx"

#include <QAbstractTableModel>

namespace PERSALYS
{
class PERSALYS_MODEL_API MorrisTableModel : public QAbstractTableModel
{
  Q_OBJECT

public:
  MorrisTableModel(const MorrisAnalysis & analysis, QObject * parent = 0);

  int columnCount(const QModelIndex & parent = QModelIndex()) const;
  int rowCount(const QModelIndex & parent = QModelIndex()) const;
  QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
  QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;
  bool setData(const QModelIndex & index, const QVariant & value, int role);
  Qt::ItemFlags flags(const QModelIndex & index) const;
  MorrisAnalysis getAnalysis() const;

signals:
  void errorMessageChanged(QString);
  void doeSizeChanged(QString);

private:
  MorrisAnalysis analysis_;
};
}
#endif
