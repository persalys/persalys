//                                               -*- C++ -*-
/**
 *  @brief QAbstractTableModel for data model sample
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
#ifndef PERSALYS_DATAMODELTABLEMODEL_HXX
#define PERSALYS_DATAMODELTABLEMODEL_HXX

#include "persalys/DataModel.hxx"

#include <QAbstractTableModel>

namespace PERSALYS
{
class PERSALYS_MODEL_API DataModelTableModel : public QAbstractTableModel
{
  Q_OBJECT

public:
  DataModelTableModel(DataModel* dataModel, QObject *parent = nullptr);

  int rowCount(const QModelIndex & parent = QModelIndex()) const;
  int columnCount(const QModelIndex & parent = QModelIndex()) const;
  Qt::ItemFlags flags(const QModelIndex & index) const;
  QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
  QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;
  bool setData(const QModelIndex & index, const QVariant & value, int role = Qt::EditRole);
  void updateData();

signals:
  void sampleChanged(const OT::Sample& sample);
  void sampleDescriptionChanged(const OT::Description& description);
  void errorMessageChanged(const QString & message);
  void temporaryErrorMessageChanged(const QString & message);

private:
  DataModel * dataModel_;
  OT::Description names_;
  OT::Indices inputColumns_;
  OT::Indices outputColumns_;
};
}
#endif
