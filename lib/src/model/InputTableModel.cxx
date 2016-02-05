#include "otgui/InputTableModel.hxx"

using namespace OT;

namespace OTGUI {

InputTableModel::InputTableModel(const PhysicalModel & physicalModel)
  : QAbstractTableModel()
  , physicalModel_(physicalModel)
{
}


int InputTableModel::columnCount(const QModelIndex & parent) const
{
  return 3;
}


int InputTableModel::rowCount(const QModelIndex & parent) const
{
  return physicalModel_.getInputs().getSize();
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
        return physicalModel_.getInputs()[index.row()].getName().c_str();
      case 1:
        return physicalModel_.getInputs()[index.row()].getDescription().c_str();
      case 2:
        return physicalModel_.getInputs()[index.row()].getValue();
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
    Input input(physicalModel_.getInputs()[index.row()]);

    switch (index.column())
    {
      case 0:
      {
        if (input.getName() == value.toString().toStdString())
          return true;
        physicalModel_.blockNotification(true, "modelInputsChanged");
        physicalModel_.removeInput(input.getName());
        input.setName(value.toString().toStdString());
        physicalModel_.addInput(input);
        break;
      }
      case 1:
      {
        if (input.getDescription() == value.toString().toStdString())
          return true;
        physicalModel_.blockNotification(true);
        physicalModel_.setInputDescription(input.getName(), value.toString().toStdString());
        break;
      }
      case 2:
      {
        if (input.getValue() == value.toDouble())
          return true;
        physicalModel_.blockNotification(true, "modelInputsChanged");
        physicalModel_.setInputValue(input.getName(), value.toDouble());
        break;
      }
    }
    physicalModel_.blockNotification(false);
//  TODO   if (!updateInput) emit errorMessage
    emit dataChanged(index, index);
    return true;
  }
  return false;
}


Qt::ItemFlags InputTableModel::flags(const QModelIndex & index) const
{
  return Qt::ItemIsEditable | QAbstractTableModel::flags(index);
}


void InputTableModel::addLine()
{
  QModelIndex lastIndex = index(-1, 0);
  beginInsertRows(lastIndex.parent(), -1, -1);
  insertRow(lastIndex.row());
  int i = 0;
  while (physicalModel_.hasInputNamed('X' + (OSS()<<i).str()))
    ++i;
  physicalModel_.blockNotification(true, "modelInputsChanged");
  physicalModel_.addInput(Input('X'+(OSS()<<i).str()));
  physicalModel_.blockNotification(false);
  endInsertRows();
}


void InputTableModel::removeLine(const QModelIndex & index)
{
  beginRemoveRows(index.parent(), index.row(), index.row());
  removeRows(index.row(), 1, index.parent());
  physicalModel_.blockNotification(true, "modelInputsChanged");
  physicalModel_.removeInput(physicalModel_.getInputs()[index.row()].getName());
  physicalModel_.blockNotification(false);
  endRemoveRows();
}
}