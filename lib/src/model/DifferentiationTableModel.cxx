//                                               -*- C++ -*-
/**
 *  @brief QAbstractTableModel to define finite difference steps
 *
 *  Copyright 2015-2022 EDF-Phimeca
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
#include "persalys/DifferentiationTableModel.hxx"

#include "persalys/StudyTreeViewModel.hxx"

using namespace OT;

namespace PERSALYS
{

DifferentiationTableModel::DifferentiationTableModel(const PhysicalModel& physicalModel, QObject* parent)
  : QAbstractTableModel(parent)
  , physicalModel_(physicalModel)
{
}


int DifferentiationTableModel::columnCount(const QModelIndex& /*parent*/) const
{
  return 2;
}


int DifferentiationTableModel::rowCount(const QModelIndex& /*parent*/) const
{
  return physicalModel_.getInputDimension();
}


Qt::ItemFlags DifferentiationTableModel::flags(const QModelIndex & index) const
{
  if (index.column() == 1)
    return QAbstractTableModel::flags(index) | Qt::ItemIsEditable;
  else
    return QAbstractTableModel::flags(index);
}


QVariant DifferentiationTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
  if (role == Qt::DisplayRole && orientation == Qt::Horizontal)
  {
    if (section == 0)
      return tr("Variable");
    else if (section == 1)
      return tr("Step");
  }

  return QAbstractTableModel::headerData(section, orientation, role);
}


QVariant DifferentiationTableModel::data(const QModelIndex& index, int role) const
{
  if (!index.isValid())
    return QVariant();

  if (role == Qt::DisplayRole || role == Qt::EditRole)
  {
    if (index.column() == 0)
      return QString::fromUtf8(physicalModel_.getInputs()[index.row()].getName().c_str());
    else if (index.column() == 1)
      return QString::number(physicalModel_.getInputs()[index.row()].getFiniteDifferenceStep(), 'g', StudyTreeViewModel::DefaultSignificantDigits);
  }

  return QVariant();
}


bool DifferentiationTableModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
  if (!index.isValid())
    return false;

  if (role == Qt::EditRole)
  {
    if (value.toDouble() == physicalModel_.getInputs()[index.row()].getFiniteDifferenceStep())
      return true;

    // update the physicalModel
    const Input input(physicalModel_.getInputs()[index.row()]);
    physicalModel_.blockNotification("PhysicalModelDefinitionItem");
    physicalModel_.setFiniteDifferenceStep(input.getName(), value.toDouble());
    physicalModel_.blockNotification();
    emit dataChanged(index, index);
    return true;
  }
  return false;
}


void DifferentiationTableModel::updateData()
{
  beginResetModel();
  endResetModel();
}


void DifferentiationTableModel::applyValueToAll(double value)
{
  for (int i = 0; i < rowCount(); ++i)
  {
    setData(index(i, 1), value, Qt::EditRole);
  }
}
}
