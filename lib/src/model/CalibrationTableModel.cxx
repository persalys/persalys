//                                               -*- C++ -*-
/**
 *  @brief QAbstractTableModel for the list of the outputs
 *
 *  Copyright 2015-2023 EDF-Phimeca
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
#include "persalys/CalibrationTableModel.hxx"

#include "persalys/SymbolicPhysicalModel.hxx"
#include "persalys/StudyTreeViewModel.hxx"
#include "persalys/DistributionDictionary.hxx"

#include <QDebug>

using namespace OT;

namespace PERSALYS
{

CalibrationTableModel::CalibrationTableModel(QObject *parent)
  : QAbstractTableModel(parent)
  , fakeModel_()
{
}


void CalibrationTableModel::updateData(const PhysicalModel &model)
{
  beginResetModel();
  Description previousInputs(fakeModel_.getInputNames());
  InputCollection inputs;
  for (UnsignedInteger i = 0; i < model.getInputDimension(); ++i)
  {
    if (!fakeModel_.hasInputNamed(model.getInputs()[i].getName()))
      inputs.add(model.getInputs()[i]);
    else
      inputs.add(fakeModel_.getInputByName(model.getInputs()[i].getName()));
  }
  fakeModel_.setInputs(inputs);
  if (!model.getDistribution().hasIndependentCopula())
  {
    for (UnsignedInteger i = 0; i < model.getCopulaCollection().getSize(); ++i)
      if (model.getCopulaCollection()[i].getImplementation()->getClassName() != "IndependentCopula")
        fakeModel_.setCopula(model.getCopulaCollection()[i].getDescription(), model.getCopulaCollection()[i]);
  }
  endResetModel();
  emit headerDataChanged(Qt::Horizontal, 0, 0);
}


int CalibrationTableModel::columnCount(const QModelIndex & /*parent*/) const
{
  return 3;
}


int CalibrationTableModel::rowCount(const QModelIndex & /*parent*/) const
{
  return fakeModel_.getInputDimension();
}


Qt::ItemFlags CalibrationTableModel::flags(const QModelIndex &index) const
{
  if (index.column() == 0)
    return QAbstractTableModel::flags(index) | Qt::ItemIsUserCheckable;
  else if (index.column() == 1)
    return QAbstractTableModel::flags(index);
  else
    return QAbstractTableModel::flags(index) | Qt::ItemIsEditable;
}


QVariant CalibrationTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
  if (role == Qt::DisplayRole && orientation == Qt::Horizontal)
  {
    switch (section)
    {
      case 0:
        return tr("Name");
      case 1:
        return tr("Description");
      case 2:
        return tr("Value");
    }
  }
  else if (role == Qt::ToolTipRole && section == 0 && rowCount())
  {
    return fakeModel_.getStochasticInputNames().getSize() == fakeModel_.getInputDimension() ? tr("Deselect all") : tr("Select all");
  }
  return QAbstractTableModel::headerData(section, orientation, role);
}


QVariant CalibrationTableModel::data(const QModelIndex &index, int role) const
{
  if (!index.isValid())
    return QVariant();

  if (role == Qt::CheckStateRole && index.column() == 0)
  {
    return fakeModel_.getInputs()[index.row()].isStochastic() ? Qt::Checked : Qt::Unchecked;
  }
  else if (role == Qt::DisplayRole || role == Qt::EditRole)
  {
    switch (index.column())
    {
      case 0:
        return QString::fromUtf8(fakeModel_.getInputs()[index.row()].getName().c_str());
      case 1:
        return QString::fromUtf8(fakeModel_.getInputs()[index.row()].getDescription().c_str());
      case 2:
        return QString::number(fakeModel_.getInputs()[index.row()].getValue(), 'g', StudyTreeViewModel::DefaultSignificantDigits);
      default:
        return QVariant();
    }
  }
  return QVariant();
}


bool CalibrationTableModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
  if (!index.isValid())
    return false;

  if (role == Qt::CheckStateRole && index.column() == 0)
  {
    const Input input(fakeModel_.getInputs()[index.row()]);

    fakeModel_.setInputStochastic(input.getName(), value.toInt() == Qt::Checked);
    emit dataChanged(index, this->index(index.row(), 1));
    emit headerDataChanged(Qt::Horizontal, 0, 0);
    emit modelChanged(fakeModel_);

    return true;
  }
  else if (role == Qt::EditRole && index.column() == 2)
  {
    const Input input(fakeModel_.getInputs()[index.row()]);

    if (input.getValue() == value.toDouble())
      return true;
    fakeModel_.setInputValue(input.getName(), value.toDouble());
    // update the Input
    const bool previousStatus = input.isStochastic();
    const Distribution distribution(DistributionDictionary::BuildDistribution("Normal", value.toDouble()));
    fakeModel_.setDistribution(input.getName(), distribution);
    fakeModel_.setInputStochastic(input.getName(), previousStatus);
    emit dataChanged(index, index);
    emit modelChanged(fakeModel_);
    return true;
  }
  return false;
}


PointWithDescription CalibrationTableModel::getFixedValues() const
{
  PointWithDescription values;
  Description names;
  for (UnsignedInteger i = 0; i < fakeModel_.getInputDimension(); ++i)
  {
    if (!fakeModel_.getInputs()[i].isStochastic())
    {
      values.add(fakeModel_.getInputs()[i].getValue());
      names.add(fakeModel_.getInputs()[i].getName());
    }
  }
  values.setDescription(names);
  return values;
}


PointWithDescription CalibrationTableModel::getCalibratedInputs() const
{
  PointWithDescription values;
  Description names;
  for (UnsignedInteger i = 0; i < fakeModel_.getInputDimension(); ++i)
  {
    if (fakeModel_.getInputs()[i].isStochastic())
    {
      values.add(fakeModel_.getInputs()[i].getValue());
      names.add(fakeModel_.getInputs()[i].getName());
    }
  }
  values.setDescription(names);
  return values;
}
}
