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
  for (int i=0; i<physicalModel_.getInputs().getSize(); ++i)
    if (physicalModel_.getInputs()[i].getDistribution().getImplementation()->getClassName() != "Dirac")
      userRoleList_ << i;
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
  return userRoleList_.count();
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

  if (role == Qt::UserRole)
    return userRoleList_[index.row()];

  else if (role == Qt::DisplayRole || role == Qt::EditRole)
  {
    Input input_i = physicalModel_.getInputs()[data(index, Qt::UserRole).toInt()];
    switch (index.column())
    {
      case 0:
        return input_i.getName().c_str();
      case 1:
        std::string distributionName = input_i.getDistribution().getImplementation()->getClassName();
        if (distributionName == "TruncatedNormal")
          return tr("Normal");
        else if (distributionName == "TruncatedDistribution")
        {
          TruncatedDistribution * dist = dynamic_cast<TruncatedDistribution*>(&*input_i.getDistribution().getImplementation());
          return tr(dist->getDistribution().getImplementation()->getClassName().c_str());
        }
        else
          return tr(input_i.getDistribution().getImplementation()->getClassName().c_str());
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
    int inputIndex = data(index, Qt::UserRole).toInt();
    Input input = Input(physicalModel_.getInputs()[inputIndex]);

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


void InputTableProbabilisticModel::addLine(int i)
{
  Input input = Input(physicalModel_.getInputs()[i]);

  physicalModel_.blockNotification(true);
  physicalModel_.setInputDistribution(input.getName(), Normal(0, 1));
  physicalModel_.blockNotification(false);

  QModelIndex lastIndex = index(-1, 0);
  beginInsertRows(lastIndex.parent(), -1, -1);
  insertRow(lastIndex.row());
  userRoleList_ << i;
  emit dataChanged(index(rowCount()-1, 0), index(rowCount()-1, 0));
  endInsertRows();
}


void InputTableProbabilisticModel::removeLine(const QModelIndex & lineIndex)
{
  Input input = Input(physicalModel_.getInputs()[data(lineIndex, Qt::UserRole).toInt()]);

  physicalModel_.blockNotification(true);
  physicalModel_.setInputDistribution(input.getName(), Dirac(input.getValue()));
  physicalModel_.blockNotification(false);

  beginRemoveRows(lineIndex.parent(), lineIndex.row(), lineIndex.row());
  removeRows(lineIndex.row(), 1, lineIndex.parent());
  userRoleList_.remove(lineIndex.row());
  endRemoveRows();
}

}