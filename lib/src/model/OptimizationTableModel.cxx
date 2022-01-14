//                                               -*- C++ -*-
/**
 *  @brief QAbstractTableModel to list the inputs
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
#include "persalys/OptimizationTableModel.hxx"

#include "persalys/StudyTreeViewModel.hxx"

using namespace OT;

namespace PERSALYS
{

OptimizationTableModel::OptimizationTableModel(const OptimizationAnalysis & analysis, QObject * parent)
  : QAbstractTableModel(parent)
  , analysis_(analysis)
  , firstColumnChecked_(false)
{
  analysis_.updateParameters();
}


int OptimizationTableModel::columnCount(const QModelIndex & /*parent*/) const
{
  return 5;
}


int OptimizationTableModel::rowCount(const QModelIndex & /*parent*/) const
{
  // +1 for the header
  return analysis_.getPhysicalModel().getInputs().getSize() + 1;
}


Qt::ItemFlags OptimizationTableModel::flags(const QModelIndex & index) const
{
  Qt::ItemFlags result = QAbstractTableModel::flags(index);

  if (index.column() == 0)
  {
    result |= Qt::ItemIsUserCheckable;
    result &= ~Qt::ItemIsEditable;
    result |= Qt::ItemIsSelectable;
  }

  // header
  if (index.row() == 0)
  {
    return result &= ~Qt::ItemIsEditable;
  }
  // not header
  else
  {
    if (index.column() == 2)
    {
      result |= Qt::ItemIsEditable;
    }
    else if (index.column() == 3 || index.column() == 4)
    {
      result |= Qt::ItemIsUserCheckable;

      if (analysis_.getVariableInputs().contains(analysis_.getPhysicalModel().getInputNames()[index.row() - 1]))
        result |= Qt::ItemIsEnabled;
      else
        result &= ~Qt::ItemIsEnabled;

      if ((index.column() == 3 && analysis_.getBounds().getFiniteLowerBound()[index.row() - 1]) ||
          (index.column() == 4 && analysis_.getBounds().getFiniteUpperBound()[index.row() - 1]))
        result |= Qt::ItemIsEditable;
    }
  }
  return result;
}


QVariant OptimizationTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
  if (orientation == Qt::Vertical && role == Qt::DisplayRole)
  {
    if (section != 0)
      return section;
    else // first line is considered as the header of the table: do not set a line number
      return QVariant();
  }
  return QAbstractItemModel::headerData(section, orientation, role);
}


QVariant OptimizationTableModel::data(const QModelIndex & ind, int role) const
{
  // header
  if (ind.row() == 0)
  {
    if (role == Qt::DisplayRole || role == Qt::EditRole)
    {
      switch (ind.column())
      {
        case 0:
          return tr("Name");
        case 1:
          return tr("Description");
        case 2:
          return tr("Starting point");
        case 3:
          return tr("Lower bound");
        case 4:
          return tr("Upper bound");
        default:
          return QVariant();
      }
    }
    else if (role == Qt::CheckStateRole && ind.column() == 0)
    {
      const UnsignedInteger nbInputs = analysis_.getPhysicalModel().getInputNames().getSize();
      return analysis_.getVariableInputs().getSize() == nbInputs ? Qt::Checked : Qt::Unchecked;
    }
    else if (role == Qt::BackgroundRole)
    {
      return QBrush("#f2f1f0");
    }
    else if (role == Qt::TextAlignmentRole)
    {
      return Qt::AlignCenter;
    }
    return QVariant();
  }

  // not header

  if (role == Qt::DisplayRole || role == Qt::EditRole)
  {
    const int inputIndex = ind.row() - 1;

    switch (ind.column())
    {
      case 0:
        return QString::fromUtf8(analysis_.getPhysicalModel().getInputs()[inputIndex].getName().c_str());
      case 1:
        return QString::fromUtf8(analysis_.getPhysicalModel().getInputs()[inputIndex].getDescription().c_str());
      case 2:
        return QString::number(analysis_.getStartingPoint()[inputIndex], 'g', StudyTreeViewModel::DefaultSignificantDigits);
      case 3:
      {
        if (analysis_.getBounds().getFiniteLowerBound()[inputIndex] || role == Qt::EditRole)
          return QString::number(analysis_.getBounds().getLowerBound()[inputIndex], 'g', StudyTreeViewModel::DefaultSignificantDigits);
        else
          return "-∞";
      }
      case 4:
      {
        if (analysis_.getBounds().getFiniteUpperBound()[inputIndex] || role == Qt::EditRole)
          return QString::number(analysis_.getBounds().getUpperBound()[inputIndex], 'g', StudyTreeViewModel::DefaultSignificantDigits);
        else
          return "+∞";
      }
      default:
        return QVariant();
    }
  }
  else if (role == Qt::CheckStateRole)
  {
    const int inputIndex = ind.row() - 1;

    switch (ind.column())
    {
      case 0:
      {
        const String currentInputName = analysis_.getPhysicalModel().getInputNames()[inputIndex];
        return analysis_.getVariableInputs().contains(currentInputName) ? Qt::Checked : Qt::Unchecked;
      }
      case 3:
        return analysis_.getBounds().getFiniteLowerBound()[inputIndex] ? Qt::Checked : Qt::Unchecked;
      case 4:
        return analysis_.getBounds().getFiniteUpperBound()[inputIndex] ? Qt::Checked : Qt::Unchecked;
    }
  }
  else if ((role == Qt::ForegroundRole || role == Qt::ToolTipRole) && ind.column() == 0)
  {
    const int inputIndex = ind.row() - 1;
    const String currentInputName = analysis_.getPhysicalModel().getInputNames()[inputIndex];
    // check bounds
    if (analysis_.getVariableInputs().contains(currentInputName))
    {
      const Interval bounds(analysis_.getBounds().getMarginal(inputIndex));
      const bool startPtInBounds = bounds.contains(Point(1, analysis_.getStartingPoint()[inputIndex]));
      if (role == Qt::ForegroundRole && (bounds.isEmpty() || !startPtInBounds))
        return QColor(Qt::red);
      if (role == Qt::ToolTipRole && bounds.isEmpty())
        return tr("The lower bound must be less than the upper bound");
      else if (role == Qt::ToolTipRole && !startPtInBounds)
        return tr("The interval must contain the starting point");
    }
  }

  return QVariant();
}


