//                                               -*- C++ -*-
/**
 *  @brief QAbstractTableModel to handle systems from ansys project file
 *
 *  Copyright 2015-2025 EDF-Phimeca
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

#ifndef PERSALYS_ANSYSSYSTEMTABLEMODEL_HXX
#define PERSALYS_ANSYSSYSTEMTABLEMODEL_HXX

#include <QAbstractTableModel>

#include "persalys/AnsysParser.hxx"

namespace PERSALYS
{
struct SysInfo
{
  OT::String text;
  OT::String type;
  bool selected;
  SysInfo();
};

class PERSALYS_MODEL_API AnsysSystemTableModel : public QAbstractTableModel
{
  Q_OBJECT
public:
  AnsysSystemTableModel(QObject *parent = nullptr);
  int columnCount(const QModelIndex & parent = QModelIndex()) const;
  int rowCount(const QModelIndex & parent = QModelIndex()) const;
  QVariant data(const QModelIndex & index, int role) const;
  bool setData(const QModelIndex & index, const QVariant & value, int role);
  Qt::ItemFlags flags(const QModelIndex & index) const;
  QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

  void loadData(AnsysParser* parser);

  QStringList getSystems() const;

private:
  QMap <QString, SysInfo> sysInfos_;
  QStringList sysInfoKeysSorted_;
};
}
#endif
