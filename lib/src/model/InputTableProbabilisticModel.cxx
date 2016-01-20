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
  return 3;
}


int InputTableProbabilisticModel::rowCount(const QModelIndex & parent) const
{
  return getPhysicalModel().getInputNames().getSize();
}


Qt::ItemFlags InputTableProbabilisticModel::flags(const QModelIndex & index) const
{
  if (index.column() == 0)
    return QAbstractTableModel::flags(index) & ~Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsUserCheckable;
  else if (index.column() == 1)
    return QAbstractTableModel::flags(index);
  else if (index.column() == 2 && data(this->index(index.row(), 2), Qt::DisplayRole).toString() != "Dirac")
    return QAbstractTableModel::flags(index) & ~Qt::ItemIsSelectable | Qt::ItemIsEditable;
  else if (index.column() == 2 && data(this->index(index.row(), 2), Qt::DisplayRole).toString() == "Dirac")
    return QAbstractTableModel::flags(index) & ~Qt::ItemIsSelectable & ~Qt::ItemIsEnabled & ~Qt::ItemIsEditable;
}


QVariant InputTableProbabilisticModel::headerData(int section, Qt::Orientation orientation, int role) const
{
  if (role == Qt::DisplayRole && orientation == Qt::Horizontal)
    switch (section)
    {
      case 0:
        return tr("");
      case 1:
        return tr("Name");
      case 2:
        return tr("Distribution");
    }

  return QVariant();
}


QVariant InputTableProbabilisticModel::data(const QModelIndex & index, int role) const
{
  if (!index.isValid())
    return QVariant();

  std::string inputName = getPhysicalModel().getInputNames()[index.row()];
  if (role == Qt::CheckStateRole && index.column() == 0)
  {
    Input input = getPhysicalModel().getInputByName(inputName);
    if (input.isStochastic())
      return Qt::Checked;
    return Qt::Unchecked;
  }
  else if (role == Qt::DisplayRole || role == Qt::EditRole)
  {
    switch (index.column())
    {
      case 1:
        return inputName.c_str();
      case 2:
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

  if (role == Qt::CheckStateRole && index.column() == 0)
  {
    std::string inputName = physicalModel_.getInputNames()[index.row()];
    Input input = physicalModel_.getInputByName(inputName);
    Distribution distribution;

    if (value.toInt() == Qt::Checked)
      distribution = Normal(0, 1);
    else if (value.toInt() == Qt::Unchecked)
      distribution = Dirac(input.getValue());

    // update the input
    physicalModel_.blockNotification(true);
    physicalModel_.setInputDistribution(input.getName(), distribution);
    physicalModel_.blockNotification(false);
    emit dataChanged(index, this->index(index.row(), 2));
    emit correlationToChange();
    return true;
  }
  else if (role == Qt::EditRole && index.column() == 2)
  {
    std::string inputName = physicalModel_.getInputNames()[index.row()];
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
      Distribution distribution;
      if (value.toString().isEmpty())
        distribution = Dirac(input.getValue());
      else
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
        distribution = factory.build();
      }
      // update the input
      physicalModel_.blockNotification(true);
      physicalModel_.setInputDistribution(input.getName(), distribution);
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
