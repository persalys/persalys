//                                               -*- C++ -*-
/**
 *  @brief QAbstractTableModel to list the inputs
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
#ifndef PERSALYS_INPUTTABLEMODEL_HXX
#define PERSALYS_INPUTTABLEMODEL_HXX

#include "persalys/PhysicalModel.hxx"

#include <QAbstractTableModel>

namespace PERSALYS
{
class PERSALYS_MODEL_API InputTableModel : public QAbstractTableModel
{
  Q_OBJECT

public:
  InputTableModel(const PhysicalModel & physicalModel, QObject *parent = nullptr);

  int columnCount(const QModelIndex & parent = QModelIndex()) const;
  int rowCount(const QModelIndex & parent = QModelIndex()) const;
  QVariant data(const QModelIndex & index, int role) const;
  bool setData(const QModelIndex & index, const QVariant & value, int role);
  Qt::ItemFlags flags(const QModelIndex & index) const;
  QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

  bool isValid();

public slots:
  void updateData();
  void addLine();
  void removeLine(const QModelIndex & index);
signals:
  void errorMessageChanged(QString);
  void inputNumberChanged();
  void inputNameChanged();

private:
  PhysicalModel physicalModel_;
};
}
#endif
