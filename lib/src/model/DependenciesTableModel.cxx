//                                               -*- C++ -*-
/**
 *  @brief QAbstractTableModel to list dependencies
 *
 *  Copyright 2015-2017 EDF-Phimeca
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
#include "otgui/DependenciesTableModel.hxx"

#include "otgui/TranslationManager.hxx"

#include <openturns/IndependentCopula.hxx>

using namespace OT;

namespace OTGUI
{

DependenciesTableModel::DependenciesTableModel(const PhysicalModel &physicalModel, QObject *parent)
  : QAbstractTableModel(parent)
  , physicalModel_(physicalModel)
  , copula_(physicalModel.getCopulaCollection())
{
}


int DependenciesTableModel::columnCount(const QModelIndex &parent) const
{
  return 2;
}


int DependenciesTableModel::rowCount(const QModelIndex &parent) const
{
  return copula_.getDimension() < 2 ? 0 : physicalModel_.getCopulaCollection().getSize();
}


QVariant DependenciesTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
  if (role == Qt::DisplayRole && orientation == Qt::Horizontal)
  {
    switch (section)
    {
      case 0:
        return tr("Variables");
      case 1:
        return tr("Copula");
      default:
        return QVariant();
    }
  }

  return QAbstractItemModel::headerData(section, orientation, role);
}


QVariant DependenciesTableModel::data(const QModelIndex &index, int role) const
{
  if (!index.isValid())
    return QVariant();

  if (role == Qt::DisplayRole)
  {
    switch (index.column())
    {
      case 0:
      {
        return physicalModel_.getCopulaCollection()[index.row()].getDescription().__str__().c_str();
      }
      case 1:
      {
        String copulaName = physicalModel_.getCopulaCollection()[index.row()].getImplementation()->getClassName();
        copulaName = copulaName.substr(0, copulaName.find("Copula"));
        return TranslationManager::GetTranslatedCopulaName(copulaName);
      }
      default:
        return QVariant();
    }
  }

  return QVariant();
}


void DependenciesTableModel::updateData()
{
  beginResetModel();
  updateCopula();
  endResetModel();
  emit dataUpdated(copula_);
}


void DependenciesTableModel::updateCopula()
{
  Collection<Copula> coll(physicalModel_.getCopulaCollection());
  Collection<Copula>::iterator iter = coll.begin();
  while (iter != coll.end())
  {
    if ((*iter).getImplementation()->getClassName() == "IndependentCopula")
    {
      iter = coll.erase(iter);
    }
    else
    {
      ++iter;
    }
  }
  if (coll.getSize())
  {
    copula_.setCopulaCollection(coll);
  }
  else
  {
    copula_ = ComposedCopula();
    copula_.setDescription(Description(1, ""));
  }
}


void DependenciesTableModel::removeLine(const QModelIndex &index)
{
  beginRemoveRows(index.parent(), index.row(), index.row());
  removeRows(index.row(), 1, index.parent());
  physicalModel_.blockNotification("ProbabilisticModel");
  const Description copulaVar(copula_.getCopulaCollection()[index.row()].getDescription());
  physicalModel_.setCopula(copulaVar, IndependentCopula(copulaVar.getSize()));
  physicalModel_.blockNotification();
  updateCopula();
  endRemoveRows();
}


void DependenciesTableModel::addLine(const Copula& copula)
{
  QModelIndex lastIndex = index(rowCount() - 1, 0);
  physicalModel_.blockNotification("ProbabilisticModel");
  physicalModel_.setCopula(copula.getDescription(), copula);
  physicalModel_.blockNotification();
  updateCopula();
  beginInsertRows(lastIndex.parent(), lastIndex.row(), lastIndex.row());
  endInsertRows();
}
}
