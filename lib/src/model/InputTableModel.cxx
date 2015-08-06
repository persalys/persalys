#include "InputTableModel.hxx"

#include "Distribution.hxx"
#include "DistributionFactory.hxx"
#include "NormalFactory.hxx"
#include "DiracFactory.hxx"
#include <iostream>
#include "DistributionImplementation.hxx"

namespace OTGUI {

InputTableModel::InputTableModel(QObject * parent)
  : QAbstractTableModel(parent)
  , data_(0)
  , validity_(false)
{
}


InputTableModel::~InputTableModel()
{
  if (data_.getSize())
    data_.erase(data_.begin(), data_.begin()+data_.getSize());
}


InputTableModel::InputTableModel(const InputTableModel & other)
  : data_( other.data_ )
  , validity_(other.validity_)
{

}


InputTableModel::InputTableModel(InputCollection inputs)
  : validity_(false)
{
  for (int i=0; i<inputs.getSize(); ++i)
    addLine(inputs[i]);
}


std::map<QString, OT::DistributionFactory> InputTableModel::GetDistributionsMap()
{
  std::map<QString, OT::DistributionFactory> m;
  m[tr("Deterministic")] = OT::DiracFactory();
  m[tr("Normal")] = OT::NormalFactory();
  return m;
}


int InputTableModel::columnCount(const QModelIndex & parent) const
{
  return 4;
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
            return QString(tr("Name"));
        case 1:
            return QString(tr("Description"));
        case 2:
            return QString(tr("Value"));
        case 3:
            return QString(tr("Distribution"));
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
      case 3:
        std::string distributionName = data_[index.row()].getDistribution().getImplementation()->getClassName();
        if (distributionName == "Dirac")
          return QString(tr("Deterministic"));
        return QString(tr(distributionName.c_str()));
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
          OT::Dirac distributionDirac(OT::NumericalPoint(1, value.toDouble()));
          data_[index.row()].setDistribution(distributionDirac);
        }
        break;
      }
      case 3:
        break;
    }
  }
  if (role == Qt::DisplayRole && index.column() == 3)
  {
    OT::DistributionFactory factory = GetDistributionsMap()[value.toString()];

    OT::Distribution distribution = factory.build();
    data_[index.row()].setDistribution(distribution);
    if (distribution.getImplementation()->getClassName() != "Dirac")
      emit distributionChanged(distribution.getParametersCollection()[0]);
    else
      emit distributionChanged(OT::NumericalPointWithDescription());
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

  emit distributionChanged(OT::NumericalPointWithDescription());

  endInsertRows();
}


void InputTableModel::addLine(Input input)
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

//   std::vector<Input*>::iterator it = data_.begin();
//   std::advance(it, index.row());
  data_.erase(data_.begin() + index.row());

  endRemoveRows();
}


InputCollection InputTableModel::getData()
{
  return data_;
}


OT::NumericalPointWithDescription InputTableModel::getParameters(int row) const
{
  if (data_[row].getDistribution().getImplementation()->getClassName() != "Dirac")
  {
    return data_[row].getDistribution().getParametersCollection()[0];
  }
  return OT::NumericalPointWithDescription();
}


void InputTableModel::updateDistributionParameters(const QModelIndex & index, OT::NumericalPoint parameters)
{
  OT::Distribution distribution = data_[index.row()].getDistribution(); 
  distribution.setParametersCollection(parameters);
  data_[index.row()].setDistribution(distribution);

  emit dataChanged(index, index);
}


bool InputTableModel::isValid()
{
  return validity_;
}


}
