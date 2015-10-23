#include "InputTableModel.hxx"

#include "NormalFactory.hxx"
#include "DiracFactory.hxx"

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


std::map<QString, DistributionFactory> InputTableModel::GetDistributionsMap()
{
  std::map<QString, DistributionFactory> m;
  m[tr("Deterministic")] = DiracFactory();
  m[tr("Normal")] = NormalFactory();
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
          Dirac distributionDirac(NumericalPoint(1, value.toDouble()));
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
    DistributionFactory factory = GetDistributionsMap()[value.toString()];

    Distribution distribution = factory.build();
    data_[index.row()].setDistribution(distribution);
    if (distribution.getImplementation()->getClassName() != "Dirac")
      emit distributionChanged(distribution.getParametersCollection()[0]);
    else
      emit distributionChanged(NumericalPointWithDescription());
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

  emit distributionChanged(NumericalPointWithDescription());

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


NumericalPointWithDescription InputTableModel::getParameters(int row) const
{
  if (data_[row].getDistribution().getImplementation()->getClassName() != "Dirac")
  {
    return data_[row].getDistribution().getParametersCollection()[0];
  }
  return NumericalPointWithDescription();
}


void InputTableModel::updateDistributionParameters(const QModelIndex & index, const NumericalPoint & parameters)
{
  Distribution distribution = data_[index.row()].getDistribution();
  try {
    distribution.setParametersCollection(parameters);
    data_[index.row()].setDistribution(distribution);
    emit dataChanged(index, index);
  }
  catch(Exception) {
    std::cerr << "InputTableModel::updateDistributionParameters invalid params:"<<parameters<<" for distribution:"<<distribution.getImplementation()->getName()<<std::endl;
  }

}


bool InputTableModel::isValid()
{
  return validity_;
}


}
