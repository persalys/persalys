//                                               -*- C++ -*-
/**
 *  @brief QAbstractTableModel for samples
 *
 *  Copyright 2015-2020 EDF-Phimeca
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

SampleTableModel::SampleTableModel(const Sample &data, const bool isEditable,
                                   const bool hasRowIDcolumn, const Description &initialDescription, QObject *parent)
  : QAbstractTableModel(parent)
  , data_(data)
  , isEditable_(isEditable)
  , hasRowIDcolumn_(hasRowIDcolumn)
  , initialDescription_(initialDescription)
{
}


SampleTableModel::SampleTableModel(const Sample &data, QObject *parent)
  : SampleTableModel(data, false, true, Description(), parent)
{
}

SampleTableModel::SampleTableModel(const Sample &data, const bool isEditable, QObject *parent)
  : SampleTableModel(data, isEditable, true, Description(), parent)
{
}

SampleTableModel::SampleTableModel(const Sample &data, const Description &initialDescription, QObject *parent)
  : SampleTableModel(data, false, initialDescription.getSize() == 0, initialDescription, parent)
{
}


int SampleTableModel::columnCount(const QModelIndex& /*parent*/) const
{
  if (data_.getSize())
    return hasRowIDcolumn_ ? data_.getDimension() + 1 : data_.getDimension();
  return 0;
}


int SampleTableModel::rowCount(const QModelIndex& /*parent*/) const
{
  return initialDescription_.getSize() ? data_.getSize() + 1 : data_.getSize();
}


Qt::ItemFlags SampleTableModel::flags(const QModelIndex & index) const
{
  Qt::ItemFlags result = QAbstractTableModel::flags(index);
  return isEditable_ ? result |= Qt::ItemIsEditable : result;
}


QVariant SampleTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
  if (orientation == Qt::Horizontal)
  {
    if (role == Qt::DisplayRole)
    {
      if (!section && hasRowIDcolumn_)
        return "Row ID";
      else
      {
        const UnsignedInteger index = hasRowIDcolumn_ ? section - 1 : section;
        if (index < data_.getDescription().getSize())
          return QString::fromUtf8(data_.getDescription()[index].c_str());
      }
    }
    else if (role == Qt::TextAlignmentRole)
      return Qt::AlignCenter;
  }
  else if (orientation == Qt::Vertical && role == Qt::DisplayRole)
  {
    if (!(!hasRowIDcolumn_ && section == 0))
      return hasRowIDcolumn_ ? section : section - 1;
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

  const int dataColIndex = hasRowIDcolumn_ ? index.column() - 1 : index.column();
  const int dataRowIndex = initialDescription_.getSize() ? index.row() - 1 : index.row();

  if (role == Qt::TextAlignmentRole)
    return int(Qt::AlignRight | Qt::AlignVCenter);

  else if (role == Qt::EditRole || role == Qt::DisplayRole)
  {
    if (hasRowIDcolumn_ && index.column() == 0)
      return QString::number(index.row());
    else if (initialDescription_.getSize() && index.row() == 0)
      return QString::fromUtf8(initialDescription_[index.column()].c_str());
    else
      return QString::number(data_(dataRowIndex, dataColIndex), 'g', StudyTreeViewModel::DefaultSignificantDigits);
  }

  else if (role == Qt::UserRole)
  {
    if (hasRowIDcolumn_ && index.column() == 0)
      return index.row();
    else
      return data_(dataRowIndex, dataColIndex);
  }

  else if (role == Qt::BackgroundRole)
  {
    if ((hasRowIDcolumn_ && index.column() != 0 && !SpecFunc::IsNormal(data_(dataRowIndex, dataColIndex))) ||
        (!hasRowIDcolumn_ && index.row() != 0 && !SpecFunc::IsNormal(data_(dataRowIndex, dataColIndex))))
      return QColor(Qt::red);
  }

  return QVariant();
}


bool SampleTableModel::setData(const QModelIndex & index, const QVariant & value, int role)
{
  if (!index.isValid() || (hasRowIDcolumn_ && index.column() > 0 && !isEditable_))
    return false;

  if (role == Qt::EditRole)
  {
    const int dataColIndex = hasRowIDcolumn_ ? index.column() - 1 : index.column();
    const int dataRowIndex = initialDescription_.getSize() ? index.row() - 1 : index.row();
    bool ok;
    Scalar val = value.toDouble(&ok);
    if(!ok)
      return false;
    data_(dataRowIndex, dataColIndex) = val;
    emit dataChanged(index, index);
    emit sampleChanged();
  }
  return true;
}


Sample SampleTableModel::getSample() const
{
  return data_;
}


void SampleTableModel::updateHeaderData(const Description& header)
{
  Q_ASSERT(hasRowIDcolumn_);
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

bool SampleTableModel::removeRows(int row, int count, const QModelIndex &)
{
  beginRemoveRows(QModelIndex(), row, row + count - 1);
  data_.erase(row, row+count);
  endRemoveRows();
  updateData(data_);
  emit sampleChanged();
  return true;
}

bool SampleTableModel::insertRows(int row, int count, const QModelIndex &)
{
  beginInsertRows(QModelIndex(), row, row+count);
  data_.add(Point(data_.getDimension(), 0.));
  updateData(data_);
  endInsertRows();
  emit sampleChanged();
  return true;
}
}
