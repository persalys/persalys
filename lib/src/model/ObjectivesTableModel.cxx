//                                               -*- C++ -*-
/**
 *  @brief QAbstractTableModel to list the objectives
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

#include "persalys/ObjectivesTableModel.hxx"

#include <QHeaderView>

using namespace OT;

namespace PERSALYS
{

ObjectivesTableModel::ObjectivesTableModel(QObject * parent)
  : QAbstractTableModel(parent)
{
}

void ObjectivesTableModel::updateTable(MultiObjectiveOptimizationAnalysis& analysis)
{
  vars_.clear();
  settings_.clear();
  minimization_.clear();

  for(UnsignedInteger i = 0; i < analysis.getInterestVariables().getSize(); ++i)
  {
    String varName = analysis.getInterestVariables()[i];
    vars_ << QString::fromUtf8(varName.c_str());
    if (analysis.getMinimization().contains(varName))
    {
      settings_ << tr("Minimization");
      minimization_.add(varName);
    }
    else
    {
      settings_ << tr("Maximization");
    }
  }
  updateData();
}

int ObjectivesTableModel::columnCount(const QModelIndex & /*parent*/) const
{
  return 2;
}


int ObjectivesTableModel::rowCount(const QModelIndex & /*parent*/) const
{
  return settings_.size();
}


QVariant ObjectivesTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
  if (role == Qt::DisplayRole && orientation == Qt::Horizontal)
  {
    switch (section)
    {
      case 0:
        return tr("Name");
      case 1:
        return tr("Objective");
    }
  }
  return QAbstractTableModel::headerData(section, orientation, role);
}


QVariant ObjectivesTableModel::data(const QModelIndex & index, int role) const
{
  if (!index.isValid())
    return QVariant();

  if (role == Qt::DisplayRole || role == Qt::EditRole)
  {
    switch (index.column())
    {
      case 0:
        return vars_[index.row()];
      case 1:
        return settings_[index.row()];
    }
  }
  else if (role == Qt::UserRole + 1)
    return QStringList() << tr("Minimization") << tr("Maximization");
  else if (role == Qt::BackgroundRole && index.column() == 1)
    return QHeaderView(Qt::Horizontal).palette().color(QPalette::Window);
  return QVariant();
}


bool ObjectivesTableModel::setData(const QModelIndex & index, const QVariant & value, int role)
{
  if (!index.isValid())
    return false;
  if (role == Qt::EditRole && index.column() == 1)
  {
    settings_[index.row()] = value.toString();
    const String varName = data(this->index(index.row(), 0), role).toString().toStdString();
    if (settings_[index.row()] == tr("Minimization"))
    {
      if ( !minimization_.contains(varName) )
        minimization_.add(varName);
    }
    else
    {
      if (minimization_.contains(varName))
        minimization_.erase(std::remove(minimization_.begin(), minimization_.end(), varName), minimization_.end());
    }
  }
  return true;
}


Qt::ItemFlags ObjectivesTableModel::flags(const QModelIndex & index) const
{
  Qt::ItemFlags result = QAbstractTableModel::flags(index);
  if (index.column() == 0)
    result &= ~Qt::ItemIsEditable;
  else if (index.column() == 1)
    result |= Qt::ItemIsEditable | Qt::ItemIsEnabled;
  return result;
}


void ObjectivesTableModel::updateData()
{
  beginResetModel();
  endResetModel();
}

Description ObjectivesTableModel::getMinimization() const
{
  return minimization_;
}
}
