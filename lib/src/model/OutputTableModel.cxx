//                                               -*- C++ -*-
/**
 *  @brief QAbstractTableModel for the list of the outputs
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
#include "otgui/OutputTableModel.hxx"

#include "otgui/SymbolicPhysicalModel.hxx"

#include <QDebug>

using namespace OT;

namespace OTGUI {

OutputTableModel::OutputTableModel(const PhysicalModel & physicalModel, QObject * parent)
  : QAbstractTableModel(parent)
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


bool OutputTableModel::setHeaderData(int section, Qt::Orientation orientation, const QVariant& value, int role)
{
  if (role == Qt::CheckStateRole && orientation == Qt::Horizontal)
  {
    for (int i=0; i<rowCount(); ++i)
      if (data(index(i, 0), role).toInt() != (value.toBool()? Qt::Checked:Qt::Unchecked))
        setData(index(i, 0), value.toBool()? Qt::Checked:Qt::Unchecked, role);
    emit selectedOutputsChanged();
  }

  return QAbstractTableModel::setHeaderData(section, orientation, value, role);
}


QVariant OutputTableModel::data(const QModelIndex & index, int role) const
{
  if (!index.isValid())
    return QVariant();

  if (role == Qt::CheckStateRole && index.column() == 0)
  {
    if (physicalModel_.getOutputs()[index.row()].isSelected())
      return Qt::Checked;
    return Qt::Unchecked;
  }
  else if (role == Qt::DisplayRole || role == Qt::EditRole)
  {
    switch (index.column())
    {
      case 0:
        return QString::fromUtf8(physicalModel_.getOutputs()[index.row()].getName().c_str());
      case 1:
        return QString::fromUtf8(physicalModel_.getOutputs()[index.row()].getDescription().c_str());
      case 2:
      {
        const SymbolicPhysicalModel * model = dynamic_cast<const SymbolicPhysicalModel*>(&*physicalModel_.getImplementation());
        if (model)
          return QString::fromUtf8(model->getFormula(physicalModel_.getOutputs()[index.row()].getName()).c_str());
        break;
      }
      case 3:
      {
        if (!physicalModel_.getOutputs()[index.row()].hasBeenComputed())
          return QString("?");
        return QString::number(physicalModel_.getOutputs()[index.row()].getValue(), 'g', 15);
      }
    }
  }
  return QVariant();
}


bool OutputTableModel::setData(const QModelIndex & index, const QVariant & value, int role)
{
  if (!index.isValid())
    return false;

  if (role == Qt::CheckStateRole && index.column() == 0)
  {
    Output output(physicalModel_.getOutputs()[index.row()]);

    // update the output
    physicalModel_.blockNotification("PhysicalModelDefinition");
    physicalModel_.selectOutput(output.getName(), value.toBool());
    physicalModel_.blockNotification();
    emit dataChanged(index, this->index(index.row(), 1));
    emit selectedOutputsChanged();
    bool allChecked = physicalModel_.getOutputNames().getSize() == physicalModel_.getSelectedOutputsNames().getSize();
    emit checked(allChecked);

    return true;
  }
  else if (role == Qt::EditRole)
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
        physicalModel_.blockNotification("PhysicalModelDefinition");
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
        physicalModel_.blockNotification("PhysicalModelDefinition");
        emit errorMessageChanged("");
        physicalModel_.setOutputDescription(output.getName(), value.toString().toUtf8().constData());
        break;
      }
      case 2:
      {
        SymbolicPhysicalModel * model = dynamic_cast<SymbolicPhysicalModel*>(physicalModel_.getImplementation().get());
        if (model)
        {
          if (model->getFormula(output.getName()) == value.toString().toUtf8().constData())
            return true;
        }
        else
          return false;
        // TODO test if value.toString() ok
        physicalModel_.blockNotification("PhysicalModelDefinition");
        emit errorMessageChanged("");
        model->setFormula(output.getName(), value.toString().toUtf8().constData());
        break;
      }
    }
    physicalModel_.blockNotification();
    emit dataChanged(index, index);
    return true;
  }
  return false;
}


Qt::ItemFlags OutputTableModel::flags(const QModelIndex & index) const
{
  if (index.column() == 0 && physicalModel_.getImplementation()->getClassName() == "SymbolicPhysicalModel")
    return QAbstractTableModel::flags(index) | Qt::ItemIsEditable | Qt::ItemIsUserCheckable;
  else if (index.column() == 0 && physicalModel_.getImplementation()->getClassName() != "SymbolicPhysicalModel")
    return QAbstractTableModel::flags(index) | Qt::ItemIsUserCheckable;
  else if (index.column() == 1
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
  int i = 0;
  while (physicalModel_.hasOutputNamed('Y' + (OSS()<<i).str()))
    ++i;
  physicalModel_.blockNotification("PhysicalModelDefinition");
  physicalModel_.addOutput(Output('Y'+(OSS()<<i).str()));
  physicalModel_.blockNotification();
  //
  QModelIndex lastIndex = index(rowCount()-1, 0);
  beginInsertRows(lastIndex.parent(), lastIndex.row(), lastIndex.row());
  insertRow(lastIndex.row());
  
  endInsertRows();
  bool allChecked = physicalModel_.getOutputNames().getSize() == physicalModel_.getSelectedOutputsNames().getSize();
  emit checked(allChecked);
}


void OutputTableModel::removeLine(const QModelIndex & index)
{
  beginRemoveRows(index.parent(), index.row(), index.row());
  removeRows(index.row(), 1, index.parent());
  physicalModel_.blockNotification("PhysicalModelDefinition");
  physicalModel_.removeOutput(physicalModel_.getOutputs()[index.row()].getName());
  physicalModel_.blockNotification();
  endRemoveRows();
  bool allChecked = physicalModel_.getOutputNames().getSize() == physicalModel_.getSelectedOutputsNames().getSize();
  emit checked(allChecked && physicalModel_.getOutputNames().getSize());
}
}
