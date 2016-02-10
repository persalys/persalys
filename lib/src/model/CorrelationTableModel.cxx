//                                               -*- C++ -*-
/**
 *  @brief QAbstractTableModel to define correlations
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
#include "otgui/CorrelationTableModel.hxx"

#include "DistributionFactory.hxx"
#include "TruncatedDistribution.hxx"
#include "Normal.hxx"
#include "NormalCopula.hxx"

#include <QBrush>

using namespace OT;

namespace OTGUI {

CorrelationTableModel::CorrelationTableModel(const PhysicalModel & physicalModel, QObject * parent)
  : QAbstractTableModel(parent)
  , physicalModel_(physicalModel)
{
}


CorrelationTableModel::~CorrelationTableModel()
{
}


int CorrelationTableModel::columnCount(const QModelIndex & parent) const
{
  return getPhysicalModel().getStochasticInputNames().getSize();
}


int CorrelationTableModel::rowCount(const QModelIndex & parent) const
{
  return getPhysicalModel().getStochasticInputNames().getSize();
}


Qt::ItemFlags CorrelationTableModel::flags(const QModelIndex & index) const
{
  if (index.row() < index.column())
    return QAbstractTableModel::flags(index) | Qt::ItemIsEditable;
  else
    return QAbstractTableModel::flags(index);
}


QVariant CorrelationTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
  if (role == Qt::DisplayRole)
    return getPhysicalModel().getStochasticInputNames()[section].c_str();

  return QVariant();
}


QVariant CorrelationTableModel::data(const QModelIndex & index, int role) const
{
  if (!index.isValid())
    return QVariant();

  if (role == Qt::DisplayRole || role == Qt::EditRole)
    return physicalModel_.getCopula().getSpearmanCorrelation()(index.row(), index.column());

  else if (role == Qt::BackgroundRole && index.row() >= index.column())
    return QBrush(Qt::lightGray);

  return QVariant();
}


bool CorrelationTableModel::setData(const QModelIndex & index, const QVariant & value, int role)
{
  if (!index.isValid())
    return false;

  if (role == Qt::EditRole)
  {
    if (value.toDouble() < -1. || value.toDouble() > 1.)
      return false;

    CorrelationMatrix correlation(physicalModel_.getCopula().getSpearmanCorrelation());
    correlation(index.row(), index.column()) = value.toDouble();
    // update the physicalModel
    physicalModel_.blockNotification(true);
    physicalModel_.setCopula(NormalCopula(NormalCopula::GetCorrelationFromSpearmanCorrelation(correlation)));
    physicalModel_.blockNotification(false);
    emit dataChanged(index, index);
    return true;
  }
  return false;
}


PhysicalModel CorrelationTableModel::getPhysicalModel() const
{
  return physicalModel_;
}
}