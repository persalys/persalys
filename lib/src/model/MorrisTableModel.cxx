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
#include "persalys/MorrisTableModel.hxx"

#include "persalys/StudyTreeViewModel.hxx"

using namespace OT;

namespace PERSALYS
{

MorrisTableModel::MorrisTableModel(const MorrisAnalysis & analysis, QObject * parent)
  : QAbstractTableModel(parent)
  , analysis_(analysis)
{
  analysis_.updateParameters();
}


int MorrisTableModel::columnCount(const QModelIndex & /*parent*/) const
{
  return 4;
}


int MorrisTableModel::rowCount(const QModelIndex & /*parent*/) const
{
  return analysis_.getPhysicalModel().getInputDimension();
}


Qt::ItemFlags MorrisTableModel::flags(const QModelIndex & index) const
{
  Qt::ItemFlags result = QAbstractTableModel::flags(index);

  if (index.column() > 1)
  {
    result |= Qt::ItemIsEditable;
  }

  return result;
}


QVariant MorrisTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
  if (orientation == Qt::Vertical && role == Qt::DisplayRole)
  {
    return section + 1;
  }
  else if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
  {
    switch (section)
    {
      case 0:
        return tr("Name");
      case 1:
        return tr("Description");
      case 2:
        return tr("Lower bound");
      case 3:
        return tr("Upper bound");
      default:
        return QVariant();
    }
  }
  return QAbstractItemModel::headerData(section, orientation, role);
}


QVariant MorrisTableModel::data(const QModelIndex & index, int role) const
{
  if (role == Qt::DisplayRole || role == Qt::EditRole)
  {
    const int inIndex = index.row();
    switch (index.column())
    {
      case 0:
        return QString::fromUtf8(analysis_.getPhysicalModel().getInputs()[inIndex].getName().c_str());
      case 1:
        return QString::fromUtf8(analysis_.getPhysicalModel().getInputs()[inIndex].getDescription().c_str());
      case 2:
        return QString::number(analysis_.getBounds().getLowerBound()[inIndex], 'g', StudyTreeViewModel::DefaultSignificantDigits);
      case 3:
        return QString::number(analysis_.getBounds().getUpperBound()[inIndex], 'g', StudyTreeViewModel::DefaultSignificantDigits);
      default:
        return QVariant();
    }
  }
  else if ((role == Qt::ForegroundRole || role == Qt::ToolTipRole) && index.column() == 0)
  {
    // check bounds
    const bool boundsIsEmpty = analysis_.getBounds().getMarginal(index.row()).isEmpty();
    if (role == Qt::ForegroundRole && boundsIsEmpty)
      return QColor(Qt::red);
    else if (role == Qt::ToolTipRole && boundsIsEmpty)
      return tr("The lower bound must be less than the upper bound");
  }
  return QVariant();
}


bool MorrisTableModel::setData(const QModelIndex & index, const QVariant & value, int role)
{
  emit errorMessageChanged("");

  // when user edit a value
  if (role == Qt::EditRole)
  {
    const int inIndex = index.row();
    switch (index.column())
    {
      case 0: // names
      case 1: // descriptions
        return false;
      case 2: // lower bounds
      {
        Point lowerBounds = analysis_.getBounds().getLowerBound();
        if (lowerBounds[inIndex] == value.toDouble())
          return false;
        lowerBounds[inIndex] = value.toDouble();

        Interval interval = analysis_.getBounds();
        interval.setLowerBound(lowerBounds);
        analysis_.setBounds(interval);

        break;
      }
      case 3: // upper bounds
      {
        Point upperBounds = analysis_.getBounds().getUpperBound();
        if (upperBounds[inIndex] == value.toDouble())
          return false;
        upperBounds[inIndex] = value.toDouble();

        Interval interval = analysis_.getBounds();
        interval.setUpperBound(upperBounds);
        analysis_.setBounds(interval);

        break;
      }
      default:
        return false;
    }
    emit dataChanged(this->index(index.row(), 0), this->index(index.row(), 3));
  }

  return true;
}


MorrisAnalysis MorrisTableModel::getAnalysis() const
{
  return analysis_;
}
}
