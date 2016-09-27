//                                               -*- C++ -*-
/**
 *  @brief QAbstractTableModel to list the distributions to test
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
#include <otgui/DistributionsTableModel.hxx>

#include <otgui/DistributionDictionary.hxx>

using namespace OT;

namespace OTGUI {

DistributionsTableModel::DistributionsTableModel(const QStringList& distributions)
  : QAbstractTableModel()
  , distributions_(distributions)
{
  distributions_.sort();
}


int DistributionsTableModel::rowCount(const QModelIndex& parent) const
{
  return distributions_.size();
}


int DistributionsTableModel::columnCount(const QModelIndex& parent) const
{
  return 1;
}


QVariant DistributionsTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
  if (orientation == Qt::Horizontal && role == Qt::DisplayRole && section == 0)
    return tr("Distribution");
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
    const Description listDistributions = DistributionDictionary::GetAvailableDistributions();
    if (listDistributions.getSize() == (UnsignedInteger)distributions_.size())
      return;
    for (UnsignedInteger i=0; i<listDistributions.getSize(); ++i)
    {
      const QString distName = tr(listDistributions[i].c_str());
      if (!distributions_.contains(distName))
      {
        ++nbRow;
        distributions_ << distName;
      }
    }
    --nbRow;
  }
  else
  {
    distributions_ << distributionName;
  }
  distributions_.sort();
  QModelIndex lastIndex = index(rowCount()-1, 0);
  beginInsertRows(lastIndex.parent(), lastIndex.row(), lastIndex.row()+nbRow);
  endInsertRows();
  emit distributionsListChanged(distributions_);
}
}