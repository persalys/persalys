//                                               -*- C++ -*-
/**
 *  @brief QAbstractTableModel for cleaning data model sample
 *
 *  Copyright 2015-2020 EDF-Phimeca
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
#ifndef PERSALYS_DATACLEANINGTABLEMODEL_HXX
#define PERSALYS_DATACLEANINGTABLEMODEL_HXX


#include "persalys/DataCleaningTools.hxx"

#include <QAbstractTableModel>

namespace PERSALYS
{
  class PERSALYS_MODEL_API DataCleaningTableModel : public QAbstractTableModel
  {
    Q_OBJECT

  public:
    DataCleaningTableModel(DataCleaningTools * cleaner, QObject* parent = 0);

    int rowCount(const QModelIndex & parent = QModelIndex()) const override;
    int columnCount(const QModelIndex & parent = QModelIndex()) const override;
    Qt::ItemFlags flags(const QModelIndex & index) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const override;
    bool setData(const QModelIndex & index, const QVariant & value, int role = Qt::EditRole) override;
    void updateReplacingValues(const OT::Point vals);
    OT::Sample clean();
    void updateData();

    void prepareForRemoval();
    void prepareForReplacement();
    //signals:
    //  void sampleChanged(const OT::Sample& sample);
    //  void sampleDescriptionChanged(const OT::Description& description);
    //  void errorMessageChanged(const QString & message);
    //temporaryErrorMessageChanged(const QString & message);

  private:
    DataCleaningTools * cleaner_;
    OT::Description names_;
    OT::Point values_;
    OT::Indices selected_;
    OT::Bool removalRequired_;
  };
}
#endif
