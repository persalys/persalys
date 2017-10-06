//                                               -*- C++ -*-
/**
 *  @brief QAbstractTableModel for data model sample
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
#ifndef OTGUI_DATAMODELTABLEMODEL_HXX
#define OTGUI_DATAMODELTABLEMODEL_HXX

#include "otgui/DataModel.hxx"
#include "otgui/DesignOfExperiment.hxx"
#include "otgui/SampleTableModel.hxx"

namespace OTGUI
{
class OTGUI_API DataModelTableModel : public SampleTableModel
{
  Q_OBJECT

public:
  DataModelTableModel(DataModel* dataModel, QObject* parent = 0);

  int rowCount(const QModelIndex & parent = QModelIndex()) const;
  Qt::ItemFlags flags(const QModelIndex & index) const;
  QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
  QVariant data(const QModelIndex & index, int role) const;
  bool setData(const QModelIndex & index, const QVariant & value, int role);
  void updateData(const OT::Sample& data, const bool isReloadAction = false, const bool useColumns = false);

protected:
  void updatePrivateData(const bool isReloadAction, const bool useColumns);

signals:
  void errorMessageChanged(const QString & message);
  void temporaryErrorMessageChanged(const QString & message);

private:
  DataModel * dataModel_;
  OT::Indices inputColumns_;
  OT::Indices outputColumns_;
};
}
#endif
