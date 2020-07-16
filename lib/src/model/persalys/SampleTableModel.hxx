//                                               -*- C++ -*-
/**
 *  @brief QAbstractTableModel for samples
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
#ifndef PERSALYS_SAMPLETABLEMODEL_HXX
#define PERSALYS_SAMPLETABLEMODEL_HXX

#include "persalys/PersalysPrivate.hxx"

#include <openturns/Sample.hxx>

#include <QSortFilterProxyModel>

namespace PERSALYS
{

class SampleTableProxyModel : public QSortFilterProxyModel
{
public:
  SampleTableProxyModel(QObject *parent = 0)
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
  SampleTableModel(const OT::Sample & data, const bool isEditable, const bool hasRowIDcolumn, const OT::Description& initialDescription, QObject * parent = 0);
  SampleTableModel(const OT::Sample & data, QObject * parent = 0);
  SampleTableModel(const OT::Sample & data, const OT::Description& initialDescription, QObject * parent = 0);

  int columnCount(const QModelIndex & parent = QModelIndex()) const;
  int rowCount(const QModelIndex & parent = QModelIndex()) const;
  Qt::ItemFlags flags(const QModelIndex & index) const;
  QVariant data(const QModelIndex & index, int role) const;
  QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
  bool setHeaderData(int section, Qt::Orientation orientation, const QVariant& value, int role = Qt::EditRole);
  bool setData(const QModelIndex & index, const QVariant & value, int role = Qt::EditRole);

  OT::Sample getSample() const;

public slots:
  void updateHeaderData(const OT::Description & header);
  void updateData(const OT::Sample & data);
signals:
  void errorMessageChanged(const QString &message);

protected:
  OT::Sample data_;
  bool isEditable_;
  bool hasRowIDcolumn_;
  OT::Description initialDescription_;
};
}
#endif
