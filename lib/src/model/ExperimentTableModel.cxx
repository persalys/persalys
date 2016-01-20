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
  return 7;
}


int ExperimentTableModel::rowCount(const QModelIndex & parent) const
{
  return designOfExperiment_.getPhysicalModel().getInputs().getSize();
}


Qt::ItemFlags ExperimentTableModel::flags(const QModelIndex & index) const
{
  if (index.column() == 0)
    return QAbstractTableModel::flags(index) & ~Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsUserCheckable;
  else if (index.column() == 3 && designOfExperiment_.getLevels()[index.row()] != 1)
    return QAbstractTableModel::flags(index) & ~Qt::ItemIsEnabled;
  else if (index.column() == 3 && designOfExperiment_.getLevels()[index.row()] == 1)
    return QAbstractTableModel::flags(index) | Qt::ItemIsEditable | Qt::ItemIsEnabled;
  else if (index.column() > 3 && designOfExperiment_.getLevels()[index.row()] != 1)
    return QAbstractTableModel::flags(index) | Qt::ItemIsEditable | Qt::ItemIsEnabled;
  else if (index.column() > 3 && designOfExperiment_.getLevels()[index.row()] == 1)
    return QAbstractTableModel::flags(index) & ~Qt::ItemIsEnabled;
  else
    return QAbstractTableModel::flags(index);
}


QVariant ExperimentTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
  if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
  {
    switch (section)
    {
      case 0:
        return tr("");
      case 1:
        return tr("Name");
      case 2:
        return tr("Description");
      case 3:
        return tr("Value");
      case 4:
        return tr("Lower bound");
      case 5:
        return tr("Upper bound");
      case 6:
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
    emit dataChanged(index(0, 6), index(rowCount()-1, 6));
    emit headerDataChanged(Qt::Horizontal, section, section);
    return true;
  }
  return QAbstractTableModel::setHeaderData(section, orientation, value, role);
}


QVariant ExperimentTableModel::data(const QModelIndex & index, int role) const
{
  if (role == Qt::CheckStateRole && index.column() == 0)
  {
    if (designOfExperiment_.getLevels()[index.row()] == 1)
      return Qt::Unchecked;
    else
      return Qt::Checked;
  }
  else if (role == Qt::DisplayRole || role == Qt::EditRole)
  {
    switch (index.column())
    {
      case 0:
        return QVariant();
      case 1:
        return designOfExperiment_.getPhysicalModel().getInputs()[index.row()].getName().c_str();
      case 2:
        return designOfExperiment_.getPhysicalModel().getInputs()[index.row()].getDescription().c_str();
      case 3:
        return designOfExperiment_.getValues()[index.row()];
      case 4:
        return designOfExperiment_.getLowerBounds()[index.row()];
      case 5:
        return designOfExperiment_.getUpperBounds()[index.row()];
      case 6:
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

  if (role == Qt::CheckStateRole && index.column() == 0)
  {
    Indices levels = designOfExperiment_.getLevels();
    if (value.toInt() == Qt::Checked)
    {
      levels[index.row()] = 2;
      designOfExperiment_.setLevels(levels);
    }
    else if (value.toInt() == Qt::Unchecked)
    {
      levels[index.row()] = 1;
      designOfExperiment_.setLevels(levels);
    }
    emit dataChanged(index, this->index(index.row(), 6));
    return true;
  }
  else if (role == Qt::EditRole)
  {
    switch (index.column())
    {
      case 0:
      case 1:
      case 2:
        return false;
      case 3:
      {
        NumericalPoint values = designOfExperiment_.getValues();
        if (values[index.row()] == value.toDouble())
          return false;
        values[index.row()] = value.toDouble();
        designOfExperiment_.setValues(values);
        break;
      }
      case 4:
      {
        if (value.toDouble() > designOfExperiment_.getUpperBounds()[index.row()])
          return false;
        NumericalPoint lowerBounds = designOfExperiment_.getLowerBounds();
        if (lowerBounds[index.row()] == value.toDouble())
          return false;
        lowerBounds[index.row()] = value.toDouble();
        designOfExperiment_.setLowerBounds(lowerBounds);
        break;
      }
      case 5:
      {
        if (value.toDouble() < designOfExperiment_.getLowerBounds()[index.row()])
          return false;
        NumericalPoint upperBounds = designOfExperiment_.getUpperBounds();
        if (upperBounds[index.row()] == value.toDouble())
          return false;
        upperBounds[index.row()] = value.toDouble();
        designOfExperiment_.setUpperBounds(upperBounds);
        break;
      }
      case 6:
      {
        if (designOfExperiment_.getTypeDesignOfExperiment() == DesignOfExperimentImplementation::FromBoundsAndLevels)
        {
          if (value.toInt() < 2)
            return false;
          Indices nbValues = designOfExperiment_.getLevels();
          if (nbValues[index.row()] == value.toInt())
            return false;
          nbValues[index.row()] = value.toInt();
          designOfExperiment_.setLevels(nbValues);
          break;
        }
        else
        {
          NumericalPoint deltas = designOfExperiment_.getDeltas();
          if (deltas[index.row()] == value.toDouble())
            return false;
          deltas[index.row()] = value.toDouble();
          designOfExperiment_.setDeltas(deltas);
          break;
        }
      }
    }
  }

  emit designOfExperimentChanged(designOfExperiment_);

  return true;
}


DesignOfExperiment ExperimentTableModel::getDesignOfExperiment() const
{
  return designOfExperiment_;
}
}