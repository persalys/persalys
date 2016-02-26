//                                               -*- C++ -*-
/**
 *  @brief QAbstractTableModel to list the inputs
 *
 *  Copyright 2015-2016 EDF
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
#include "otgui/ExperimentTableModel.hxx"

using namespace OT;

namespace OTGUI {

ExperimentTableModel::ExperimentTableModel(const DesignOfExperiment & designOfExperiment)
  : QAbstractTableModel()
  , designOfExperiment_(designOfExperiment)
{
}


int ExperimentTableModel::columnCount(const QModelIndex & parent) const
{
  return 6;
}


int ExperimentTableModel::rowCount(const QModelIndex & parent) const
{
  return designOfExperiment_.getPhysicalModel().getInputs().getSize();
}


Qt::ItemFlags ExperimentTableModel::flags(const QModelIndex & index) const
{
  Qt::ItemFlags result = QAbstractTableModel::flags(index);
  if (index.column() == 0)
  {
    result &= ~Qt::ItemIsEditable;
    result |= Qt::ItemIsSelectable | Qt::ItemIsUserCheckable;
  }
  else if (index.column() == 2 && designOfExperiment_.getLevels()[index.row()] != 1)
    result &= ~Qt::ItemIsEnabled;
  else if (index.column() == 2 && designOfExperiment_.getLevels()[index.row()] == 1)
    result |= Qt::ItemIsEditable | Qt::ItemIsEnabled;
  else if (index.column() > 2 && designOfExperiment_.getLevels()[index.row()] != 1)
    result |= Qt::ItemIsEditable | Qt::ItemIsEnabled;
  else if (index.column() > 2 && designOfExperiment_.getLevels()[index.row()] == 1)
    result &= ~Qt::ItemIsEnabled;
  return result;
}


QVariant ExperimentTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
  if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
  {
    switch (section)
    {
      case 0:
        return tr("Name");
      case 1:
        return tr("Description");
      case 2:
        return tr("Value");
      case 3:
        return tr("Lower bound");
      case 4:
        return tr("Upper bound");
      case 5:
        if (designOfExperiment_.getTypeDesignOfExperiment() == DesignOfExperimentImplementation::FromBoundsAndLevels)
          return tr("Levels");
        else
          return tr("Delta");
      default:
        return QVariant();
    }
  }
  return QAbstractTableModel::headerData(section, orientation, role);
}


bool ExperimentTableModel::setHeaderData(int section, Qt::Orientation orientation, const QVariant& value, int role)
{
  if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
  {
    if (value.toString() == tr("Levels") && designOfExperiment_.getTypeDesignOfExperiment() == DesignOfExperimentImplementation::FromBoundsAndDeltas)
      designOfExperiment_.setLevels(designOfExperiment_.getLevels());
    else if (value.toString() == tr("Delta") && designOfExperiment_.getTypeDesignOfExperiment() == DesignOfExperimentImplementation::FromBoundsAndLevels)
      designOfExperiment_.setDeltas(designOfExperiment_.getDeltas());
    else
      return false;
    emit dataChanged(index(0, 5), index(rowCount()-1, 5));
    emit headerDataChanged(Qt::Horizontal, section, section);
    return true;
  }
  return QAbstractTableModel::setHeaderData(section, orientation, value, role);
}


QVariant ExperimentTableModel::data(const QModelIndex & index, int role) const
{
  if (role == Qt::CheckStateRole && index.column() == 0)
  {
    if (designOfExperiment_.getLevels()[index.row()] == 1)
      return Qt::Unchecked;
    else
      return Qt::Checked;
  }
  else if (role == Qt::DisplayRole || role == Qt::EditRole)
  {
    switch (index.column())
    {
      case 0:
        return designOfExperiment_.getPhysicalModel().getInputs()[index.row()].getName().c_str();
      case 1:
        return designOfExperiment_.getPhysicalModel().getInputs()[index.row()].getDescription().c_str();
      case 2:
        return designOfExperiment_.getValues()[index.row()];
      case 3:
        return designOfExperiment_.getLowerBounds()[index.row()];
      case 4:
        return designOfExperiment_.getUpperBounds()[index.row()];
      case 5:
      {
        if (designOfExperiment_.getTypeDesignOfExperiment() == DesignOfExperimentImplementation::FromBoundsAndLevels)
          return int(designOfExperiment_.getLevels()[index.row()]);
        else
          return designOfExperiment_.getDeltas()[index.row()];
      }
      default:
        return QVariant();
    }
  }
  return QVariant();
}


bool ExperimentTableModel::setData(const QModelIndex & index, const QVariant & value, int role)
{

  if (role == Qt::CheckStateRole && index.column() == 0)
  {
    if (designOfExperiment_.getTypeDesignOfExperiment() == DesignOfExperimentImplementation::FromBoundsAndLevels)
    {
      Indices levels = designOfExperiment_.getLevels();
      if (value.toInt() == Qt::Checked)
        levels[index.row()] = 2;
      else if (value.toInt() == Qt::Unchecked)
        levels[index.row()] = 1;
      else
        return false;
      designOfExperiment_.setLevels(levels);
    }
    else
    {
      NumericalPoint deltas = designOfExperiment_.getDeltas();
      if (value.toDouble() == Qt::Checked)
        deltas[index.row()] = designOfExperiment_.getUpperBounds()[index.row()] - designOfExperiment_.getLowerBounds()[index.row()];
      else if (value.toDouble() == Qt::Unchecked)
        deltas[index.row()] = 0;
      else
        return false;
      designOfExperiment_.setDeltas(deltas);
    }
    emit dataChanged(index, this->index(index.row(), 6));
    return true;
  }
  else if (role == Qt::EditRole)
  {
    switch (index.column())
    {
      case 0:
      case 1:
        return false;
      case 2:
      {
        NumericalPoint values = designOfExperiment_.getValues();
        if (values[index.row()] == value.toDouble())
          return false;
        values[index.row()] = value.toDouble();
        designOfExperiment_.setValues(values);
        break;
      }
      case 3:
      {
        if (value.toDouble() > designOfExperiment_.getUpperBounds()[index.row()])
          return false;
        NumericalPoint lowerBounds = designOfExperiment_.getLowerBounds();
        if (lowerBounds[index.row()] == value.toDouble())
          return false;
        lowerBounds[index.row()] = value.toDouble();
        designOfExperiment_.setLowerBounds(lowerBounds);
        break;
      }
      case 4:
      {
        if (value.toDouble() < designOfExperiment_.getLowerBounds()[index.row()])
          return false;
        NumericalPoint upperBounds = designOfExperiment_.getUpperBounds();
        if (upperBounds[index.row()] == value.toDouble())
          return false;
        upperBounds[index.row()] = value.toDouble();
        designOfExperiment_.setUpperBounds(upperBounds);
        break;
      }
      case 5:
      {
        if (designOfExperiment_.getTypeDesignOfExperiment() == DesignOfExperimentImplementation::FromBoundsAndLevels)
        {
          if (value.toInt() < 2)
            return false;
          Indices nbValues = designOfExperiment_.getLevels();
          if (nbValues[index.row()] == value.toUInt())
            return false;
          nbValues[index.row()] = value.toUInt();
          designOfExperiment_.setLevels(nbValues);
          break;
        }
        else
        {
          NumericalPoint deltas = designOfExperiment_.getDeltas();
          if (deltas[index.row()] == value.toDouble())
            return false;
          deltas[index.row()] = value.toDouble();
          designOfExperiment_.setDeltas(deltas);
          break;
        }
      }
      default:
        return false;
    }
  }

  emit designOfExperimentChanged(designOfExperiment_);

  return true;
}


DesignOfExperiment ExperimentTableModel::getDesignOfExperiment() const
{
  return designOfExperiment_;
}
}
