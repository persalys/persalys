//                                               -*- C++ -*-
/**
 *  @brief QAbstractTableModel to list dependencies
 *
 *  Copyright 2015-2019 EDF-Phimeca
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
#ifndef PERSALYS_DEPENDENCIESTABLEMODEL_HXX
#define PERSALYS_DEPENDENCIESTABLEMODEL_HXX

#include "persalys/PhysicalModel.hxx"

#include <QAbstractTableModel>
#include <QMetaType> // mandatory to specify it to avoid windows compilation problem

namespace PERSALYS
{
class PERSALYS_API DependenciesTableModel : public QAbstractTableModel
{
  Q_OBJECT

public:
  DependenciesTableModel(const PhysicalModel &physicalModel, const bool failSoftMode, QObject *parent = 0);

  int rowCount(const QModelIndex &parent = QModelIndex()) const;
  int columnCount(const QModelIndex &parent = QModelIndex()) const;
  Qt::ItemFlags flags(const QModelIndex & index) const;
  QVariant headerData(int section,
                      Qt::Orientation orientation,
                      int role = Qt::DisplayRole) const;
  QVariant data(const QModelIndex &index, int role) const;
  bool setData(const QModelIndex & index, const QVariant & value, int role);
  OT::Description getGroup(const QModelIndex & index) const;

public slots:
  void updateData();
  void removeLine(const QModelIndex &index);
signals:
  void dataUpdated(const int index, const OT::Distribution &copula);
  void inferenceResultRequested(const QModelIndex&);

protected:
  void updateCopula();

private:
  bool failSoftMode_;
  PhysicalModel physicalModel_;
  OT::ComposedCopula copula_;
};
}
#endif
