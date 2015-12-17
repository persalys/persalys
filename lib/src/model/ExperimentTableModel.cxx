#include "otgui/ExperimentTableModel.hxx"

using namespace OT;

namespace OTGUI {

ExperimentTableModel::ExperimentTableModel(const DesignOfExperiment & designOfExperiment)
  : QAbstractTableModel()
  , designOfExperiment_(designOfExperiment)
{
}


int ExperimentTableModel::columnCount(const QModelIndex & parent) const
{
  return 5;
}


int ExperimentTableModel::rowCount(const QModelIndex & parent) const
{
  return designOfExperiment_.getPhysicalModel().getInputs().getSize();
}


QVariant ExperimentTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
  if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
  {
    switch (section)
    {
      case 0:
        return tr("Name");
      case 1:
        return tr("Description");
      case 2:
        return tr("Lower bound");
      case 3:
        return tr("Upper bound");
      case 4:
        if (designOfExperiment_.getTypeDesignOfExperiment() == DesignOfExperimentImplementation::FromBoundsAndLevels)
          return tr("Levels");
        else
          return tr("Delta");
    }
  }
  return QAbstractTableModel::headerData(section, orientation, role);
}


bool ExperimentTableModel::setHeaderData(int section, Qt::Orientation orientation, const QVariant& value, int role)
{
  if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
  {
    if (value.toString() == tr("Levels") && designOfExperiment_.getTypeDesignOfExperiment() == DesignOfExperimentImplementation::FromBoundsAndDeltas)
      designOfExperiment_.setLevels(designOfExperiment_.getLevels());
    else if (value.toString() == tr("Delta") && designOfExperiment_.getTypeDesignOfExperiment() == DesignOfExperimentImplementation::FromBoundsAndLevels)
      designOfExperiment_.setDeltas(designOfExperiment_.getDeltas());
    else
      return true;
    emit dataChanged(index(0, 4), index(rowCount()-1, 4));
    emit headerDataChanged(Qt::Horizontal, section, section);
    return true;
  }
  return QAbstractTableModel::setHeaderData(section, orientation, value, role);
}


QVariant ExperimentTableModel::data(const QModelIndex & index, int role) const
{
  if (role == Qt::DisplayRole || role == Qt::EditRole)
  {
    int column = index.column();
    switch (column)
    {
      case 0:
        return designOfExperiment_.getPhysicalModel().getInputs()[index.row()].getName().c_str();
      case 1:
        return designOfExperiment_.getPhysicalModel().getInputs()[index.row()].getDescription().c_str();
      case 2:
        return designOfExperiment_.getLowerBounds()[index.row()];
      case 3:
        return designOfExperiment_.getUpperBounds()[index.row()];
      case 4:
      {
        if (designOfExperiment_.getTypeDesignOfExperiment() == DesignOfExperimentImplementation::FromBoundsAndLevels)
          return int(designOfExperiment_.getLevels()[index.row()]);
        else
          return designOfExperiment_.getDeltas()[index.row()];
      }
    }
  }
  return QVariant();
}


bool ExperimentTableModel::setData(const QModelIndex & index, const QVariant & value, int role)
{
  if (role == Qt::EditRole)
  {
    int column = index.column();
    switch (column)
    {
      case 0:
      case 1:
        break;
      case 2:
      {
        if (value.toDouble() > designOfExperiment_.getUpperBounds()[index.row()])
          break;
        NumericalPoint lowerBounds = designOfExperiment_.getLowerBounds();
        lowerBounds[index.row()] = value.toDouble();
        designOfExperiment_.setLowerBounds(lowerBounds);
        break;
      }
      case 3:
      {
        if (value.toDouble() < designOfExperiment_.getLowerBounds()[index.row()])
          break;
        NumericalPoint upperBounds = designOfExperiment_.getUpperBounds();
        upperBounds[index.row()] = value.toDouble();
        designOfExperiment_.setUpperBounds(upperBounds);
        break;
      }
      case 4:
      {
        if (value.toInt() < 1)
          break;
        Indices nbValues = designOfExperiment_.getLevels();
        nbValues[index.row()] = value.toInt();
        designOfExperiment_.setLevels(nbValues);
        break;
      }
    }
  }

  emit designOfExperimentChanged(designOfExperiment_);

  return true;
}


Qt::ItemFlags ExperimentTableModel::flags(const QModelIndex & index) const
{
  if (index.column()!=0 && index.column()!=1)
    return Qt::ItemIsEditable | QAbstractTableModel::flags(index);
  else
    return QAbstractTableModel::flags(index);
}


DesignOfExperiment ExperimentTableModel::getDesignOfExperiment() const
{
  return designOfExperiment_;
}
}