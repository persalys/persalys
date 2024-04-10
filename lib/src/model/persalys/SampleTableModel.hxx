//                                               -*- C++ -*-
/**
 *  @brief QAbstractTableModel for samples
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
#ifndef PERSALYS_SAMPLETABLEMODEL_HXX
#define PERSALYS_SAMPLETABLEMODEL_HXX

#include "persalys/PersalysPrivate.hxx"

#include <openturns/Sample.hxx>

#include <QSortFilterProxyModel>

namespace PERSALYS
{

class PERSALYS_MODEL_API SampleTableProxyModel : public QSortFilterProxyModel
{
  Q_OBJECT
public:
  SampleTableProxyModel(QObject *parent = nullptr)
  : QSortFilterProxyModel(parent)
  {}

  QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const
  {
    // do not sort the vertical header items
    if (orientation == Qt::Vertical && role == Qt::DisplayRole)
      return section;
    return QSortFilterProxyModel::headerData(section, orientation, role);
  }
};


class PERSALYS_MODEL_API SampleTableModel : public QAbstractTableModel
{
  Q_OBJECT

public:
  SampleTableModel(const OT::Sample & data, const bool isEditable, const bool hasRowIDcolumn, const OT::Description& initialDescription, QObject *parent = nullptr);
  SampleTableModel(const OT::Sample & data, QObject *parent = nullptr);
  SampleTableModel(const OT::Sample & data, const bool isEditable, QObject *parent = nullptr);
  SampleTableModel(const OT::Sample & data, const OT::Description& initialDescription, QObject *parent = nullptr);

  int columnCount(const QModelIndex & parent = QModelIndex()) const override;
  int rowCount(const QModelIndex & parent = QModelIndex()) const override;
  Qt::ItemFlags flags(const QModelIndex & index) const override;
  QVariant data(const QModelIndex & index, int role) const override;
  QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
  bool setHeaderData(int section, Qt::Orientation orientation, const QVariant& value, int role = Qt::EditRole) override;
  bool setData(const QModelIndex & index, const QVariant & value, int role = Qt::EditRole) override;
  bool insertRows(int row, int count, const QModelIndex &index = QModelIndex()) override;
  bool removeRows(int row, int count, const QModelIndex &index = QModelIndex()) override;
  OT::Sample getSample() const;

public slots:
  void updateHeaderData(const OT::Description & header);
  void updateData(const OT::Sample & data);
signals:
  void errorMessageChanged(const QString &message);
  void sampleChanged();

protected:
  OT::Sample data_;
  bool isEditable_;
  bool hasRowIDcolumn_;
  OT::Description initialDescription_;
};
}
#endif