bool OptimizationTableModel::setData(const QModelIndex & index, const QVariant & value, int role)
{
  emit errorMessageChanged("");

  // header
  if (index.row() == 0)
  {
    // first column : select variables
    if (role == Qt::CheckStateRole && index.column() == 0)
    {
      for (int i = 1; i < rowCount(); ++i)
        if (data(this->index(i, 0), role).toInt() != (value.toBool() ? Qt::Checked : Qt::Unchecked))
          setData(this->index(i, 0), value.toBool() ? Qt::Checked : Qt::Unchecked, role);

      return true;
    }
    return false;
  }

  // not header

  if (role == Qt::EditRole)
  {
    const int inputIndex = index.row() - 1;
    const QString currentInputName = analysis_.getPhysicalModel().getInputNames()[inputIndex].c_str();

    switch (index.column())
    {
      case 2: // starting point
      {
        Point values = analysis_.getStartingPoint();
        if (values[inputIndex] == value.toDouble())
          return false;

        values[inputIndex] = value.toDouble();
        analysis_.setStartingPoint(values);

        break;
      }
      case 3: // lower bounds
      {
        Point lowerBounds = analysis_.getBounds().getLowerBound();
        if (lowerBounds[inputIndex] == value.toDouble())
          return false;
        lowerBounds[inputIndex] = value.toDouble();
        Interval newInterval(analysis_.getBounds());
        newInterval.setLowerBound(lowerBounds);

        analysis_.setBounds(newInterval);

        break;
      }
      case 4: // upper bounds
      {
        Point upperBounds = analysis_.getBounds().getUpperBound();
        if (upperBounds[inputIndex] == value.toDouble())
          return false;
        upperBounds[inputIndex] = value.toDouble();
        Interval newInterval(analysis_.getBounds());
        newInterval.setUpperBound(upperBounds);

        analysis_.setBounds(newInterval);

        break;
      }
      default:
        return false;
    }
  }
  else if (role == Qt::CheckStateRole)
  {
    const int inputIndex = index.row() - 1;
    String currentInputName = analysis_.getPhysicalModel().getInputNames()[inputIndex];

    switch (index.column())
    {
      case 0: // input of interest
      {
        Description variableInputs = analysis_.getVariableInputs();
        if (value.toBool() && !variableInputs.contains(currentInputName))
          variableInputs.add(currentInputName);
        else if (!value.toBool() && variableInputs.contains(currentInputName))
          variableInputs.erase(std::remove(variableInputs.begin(), variableInputs.end(), currentInputName), variableInputs.end());
        else
          return false;

        analysis_.setVariableInputs(variableInputs);

        break;
      }
      case 3: // lower bounds
      {
        Interval::BoolCollection finiteLowerBounds = analysis_.getBounds().getFiniteLowerBound();
        if (finiteLowerBounds[inputIndex] == value.toBool())
          return false;
        finiteLowerBounds[inputIndex] = value.toBool();
        Interval newInterval(analysis_.getBounds());
        newInterval.setFiniteLowerBound(finiteLowerBounds);

        analysis_.setBounds(newInterval);

        break;
      }
      case 4: // upper bounds
      {
        Interval::BoolCollection finiteUpperBounds = analysis_.getBounds().getFiniteUpperBound();
        if (finiteUpperBounds[inputIndex] == value.toBool())
          return false;
        finiteUpperBounds[inputIndex] = value.toBool();
        Interval newInterval(analysis_.getBounds());
        newInterval.setFiniteUpperBound(finiteUpperBounds);

        analysis_.setBounds(newInterval);

        break;
      }
      default:
        return false;
    }
    emit dataChanged(this->index(index.row(), 0), this->index(index.row(), 4));
  }

  return true;
}


OptimizationAnalysis OptimizationTableModel::getAnalysis() const
{
  return analysis_;
}
}
