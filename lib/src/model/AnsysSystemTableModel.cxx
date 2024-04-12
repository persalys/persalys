//                                               -*- C++ -*-
/**
 *  @brief QAbstractTableModel to handle systems from ansys project file
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

#include "persalys/AnsysSystemTableModel.hxx"

#include "persalys/QtTools.hxx"

using namespace OT;

namespace PERSALYS
{

SysInfo::SysInfo()
  : text(""), selected(true)
{}


AnsysSystemTableModel::AnsysSystemTableModel(QObject * parent)
  : QAbstractTableModel(parent)
{}

int AnsysSystemTableModel::columnCount(const QModelIndex & /*parent*/) const
{
  return 3;
}

int AnsysSystemTableModel::rowCount(const QModelIndex & /* parent */) const
{
  return sysInfos_.count();
}

QVariant AnsysSystemTableModel::data(const QModelIndex & index, int role) const
{
  if (!index.isValid())
    return QVariant();

  if (role == Qt::TextAlignmentRole)
    return Qt::AlignLeft;

  SysInfo info(sysInfos_[sysInfoKeysSorted_[index.row()]]);

  if (role == Qt::CheckStateRole)
  {
    if (index.column() == 0)
      return info.selected ? Qt::Checked : Qt::Unchecked;
    else
      return QVariant();
  }

  if (role != Qt::DisplayRole)
    return QVariant();
  switch(index.column())
  {
    case 0:
      return sysInfoKeysSorted_[index.row()];
    case 1:
      return QString(info.text.c_str());
    case 2:
      return QString(info.type.c_str());
    default:
      return QVariant();
  }
  return QVariant();
}

bool AnsysSystemTableModel::setData(const QModelIndex & index, const QVariant & value, int role)
{
  if (!index.isValid())
    return false;

  if ((index.column() == 0) && (role == Qt::CheckStateRole))
  {
    sysInfos_[sysInfoKeysSorted_[index.row()]].selected = (value.toInt() == Qt::Checked);
    QModelIndex topLeft = index;
    QModelIndex bottomRight = index;
    emit dataChanged(topLeft, bottomRight);
    emit headerDataChanged(Qt::Horizontal, 0, 0);
    return true;
  }
  return false;
}

Qt::ItemFlags AnsysSystemTableModel::flags(const QModelIndex & index) const
{
  Qt::ItemFlags mflags = QAbstractTableModel::flags(index);
  if (index.column() == 0)
  {
    mflags &= ~Qt::ItemIsEditable;
    mflags |= Qt::ItemIsUserCheckable;
  }
  return mflags;
}

QVariant AnsysSystemTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
  if (role == Qt::DisplayRole && orientation == Qt::Horizontal)
  {
    switch (section)
    {
      case 0:
        return tr("System name");
      case 1:
        return tr("System header text");
      case 2:
        return tr("System type");
      case 3:
        return tr("Selected");
    }
  }
  return QAbstractTableModel::headerData(section, orientation, role);
}

QStringList AnsysSystemTableModel::getSystems() const
{
  QStringList result;
  foreach (QString sysName, sysInfoKeysSorted_)
  {
    if (sysInfos_[sysName].selected)
      result.append(sysName);
  }
  return result;
}

void AnsysSystemTableModel::loadData(AnsysParser* parser)
{
  sysInfos_.clear();
  for (auto const& sys : parser->getSystems())
  {
    SysInfo info;
    info.text = sys.second.first;
    info.type = sys.second.second;
    sysInfos_[QString(sys.first.c_str())] = info;
  }
  sysInfoKeysSorted_ = QtOT::NaturalSorting(sysInfos_.keys());
  emit layoutChanged();
}
}
