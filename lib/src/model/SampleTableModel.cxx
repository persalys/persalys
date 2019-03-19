//                                               -*- C++ -*-
/**
 *  @brief QAbstractTableModel for samples
 *
 *  Copyright 2015-2019 EDF-Phimeca
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
  , sampleIsSortable_(true)
{
}


SampleTableModel::SampleTableModel(const Sample & data, const bool isSortable, QObject * parent)
  : QAbstractTableModel(parent)
  , data_(data)
  , sampleIsSortable_(isSortable)
{
}


int SampleTableModel::columnCount(const QModelIndex& /*parent*/) const
{
  if (data_.getSize())
    return sampleIsSortable_ ? data_.getDimension() + 1 : data_.getDimension();
  return 0;
}


int SampleTableModel::rowCount(const QModelIndex& /*parent*/) const
{
  return data_.getSize();
}


QVariant SampleTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
  if (orientation == Qt::Horizontal)
  {
    if (role == Qt::DisplayRole)
    {
      if (!section && sampleIsSortable_)
        return "Row ID";
      else
        return QString::fromUtf8(data_.getDescription()[sampleIsSortable_ ? section - 1 : section].c_str());
    }
    else if (role == Qt::TextAlignmentRole)
      return Qt::AlignCenter;
  }
  else if (orientation == Qt::Vertical && role == Qt::DisplayRole)
    return section;

  return QAbstractTableModel::headerData(section, orientation, role);
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
  return QAbstractTableModel::setHeaderData(section, orientation, value, role);
}


QVariant SampleTableModel::data(const QModelIndex & index, int role) const
{
  if (!index.isValid())
    return QVariant();

  const int dataIndex = sampleIsSortable_ ? index.column() - 1 : index.column();

  if (role == Qt::TextAlignmentRole)
    return int(Qt::AlignRight | Qt::AlignVCenter);

  else if (role == Qt::DisplayRole)
  {
    if (!index.column() && sampleIsSortable_)
      return QString::number(index.row());
    else
      return QString::number(data_(index.row(), dataIndex), 'g', StudyTreeViewModel::DefaultSignificantDigits);
  }

  else if (role == Qt::UserRole)
  {
    if (!index.column() && sampleIsSortable_)
      return index.row();
    else
      return data_(index.row(), dataIndex);
  }

  else if (role == Qt::BackgroundRole && index.column() && !SpecFunc::IsNormal(data_(index.row(), dataIndex)))
    return QColor(Qt::red);

  return QVariant();
}


Sample SampleTableModel::getSample() const
{
  return data_;
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
