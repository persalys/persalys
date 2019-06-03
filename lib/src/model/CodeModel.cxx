//                                               -*- C++ -*-
/**
 *  @brief QAbstractTableModel to define Python script
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
#include "persalys/CodeModel.hxx"

#include "persalys/PythonPhysicalModel.hxx"

using namespace OT;

namespace PERSALYS
{

CodeModel::CodeModel(const PhysicalModel & physicalModel, QObject* parent)
  : QAbstractTableModel(parent)
  , physicalModel_(physicalModel)
{
}


int CodeModel::columnCount(const QModelIndex & parent) const
{
  return 1;
}


int CodeModel::rowCount(const QModelIndex & parent) const
{
  return 1;
}


QVariant CodeModel::data(const QModelIndex & index, int role) const
{
  if (!index.isValid())
    return QVariant();

  if (role == Qt::DisplayRole || role == Qt::EditRole)
  {
    PythonPhysicalModel *pyModel = dynamic_cast<PythonPhysicalModel*>(physicalModel_.getImplementation().get());
    return pyModel ? QString::fromUtf8(pyModel->getCode().c_str()) : "";
  }
  return QVariant();
}


bool CodeModel::setData(const QModelIndex & index, const QVariant & value, int role)
{
  if (!index.isValid())
    return false;

  if (role == Qt::EditRole)
  {
    physicalModel_.blockNotification("PhysicalModelDefinition");
    try
    {
      dynamic_cast<PythonPhysicalModel*>(physicalModel_.getImplementation().get())->setCode(value.toString().toUtf8().data());
    }
    catch (std::exception& ex)
    {
      physicalModel_.blockNotification();
      emit errorMessageChanged(ex.what());
      return false;
    }
    physicalModel_.blockNotification();
    emit dataChanged(index, index);
    emit variablesChanged();
    return true;
  }
  return false;
}


Qt::ItemFlags CodeModel::flags(const QModelIndex & index) const
{
  return Qt::ItemIsEditable | QAbstractTableModel::flags(index);
}


void CodeModel::updateData()
{
  beginResetModel();
  endResetModel();
}
}
