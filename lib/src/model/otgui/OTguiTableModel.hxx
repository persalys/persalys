//                                               -*- C++ -*-
/**
 *  @brief QAbstractTableModel for samples
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
#ifndef OTGUI_OTGUITABLEMODEL_HXX
#define OTGUI_OTGUITABLEMODEL_HXX

#include "NumericalSample.hxx"

#include <QAbstractTableModel>
#include <QModelIndex>

#include "otgui/OTGuiprivate.hxx"

namespace OTGUI {
class OTGUI_API OTguiTableModel : public QAbstractTableModel
{
public:
  OTguiTableModel(const OT::NumericalSample & data);

  int columnCount(const QModelIndex & parent = QModelIndex()) const;
  int rowCount(const QModelIndex & parent) const;
  QVariant data(const QModelIndex & index, int role) const;
  QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
  bool setHeaderData(int section, Qt::Orientation orientation, const QVariant& value, int role = Qt::EditRole);

  bool exportData(const QString & fileName);

  bool sampleIsValid();
private:
  OT::NumericalSample data_;
  mutable bool sampleIsValid_;
};
}
#endif