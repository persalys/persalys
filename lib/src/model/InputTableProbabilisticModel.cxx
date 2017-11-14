//                                               -*- C++ -*-
/**
 *  @brief QAbstractTableModel to list the inputs
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
#include "otgui/InputTableProbabilisticModel.hxx"

#include "otgui/DistributionDictionary.hxx"
#include "otgui/TranslationManager.hxx"

#include <openturns/TruncatedDistribution.hxx>
#include <openturns/Dirac.hxx>

using namespace OT;

namespace OTGUI
{

InputTableProbabilisticModel::InputTableProbabilisticModel(const PhysicalModel & physicalModel, QObject * parent)
  : QAbstractTableModel(parent)
  , physicalModel_(physicalModel)
{
}


int InputTableProbabilisticModel::columnCount(const QModelIndex & parent) const
{
  return 2;
}


int InputTableProbabilisticModel::rowCount(const QModelIndex & parent) const
{
  return getPhysicalModel().getInputNames().getSize();
}


Qt::ItemFlags InputTableProbabilisticModel::flags(const QModelIndex & index) const
{
  Qt::ItemFlags result = QAbstractTableModel::flags(index);
  if (index.column() == 0)
  {
    result &= ~Qt::ItemIsEditable;
    result |= Qt::ItemIsUserCheckable;
  }
  else if (index.column() == 1 && data(this->index(index.row(), 1), Qt::DisplayRole).toString() != "Dirac")
    result &= ~Qt::ItemIsSelectable;
  else if (index.column() == 1 && data(this->index(index.row(), 1), Qt::DisplayRole).toString() == "Dirac")
  {
    result &= ~Qt::ItemIsSelectable;
    result &= ~Qt::ItemIsEnabled;
    result &= ~Qt::ItemIsEditable;
  }
  return result;
}


QVariant InputTableProbabilisticModel::headerData(int section, Qt::Orientation orientation, int role) const
{
  if (role == Qt::DisplayRole && orientation == Qt::Horizontal)
    switch (section)
    {
      case 0:
        return tr("Name");
      case 1:
        return tr("Distribution");
      default:
        return QVariant();
    }
  return QAbstractTableModel::headerData(section, orientation, role);
}


bool InputTableProbabilisticModel::setHeaderData(int section, Qt::Orientation orientation, const QVariant& value, int role)
{
  if (role == Qt::CheckStateRole && orientation == Qt::Horizontal)
  {
    for (int i = 0; i < rowCount(); ++i)
      if (data(index(i, 0), role).toInt() != (value.toBool() ? Qt::Checked : Qt::Unchecked))
        setData(index(i, 0), value.toBool() ? Qt::Checked : Qt::Unchecked, role);
    emit distributionsChanged();
  }
  return QAbstractTableModel::setHeaderData(section, orientation, value, role);
}


QVariant InputTableProbabilisticModel::data(const QModelIndex & index, int role) const
{
  if (!index.isValid())
    return QVariant();

  if (role == Qt::CheckStateRole && index.column() == 0)
  {
    const Input input(physicalModel_.getInputs()[index.row()]);
    if (input.isStochastic())
      return Qt::Checked;
    return Qt::Unchecked;
  }
  else if (role == Qt::DisplayRole || role == Qt::EditRole)
  {
    const Input input(physicalModel_.getInputs()[index.row()]);
    switch (index.column())
    {
      case 0:
        return QString::fromUtf8(input.getName().c_str());
      case 1:
      {
        String distributionName = input.getDistribution().getImplementation()->getClassName();
        if (distributionName == "TruncatedNormal")
        {
          distributionName = "Normal";
        }
        else if (distributionName == "TruncatedDistribution")
        {
          TruncatedDistribution dist = *dynamic_cast<TruncatedDistribution*>(input.getDistribution().getImplementation().get());
          distributionName = dist.getDistribution().getImplementation()->getClassName();
        }
        return TranslationManager::GetTranslatedDistributionName(distributionName);
      }
      default:
        return QVariant();
    }
  }
  return QVariant();
}


bool InputTableProbabilisticModel::setData(const QModelIndex & index, const QVariant & value, int role)
{
  if (!index.isValid())
    return false;

  if (role == Qt::CheckStateRole && index.column() == 0)
  {
    const Input input(physicalModel_.getInputs()[index.row()]);
    Distribution distribution;

    if (value.toInt() == Qt::Checked)
      distribution = DistributionDictionary::BuildDistribution("Normal", input.getValue());
    else if (value.toInt() == Qt::Unchecked)
      distribution = Dirac(input.getValue());
    else
      return false;

    // update the input
    physicalModel_.blockNotification("ProbabilisticModel");
    physicalModel_.setDistribution(input.getName(), distribution);
    physicalModel_.setDistributionParametersType(input.getName(), 0);
    physicalModel_.blockNotification();
    emit dataChanged(index, this->index(index.row(), 1));
    emit correlationToChange();
    emit checked(physicalModel_.hasStochasticInputs() && (physicalModel_.getComposedDistribution().getDimension() == physicalModel_.getInputs().getSize()));

    return true;
  }
  else if (role == Qt::EditRole && index.column() == 1)
  {
    const QString newName = value.toString();
    if (newName != tr("Inference result"))
    {
      const Input input(physicalModel_.getInputs()[index.row()]);

      String distributionName = input.getDistribution().getImplementation()->getClassName();
      if (distributionName == "TruncatedNormal")
      {
        distributionName = "Normal";
      }
      else if (distributionName == "TruncatedDistribution")
      {
        TruncatedDistribution dist = *dynamic_cast<TruncatedDistribution*>(input.getDistribution().getImplementation().get());
        distributionName = dist.getDistribution().getImplementation()->getClassName();
      }

      if (newName != TranslationManager::GetTranslatedDistributionName(distributionName))
      {
        Distribution distribution;
        if (newName.isEmpty())
        {
          distribution = Dirac(input.getValue());
        }
        else
        {
          // search the distribution corresponding to 'value'
          const String newDist = TranslationManager::GetDistributionName(newName);
          distribution = DistributionDictionary::BuildDistribution(newDist, input.getValue());
        }
        // update the input
        physicalModel_.blockNotification("ProbabilisticModel");
        physicalModel_.setDistribution(input.getName(), distribution);
        physicalModel_.setDistributionParametersType(input.getName(), 0);
        physicalModel_.blockNotification();
        emit distributionChanged(index);
        return true;
      }
    }
    else
    {
      emit inferenceResultRequested(index);
      emit dataChanged(index, this->index(index.row(), 1));
    }
  }
  return false;
}


void InputTableProbabilisticModel::updateData()
{
  beginResetModel();
  endResetModel();
  emit checked(physicalModel_.hasStochasticInputs() && (physicalModel_.getComposedDistribution().getDimension() == physicalModel_.getInputs().getSize()));
}


PhysicalModel InputTableProbabilisticModel::getPhysicalModel() const
{
  return physicalModel_;
}
}
