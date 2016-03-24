//                                               -*- C++ -*-
/**
 *  @brief QAbstractTableModel to define Python script
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
#include "otgui/CodeModel.hxx"

#include "otgui/PythonPhysicalModel.hxx"

using namespace OT;

namespace OTGUI {

CodeModel::CodeModel(const PhysicalModel & physicalModel)
  : QAbstractTableModel()
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
    PythonPhysicalModel *pyModel =  dynamic_cast<PythonPhysicalModel*>(&*physicalModel_.getImplementation().get());
    return pyModel? QString::fromUtf8(pyModel->getCode().c_str()) : "";
  }
  return QVariant();
}


bool CodeModel::setData(const QModelIndex & index, const QVariant & value, int role)
{
  if (!index.isValid())
    return false;

  if (role == Qt::EditRole)
  {
    physicalModel_.blockNotification(true);
    dynamic_cast<PythonPhysicalModel*>(&*physicalModel_.getImplementation().get())->setCode(value.toString().toLocal8Bit().data());
    physicalModel_.blockNotification(false);
    emit dataChanged(index, index);
    return true;
  }
  return false;
}


Qt::ItemFlags CodeModel::flags(const QModelIndex & index) const
{
  return Qt::ItemIsEditable | QAbstractTableModel::flags(index);
}


}
