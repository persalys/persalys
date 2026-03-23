//                                               -*- C++ -*-
/**
 *  @brief QAbstractTableModel to define the critical domain bounds for ROSA
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

#include "persalys/CriticalDomainTableModel.hxx"

#include <limits>
#include <QColor>

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
    lowerBounds_ << -std::numeric_limits<double>::infinity();
    upperBounds_ << std::numeric_limits<double>::infinity();
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
    return result | Qt::ItemIsEditable;

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
      return lowerBounds_[row];
    case 2:
      return upperBounds_[row];
    default:
      return QVariant();
    }
  }

  if (role == Qt::ForegroundRole)
  {
    if ((index.column() == 1 || index.column() == 2) && errorRows_.contains(row))
      return QColor(Qt::red);
  }

  return QVariant();
}

bool CriticalDomainTableModel::setData(const QModelIndex & index, const QVariant & value, int role)
{
  if (!index.isValid() || role != Qt::EditRole)
    return false;

  bool ok;
  const double val = value.toDouble(&ok);
  if (!ok)
    return false;

  const int row = index.row();

  if (index.column() == 1)
  {
    lowerBounds_[row] = val;
    emit dataChanged(index, index);
    updateErrors();
    return true;
  }
  else if (index.column() == 2)
  {
    upperBounds_[row] = val;
    emit dataChanged(index, index);
    updateErrors();
    return true;
  }

  return false;
}

void CriticalDomainTableModel::setInterval(const Interval & interval)
{
  if (interval.getDimension() != variableNames_.getSize())
    return;

  beginResetModel();
  for (int i = 0; i < static_cast<int>(interval.getDimension()); ++i)
  {
    lowerBounds_[i] = interval.getLowerBound()[i];
    upperBounds_[i] = interval.getUpperBound()[i];
  }
  endResetModel();
}

Interval CriticalDomainTableModel::getInterval() const
{
  const UnsignedInteger dim = variableNames_.getSize();
  Point lower(dim);
  Point upper(dim);
  for (UnsignedInteger i = 0; i < dim; ++i)
  {
    lower[i] = lowerBounds_[i];
    upper[i] = upperBounds_[i];
  }
  return Interval(lower, upper);
}

void CriticalDomainTableModel::updateErrors()
{
  errorRows_.clear();
  for (int i = 0; i < lowerBounds_.size(); ++i)
  {
    if (lowerBounds_[i] > upperBounds_[i])
      errorRows_.insert(i);
  }

  if (!errorRows_.isEmpty())
    emit errorMessageChanged(tr("Lower bound must be less than or equal to upper bound"));
  else
    emit errorMessageChanged("");

  // Refresh all bound cells to update foreground color
  if (rowCount() > 0)
    emit dataChanged(index(0, 1), index(rowCount() - 1, 2));
}

bool CriticalDomainTableModel::hasErrors() const
{
  return !errorRows_.isEmpty();
}

} // namespace PERSALYS
