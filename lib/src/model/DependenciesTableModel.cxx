//                                               -*- C++ -*-
/**
 *  @brief QAbstractTableModel to list dependencies
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
#include "otgui/DependenciesTableModel.hxx"

#include "otgui/TranslationManager.hxx"
#include "otgui/DistributionDictionary.hxx"

#include <openturns/IndependentCopula.hxx>

#include <QColor>

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
  return copula_.getDimension() < 2 ? 0 : copula_.getCopulaCollection().getSize();
}


Qt::ItemFlags DependenciesTableModel::flags(const QModelIndex & index) const
{
  Qt::ItemFlags result = QAbstractTableModel::flags(index);
  if (index.column() == 0)
  {
    result &= ~Qt::ItemIsEditable;
  }
  else if (index.column() == 1)
    result &= ~Qt::ItemIsSelectable;

  return result;
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

  if (role == Qt::DisplayRole || role == Qt::EditRole)
  {
    switch (index.column())
    {
      case 0:
      {
        return copula_.getCopulaCollection()[index.row()].getDescription().__str__().c_str();
      }
      case 1:
      {
        String copulaName = copula_.getCopulaCollection()[index.row()].getImplementation()->getClassName();
        copulaName = copulaName.substr(0, copulaName.find("Copula"));
        return TranslationManager::GetTranslatedCopulaName(copulaName);
      }
      default:
        return QVariant();
    }
  }
  else if (role == Qt::UserRole + 1)
  {
    const UnsignedInteger groupSize = copula_.getCopulaCollection()[index.row()].getDescription().getSize();
    return (groupSize > 2 ? QStringList() << tr("Normal") : TranslationManager::GetAvailableCopulas()) << tr("Inference result");
  }
  else if (role == Qt::BackgroundRole)
  {
    return QColor(Qt::white);
  }

  return QVariant();
}


void DependenciesTableModel::updateData()
{
  beginResetModel();
  updateCopula();
  endResetModel();
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
    copula_.setDescription(Description(2, ""));
  }
}


bool DependenciesTableModel::setData(const QModelIndex & index, const QVariant & value, int role)
{
  if (!index.isValid() && index.column() != 1 && role != Qt::EditRole)
    return false;

  const QString newName = value.toString();

  if (newName == tr("Inference result"))
  {
    emit inferenceResultRequested(index);
    updateCopula();
    emit dataChanged(index, this->index(index.row(), 1));
    return true;
  }

  String copulaName = copula_.getCopulaCollection()[index.row()].getImplementation()->getClassName();
  copulaName = copulaName.substr(0, copulaName.find("Copula"));
  if (newName == TranslationManager::GetTranslatedCopulaName(copulaName))
    return false;

  // search the copula corresponding to 'value'
  const String newCopula = TranslationManager::GetCopulaName(newName);
  // the built copula has a dimension equal to 2
  // if dimension > 2 : do not enter here because there is for now only the Normal copula
  // improve this part if another copula is added in OpenTURNS
  Copula copula = DistributionDictionary::BuildCopulaFactory(newCopula).build();

  // update the copula
  physicalModel_.blockNotification("ProbabilisticModel");
  const Description vars(copula_.getCopulaCollection()[index.row()].getDescription());
  physicalModel_.setCopula(vars, copula);
  physicalModel_.blockNotification();
  updateCopula();
  copula.setDescription(vars);
  emit dataUpdated(index.row(), copula);

  return true;
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


Description DependenciesTableModel::getGroup(const QModelIndex & index) const
{
  return copula_.getCopulaCollection()[index.row()].getDescription();
}
}
