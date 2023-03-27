//                                               -*- C++ -*-
/**
 *  @brief QAbstractTableModel to set coupling command environment
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
#ifndef PERSALYS_ENVIRONMENTTABLEMODEL_HXX
#define PERSALYS_ENVIRONMENTTABLEMODEL_HXX

#include "persalys/CouplingStep.hxx"

#include <QAbstractTableModel>

namespace PERSALYS
{
class PERSALYS_MODEL_API EnvironmentTableModel : public QAbstractTableModel
{
  Q_OBJECT

public:
  EnvironmentTableModel(const CouplingStep & step, QObject * parent = 0);

  int columnCount(const QModelIndex & parent = QModelIndex()) const;
  int rowCount(const QModelIndex & parent = QModelIndex()) const;
  QVariant data(const QModelIndex & index, int role) const;
  bool setData(const QModelIndex & index, const QVariant & value, int role);
  Qt::ItemFlags flags(const QModelIndex & index) const;
  QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

  bool isValid();
  CouplingStep getStep() const {return step_;};

signals:
  void stepChanged();

public slots:
  void updateData();
  void addLine();
  void removeLine();

private:
  CouplingStep step_;
};
}
#endif
