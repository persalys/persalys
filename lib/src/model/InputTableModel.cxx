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
#include "persalys/InputTableModel.hxx"

#include "persalys/StudyTreeViewModel.hxx"

using namespace OT;

namespace PERSALYS
{

InputTableModel::InputTableModel(const PhysicalModel & physicalModel, QObject * parent)
  : QAbstractTableModel(parent)
  , physicalModel_(physicalModel)
{
}


int InputTableModel::columnCount(const QModelIndex & /*parent*/) const
{
  return 3;
}


int InputTableModel::rowCount(const QModelIndex & /*parent*/) const
{
  return physicalModel_.getInputDimension();
}


QVariant InputTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
  if (role == Qt::DisplayRole && orientation == Qt::Horizontal)
  {
    switch (section)
    {
      case 0:
        return tr("Name");
      case 1:
        return tr("Description");
      case 2:
        return tr("Value");
    }
  }
  return QAbstractTableModel::headerData(section, orientation, role);
}


QVariant InputTableModel::data(const QModelIndex & index, int role) const
{
  if (!index.isValid())
    return QVariant();

  if (role == Qt::DisplayRole || role == Qt::EditRole)
  {
    switch (index.column())
    {
      case 0:
        return QString::fromUtf8(physicalModel_.getInputs()[index.row()].getName().c_str());
      case 1:
        return QString::fromUtf8(physicalModel_.getInputs()[index.row()].getDescription().c_str());
      case 2:
        return QString::number(physicalModel_.getInputs()[index.row()].getValue(), 'g', StudyTreeViewModel::DefaultSignificantDigits);
    }
  }
  return QVariant();
}


bool InputTableModel::setData(const QModelIndex & index, const QVariant & value, int role)
{
  if (!index.isValid())
    return false;

  if (role == Qt::EditRole)
  {
    const Input input(physicalModel_.getInputs()[index.row()]);

    switch (index.column())
    {
      case 0:
      {
        if (input.getName() == value.toString().toUtf8().constData())
          return true;
        if (value.toString().isEmpty())
          return false;
        physicalModel_.blockNotification("PhysicalModelDefinitionItem");
        emit errorMessageChanged("");
        try
        {
          physicalModel_.setInputName(input.getName(), value.toString().toUtf8().constData());
          emit inputNameChanged();
        }
        catch (std::exception & ex)
        {
          emit errorMessageChanged(ex.what());
        }
        break;
      }
      case 1:
      {
        if (input.getDescription() == value.toString().toUtf8().constData())
          return true;
        physicalModel_.blockNotification("PhysicalModelDefinitionItem");
        emit errorMessageChanged("");
        physicalModel_.setInputDescription(input.getName(), value.toString().toUtf8().constData());
        break;
      }
      case 2:
      {
        if (input.getValue() == value.toDouble())
          return true;
        emit errorMessageChanged("");
        physicalModel_.setInputValue(input.getName(), value.toDouble());
        break;
      }
    }
    physicalModel_.blockNotification();
    emit dataChanged(index, index);
    return true;
  }
  return false;
}


Qt::ItemFlags InputTableModel::flags(const QModelIndex & index) const
{
  if (index.column() == 0 && !QString(physicalModel_.getImplementation()->getClassName().c_str()).contains("Symbolic"))
    return QAbstractTableModel::flags(index);
  return Qt::ItemIsEditable | QAbstractTableModel::flags(index);
}


void InputTableModel::updateData()
{
  beginResetModel();
  endResetModel();
}


void InputTableModel::addLine()
{
  int i = 0;
  while (physicalModel_.hasInputNamed('X' + (OSS() << i).str()))
    ++i;
  physicalModel_.blockNotification("PhysicalModelDefinitionItem");
  physicalModel_.addInput(Input('X' + (OSS() << i).str()));
  emit inputNumberChanged();
  physicalModel_.blockNotification();
  QModelIndex lastIndex = index(rowCount() - 1, 0);
  beginInsertRows(lastIndex.parent(), lastIndex.row(), lastIndex.row());
  endInsertRows();
}


void InputTableModel::removeLine(const QModelIndex & index)
{
  beginRemoveRows(index.parent(), index.row(), index.row());
  removeRows(index.row(), 1, index.parent());
  physicalModel_.blockNotification("PhysicalModelDefinitionItem");
  physicalModel_.removeInput(physicalModel_.getInputs()[index.row()].getName());
  emit inputNumberChanged();
  physicalModel_.blockNotification();
  endRemoveRows();
}
}
