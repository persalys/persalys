#include "otgui/InputTableProbabilisticModel.hxx"

#include "NormalFactory.hxx"
#include "DiracFactory.hxx"

using namespace OT;

namespace OTGUI {

InputTableProbabilisticModel::InputTableProbabilisticModel(const InputCollection & inputs, QObject * parent)
  : QAbstractTableModel(parent)
{
  for (int i=0; i<inputs.getSize(); ++i)
    addLine(inputs[i]);
}


InputTableProbabilisticModel::InputTableProbabilisticModel(const InputTableProbabilisticModel & other)
  : data_(other.data_)
{

}


InputTableProbabilisticModel::~InputTableProbabilisticModel()
{
  if (data_.getSize())
    data_.erase(data_.begin(), data_.begin()+data_.getSize());
}


std::map<QString, DistributionFactory> InputTableProbabilisticModel::GetDistributionsMap()
{
  std::map<QString, DistributionFactory> m;
  m[tr("Deterministic")] = DiracFactory();
  m[tr("Normal")] = NormalFactory();
  return m;
}


int InputTableProbabilisticModel::columnCount(const QModelIndex & parent) const
{
  return 2;
}


int InputTableProbabilisticModel::rowCount(const QModelIndex & parent) const
{
  return data_.getSize();
}


Qt::ItemFlags InputTableProbabilisticModel::flags(const QModelIndex & index) const
{
  if (index.column() == 1)
    return QAbstractTableModel::flags(index) | Qt::ItemIsEditable;
  else
    return QAbstractTableModel::flags(index);
}


QVariant InputTableProbabilisticModel::headerData(int section, Qt::Orientation orientation, int role) const
{
  if (role == Qt::DisplayRole)
    if (orientation == Qt::Horizontal)
      switch (section)
      {
        case 0:
          return tr("Name");
        case 1:
          return tr("Distribution");
      }

  return QVariant();
}


QVariant InputTableProbabilisticModel::data(const QModelIndex & index, int role) const
{
  if (!index.isValid()) return QVariant();

  if (role == Qt::DisplayRole || role == Qt::EditRole)
  {
    switch (index.column())
    {
      case 0:
        return data_[index.row()].getName().c_str();
      case 1:
      {
        std::string distributionName = data_[index.row()].getDistribution().getImplementation()->getClassName();
        if (distributionName == "Dirac")
          return tr("Deterministic");
        return tr(distributionName.c_str());
      }
    }
  }

  return QVariant();
}


bool InputTableProbabilisticModel::setData(const QModelIndex & index, const QVariant & value, int role)
{
  if (role == Qt::DisplayRole && index.column() == 1)
  {
    DistributionFactory factory = GetDistributionsMap()[value.toString()];
    Distribution distribution = factory.build();
    if (distribution.getImplementation()->getClassName() != data_[index.row()].getDistribution().getImplementation()->getClassName())
      data_[index.row()].setDistribution(distribution);
  }
  emit dataChanged(index, index);
  return true;
}


void InputTableProbabilisticModel::addLine(const Input & input)
{
  QModelIndex lastIndex = index(-1, 0);
  beginInsertRows(lastIndex.parent(), -1, -1);
  insertRow(lastIndex.row());
  data_.add(input);
  emit dataChanged(index(-1, 0), index(-1, 0));
  endInsertRows();
}


InputCollection InputTableProbabilisticModel::getData()
{
  return data_;
}


NumericalPointWithDescription InputTableProbabilisticModel::getParameters(int row) const
{
  if (data_[row].getDistribution().getImplementation()->getClassName() != "Dirac")
    return data_[row].getDistribution().getParametersCollection()[0];

  return NumericalPointWithDescription();
}


void InputTableProbabilisticModel::updateDistributionParameters(const QModelIndex & index, const NumericalPoint & parameters)
{
  Distribution distribution = data_[index.row()].getDistribution();
  try
  {
    distribution.setParametersCollection(parameters);
    data_[index.row()].setDistribution(distribution);
    emit dataChanged(index, index);
  }
  catch(Exception) {
    std::cerr << "InputTableProbabilisticModel::updateDistributionParameters invalid params:"<<parameters<<" for distribution:"<<distribution.getImplementation()->getName()<<std::endl;
  }
}
}
