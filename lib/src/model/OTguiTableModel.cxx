#include "otgui/OTguiTableModel.hxx"

#include <QFile>
#include <QColor>

using namespace OT;

namespace OTGUI {

OTguiTableModel::OTguiTableModel(const NumericalSample & data)
  : QAbstractTableModel()
  , data_(data)
  , sampleIsValid_(true)
{
  for (UnsignedInteger i=0; i<data_.getDimension(); ++i)
    for (UnsignedInteger j=0; j<data_.getSize(); ++j)
      if (std::isnan(data_[j][i]))
      {
        sampleIsValid_ = false;
        break;
      }
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


bool OTguiTableModel::setHeaderData(int section, Qt::Orientation orientation, const QVariant& value, int role)
{
  if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
  {
    Description description = data_.getDescription();
    description[section] = value.toString().toStdString();
    data_.setDescription(description);
    emit headerDataChanged(Qt::Horizontal, section, section);
    return true;
  }
  return QAbstractTableModel::setHeaderData(section, orientation, value, role);
}


QVariant OTguiTableModel::data(const QModelIndex & index, int role) const
{
  if (!index.isValid())
    return QVariant();

  if (role == Qt::TextAlignmentRole)
    return int(Qt::AlignRight | Qt::AlignVCenter);
  else if (role == Qt::DisplayRole)
    return QString::number(data_[index.row()][index.column()], 'g', 8);
  else if (role == Qt::BackgroundColorRole)
  {
    if (std::isnan(data_[index.row()][index.column()]))
      return QColor(Qt::red);
    return QVariant();
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


bool OTguiTableModel::sampleIsValid()
{
  return sampleIsValid_;
}
}