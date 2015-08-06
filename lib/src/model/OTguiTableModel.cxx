#include "OTguiTableModel.hxx"

#include <QFile>

namespace OTGUI {

OTguiTableModel::OTguiTableModel(const OT::NumericalSample & data)
  : QAbstractTableModel()
  , data_(data)
{
}


OTguiTableModel::~OTguiTableModel()
{

}


int OTguiTableModel::columnCount(const QModelIndex & parent) const
{
  return data_.getDimension();
}


int OTguiTableModel::rowCount(const QModelIndex & parent) const
{
  return data_.getSize();
}


QVariant OTguiTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
  if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
    return QVariant(data_.getDescription()[section].c_str());

  return QAbstractTableModel::headerData(section, orientation, role);
}


QVariant OTguiTableModel::data(const QModelIndex & index, int role) const
{
  if (!index.isValid()) return QVariant();

  if (role == Qt::TextAlignmentRole)
      return int(Qt::AlignRight | Qt::AlignVCenter);

  if (role == Qt::DisplayRole)
  {
    return QString::number(data_[index.row()][index.column()], 'g', 8);
  }

  return QVariant();
}


bool OTguiTableModel::exportData(const QString & fileName)
{
  QFile file(fileName);

  // check
  if (!file.open(QFile::WriteOnly))
  {
//     "cannot open"<<file.fileName();
    return false;
  }

  // write
  data_.exportToCSVFile(fileName.toLocal8Bit().data());
  file.close();

  return true;
}


}
