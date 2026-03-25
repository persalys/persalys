//                                               -*- C++ -*-
/**
 *  @brief QAbstractTableModel to configure alpha parameter for HSIC filter/weight functions
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

#include "persalys/HSICAlphaTableModel.hxx"

#include "persalys/StudyTreeViewModel.hxx"

#include <QColor>

using namespace OT;

namespace PERSALYS
{

HSICAlphaTableModel::HSICAlphaTableModel(const Description & variableNames,
                                         QObject *parent)
  : QAbstractTableModel(parent)
  , variableNames_(variableNames)
{
  for (UnsignedInteger i = 0; i < variableNames_.getSize(); ++i)
    alphaValues_ << 1.0;
}

int HSICAlphaTableModel::rowCount(const QModelIndex & /*parent*/) const
{
  return variableNames_.getSize();
}

int HSICAlphaTableModel::columnCount(const QModelIndex & /*parent*/) const
{
  return 2; // variable name, alpha
}

QVariant HSICAlphaTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
  if (role == Qt::DisplayRole && orientation == Qt::Horizontal)
  {
    switch (section)
    {
    case 0:
      return tr("Variable");
    case 1:
      return QString::fromUtf8("\xce\xb1"); // α
    default:
      return QVariant();
    }
  }
  return QAbstractTableModel::headerData(section, orientation, role);
}

Qt::ItemFlags HSICAlphaTableModel::flags(const QModelIndex & index) const
{
  Qt::ItemFlags result = QAbstractTableModel::flags(index);

  if (index.column() == 1)
    return result | Qt::ItemIsEditable;

  return result;
}

QVariant HSICAlphaTableModel::data(const QModelIndex & index, int role) const
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
      if (role == Qt::DisplayRole)
        return QString::number(alphaValues_[row], 'g', StudyTreeViewModel::DefaultSignificantDigits);
      else
        return alphaValues_[row];
    default:
      return QVariant();
    }
  }

  if (role == Qt::ForegroundRole)
  {
    if (index.column() == 1 && errorRows_.contains(row))
      return QColor(Qt::red);
  }

  return QVariant();
}

bool HSICAlphaTableModel::setData(const QModelIndex & index, const QVariant & value, int role)
{
  if (!index.isValid() || role != Qt::EditRole)
    return false;

  if (index.column() == 1)
  {
    bool ok;
    const double alpha = value.toDouble(&ok);
    if (!ok)
      return false;

    alphaValues_[index.row()] = alpha;
    emit dataChanged(index, index);

    if (alpha <= 0.0)
    {
      errorRows_.insert(index.row());
      emit errorMessageChanged(tr("Alpha must be strictly positive"));
    }
    else
    {
      errorRows_.remove(index.row());
      if (errorRows_.isEmpty())
        emit errorMessageChanged("");
    }
    // Refresh foreground color
    emit dataChanged(index, index);
    return true;
  }

  return false;
}

void HSICAlphaTableModel::setAlphas(const Point & alphas)
{
  beginResetModel();
  for (int i = 0; i < alphaValues_.size() && i < static_cast<int>(alphas.getSize()); ++i)
    alphaValues_[i] = alphas[i];
  endResetModel();
}

Point HSICAlphaTableModel::getAlphas() const
{
  Point alphas(alphaValues_.size());
  for (int i = 0; i < alphaValues_.size(); ++i)
    alphas[i] = alphaValues_[i];
  return alphas;
}

bool HSICAlphaTableModel::hasErrors() const
{
  return !errorRows_.isEmpty();
}

} // namespace PERSALYS
