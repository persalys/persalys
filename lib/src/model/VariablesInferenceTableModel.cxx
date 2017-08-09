//                                               -*- C++ -*-
/**
 *  @brief QAbstractTableModel to list the variables of a DOE
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
#include "otgui/VariablesInferenceTableModel.hxx"

using namespace OT;

namespace OTGUI {

VariablesInferenceTableModel::VariablesInferenceTableModel(const Description& variablesNames, const Interval::BoolCollection& isVariablesChecked, QObject* parent)
  : QAbstractTableModel(parent)
  , variablesNames_(variablesNames)
  , isVariablesChecked_(isVariablesChecked)
{
  if (variablesNames_.getSize() != isVariablesChecked_.getSize())
    throw InvalidArgumentException(HERE) << "VariablesInferenceTableModel: The 2 arguments must have the same dimension";
}


int VariablesInferenceTableModel::columnCount(const QModelIndex& parent) const
{
  return 1;
}


int VariablesInferenceTableModel::rowCount(const QModelIndex& parent) const
{
  return variablesNames_.getSize();
}


Qt::ItemFlags VariablesInferenceTableModel::flags(const QModelIndex& index) const
{
  if (index.column() == 0)
    return QAbstractTableModel::flags(index) | Qt::ItemIsEditable | Qt::ItemIsUserCheckable;
  return QAbstractTableModel::flags(index);
}


QVariant VariablesInferenceTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
  if (role == Qt::DisplayRole && orientation == Qt::Horizontal && section == 0)
    return tr("Variable(s)", "", variablesNames_.getSize());
  return QAbstractTableModel::headerData(section, orientation, role);
}


bool VariablesInferenceTableModel::setHeaderData(int section, Qt::Orientation orientation, const QVariant& value, int role)
{
  if (role == Qt::CheckStateRole && orientation == Qt::Horizontal)
  {
    for (int i = 0; i < rowCount(); ++i)
      if (data(index(i, 0), role).toInt() != (value.toBool() ? Qt::Checked : Qt::Unchecked))
        setData(index(i, 0), value.toBool() ? Qt::Checked : Qt::Unchecked, role);
  }
  return QAbstractTableModel::setHeaderData(section, orientation, value, role);
}


QVariant VariablesInferenceTableModel::data(const QModelIndex& index, int role) const
{
  if (!index.isValid())
    return QVariant();

  if (role == Qt::CheckStateRole && index.column() == 0)
  {
    return (isVariablesChecked_[index.row()] ? Qt::Checked : Qt::Unchecked);
  }
  else if (role == Qt::DisplayRole || role == Qt::EditRole)
  {
    return QString::fromUtf8(variablesNames_[index.row()].c_str());
  }
  return QVariant();
}


bool VariablesInferenceTableModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
  if (!index.isValid())
    return false;

  if (role == Qt::CheckStateRole && index.column() == 0)
  {
    isVariablesChecked_[index.row()] = ((value.toInt() == Qt::Checked) ? true : false);
    Description checkedVariables;
    for (UnsignedInteger i = 0; i < isVariablesChecked_.getSize(); ++i)
      if (isVariablesChecked_[i])
        checkedVariables.add(variablesNames_[i]);
    emit dataChanged(index, this->index(index.row(), 1));
    emit checked(!isVariablesChecked_.contains(false));
    emit selectionChanged(checkedVariables, variablesNames_[index.row()]);
    return true;
  }
  return false;
}
}
