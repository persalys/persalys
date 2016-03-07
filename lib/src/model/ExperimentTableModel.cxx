//                                               -*- C++ -*-
/**
 *  @brief QAbstractTableModel to list the inputs
 *
 *  Copyright 2015-2016 EDF-Phimeca
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
  // +1 for the header
  return designOfExperiment_.getPhysicalModel().getInputs().getSize() + 1;
}


Qt::ItemFlags ExperimentTableModel::flags(const QModelIndex & index) const
{
  Qt::ItemFlags result = QAbstractTableModel::flags(index);
  if (index.row() == 0)
  {
    if (index.column() == 5)
      return result |= Qt::ItemIsEditable | Qt::ItemIsEnabled;
    else if (index.column() != 5)
      return result &= ~Qt::ItemIsEditable;
  }
  else
  {
    int indexInput = index.row() - 1;
    if (index.column() == 0)
    {
      result &= ~Qt::ItemIsEditable;
      result |= Qt::ItemIsSelectable | Qt::ItemIsUserCheckable;
    }
    else if (index.column() == 2 && designOfExperiment_.getLevels()[indexInput] != 1)
      result &= ~Qt::ItemIsEnabled;
    else if (index.column() == 2 && designOfExperiment_.getLevels()[indexInput] == 1)
      result |= Qt::ItemIsEditable | Qt::ItemIsEnabled;
    else if (index.column() > 2 && designOfExperiment_.getLevels()[indexInput] != 1)
      result |= Qt::ItemIsEditable | Qt::ItemIsEnabled;
    else if (index.column() > 2 && designOfExperiment_.getLevels()[indexInput] == 1)
      result &= ~Qt::ItemIsEnabled;
  }
  return result;

}


QVariant ExperimentTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
  if (orientation == Qt::Vertical && role == Qt::DisplayRole)
  {
    if (section != 0)
      return section - 1;
    else
      return QVariant();
  }
  return QAbstractItemModel::headerData(section, orientation, role);
}


QVariant ExperimentTableModel::data(const QModelIndex & index, int role) const
{
  // header
  if (index.row() == 0)
  {
    if (role == Qt::DisplayRole || role == Qt::EditRole)
    {
      switch (index.column())
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
    else
    {
      return headerData(index.column(), Qt::Horizontal, role);
    }
  }
  // not header
  else
  {
    if (role == Qt::CheckStateRole && index.column() == 0)
    {
      if (designOfExperiment_.getLevels()[index.row()-1] == 1)
        return Qt::Unchecked;
      else
        return Qt::Checked;
    }
    else if (role == Qt::DisplayRole || role == Qt::EditRole)
    {
      int indexInput = index.row() - 1;
      switch (index.column())
      {
        case 0:
          return QString::fromLocal8Bit(designOfExperiment_.getPhysicalModel().getInputs()[indexInput].getName().c_str());
        case 1:
          return QString::fromLocal8Bit(designOfExperiment_.getPhysicalModel().getInputs()[indexInput].getDescription().c_str());
        case 2:
          return designOfExperiment_.getValues()[indexInput];
        case 3:
          return designOfExperiment_.getLowerBounds()[indexInput];
        case 4:
          return designOfExperiment_.getUpperBounds()[indexInput];
        case 5:
        {
          if (designOfExperiment_.getTypeDesignOfExperiment() == DesignOfExperimentImplementation::FromBoundsAndLevels)
            return int(designOfExperiment_.getLevels()[indexInput]);
          else
            return designOfExperiment_.getDeltas()[indexInput];
        }
        default:
          return QVariant();
      }
    }
  }
  return QVariant();
}


bool ExperimentTableModel::setData(const QModelIndex & index, const QVariant & value, int role)
{

  if (role == Qt::CheckStateRole && index.column() == 0 && index.row() != 0)
  {
    int indexInput = index.row() - 1;
    if (designOfExperiment_.getTypeDesignOfExperiment() == DesignOfExperimentImplementation::FromBoundsAndLevels)
    {
      Indices levels = designOfExperiment_.getLevels();
      if (value.toInt() == Qt::Checked)
        levels[indexInput] = 2;
      else if (value.toInt() == Qt::Unchecked)
        levels[indexInput] = 1;
      else
        return false;
      designOfExperiment_.setLevels(levels);
    }
    else
    {
      NumericalPoint deltas = designOfExperiment_.getDeltas();
      if (value.toDouble() == Qt::Checked)
        deltas[indexInput] = designOfExperiment_.getUpperBounds()[indexInput] - designOfExperiment_.getLowerBounds()[indexInput];
      else if (value.toDouble() == Qt::Unchecked)
        deltas[indexInput] = 0;
      else
        return false;
      designOfExperiment_.setDeltas(deltas);
    }
    emit dataChanged(index, this->index(indexInput, 6));
    return true;
  }
  else if (role == Qt::EditRole && index.row() == 0 && index.column() == 5)
  {
    if (value.toString() == tr("Levels") && designOfExperiment_.getTypeDesignOfExperiment() == DesignOfExperimentImplementation::FromBoundsAndDeltas)
      designOfExperiment_.setLevels(designOfExperiment_.getLevels());
    else if (value.toString() == tr("Delta") && designOfExperiment_.getTypeDesignOfExperiment() == DesignOfExperimentImplementation::FromBoundsAndLevels)
      designOfExperiment_.setDeltas(designOfExperiment_.getDeltas());
    else
      return false;
    emit dataChanged(this->index(1, 5), this->index(rowCount()-1, 5));
    return true;
  }
  else if (role == Qt::EditRole && index.row() != 0)
  {
    int indexInput = index.row() - 1;
    switch (index.column())
    {
      case 0:
      case 1:
        return false;
      case 2:
      {
        NumericalPoint values = designOfExperiment_.getValues();
        if (values[indexInput] == value.toDouble())
          return false;
        values[indexInput] = value.toDouble();
        designOfExperiment_.setValues(values);
        break;
      }
      case 3:
      {
        if (value.toDouble() >= designOfExperiment_.getUpperBounds()[indexInput])
          return false;
        NumericalPoint lowerBounds = designOfExperiment_.getLowerBounds();
        if (lowerBounds[indexInput] == value.toDouble())
          return false;
        lowerBounds[indexInput] = value.toDouble();
        designOfExperiment_.setLowerBounds(lowerBounds);
        break;
      }
      case 4:
      {
        if (value.toDouble() <= designOfExperiment_.getLowerBounds()[indexInput])
          return false;
        NumericalPoint upperBounds = designOfExperiment_.getUpperBounds();
        if (upperBounds[indexInput] == value.toDouble())
          return false;
        upperBounds[indexInput] = value.toDouble();
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
          if (nbValues[indexInput] == value.toUInt())
            return false;
          nbValues[indexInput] = value.toUInt();
          designOfExperiment_.setLevels(nbValues);
          break;
        }
        else
        {
          NumericalPoint deltas = designOfExperiment_.getDeltas();
          if (deltas[indexInput] == value.toDouble())
            return false;
          deltas[indexInput] = value.toDouble();
          if (deltas[indexInput] <= (designOfExperiment_.getUpperBounds()[indexInput] - designOfExperiment_.getLowerBounds()[indexInput]))
            designOfExperiment_.setDeltas(deltas);
          else
            return false;
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
