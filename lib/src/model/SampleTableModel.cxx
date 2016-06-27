//                                               -*- C++ -*-
/**
 *  @brief QStandardItemModel for samples
 *
 *  Copyright 2015-2016 EDF-Phimeca
 *
 *  This library is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
#include "otgui/SampleTableModel.hxx"

#include <QColor>

using namespace OT;

namespace OTGUI {

SampleTableModel::SampleTableModel(const NumericalSample & data, QObject * parent)
  : QStandardItemModel(data.getSize(), data.getDimension(), parent)
  , data_(data)
  , sampleIsValid_(true)
{
  for (UnsignedInteger j=0; j<data_.getSize(); ++j)
    for (UnsignedInteger i=0; i<data_.getDimension(); ++i)
      if (std::isnan(data_[j][i]))
      {
        sampleIsValid_ = false;
        break;
      }
}


QVariant SampleTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
  if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
    return QString::fromUtf8(data_.getDescription()[section].c_str());

  return QStandardItemModel::headerData(section, orientation, role);
}


bool SampleTableModel::setHeaderData(int section, Qt::Orientation orientation, const QVariant& value, int role)
{
  if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
  {
    Description description = data_.getDescription();
    description[section] = value.toString().toStdString();
    data_.setDescription(description);
    emit headerDataChanged(Qt::Horizontal, section, section);
    return true;
  }
  return QStandardItemModel::setHeaderData(section, orientation, value, role);
}


QVariant SampleTableModel::data(const QModelIndex & index, int role) const
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


void SampleTableModel::exportData(const QString & fileName)
{
  // write
  data_.exportToCSVFile(fileName.toLocal8Bit().data());
}


bool SampleTableModel::sampleIsValid()
{
  return sampleIsValid_;
}
}