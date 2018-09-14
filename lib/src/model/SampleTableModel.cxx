//                                               -*- C++ -*-
/**
 *  @brief QAbstractTableModel for samples
 *
 *  Copyright 2015-2018 EDF-Phimeca
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
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
#include "otgui/SampleTableModel.hxx"

#include "otgui/StudyTreeViewModel.hxx"

#include <openturns/SpecFunc.hxx>

#include <QColor>

using namespace OT;

namespace OTGUI
{

SampleTableModel::SampleTableModel(const Sample & data, QObject * parent)
  : QAbstractTableModel(parent)
  , data_(data)
{
}


int SampleTableModel::columnCount(const QModelIndex& parent) const
{
  if (data_.getSize())
    return data_.getDimension() + 1;
  return 0;
}


int SampleTableModel::rowCount(const QModelIndex& parent) const
{
  return data_.getSize();
}


QVariant SampleTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
  if (orientation == Qt::Horizontal)
  {
    if (role == Qt::DisplayRole)
    {
      if (!section)
        return "Row ID";
      else
        return QString::fromUtf8(data_.getDescription()[section - 1].c_str());
    }
    else if (role == Qt::TextAlignmentRole)
      return Qt::AlignCenter;
  }
  else if (orientation == Qt::Vertical && role == Qt::DisplayRole)
    return section;

  return QAbstractTableModel::headerData(section, orientation, role);
}


QVariant SampleTableModel::data(const QModelIndex & index, int role) const
{
  if (!index.isValid())
    return QVariant();

  if (role == Qt::TextAlignmentRole)
    return int(Qt::AlignRight | Qt::AlignVCenter);

  else if (role == Qt::DisplayRole)
  {
    if (!index.column())
      return QString::number(index.row());
    else
      return QString::number(data_(index.row(), index.column() - 1), 'g', StudyTreeViewModel::DefaultSignificantDigits);
  }

  else if (role == Qt::UserRole)
  {
    if (!index.column())
      return index.row();
    else
      return data_(index.row(), index.column() - 1);
  }

  else if (role == Qt::BackgroundRole && index.column() && !SpecFunc::IsNormal(data_(index.row(), index.column() - 1)))
    return QColor(Qt::red);

  return QVariant();
}


void SampleTableModel::exportData(const QString & fileName)
{
  // write
  data_.exportToCSVFile(fileName.toLocal8Bit().data(), ",");
}


void SampleTableModel::updateHeaderData(const Description& header)
{
  Q_ASSERT(header.getSize() == data_.getDimension());
  data_.setDescription(header);
  emit headerDataChanged(Qt::Horizontal, 0, columnCount() - 1);
}


void SampleTableModel::updateData(const Sample& data)
{
  beginResetModel();
  data_ = data;
  endResetModel();
}
}
