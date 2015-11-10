#include "otgui/ParametricAnalysisTableModel.hxx"

#include "otgui/ParametricAnalysis.hxx"

namespace OTGUI {

ParametricAnalysisTableModel::ParametricAnalysisTableModel(const Analysis & analysis)
  : QAbstractTableModel()
  , analysis_(analysis)
{
  fillTable();
}


ParametricAnalysisTableModel::~ParametricAnalysisTableModel()
{
}


int ParametricAnalysisTableModel::columnCount(const QModelIndex & parent) const
{
  return 5;
}


int ParametricAnalysisTableModel::rowCount(const QModelIndex & parent) const
{
  return analysis_.getPhysicalModel().getInputs().getSize();
}


QVariant ParametricAnalysisTableModel::headerData(int section, Qt::Orientation orientation, int role) const
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


QVariant ParametricAnalysisTableModel::data(const QModelIndex & index, int role) const
{
  if (role == Qt::DisplayRole || role == Qt::EditRole)
  {
    int column = index.column();
    switch (column)
    {
      case 0:
        return analysis_.getPhysicalModel().getInputs()[index.row()].getName().c_str();
      case 1:
        return analysis_.getPhysicalModel().getInputs()[index.row()].getDescription().c_str();
      case 2:
        return dynamic_cast<ParametricAnalysis*>(&*getAnalysis().getImplementation())->getInfBounds()[index.row()];
      case 3:
        return dynamic_cast<ParametricAnalysis*>(&*getAnalysis().getImplementation())->getSupBounds()[index.row()];
      case 4:
        return int(dynamic_cast<ParametricAnalysis*>(&*getAnalysis().getImplementation())->getNbValues()[index.row()]);
    }
  }
  return QVariant();
}


bool ParametricAnalysisTableModel::setData(const QModelIndex & index, const QVariant & value, int role)
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
        if (value.toDouble() > dynamic_cast<ParametricAnalysis*>(&*getAnalysis().getImplementation())->getSupBounds()[index.row()])
          break;
        dynamic_cast<ParametricAnalysis*>(&*analysis_.getImplementation())->setInfBound(index.row(), value.toDouble());
        break;
      }
      case 3:
      {
        if (value.toDouble() < dynamic_cast<ParametricAnalysis*>(&*getAnalysis().getImplementation())->getInfBounds()[index.row()])
          break;
        dynamic_cast<ParametricAnalysis*>(&*analysis_.getImplementation())->setSupBound(index.row(), value.toDouble());
        break;
      }
      case 4:
      {
        if (value.toInt() < 1)
          break;
        dynamic_cast<ParametricAnalysis*>(&*analysis_.getImplementation())->setNbValues(index.row(), value.toInt());
        break;
      }
    }
  }

  emit dataChanged(analysis_);
  return true;
}


Qt::ItemFlags ParametricAnalysisTableModel::flags(const QModelIndex & index) const
{
  if (index.column()!=0 && index.column()!=1)
    return Qt::ItemIsEditable | QAbstractTableModel::flags(index);
  else
    return QAbstractTableModel::flags(index);
}


void ParametricAnalysisTableModel::fillTable()
{
  QModelIndex lastIndex = index(-1, 0);
  beginInsertRows(lastIndex.parent(), lastIndex.row(), lastIndex.row()+analysis_.getPhysicalModel().getInputs().getSize()-1);
  insertRows(lastIndex.row(), lastIndex.row()+analysis_.getPhysicalModel().getInputs().getSize()-1);

  endInsertRows();
}


Analysis ParametricAnalysisTableModel::getAnalysis() const
{
  return analysis_;
}


}




