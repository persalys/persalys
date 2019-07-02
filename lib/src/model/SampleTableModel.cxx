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
#include "persalys/SampleTableModel.hxx"

#include "persalys/StudyTreeViewModel.hxx"

#include <openturns/SpecFunc.hxx>

#include <QColor>

using namespace OT;

namespace PERSALYS
{

SampleTableModel::SampleTableModel(const Sample & data, QObject * parent)
  : QAbstractTableModel(parent)
  , data_(data)
  , sampleIsSortable_(true)
  , initialDescription_()
{
}


SampleTableModel::SampleTableModel(const Sample & data, const Description& initialDescription, QObject * parent)
  : QAbstractTableModel(parent)
  , data_(data)
  , sampleIsSortable_(initialDescription.getSize() == 0)
  , initialDescription_(initialDescription)
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
  return sampleIsSortable_ ? data_.getSize() : data_.getSize() + 1;
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
  {
    if (!(!sampleIsSortable_ && section == 0))
      return sampleIsSortable_ ? section : section - 1;
    else
      return QVariant();
  }

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

  const int dataColIndex = sampleIsSortable_ ? index.column() - 1 : index.column();
  const int dataRowIndex = sampleIsSortable_ ? index.row() : index.row() - 1;

  if (role == Qt::TextAlignmentRole)
    return int(Qt::AlignRight | Qt::AlignVCenter);

  else if (role == Qt::DisplayRole)
  {
    if (sampleIsSortable_ && index.column() == 0)
      return QString::number(index.row());
    else if (!sampleIsSortable_ && index.row() == 0)
      return QString::fromUtf8(initialDescription_[index.column()].c_str());
    else
      return QString::number(data_(dataRowIndex, dataColIndex), 'g', StudyTreeViewModel::DefaultSignificantDigits);
  }

  else if (sampleIsSortable_ && role == Qt::UserRole)
  {
    if (index.column() == 0)
      return index.row();
    else
      return data_(index.row(), dataColIndex);
  }

  else if (role == Qt::BackgroundRole)
  {
    if ((sampleIsSortable_ && index.column() != 0 && !SpecFunc::IsNormal(data_(dataRowIndex, dataColIndex))) ||
        (!sampleIsSortable_ && index.row() != 0 && !SpecFunc::IsNormal(data_(dataRowIndex, dataColIndex))))
      return QColor(Qt::red);
  }

  return QVariant();
}


Sample SampleTableModel::getSample() const
{
  return data_;
}


void SampleTableModel::updateHeaderData(const Description& header)
{
  Q_ASSERT(sampleIsSortable_);
  Q_ASSERT(header.getSize() == data_.getDimension());
  data_.setDescription(header);
  emit headerDataChanged(Qt::Horizontal, 0, columnCount() - 1);
}


void SampleTableModel::updateData(const Sample& data)
{
  Q_ASSERT(sampleIsSortable_);
  beginResetModel();
  data_ = data;
  endResetModel();
}
}
