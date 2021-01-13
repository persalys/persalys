//                                               -*- C++ -*-
/**
 *  @brief QAbstractTableModel to list and select variables
 *
 *  Copyright 2015-2021 EDF-Phimeca
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
#include "persalys/VariablesSelectionTableModel.hxx"

using namespace OT;

namespace PERSALYS
{

VariablesSelectionTableModel::VariablesSelectionTableModel(const Description& variablesNames, const Interval::BoolCollection& isVariablesChecked, QObject* parent)
  : QAbstractTableModel(parent)
  , variablesNames_(variablesNames)
  , isVariableChecked_(isVariablesChecked)
  , isVariableEnabled_(variablesNames.getSize(), true)
{
  if (variablesNames_.getSize() != isVariableChecked_.getSize())
    throw InvalidArgumentException(HERE) << "VariablesSelectionTableModel: The 2 arguments must have the same dimension";
}


int VariablesSelectionTableModel::columnCount(const QModelIndex& /*parent*/) const
{
  return 1;
}


int VariablesSelectionTableModel::rowCount(const QModelIndex& /*parent*/) const
{
  return variablesNames_.getSize();
}


Qt::ItemFlags VariablesSelectionTableModel::flags(const QModelIndex& index) const
{
  Qt::ItemFlags result = QAbstractTableModel::flags(index);
  if (index.column() == 0)
    result |= Qt::ItemIsEditable | Qt::ItemIsUserCheckable;
  if (!isVariableEnabled_[index.row()])
    result &= ~Qt::ItemIsEnabled;
  return result;
}


QVariant VariablesSelectionTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
  if (role == Qt::DisplayRole && orientation == Qt::Horizontal && section == 0)
  {
    return tr("Variables");
  }
  else if (role == Qt::ToolTipRole && section == 0 && rowCount())
  {
    bool allChecked = true;
    for (UnsignedInteger i = 0; i < isVariableChecked_.getSize(); ++i)
    {
      if (isVariableEnabled_[i])
        allChecked = allChecked && isVariableChecked_[i];
    }
    return allChecked ? tr("Deselect all") : tr("Select all");
  }

  return QAbstractTableModel::headerData(section, orientation, role);
}


QVariant VariablesSelectionTableModel::data(const QModelIndex& index, int role) const
{
  if (!index.isValid())
    return QVariant();

  if (role == Qt::CheckStateRole && index.column() == 0)
  {
    return (isVariableChecked_[index.row()] ? Qt::Checked : Qt::Unchecked);
  }
  else if (role == Qt::DisplayRole || role == Qt::EditRole)
  {
    return QString::fromUtf8(variablesNames_[index.row()].c_str());
  }
  return QVariant();
}


bool VariablesSelectionTableModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
  if (!index.isValid())
    return false;

  if (role == Qt::CheckStateRole && index.column() == 0)
  {
    isVariableChecked_[index.row()] = ((value.toInt() == Qt::Checked) ? true : false);
    Description checkedVariables;
    for (UnsignedInteger i = 0; i < isVariableChecked_.getSize(); ++i)
      if (isVariableChecked_[i])
        checkedVariables.add(variablesNames_[i]);
    emit dataChanged(index, this->index(index.row(), 1));
    emit headerDataChanged(Qt::Horizontal, 0, 0);
    emit selectionChanged(checkedVariables, variablesNames_[index.row()]);
    return true;
  }
  return false;
}


Description VariablesSelectionTableModel::getSelectedVariables() const
{
  Description selectedVariables;
  for (UnsignedInteger i = 0; i < isVariableChecked_.getSize(); ++i)
    if (isVariableChecked_[i])
      selectedVariables.add(variablesNames_[i]);
  return selectedVariables;
}


void VariablesSelectionTableModel::updateData(const Description &variablesNames, const OT::Interval::BoolCollection &isVariablesEnabled)
{
  Q_ASSERT(isVariablesEnabled.getSize() == variablesNames.getSize());
  beginResetModel();
  variablesNames_ = variablesNames;
  isVariableChecked_ = Interval::BoolCollection(variablesNames_.getSize(), false);
  isVariableEnabled_ = isVariablesEnabled;
  endResetModel();
  emit headerDataChanged(Qt::Horizontal, 0, 0);
}
}
