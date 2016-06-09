//                                               -*- C++ -*-
/**
 *  @brief QAbstractTableModel to list the inputs
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
#include "otgui/InputTableProbabilisticModel.hxx"

#include "otgui/DistributionDictionary.hxx"

#include "TruncatedDistribution.hxx"
#include "Normal.hxx"

using namespace OT;

namespace OTGUI {

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
    for (int i=0; i<rowCount(); ++i)
      if (data(index(i, 0), role).toInt() != (value.toBool()? Qt::Checked:Qt::Unchecked))
        setData(index(i, 0), value.toBool()? Qt::Checked:Qt::Unchecked, role);
    emit distributionsChanged();
  }

  return QAbstractTableModel::setHeaderData(section, orientation, value, role);
}


QVariant InputTableProbabilisticModel::data(const QModelIndex & index, int role) const
{
  if (!index.isValid())
    return QVariant();

  String inputName = getPhysicalModel().getInputNames()[index.row()];
  if (role == Qt::CheckStateRole && index.column() == 0)
  {
    Input input = getPhysicalModel().getInputByName(inputName);
    if (input.isStochastic())
      return Qt::Checked;
    return Qt::Unchecked;
  }
  else if (role == Qt::DisplayRole || role == Qt::EditRole)
  {
    switch (index.column())
    {
      case 0:
        return QString::fromUtf8(inputName.c_str());
      case 1:
      {
        Input input = getPhysicalModel().getInputByName(inputName);
        String distributionName = input.getDistribution().getImplementation()->getClassName();
        if (distributionName == "TruncatedNormal")
          return tr("Normal");
        else if (distributionName == "TruncatedDistribution")
        {
          TruncatedDistribution * dist = dynamic_cast<TruncatedDistribution*>(&*input.getDistribution().getImplementation());
          return tr(dist->getDistribution().getImplementation()->getClassName().c_str());
        }
        else
          return tr(input.getDistribution().getImplementation()->getClassName().c_str());
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
    String inputName = physicalModel_.getInputNames()[index.row()];
    Input input = physicalModel_.getInputByName(inputName);
    Distribution distribution;

    if (value.toInt() == Qt::Checked)
      distribution = DistributionDictionary::BuildDistribution("Normal", input.getValue());
    else if (value.toInt() == Qt::Unchecked)
      distribution = Dirac(input.getValue());
    else
      return false;

    // update the input
    physicalModel_.blockNotification(true);
    physicalModel_.setInputDistribution(input.getName(), distribution);
    physicalModel_.setInputDistributionParametersType(input.getName(), 0);
    physicalModel_.blockNotification(false);
    emit dataChanged(index, this->index(index.row(), 1));
    emit correlationToChange();
    if (physicalModel_.getComposedDistribution().getDimension() == physicalModel_.getInputs().getSize())
      emit checked(true);
    else
      emit checked(false);
    return true;
  }
  else if (role == Qt::EditRole && index.column() == 1)
  {
    String inputName = physicalModel_.getInputNames()[index.row()];
    Input input = physicalModel_.getInputByName(inputName);

    String distributionName = input.getDistribution().getImplementation()->getClassName();
    if (distributionName == "TruncatedNormal")
      distributionName = "Normal";
    else if (distributionName == "TruncatedDistribution")
    {
      TruncatedDistribution * dist = dynamic_cast<TruncatedDistribution*>(&*input.getDistribution().getImplementation());
      distributionName = dist->getDistribution().getImplementation()->getClassName();
    }

    if (value.toString().toStdString() != distributionName)
    {
      Distribution distribution;
      if (value.toString().isEmpty())
        distribution = Dirac(input.getValue());
      else
      {
        // search the distribution corresponding to 'value'
        distribution = DistributionDictionary::BuildDistribution(value.toString().toStdString(), input.getValue());
      }
      // update the input
      physicalModel_.blockNotification(true);
      physicalModel_.setInputDistribution(input.getName(), distribution);
      physicalModel_.setInputDistributionParametersType(input.getName(), 0);
      physicalModel_.blockNotification(false);
      emit distributionChanged(index);
      return true;
    }
  }
  return false;
}


PhysicalModel InputTableProbabilisticModel::getPhysicalModel() const
{
  return physicalModel_;
}
}
