//                                               -*- C++ -*-
/**
 *  @brief QAbstractTableModel for the list of the outputs
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
#include "otgui/OutputTableModel.hxx"

using namespace OT;

namespace OTGUI {

OutputTableModel::OutputTableModel(const PhysicalModel & physicalModel)
  : QAbstractTableModel()
  , physicalModel_(physicalModel)
{
}


int OutputTableModel::columnCount(const QModelIndex & parent) const
{
  return 4;
}


int OutputTableModel::rowCount(const QModelIndex & parent) const
{
  return physicalModel_.getOutputs().getSize();
}


QVariant OutputTableModel::headerData(int section, Qt::Orientation orientation, int role) const
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
        return tr("Formula");
      case 3:
        return tr("Value");
    }
  }
  return QAbstractTableModel::headerData(section, orientation, role);
}


QVariant OutputTableModel::data(const QModelIndex & index, int role) const
{
  if (!index.isValid())
    return QVariant();

  if (role == Qt::DisplayRole || role == Qt::EditRole)
  {
    switch (index.column())
    {
      case 0:
        return QString::fromUtf8(physicalModel_.getOutputs()[index.row()].getName().c_str());
      case 1:
        return QString::fromUtf8(physicalModel_.getOutputs()[index.row()].getDescription().c_str());
      case 2:
        return QString::fromUtf8(physicalModel_.getOutputs()[index.row()].getFormula().c_str());
      case 3:
      {
        if (!physicalModel_.getOutputs()[index.row()].hasBeenComputed())
          return QString("?");
        return QString::number(physicalModel_.getOutputs()[index.row()].getValue());
      }
    }
  }
  return QVariant();
}


bool OutputTableModel::setData(const QModelIndex & index, const QVariant & value, int role)
{
  if (!index.isValid())
    return false;

  if (role == Qt::EditRole)
  {
    Output output(physicalModel_.getOutputs()[index.row()]);

    switch (index.column())
    {
      case 0:
      {
        if (output.getName() == value.toString().toUtf8().constData())
          return true;
        if (value.toString().isEmpty())
          return false;
        physicalModel_.blockNotification(true, "modelOutputsChanged");
        emit errorMessageChanged("");
        try
        {
          physicalModel_.setOutputName(output.getName(), value.toString().toUtf8().constData());
        }
        catch (std::exception & ex)
        {
          emit errorMessageChanged(ex.what());
        }
        break;
      }
      case 1:
      {
        if (output.getDescription() == value.toString().toUtf8().constData())
          return true;
        physicalModel_.blockNotification(true);
        emit errorMessageChanged("");
        physicalModel_.setOutputDescription(output.getName(), value.toString().toUtf8().constData());
        break;
      }
      case 2:
      {
        if (output.getFormula() == value.toString().toUtf8().constData())
          return true;
        // TODO test if value.toString() ok
        physicalModel_.blockNotification(true);
        emit errorMessageChanged("");
        physicalModel_.setOutputFormula(output.getName(), value.toString().toUtf8().constData());
        break;
      }
    }
    physicalModel_.blockNotification(false);
    emit dataChanged(index, index);
    return true;
  }
  return false;
}


Qt::ItemFlags OutputTableModel::flags(const QModelIndex & index) const
{
  if (index.column() == 0
      && (physicalModel_.getImplementation()->getClassName() == "YACSPhysicalModel" ||
          physicalModel_.getImplementation()->getClassName() == "MetaModel"))
    return QAbstractTableModel::flags(index);
  else if (index.column() == 3)
    return QAbstractTableModel::flags(index);
  else
    return QAbstractTableModel::flags(index) | Qt::ItemIsEditable;
}


void OutputTableModel::addLine()
{
  QModelIndex lastIndex = index(-1, 0);
  beginInsertRows(lastIndex.parent(), -1, -1);
  insertRow(lastIndex.row());
  int i = 0;
  while (physicalModel_.hasOutputNamed('Y' + (OSS()<<i).str()))
    ++i;
  physicalModel_.blockNotification(true, "modelOutputsChanged");
  physicalModel_.addOutput(Output('Y'+(OSS()<<i).str()));
  physicalModel_.blockNotification(false);
  endInsertRows();
}


void OutputTableModel::removeLine(const QModelIndex & index)
{
  beginRemoveRows(index.parent(), index.row(), index.row());
  removeRows(index.row(), 1, index.parent());
  physicalModel_.blockNotification(true, "modelOutputsChanged");
  physicalModel_.removeOutput(physicalModel_.getOutputs()[index.row()].getName());
  physicalModel_.blockNotification(false);
  endRemoveRows();
}
}