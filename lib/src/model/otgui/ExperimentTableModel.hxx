//                                               -*- C++ -*-
/**
 *  @brief QAbstractTableModel to list inputs
 *
 *  Copyright 2015-2016 EDF-Phimeca
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
#ifndef OTGUI_EXPERIMENTTABLEMODEL_HXX
#define OTGUI_EXPERIMENTTABLEMODEL_HXX

#include "otgui/DesignOfExperiment.hxx"

#include <QAbstractTableModel>

namespace OTGUI {
class OTGUI_API ExperimentTableModel : public QAbstractTableModel
{
  Q_OBJECT

public:
  ExperimentTableModel(const DesignOfExperiment & designOfExperiment);

  int columnCount(const QModelIndex & parent = QModelIndex()) const;
  int rowCount(const QModelIndex & parent = QModelIndex()) const;
  QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
  QVariant data(const QModelIndex & index, int role) const;
  bool setData(const QModelIndex & index, const QVariant & value, int role);
  Qt::ItemFlags flags(const QModelIndex & index) const;
  DesignOfExperiment getDesignOfExperiment() const;

public slots:
signals:
  void designOfExperimentChanged(DesignOfExperiment&);

private:
  DesignOfExperiment designOfExperiment_;
  bool firstColumnChecked_;
};
}
#endif