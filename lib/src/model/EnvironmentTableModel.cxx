//                                               -*- C++ -*-
/**
 *  @brief QAbstractTableModel to set coupling command environment
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
#include "persalys/EnvironmentTableModel.hxx"

#include "persalys/StudyTreeViewModel.hxx"

#include <QTableView>

using namespace OT;

namespace PERSALYS
{

EnvironmentTableModel::EnvironmentTableModel(const CouplingStep & step, QObject * parent)
  : QAbstractTableModel(parent)
  , step_(step)
{
}


int EnvironmentTableModel::columnCount(const QModelIndex & /*parent*/) const
{
  return 2;
}


int EnvironmentTableModel::rowCount(const QModelIndex & /*parent*/) const
{
  return step_.getEnvironmentKeys().getSize();
}


QVariant EnvironmentTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
  if (role == Qt::DisplayRole && orientation == Qt::Horizontal)
  {
    switch (section)
    {
      case 0:
        return tr("Environment variable");
      case 1:
        return tr("Value");
    }
  }
  return QAbstractTableModel::headerData(section, orientation, role);
}


QVariant EnvironmentTableModel::data(const QModelIndex & index, int role) const
{
  if (!index.isValid())
    return QVariant();
  if (role == Qt::DisplayRole || role == Qt::EditRole)
  {
    switch (index.column())
    {
      case 0:
        return QString::fromUtf8(step_.getEnvironmentKeys()[index.row()].c_str());
      case 1:
        return QString::fromUtf8(step_.getEnvironmentValues()[index.row()].c_str());
    }
  }
  return QVariant();
}


bool EnvironmentTableModel::setData(const QModelIndex & index, const QVariant & value, int role)
{
  if (!index.isValid())
    return false;

  if (role == Qt::EditRole)
  {
    Description keys(step_.getEnvironmentKeys());
    Description vals(step_.getEnvironmentValues());

    switch (index.column())
    {
      case 0:
      {
        if (value.toString().isEmpty())
          return false;
        if (keys[index.row()] == value.toString().toUtf8().constData())
          return true;
        keys[index.row()] = value.toString().toUtf8().constData();
        step_.setEnvironment(keys, vals);
        break;
      }
      case 1:
      {
        if (vals[index.row()] == value.toString().toUtf8().constData())
          return true;
        vals[index.row()] = value.toString().toUtf8().constData();
        step_.setEnvironment(keys, vals);
        break;
      }
    }
    emit stepChanged();
    return true;
  }
  return false;
}


Qt::ItemFlags EnvironmentTableModel::flags(const QModelIndex & index) const
{
  return Qt::ItemIsEditable | QAbstractTableModel::flags(index);
}


void EnvironmentTableModel::updateData()
{
  beginResetModel();
  endResetModel();
}


void EnvironmentTableModel::addLine()
{
  Description keys(step_.getEnvironmentKeys());
  Description vals(step_.getEnvironmentValues());

  keys.add("Variable");
  vals.add("");
  step_.setEnvironment(keys, vals);
  emit stepChanged();
  QModelIndex lastIndex = index(rowCount() - 1, 0);
  beginInsertRows(lastIndex.parent(), lastIndex.row(), lastIndex.row());
  endInsertRows();
  updateData();
}


void EnvironmentTableModel::removeLine()
{
  QTableView * tableView = static_cast<QTableView*>(parent());
  if (!tableView || !tableView->selectionModel() || !tableView->selectionModel()->hasSelection())
    return;

  QModelIndex index = tableView->selectionModel()->currentIndex();
  beginRemoveRows(index.parent(), index.row(), index.row());
  removeRows(index.row(), 1, index.parent());

  Description keys(step_.getEnvironmentKeys());
  Description vals(step_.getEnvironmentValues());
  keys.erase(keys.begin() + index.row());
  vals.erase(vals.begin() + index.row());
  step_.setEnvironment(keys, vals);
  emit stepChanged();
  endRemoveRows();
  updateData();
}
}
