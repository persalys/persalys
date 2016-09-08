//                                               -*- C++ -*-
/**
 *  @brief QStandardItemModel for data model sample
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
#include "otgui/DataModelTableModel.hxx"

#include <QColor>

using namespace OT;

namespace OTGUI {

DataModelTableModel::DataModelTableModel(const NumericalSample & data, DataModel & dataModel, const bool variablesNamesEditable)
  : QAbstractTableModel()
  , variablesNamesEditable_(variablesNamesEditable)
  , data_(data)
  , inputColumns_(dataModel.getInputColumns())
  , outputColumns_(dataModel.getOutputColumns())
  , sampleIsValid_(true)
  , dataModel_(dataModel)
{
  initialize();
}


void DataModelTableModel::initialize()
{
  // data model
  Indices indices(inputColumns_);
  indices.add(outputColumns_);

  if (!indices.check(data_.getDimension()) || !indices.getSize())
  {
    inputColumns_ = Indices(data_.getDimension()-1);
    inputColumns_.fill();
    outputColumns_ = Indices(1, data_.getDimension()-1);
    dataModel_.setColumns(inputColumns_, outputColumns_);
  }

  // sample
  for (UnsignedInteger j=0; j<data_.getSize(); ++j)
    for (UnsignedInteger i=0; i<data_.getDimension(); ++i)
      if (std::isnan(data_[j][i]))
      {
        sampleIsValid_ = false;
        break;
      }
}


int DataModelTableModel::columnCount(const QModelIndex& parent) const
{
  return data_.getDimension();
}


int DataModelTableModel::rowCount(const QModelIndex& parent) const
{
  return data_.getSize()+2;
}


Qt::ItemFlags DataModelTableModel::flags(const QModelIndex & index) const
{
  if (index.row() == 0 && variablesNamesEditable_) // variables names
    return QAbstractItemModel::flags(index) | Qt::ItemIsEditable;
  return QAbstractItemModel::flags(index);
}


QVariant DataModelTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
  if (orientation == Qt::Vertical && role == Qt::DisplayRole)
  {
    if (section == 0)
      return tr("Name");
    else if (section == 1)
      return tr("Type");
    else
      return section-1;
  }

  return QAbstractTableModel::headerData(section, orientation, role);
}


QVariant DataModelTableModel::data(const QModelIndex & index, int role) const
{
  if (!index.isValid())
    return QVariant();

  if (index.row() == 0) // variable name
  {
    if (role == Qt::DisplayRole || role == Qt::EditRole)
      return QString::fromUtf8(data_.getDescription()[index.column()].c_str());

    else if (role == Qt::TextAlignmentRole)
      return Qt::AlignCenter;

    else if (role == Qt::BackgroundRole && !variablesNamesEditable_)
      return QColor("#f2f1f0");
  }
  else if (index.row() == 1) // variable type
  {
    if (role == Qt::DisplayRole || role == Qt::EditRole)
    {
      if (inputColumns_.contains(index.column()))
        return tr("Input");
      else if (outputColumns_.contains(index.column()))
        return tr("Output");
      else
        return tr("Disable");
    }

    else if (role == Qt::TextAlignmentRole)
      return Qt::AlignCenter;
  }
  else // variable value
  {
    if (role == Qt::DisplayRole || role == Qt::EditRole)
      return QString::number(data_[index.row()-2][index.column()], 'g', 8);

    else if (role == Qt::TextAlignmentRole)
      return int(Qt::AlignRight | Qt::AlignVCenter);

    else if (role == Qt::BackgroundRole)
      if (std::isnan(data_[index.row()-2][index.column()]))
        return QColor(Qt::red);
  }
  return QVariant();
}


bool DataModelTableModel::setData(const QModelIndex & index, const QVariant & value, int role)
{
  if (!index.isValid())
    return false;

  if (role == Qt::EditRole)
  {
    // change name
    if (index.row() == 0)
    {
      if (value.toString().isEmpty())
        return false;

      Description description = data_.getDescription();
      description[index.column()] = value.toString().toUtf8().constData();
      data_.setDescription(description);
    }
    // change type
    else if (index.row() == 1)
    {
      if (value.toString() == tr("Input"))
      {
        if (inputColumns_.contains(index.column()))
          return true;
        else if (outputColumns_.contains(index.column()))
        {
          for (UnsignedInteger i=0; i<outputColumns_.getSize(); ++i)
            if (outputColumns_[i] == index.column())
              outputColumns_.erase(outputColumns_.begin() + i);
        }
        inputColumns_.add(index.column());
      }
      else if (value.toString() == tr("Output"))
      {
        if (outputColumns_.contains(index.column()))
          return true;
        else if (inputColumns_.contains(index.column()))
        {
          for (UnsignedInteger i=0; i<inputColumns_.getSize(); ++i)
            if (inputColumns_[i] == index.column())
              inputColumns_.erase(inputColumns_.begin() + i);
        }
        outputColumns_.add(index.column());
      }
      else
      {
        if (inputColumns_.contains(index.column()))
        {
          for (UnsignedInteger i=0; i<inputColumns_.getSize(); ++i)
            if (inputColumns_[i] == index.column())
              inputColumns_.erase(inputColumns_.begin() + i);
        }
        else if (outputColumns_.contains(index.column()))
        {
          for (UnsignedInteger i=0; i<outputColumns_.getSize(); ++i)
            if (outputColumns_[i] == index.column())
              outputColumns_.erase(outputColumns_.begin() + i);
        }
      }
    }

    try
    {
      Description inNames(data_.getMarginal(inputColumns_).getDescription());
      Description outNames(data_.getMarginal(outputColumns_).getDescription());
      dataModel_.setColumns(inputColumns_, outputColumns_, inNames, outNames);
      emit dataChanged(index, index);
      emit errorMessageChanged("");
    }
    catch (std::exception & ex)
    {
      if (!inputColumns_.getSize())
        emit errorMessageChanged(tr("The data model must contain at least one input"));
      else
        emit errorMessageChanged(ex.what());
    }
  }
  return true;
}


DataModel DataModelTableModel::getDataModel() const
{
  return dataModel_;
}


bool DataModelTableModel::sampleIsValid()
{
  return sampleIsValid_;
}
}