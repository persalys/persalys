//                                               -*- C++ -*-
/**
 *  @brief QAbstractTableModel to define correlations
 *
 *  Copyright 2015-2021 EDF-Phimeca
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
#include "persalys/CorrelationTableModel.hxx"

#include "persalys/StudyTreeViewModel.hxx"

#include <openturns/NormalCopula.hxx>

#include <QBrush>

using namespace OT;

namespace PERSALYS
{

CorrelationTableModel::CorrelationTableModel(const PhysicalModel &model, const OT::Distribution &copula, QObject *parent)
  : QAbstractTableModel(parent)
  , physicalModel_(model)
  , copula_(copula)
{
}


int CorrelationTableModel::columnCount(const QModelIndex & /*parent*/) const
{
  return copula_.getDimension();
}


int CorrelationTableModel::rowCount(const QModelIndex & /*parent*/) const
{
  return copula_.getDimension();
}


Qt::ItemFlags CorrelationTableModel::flags(const QModelIndex & index) const
{
  if (index.row() < index.column())
    return QAbstractTableModel::flags(index) | Qt::ItemIsEditable;
  else
    return QAbstractTableModel::flags(index);
}


QVariant CorrelationTableModel::headerData(int section, Qt::Orientation /*orientation*/, int role) const
{
  if (role == Qt::DisplayRole)
    return QString::fromUtf8(copula_.getDescription()[section].c_str());

  return QVariant();
}


QVariant CorrelationTableModel::data(const QModelIndex & index, int role) const
{
  if (!index.isValid())
    return QVariant();

  if (role == Qt::DisplayRole || role == Qt::EditRole)
    return QString::number(copula_.getSpearmanCorrelation()(index.row(), index.column()), 'g', StudyTreeViewModel::DefaultSignificantDigits);

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

    CorrelationMatrix correlation(copula_.getSpearmanCorrelation());
    if (value.toDouble() == correlation(index.row(), index.column()))
      return true;

    // update the physicalModel
    correlation(index.row(), index.column()) = value.toDouble();
    emit errorMessageChanged("");
    try
    {
      physicalModel_.blockNotification("ProbabilisticModelItem");
      const Description oldDescription = copula_.getDescription();
      copula_ = NormalCopula(NormalCopula::GetCorrelationFromSpearmanCorrelation(correlation));
      copula_.setDescription(oldDescription);
      physicalModel_.setCopula(oldDescription, copula_);
      emit dataChanged(index, index);
      emit dataUpdated(copula_);
      return true;
    }
    catch (std::exception & ex)
    {
      QString input1 = QString::fromUtf8(copula_.getDescription()[index.row()].c_str());
      QString input2 = QString::fromUtf8(copula_.getDescription()[index.column()].c_str());
      emit errorMessageChanged(tr("The correlation between %1 and %2 cannot be equal to '%3'.\n %4").arg(input1).arg(input2).arg(value.toString()).arg(ex.what()));
    }
    physicalModel_.blockNotification();
  }
  return false;
}


void CorrelationTableModel::updateData()
{
  beginResetModel();
  endResetModel();
}
}
