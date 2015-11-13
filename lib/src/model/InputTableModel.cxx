#include "otgui/InputTableModel.hxx"

using namespace OT;

namespace OTGUI {

InputTableModel::InputTableModel(QObject * parent)
  : QAbstractTableModel(parent)
  , data_(0)
  , validity_(false)
{
}


InputTableModel::InputTableModel(const InputCollection & inputs)
  : validity_(false)
{
  for (int i=0; i<inputs.getSize(); ++i)
    addLine(inputs[i]);
}


InputTableModel::InputTableModel(const InputTableModel & other)
  : data_( other.data_ )
  , validity_(other.validity_)
{

}


InputTableModel::~InputTableModel()
{
  if (data_.getSize())
    data_.erase(data_.begin(), data_.begin()+data_.getSize());
}


int InputTableModel::columnCount(const QModelIndex & parent) const
{
  return 3;
}


int InputTableModel::rowCount(const QModelIndex & parent) const
{
  return data_.getSize();
}


QVariant InputTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
  if (role == Qt::DisplayRole)
  {
    if (orientation == Qt::Horizontal)
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
  }
  return QVariant();
}


QVariant InputTableModel::data(const QModelIndex & index, int role) const
{
  if (!index.isValid()) return QVariant();

  if (role == Qt::DisplayRole || role == Qt::EditRole)
  {
    switch (index.column())
    {
      case 0:
        return data_[index.row()].getName().c_str();
      case 1:
        return data_[index.row()].getDescription().c_str();
      case 2:
        return data_[index.row()].getValue();
    }
  }

  return QVariant();
}


bool InputTableModel::setData(const QModelIndex & index, const QVariant & value, int role)
{
  if (role == Qt::EditRole)
  {
    switch (index.column())
    {
      case 0:
        data_[index.row()].setName(value.toString().toStdString());
        break;
      case 1:
        data_[index.row()].setDescription(value.toString().toStdString());
        break;
      case 2:
      {
        data_[index.row()].setValue(value.toDouble());
        std::string distributionName = data_[index.row()].getDistribution().getImplementation()->getClassName();
        if (distributionName == "Dirac")
        {
          Dirac distributionDirac(NumericalPoint(1, value.toDouble()));
          data_[index.row()].setDistribution(distributionDirac);
        }
        break;
      }
    }
  }
  emit dataChanged(index, index);
  return true;
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
  Input input = Input();
  data_.add(input);

  endInsertRows();
}


void InputTableModel::addLine(const Input & input)
{
  QModelIndex lastIndex = index(-1, 0);
  beginInsertRows(lastIndex.parent(), -1, -1);
  insertRow(lastIndex.row());
  data_.add(input);
  emit dataChanged(index(-1, 0), index(-1, 0));

  endInsertRows();
}


void InputTableModel::removeLine(const QModelIndex & index)
{
  beginRemoveRows(index.parent(), index.row(), index.row());
  removeRows(index.row(), 1, index.parent());

  data_.erase(data_.begin() + index.row());
  emit dataChanged(QModelIndex(), QModelIndex());

  endRemoveRows();
}


InputCollection InputTableModel::getData()
{
  return data_;
}


bool InputTableModel::isValid()
{
  return validity_;
}
}
