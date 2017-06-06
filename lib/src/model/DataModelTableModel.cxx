//                                               -*- C++ -*-
/**
 *  @brief QAbstractTableModel for data model sample
 *
 *  Copyright 2015-2017 EDF-Phimeca
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

DataModelTableModel::DataModelTableModel(const NumericalSample& data, DataModel* dataModel, const bool useColumns, QObject* parent)
  : SampleTableModel(data, parent)
  , dataModel_(dataModel)
{
  if (useColumns)
  {
    inputColumns_ = dataModel->getInputColumns();
    outputColumns_ = dataModel->getOutputColumns();
  }
  else
  {
    inputColumns_ = Indices(dataModel->getInputSample().getSize() > 0 ? dataModel->getInputSample().getDimension() : 0);
    inputColumns_.fill();
    outputColumns_ = Indices(dataModel->getOutputSample().getSize() > 0 ? dataModel->getOutputSample().getDimension() : 0);
    outputColumns_.fill(inputColumns_.getSize());
  }

  // data model
  Indices indices(inputColumns_);
  indices.add(outputColumns_);

  if (!indices.check(data_.getDimension()-1) || !indices.getSize())
  {
    inputColumns_ = Indices(data_.getDimension() > 1? data_.getDimension()-1 : 1);
    inputColumns_.fill();
    outputColumns_ = Indices(data_.getDimension() > 1? 1 : 0, data_.getDimension()-1);
    dataModel->blockNotification("DataModelDefinition");
    dataModel->setColumns(inputColumns_, outputColumns_);
    dataModel->blockNotification();
  }
}


int DataModelTableModel::rowCount(const QModelIndex& parent) const
{
  return data_.getSize()+2;
}


Qt::ItemFlags DataModelTableModel::flags(const QModelIndex & index) const
{
  if (index.row() == 0) // variables names
    return QAbstractTableModel::flags(index) | Qt::ItemIsEditable;

  if (index.row() == 1)
    return QAbstractTableModel::flags(index) & ~Qt::ItemIsSelectable;

  return QAbstractTableModel::flags(index);
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

    else if (role == Qt::FontRole)
    {
      QFont font;
      font.setBold(true);
      return font;
    }
  }
  else // variable value
  {
    if (role == Qt::DisplayRole || role == Qt::EditRole)
      return QString::number(data_[index.row()-2][index.column()], 'g', 15);

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
      if (description[index.column()] == value.toString().toUtf8().constData())
        return true;

      if (description.contains(value.toString().toUtf8().constData()))
      {
        emit temporaryErrorMessageChanged(tr("The name %2 is already used by another variable").arg(value.toString()));
        return false;
      }
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
            if ((int)outputColumns_[i] == index.column())
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
            if ((int)inputColumns_[i] == index.column())
              inputColumns_.erase(inputColumns_.begin() + i);
        }
        outputColumns_.add(index.column());
      }
      else
      {
        if (inputColumns_.contains(index.column()))
        {
          for (UnsignedInteger i=0; i<inputColumns_.getSize(); ++i)
            if ((int)inputColumns_[i] == index.column())
              inputColumns_.erase(inputColumns_.begin() + i);
        }
        else if (outputColumns_.contains(index.column()))
        {
          for (UnsignedInteger i=0; i<outputColumns_.getSize(); ++i)
            if ((int)outputColumns_[i] == index.column())
              outputColumns_.erase(outputColumns_.begin() + i);
        }
      }
    }

    Description inNames;
    if (inputColumns_.getSize())
      inNames = data_.getMarginal(inputColumns_).getDescription();
    Description outNames;
    if (outputColumns_.getSize())
      outNames = data_.getMarginal(outputColumns_).getDescription();

    dataModel_->blockNotification("DataModelDefinition");
    dataModel_->setColumns(inputColumns_, outputColumns_, inNames, outNames);
    emit dataChanged(index, index);
    emit errorMessageChanged("");
    if (!inputColumns_.getSize() && !outputColumns_.getSize())
      emit errorMessageChanged(tr("Define at least a variable"));

    dataModel_->blockNotification();
  }
  return true;
}
}