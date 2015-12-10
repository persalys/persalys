#include "otgui/InputTableModel.hxx"

using namespace OT;

namespace OTGUI {

InputTableModel::InputTableModel(const PhysicalModel & physicalModel)
  : QAbstractTableModel()
  , physicalModel_(physicalModel)
{
}


InputTableModel::~InputTableModel()
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
  return QVariant();
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
    physicalModel_.blockNotification(true, "updateProbabilisticModelWindow");
    switch (index.column())
    {
      case 0:
        physicalModel_.removeInput(input.getName());
        input.setName(value.toString().toStdString());
        physicalModel_.addInput(input);
        break;
      case 1:
        physicalModel_.setInputDescription(input.getName(), value.toString().toStdString());
        break;
      case 2:
        physicalModel_.setInputValue(input.getName(), value.toDouble());
        break;
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
  physicalModel_.blockNotification(true, "updateProbabilisticModelWindow");
  // TODO set a default name for Input()
  physicalModel_.addInput(Input('X'+(OSS()<<physicalModel_.getInputs().getSize()).str()));
  physicalModel_.blockNotification(false);
  endInsertRows();
}


void InputTableModel::removeLine(const QModelIndex & index)
{
  beginRemoveRows(index.parent(), index.row(), index.row());
  removeRows(index.row(), 1, index.parent());
  physicalModel_.blockNotification(true, "updateProbabilisticModelWindow");
  physicalModel_.removeInput(physicalModel_.getInputs()[index.row()].getName());
  physicalModel_.blockNotification(false);
  endRemoveRows();
}

}
