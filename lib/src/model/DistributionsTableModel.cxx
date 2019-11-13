//                                               -*- C++ -*-
/**
 *  @brief QAbstractTableModel to list the distributions to test
 *
 *  Copyright 2015-2019 EDF-Phimeca
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
#include "persalys/DistributionsTableModel.hxx"

#include "persalys/TranslationManager.hxx"

namespace PERSALYS
{

DistributionsTableModel::DistributionsTableModel(const QStringList &distributions, const QStringList &availableDistributions, QWidget *parent)
  : QAbstractTableModel(parent)
  , distributions_(distributions)
  , availableDistributions_(availableDistributions)
{
  distributions_.sort();
}


int DistributionsTableModel::rowCount(const QModelIndex& /*parent*/) const
{
  return distributions_.size();
}


int DistributionsTableModel::columnCount(const QModelIndex& /*parent*/) const
{
  return 1;
}


QVariant DistributionsTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
  if (orientation == Qt::Horizontal && role == Qt::DisplayRole && section == 0)
  {
    if (availableDistributions_.size() == TranslationManager::GetAvailableDistributions().size())
      return tr("Distributions");
    else
      return tr("Copulas");
  }
  return QAbstractItemModel::headerData(section, orientation, role);
}


QVariant DistributionsTableModel::data(const QModelIndex& index, int role) const
{
  if (!index.isValid())
    return QVariant();

  if (role == Qt::DisplayRole && index.column() == 0)
    return distributions_[index.row()];

  return QVariant();
}


void DistributionsTableModel::appendDistribution(const QString& distributionName)
{
  int nbRow = 0;
  if (distributionName == tr("All"))
  {
    if (availableDistributions_.size() == distributions_.size())
      return;
    for (int i = 0; i < availableDistributions_.size(); ++i)
    {
      if (!distributions_.contains(availableDistributions_[i]))
      {
        ++nbRow;
        distributions_ << availableDistributions_[i];
      }
    }
    --nbRow;
  }
  else
  {
    distributions_ << distributionName;
  }
  distributions_.sort();
  QModelIndex lastIndex = index(rowCount() - 1, 0);
  beginInsertRows(lastIndex.parent(), lastIndex.row(), lastIndex.row() + nbRow);
  endInsertRows();
  emit distributionsListChanged(distributions_);
}


void DistributionsTableModel::updateData(const QStringList & distributions)
{
  beginResetModel();
  distributions_ = distributions;
  endResetModel();
}


QStringList DistributionsTableModel::getDistributions() const
{
  return distributions_;
}
}
