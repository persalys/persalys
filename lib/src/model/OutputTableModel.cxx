#include "OutputTableModel.hxx"

#include <iostream>

namespace OTGUI {

OutputTableModel::OutputTableModel()
  : QAbstractTableModel()
  , data_(0)
  , validity_(false)
{
}


OutputTableModel::OutputTableModel(const OutputCollection & outputs)
  : QAbstractTableModel()
  , validity_(false)
{
  for (int i=0; i<outputs.getSize(); ++i)
    addLine(outputs[i]);
}


OutputTableModel::OutputTableModel(const OutputTableModel & other)
  : data_( other.data_ )
  , validity_(other.validity_)
{

}


OutputTableModel::~OutputTableModel()
{
  if (data_.getSize())
    data_.erase(data_.begin(), data_.begin()+data_.getSize());
}


int OutputTableModel::columnCount(const QModelIndex & parent) const
{
  return 4;
}


int OutputTableModel::rowCount(const QModelIndex & parent) const
{
  return data_.getSize();
}


QVariant OutputTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
  if (role == Qt::DisplayRole)
  {
    if (orientation == Qt::Horizontal)
    {
      switch (section)
      {
        case 0:
            return QString(tr("Name"));
        case 1:
            return QString(tr("Description"));
        case 2:
            return QString(tr("Value"));
        case 3:
            return QString(tr("Formula"));
      }
    }
  }
  return QVariant();
}


QVariant OutputTableModel::data(const QModelIndex & index, int role) const
{
  if (!index.isValid()) return QVariant();

  if (role == Qt::DisplayRole || role == Qt::EditRole)
  {
    int column = index.column();
    switch (column)
    {
      case 0:
        return data_[index.row()].getName().c_str();
      case 1:
        return data_[index.row()].getDescription().c_str();
      case 2:
        return data_[index.row()].getValue();
      case 3:
        return data_[index.row()].getFormula().c_str();
    }
  }

  return QVariant();
}


// fill the table if signal from action


bool OutputTableModel::setData(const QModelIndex & index, const QVariant & value, int role)
{
  if (role == Qt::EditRole)
  {
    int column = index.column();
    switch (column)
    {
      case 0:
        data_[index.row()].setName(value.toString().toStdString());
        break;
      case 1:
        data_[index.row()].setDescription(value.toString().toStdString());
        break;
      case 2:
        data_[index.row()].setValue(value.toDouble());
        break;
      case 3:
        // if value.toString() ok => validity_=true;
        data_[index.row()].setFormula(value.toString().toStdString());
        break;
    }
  }

  emit dataChanged(index, index);

  return true;
}


Qt::ItemFlags OutputTableModel::flags(const QModelIndex & index) const
{
  return Qt::ItemIsEditable | QAbstractTableModel::flags(index);
}


void OutputTableModel::addLine()
{
  QModelIndex lastIndex = index(-1, 0);
  beginInsertRows(lastIndex.parent(), -1, -1);
  insertRow(lastIndex.row());
  Output output = Output();
  data_.add(output);

  endInsertRows();
}


void OutputTableModel::addLine(Output output)
{
  QModelIndex lastIndex = index(-1, 0);
  beginInsertRows(lastIndex.parent(), -1, -1);
  insertRow(lastIndex.row());
  data_.add(output);
  emit dataChanged(index(-1, 0), index(-1, 0));
  // parameters of the first one or the last one
  // emit distributionChanged(parametersLabels, parametersValues);
  endInsertRows();
}


void OutputTableModel::removeLine(const QModelIndex & index)
{
  beginRemoveRows(index.parent(), index.row(), index.row());
  removeRows(index.row(), 1, index.parent());

//   std::vector<Output*>::iterator it = data_.begin();
//   std::advance(it, index.row());
  data_.erase(data_.begin() + index.row());

  endRemoveRows();
}


OutputCollection OutputTableModel::getData()
{
  return data_;
}


bool OutputTableModel::isValid()
{
  return validity_;
}

}
