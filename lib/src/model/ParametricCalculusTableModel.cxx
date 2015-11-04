#include "otgui/ParametricCalculusTableModel.hxx"

namespace OTGUI {

ParametricCalculusTableModel::ParametricCalculusTableModel(const Calculus & calculus)
  : QAbstractTableModel()
  , calculus_(calculus)
{
  fillTable();
}


ParametricCalculusTableModel::~ParametricCalculusTableModel()
{
}


int ParametricCalculusTableModel::columnCount(const QModelIndex & parent) const
{
  return 5;
}


int ParametricCalculusTableModel::rowCount(const QModelIndex & parent) const
{
  return calculus_.getPhysicalModel().getInputs().getSize();
}


QVariant ParametricCalculusTableModel::headerData(int section, Qt::Orientation orientation, int role) const
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
            return QString(tr("Min bound"));
        case 3:
            return QString(tr("Max bound"));
        case 4:
            return QString(tr("Number values"));
      }
    }
  }
  return QVariant();
}


QVariant ParametricCalculusTableModel::data(const QModelIndex & index, int role) const
{
  if (role == Qt::DisplayRole || role == Qt::EditRole)
  {
    int column = index.column();
    switch (column)
    {
      case 0:
        return calculus_.getPhysicalModel().getInputs()[index.row()].getName().c_str();
      case 1:
        return calculus_.getPhysicalModel().getInputs()[index.row()].getDescription().c_str();
      case 2:
        return dynamic_cast<ParametricCalculus*>(&*getCalculus().getImplementation())->getInfBounds()[index.row()];
      case 3:
        return dynamic_cast<ParametricCalculus*>(&*getCalculus().getImplementation())->getSupBounds()[index.row()];
      case 4:
        return int(dynamic_cast<ParametricCalculus*>(&*getCalculus().getImplementation())->getNbValues()[index.row()]);
    }
  }
  return QVariant();
}


bool ParametricCalculusTableModel::setData(const QModelIndex & index, const QVariant & value, int role)
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
        if (value.toDouble() > dynamic_cast<ParametricCalculus*>(&*getCalculus().getImplementation())->getSupBounds()[index.row()])
          break;
        dynamic_cast<ParametricCalculus*>(&*calculus_.getImplementation())->setInfBound(index.row(), value.toDouble());
        break;
      }
      case 3:
      {
        if (value.toDouble() < dynamic_cast<ParametricCalculus*>(&*getCalculus().getImplementation())->getInfBounds()[index.row()])
          break;
        dynamic_cast<ParametricCalculus*>(&*calculus_.getImplementation())->setSupBound(index.row(), value.toDouble());
        break;
      }
      case 4:
      {
        if (value.toInt() < 1)
          break;
        dynamic_cast<ParametricCalculus*>(&*calculus_.getImplementation())->setNbValues(index.row(), value.toInt());
        break;
      }
    }
  }

  emit dataChanged(calculus_);
  return true;
}


Qt::ItemFlags ParametricCalculusTableModel::flags(const QModelIndex & index) const
{
  if (index.column()!=0 && index.column()!=1)
    return Qt::ItemIsEditable | QAbstractTableModel::flags(index);
  else
    return QAbstractTableModel::flags(index);
}


void ParametricCalculusTableModel::fillTable()
{
  QModelIndex lastIndex = index(-1, 0);
  beginInsertRows(lastIndex.parent(), lastIndex.row(), lastIndex.row()+calculus_.getPhysicalModel().getInputs().getSize()-1);
  insertRows(lastIndex.row(), lastIndex.row()+calculus_.getPhysicalModel().getInputs().getSize()-1);

  endInsertRows();
}


Calculus ParametricCalculusTableModel::getCalculus() const
{
  return calculus_;
}


}




