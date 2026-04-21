//                                               -*- C++ -*-
/**
 *  @brief QAbstractTableModel to define the critical domain bounds for ROSA
 *
 *  Copyright 2015-2026 EDF-Phimeca
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

#include "persalys/CriticalDomainTableModel.hxx"

#include "persalys/StudyTreeViewModel.hxx"

#include <limits>

using namespace OT;

namespace PERSALYS
{

CriticalDomainTableModel::CriticalDomainTableModel(const Description & variableNames,
                                                   QObject *parent)
  : QAbstractTableModel(parent)
  , variableNames_(variableNames)
{
  for (UnsignedInteger i = 0; i < variableNames_.getSize(); ++i)
  {
    lowerBounds_ << 0.0;
    upperBounds_ << 0.0;
    finiteLowerBounds_ << false;
    finiteUpperBounds_ << false;
  }
}

int CriticalDomainTableModel::rowCount(const QModelIndex & /*parent*/) const
{
  return variableNames_.getSize();
}

int CriticalDomainTableModel::columnCount(const QModelIndex & /*parent*/) const
{
  return 3; // variable name, lower bound, upper bound
}

QVariant CriticalDomainTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
  if (role == Qt::DisplayRole && orientation == Qt::Horizontal)
  {
    switch (section)
    {
    case 0:
      return tr("Variable");
    case 1:
      return tr("Lower bound");
    case 2:
      return tr("Upper bound");
    default:
      return QVariant();
    }
  }
  return QAbstractTableModel::headerData(section, orientation, role);
}

Qt::ItemFlags CriticalDomainTableModel::flags(const QModelIndex & index) const
{
  Qt::ItemFlags result = QAbstractTableModel::flags(index);

  if (index.column() == 1 || index.column() == 2)
  {
    result |= Qt::ItemIsUserCheckable;
    if ((index.column() == 1 && finiteLowerBounds_[index.row()]) ||
        (index.column() == 2 && finiteUpperBounds_[index.row()]))
      result |= Qt::ItemIsEditable;
  }

  return result;
}

QVariant CriticalDomainTableModel::data(const QModelIndex & index, int role) const
{
  if (!index.isValid())
    return QVariant();

  const int row = index.row();

  if (role == Qt::DisplayRole || role == Qt::EditRole)
  {
    switch (index.column())
    {
    case 0:
      return QString::fromStdString(variableNames_[row]);
    case 1:
      if (finiteLowerBounds_[row] || role == Qt::EditRole)
        return QString::number(lowerBounds_[row], 'g', StudyTreeViewModel::DefaultSignificantDigits);
      else
        return QString("-\xe2\x88\x9e");
    case 2:
      if (finiteUpperBounds_[row] || role == Qt::EditRole)
        return QString::number(upperBounds_[row], 'g', StudyTreeViewModel::DefaultSignificantDigits);
      else
        return QString("+\xe2\x88\x9e");
    default:
      return QVariant();
    }
  }

  if (role == Qt::CheckStateRole)
  {
    switch (index.column())
    {
    case 1:
      return finiteLowerBounds_[row] ? Qt::Checked : Qt::Unchecked;
    case 2:
      return finiteUpperBounds_[row] ? Qt::Checked : Qt::Unchecked;
    default:
      return QVariant();
    }
  }

  return QVariant();
}

bool CriticalDomainTableModel::setData(const QModelIndex & index, const QVariant & value, int role)
{
  if (!index.isValid())
    return false;

  const int row = index.row();

  if (role == Qt::EditRole)
  {
    bool ok;
    const double val = value.toDouble(&ok);
    if (!ok)
      return false;

    if (index.column() == 1)
    {
      lowerBounds_[row] = val;
      emit dataChanged(index, index);
      return true;
    }
    else if (index.column() == 2)
    {
      upperBounds_[row] = val;
      emit dataChanged(index, index);
      return true;
    }
  }
  else if (role == Qt::CheckStateRole)
  {
    const bool finite = value.toBool();
    if (index.column() == 1)
    {
      if (finiteLowerBounds_[row] == finite)
        return false;
      finiteLowerBounds_[row] = finite;
      emit dataChanged(index, index);
      return true;
    }
    else if (index.column() == 2)
    {
      if (finiteUpperBounds_[row] == finite)
        return false;
      finiteUpperBounds_[row] = finite;
      emit dataChanged(index, index);
      return true;
    }
  }

  return false;
}

void CriticalDomainTableModel::setDefaultBounds(const Point & lower, const Point & upper)
{
  if (lower.getSize() != variableNames_.getSize() || upper.getSize() != variableNames_.getSize())
    return;
  for (int i = 0; i < static_cast<int>(lower.getSize()); ++i)
  {
    lowerBounds_[i] = lower[i];
    upperBounds_[i] = upper[i];
    // finiteLowerBounds_ and finiteUpperBounds_ remain false
  }
}

void CriticalDomainTableModel::setInterval(const Interval & interval)
{
  if (interval.getDimension() != variableNames_.getSize())
    return;

  beginResetModel();
  for (int i = 0; i < static_cast<int>(interval.getDimension()); ++i)
  {
    finiteLowerBounds_[i] = interval.getFiniteLowerBound()[i];
    finiteUpperBounds_[i] = interval.getFiniteUpperBound()[i];
    if (interval.getFiniteLowerBound()[i])
      lowerBounds_[i] = interval.getLowerBound()[i];
    // else: keep existing lowerBounds_[i] (quantile default or last user value)
    if (interval.getFiniteUpperBound()[i])
      upperBounds_[i] = interval.getUpperBound()[i];
    // else: keep existing upperBounds_[i]
  }
  endResetModel();
}

Interval CriticalDomainTableModel::getInterval() const
{
  const UnsignedInteger dim = variableNames_.getSize();
  Point lower(dim);
  Point upper(dim);
  Interval::BoolCollection finiteLower(dim);
  Interval::BoolCollection finiteUpper(dim);
  for (UnsignedInteger i = 0; i < dim; ++i)
  {
    finiteLower[i] = finiteLowerBounds_[i];
    finiteUpper[i] = finiteUpperBounds_[i];
    lower[i] = finiteLowerBounds_[i] ? lowerBounds_[i] : -std::numeric_limits<double>::infinity();
    upper[i] = finiteUpperBounds_[i] ? upperBounds_[i] :  std::numeric_limits<double>::infinity();
  }
  Interval result(lower, upper);
  result.setFiniteLowerBound(finiteLower);
  result.setFiniteUpperBound(finiteUpper);
  return result;
}

} // namespace PERSALYS
