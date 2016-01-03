#include "otgui/InputTableProbabilisticModel.hxx"

#include "DistributionFactory.hxx"
#include "TruncatedDistribution.hxx"
#include "Normal.hxx"

using namespace OT;

namespace OTGUI {

InputTableProbabilisticModel::InputTableProbabilisticModel(const PhysicalModel & physicalModel, QObject * parent)
  : QAbstractTableModel(parent)
  , physicalModel_(physicalModel)
{
}


InputTableProbabilisticModel::~InputTableProbabilisticModel()
{
}


int InputTableProbabilisticModel::columnCount(const QModelIndex & parent) const
{
  return 2;
}


int InputTableProbabilisticModel::rowCount(const QModelIndex & parent) const
{
  return getPhysicalModel().getStochasticInputNames().getSize();
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
  if (role == Qt::DisplayRole && orientation == Qt::Horizontal)
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
  if (!index.isValid())
    return QVariant();

  if (role == Qt::DisplayRole || role == Qt::EditRole)
  {
    std::string inputName = getPhysicalModel().getStochasticInputNames()[index.row()];
    switch (index.column())
    {
      case 0:
        return inputName.c_str();
      case 1:
      {
        Input input = getPhysicalModel().getInputByName(inputName);
        std::string distributionName = input.getDistribution().getImplementation()->getClassName();
        if (distributionName == "TruncatedNormal")
          return tr("Normal");
        else if (distributionName == "TruncatedDistribution")
        {
          TruncatedDistribution * dist = dynamic_cast<TruncatedDistribution*>(&*input.getDistribution().getImplementation());
          return tr(dist->getDistribution().getImplementation()->getClassName().c_str());
        }
        else
          return tr(input.getDistribution().getImplementation()->getClassName().c_str());
      }
    }
  }
  return QVariant();
}


bool InputTableProbabilisticModel::setData(const QModelIndex & index, const QVariant & value, int role)
{
  if (!index.isValid())
    return false;

  if (role == Qt::EditRole && index.column() == 1)
  {
    std::string inputName = physicalModel_.getStochasticInputNames()[index.row()];
    Input input = physicalModel_.getInputByName(inputName);

    std::string distributionName = input.getDistribution().getImplementation()->getClassName();
    if (distributionName == "TruncatedNormal")
      distributionName = "Normal";
    else if (distributionName == "TruncatedDistribution")
    {
      TruncatedDistribution * dist = dynamic_cast<TruncatedDistribution*>(&*input.getDistribution().getImplementation());
      distributionName = dist->getDistribution().getImplementation()->getClassName().c_str();
    }

    if (value.toString().toStdString() != distributionName)
    {
      // search the distribution corresponding to 'value'
      DistributionFactory::DistributionFactoryCollection collection = DistributionFactory::GetContinuousUniVariateFactories();
      DistributionFactory factory;

      for (int i=0; i<collection.getSize(); ++i)
      {
        std::string nameFactory = collection[i].getImplementation()->getClassName();
        nameFactory.resize(nameFactory.find("Factory"));
        if (value.toString().toStdString() == nameFactory)
        {
          factory = collection[i];
          break;
        }
      }

      // update the input
      physicalModel_.blockNotification(true);
      physicalModel_.setInputDistribution(input.getName(), factory.build());
      physicalModel_.blockNotification(false);
      emit dataChanged(index, index);
      return true;
    }
  }
  return false;
}


PhysicalModel InputTableProbabilisticModel::getPhysicalModel() const
{
  return physicalModel_;
}
}
