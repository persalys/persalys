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
        return physicalModel_.getOutputs()[index.row()].getName().c_str();
      case 1:
        return physicalModel_.getOutputs()[index.row()].getDescription().c_str();
      case 2:
        return physicalModel_.getOutputs()[index.row()].getFormula().c_str();
      case 3:
      {
        if (!physicalModel_.getOutputs()[index.row()].hasBeenComputed())
          return QString("?");
        return physicalModel_.getOutputs()[index.row()].getValue();
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
        if (output.getName() == value.toString().toStdString())
          break;
        physicalModel_.blockNotification(true, "modelOutputsChanged");
        physicalModel_.removeOutput(output.getName());
        output.setName(value.toString().toStdString());
        physicalModel_.addOutput(output);
        break;
      }
      case 1:
      {
        if (output.getDescription() == value.toString().toStdString())
          break;
        physicalModel_.blockNotification(true);
        physicalModel_.setOutputDescription(output.getName(), value.toString().toStdString());
        break;
      }
      case 2:
      {
        if (output.getFormula() == value.toString().toStdString())
          break;
        // TODO test if value.toString() ok
        physicalModel_.blockNotification(true);
        physicalModel_.setOutputFormula(output.getName(), value.toString().toStdString());
        break;
      }
    }
    physicalModel_.blockNotification(false);
//  TODO   if (!updateOutput) emit errorMessage
    emit dataChanged(index, index);
    return true;
  }
  return false;
}


Qt::ItemFlags OutputTableModel::flags(const QModelIndex & index) const
{
  if (index.column() == 3)
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