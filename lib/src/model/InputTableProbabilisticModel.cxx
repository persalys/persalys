//                                               -*- C++ -*-
/**
 *  @brief QAbstractTableModel to list the inputs
 *
 *  Copyright 2015-2022 EDF-Phimeca
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
#include "persalys/InputTableProbabilisticModel.hxx"

#include "persalys/DistributionDictionary.hxx"
#include "persalys/TranslationManager.hxx"

#include <openturns/TruncatedDistribution.hxx>
#include <openturns/Dirac.hxx>

#include <QColor>

using namespace OT;

namespace PERSALYS
{

InputTableProbabilisticModel::InputTableProbabilisticModel(const PhysicalModel & physicalModel, const bool failSoftMode, QObject * parent)
  : QAbstractTableModel(parent)
  , failSoftMode_(failSoftMode)
  , physicalModel_(physicalModel)
{
  allDistributions_ = TranslationManager::GetTranslatedContinuousDistributions() <<
                      TranslationManager::GetTranslatedDiscreteDistributions() <<
                      tr("Inference result");
}


int InputTableProbabilisticModel::columnCount(const QModelIndex & /*parent*/) const
{
  return 2;
}


int InputTableProbabilisticModel::rowCount(const QModelIndex & /*parent*/) const
{
  return physicalModel_.getInputDimension();
}


Qt::ItemFlags InputTableProbabilisticModel::flags(const QModelIndex & index) const
{
  Qt::ItemFlags result = QAbstractTableModel::flags(index);
  if (failSoftMode_)
  {
    result &= ~Qt::ItemIsEditable;
  }
  else
  {
    if (index.column() == 0)
    {
      result &= ~Qt::ItemIsEditable;
      result |= Qt::ItemIsUserCheckable;
    }
    else if (index.column() == 1 && data(this->index(index.row(), 1), Qt::DisplayRole).toString() == "Dirac")
    {
      result &= ~Qt::ItemIsEnabled;
      result &= ~Qt::ItemIsEditable;
    }
  }
  return result;
}


QVariant InputTableProbabilisticModel::headerData(int section, Qt::Orientation orientation, int role) const
{
  if (role == Qt::DisplayRole && orientation == Qt::Horizontal)
  {
    switch (section)
    {
      case 0:
        return tr("Variable");
      case 1:
        return tr("Distribution");
      default:
        return QVariant();
    }
  }
  else if (role == Qt::ToolTipRole && section == 0 && rowCount())
  {
    const bool allChecked = physicalModel_.getStochasticInputNames().getSize() == physicalModel_.getInputDimension();
    return allChecked ? tr("Deselect all") : tr("Select all");
  }
  return QAbstractTableModel::headerData(section, orientation, role);
}


QVariant InputTableProbabilisticModel::data(const QModelIndex & index, int role) const
{
  if (!index.isValid())
    return QVariant();

  if (!failSoftMode_ && role == Qt::CheckStateRole && index.column() == 0)
  {
    const Input input(physicalModel_.getInputs()[index.row()]);
    return input.isStochastic() ? Qt::Checked : Qt::Unchecked;
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
        if (!input.isStochastic())
          return QVariant();

        String distributionName = input.getDistribution().getImplementation()->getClassName();
        if (distributionName == "TruncatedNormal")
        {
          distributionName = "Normal";
        }
        else if (distributionName == "TruncatedDistribution")
        {
          const TruncatedDistribution dist(*dynamic_cast<TruncatedDistribution*>(input.getDistribution().getImplementation().get()));
          distributionName = dist.getDistribution().getImplementation()->getClassName();
        }
        return TranslationManager::GetTranslatedDistributionName(distributionName);
      }
      default:
        return QVariant();
    }
  }
  else if (role == Qt::UserRole + 1)
  {
    const Input input(physicalModel_.getInputs()[index.row()]);
    return input.isStochastic() ? allDistributions_ : QStringList();
  }
  else if (role == Qt::BackgroundRole)
  {
    return QColor(Qt::white);
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

    // update the input
    physicalModel_.blockNotification("ProbabilisticModelItem");
    physicalModel_.setInputStochastic(input.getName(), value.toInt() == Qt::Checked);
    physicalModel_.setDistributionParametersType(input.getName(), 0);
    physicalModel_.blockNotification();
    emit dataChanged(index, this->index(index.row(), 1));
    emit headerDataChanged(Qt::Horizontal, 0, 0);
    emit distributionsChanged();

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
        const TruncatedDistribution dist(*dynamic_cast<TruncatedDistribution*>(input.getDistribution().getImplementation().get()));
        distributionName = dist.getDistribution().getImplementation()->getClassName();
      }

      if (newName != TranslationManager::GetTranslatedDistributionName(distributionName))
      {
        // search the distribution corresponding to 'value'
        const String newDist = TranslationManager::GetDistributionName(newName);
        Distribution distribution(DistributionDictionary::BuildDistribution(newDist, input.getValue()));
        // update the input
        physicalModel_.blockNotification("ProbabilisticModelItem");
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
  emit headerDataChanged(Qt::Horizontal, 0, 0);
}
}
