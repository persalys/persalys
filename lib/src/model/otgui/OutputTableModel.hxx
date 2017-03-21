//                                               -*- C++ -*-
/**
 *  @brief QAbstractTableModel for the list of the outputs
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
#ifndef OTGUI_OUTPUTTABLEMODEL_HXX
#define OTGUI_OUTPUTTABLEMODEL_HXX

#include "otgui/PhysicalModel.hxx"

#include <QAbstractTableModel>
#include <QModelIndex>

namespace OTGUI {
class OTGUI_API OutputTableModel : public QAbstractTableModel
{
  Q_OBJECT

public:
  OutputTableModel(const PhysicalModel & physicalModel, QObject * parent = 0);

  int columnCount(const QModelIndex & parent = QModelIndex()) const;
  int rowCount(const QModelIndex & parent  = QModelIndex()) const;
  QVariant data(const QModelIndex & index, int role) const;
  bool setData(const QModelIndex & index, const QVariant & value, int role);
  Qt::ItemFlags flags(const QModelIndex & index) const;
  QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
  bool setHeaderData(int section, Qt::Orientation orientation, const QVariant& value, int role);

  void removeLine(const QModelIndex & index);
  bool isValid();

public slots:
    void addLine();
signals:
  void errorMessageChanged(QString);
  void checked(const bool) const;
  void selectedOutputsChanged();

private:
  PhysicalModel physicalModel_;
};
}
#endif
