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

DataModelTableModel::DataModelTableModel(const Sample& data, DataModel* dataModel, const bool useColumns, QObject* parent)
  : SampleTableModel(data, parent)
  , dataModel_(dataModel)
  , inputColumns_()
  , outputColumns_()
{
  if (useColumns)
  {
    inputColumns_ = dataModel->getInputColumns();
    outputColumns_ = dataModel->getOutputColumns();
  }
  else
  {
    // condition used when opening a data model: we use dataModel.getSample() and not the sample from the file
    // so we can not use getInputColumns and getOutputColumns.
    // In dataModel.getSample(): we put first the input sample, then the output sample
    // so we can retrieve easily the indices
    inputColumns_ = Indices(dataModel->getInputSample().getSize() > 0 ? dataModel->getInputSample().getDimension() : 0);
    inputColumns_.fill();
    outputColumns_ = Indices(dataModel->getOutputSample().getSize() > 0 ? dataModel->getOutputSample().getDimension() : 0);
    outputColumns_.fill(inputColumns_.getSize());
  }

  // data model
  Indices indices(inputColumns_);
  indices.add(outputColumns_);

  if (!indices.check(data_.getDimension()) || !indices.getSize())
  {
    inputColumns_ = Indices(data_.getDimension() > 1 ? data_.getDimension() - 1 : 1);
    inputColumns_.fill();
    outputColumns_ = Indices(data_.getDimension() > 1 ? 1 : 0, data_.getDimension() - 1);
    dataModel->blockNotification("DataModelDefinition");
    dataModel->setColumns(inputColumns_, outputColumns_);
    dataModel->blockNotification();
  }
  else // the indices are ok so we can use the variables names stored in the model
  {
    Description dataDescription(data_.getDescription());
    if (dataModel_->getInputNames().getSize() == inputColumns_.getSize())
    {
      for (UnsignedInteger i = 0; i < dataModel_->getInputNames().getSize(); ++i)
        dataDescription[inputColumns_[i]] = dataModel_->getInputNames()[i];
    }
    if (dataModel_->getOutputNames().getSize() == outputColumns_.getSize())
    {
      for (UnsignedInteger i = 0; i < dataModel_->getOutputNames().getSize(); ++i)
        dataDescription[outputColumns_[i]] = dataModel_->getOutputNames()[i];
    }
    data_.setDescription(dataDescription);
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
      return section - 1;
  }
  return QAbstractTableModel::headerData(section, orientation, role);
}


QVariant DataModelTableModel::data(const QModelIndex & index, int role) const
{
  if (!index.isValid())
    return QVariant();

  // variable name
  if (index.row() == 0)
  {
    // text
    if (role == Qt::DisplayRole || role == Qt::EditRole)
      return QString::fromUtf8(data_.getDescription()[index.column()].c_str());

    // alignment
    else if (role == Qt::TextAlignmentRole)
      return Qt::AlignCenter;
  }
  // variable type
  else if (index.row() == 1)
  {
    // text
    if (role == Qt::DisplayRole || role == Qt::EditRole)
    {
      if (inputColumns_.contains(index.column()))
        return tr("Input");
      else if (outputColumns_.contains(index.column()))
        return tr("Output");
      else
        return tr("Disable");
    }

    // alignment
    else if (role == Qt::TextAlignmentRole)
      return Qt::AlignCenter;

    // font
    else if (role == Qt::FontRole)
    {
      QFont font;
      font.setBold(true);
      return font;
    }
  }
  // variable value
  else
  {
    // text
    if (role == Qt::DisplayRole || role == Qt::EditRole)
      return QString::number(data_[index.row() - 2][index.column()], 'g', 15);

    // alignment
    else if (role == Qt::TextAlignmentRole)
      return int(Qt::AlignRight | Qt::AlignVCenter);

    // background
    else if (role == Qt::BackgroundRole)
      if (std::isnan(data_[index.row() - 2][index.column()]))
        return QColor(Qt::red);
  }
  return QVariant();
}


bool DataModelTableModel::setData(const QModelIndex & index, const QVariant & value, int role)
{
  if (!index.isValid())
    return false;

  if (role != Qt::EditRole)
    return true;

  // change name
  if (index.row() == 0)
  {
    // if name not valid : reset
    if (value.toString().isEmpty())
      return false;

    // if the variable is already named like value : do nothing
    Description description = data_.getDescription();
    if (description[index.column()] == value.toString().toUtf8().constData())
      return true;

    // if another variable is named like value : reset
    if (description.contains(value.toString().toUtf8().constData()))
    {
      emit temporaryErrorMessageChanged(tr("The name %2 is already used by another variable").arg(value.toString()));
      return false;
    }
    // update the variable name
    description[index.column()] = value.toString().toUtf8().constData();
    data_.setDescription(description);
  }

  // change type
  else if (index.row() == 1)
  {
    // Input
    if (value.toString() == tr("Input"))
    {
      // if already in inputColumns_ : do nothing
      if (inputColumns_.contains(index.column()))
        return true;
      // if it was an output before : rm index from outputColumns_
      else if (outputColumns_.contains(index.column()))
      {
        for (UnsignedInteger i = 0; i < outputColumns_.getSize(); ++i)
          if ((int)outputColumns_[i] == index.column())
            outputColumns_.erase(outputColumns_.begin() + i);
      }
      // update inputColumns_
      inputColumns_.add(index.column());
    }
    // Output
    else if (value.toString() == tr("Output"))
    {
      // if already in outputColumns_ : do nothing
      if (outputColumns_.contains(index.column()))
        return true;
      // if it was an input before : rm index from inputColumns_
      else if (inputColumns_.contains(index.column()))
      {
        for (UnsignedInteger i = 0; i < inputColumns_.getSize(); ++i)
          if ((int)inputColumns_[i] == index.column())
            inputColumns_.erase(inputColumns_.begin() + i);
      }
      // update outputColumns_
      outputColumns_.add(index.column());
    }
    // Disable
    else
    {
      // if it was an input before : rm index from inputColumns_
      if (inputColumns_.contains(index.column()))
      {
        for (UnsignedInteger i = 0; i < inputColumns_.getSize(); ++i)
          if ((int)inputColumns_[i] == index.column())
            inputColumns_.erase(inputColumns_.begin() + i);
      }
      // if it was an output before : rm index from outputColumns_
      else if (outputColumns_.contains(index.column()))
      {
        for (UnsignedInteger i = 0; i < outputColumns_.getSize(); ++i)
          if ((int)outputColumns_[i] == index.column())
            outputColumns_.erase(outputColumns_.begin() + i);
      }
    }
  }

  // update dataModel_
  Description inNames;
  if (inputColumns_.getSize())
    inNames = data_.getMarginal(inputColumns_).getDescription();
  Description outNames;
  if (outputColumns_.getSize())
    outNames = data_.getMarginal(outputColumns_).getDescription();

  dataModel_->blockNotification("DataModelDefinition");
  dataModel_->setColumns(inputColumns_, outputColumns_, inNames, outNames);

  // emit error if all variables are disabled
  emit errorMessageChanged("");
  if (!inputColumns_.getSize() && !outputColumns_.getSize())
    emit errorMessageChanged(tr("Define at least a variable"));

  dataModel_->blockNotification();

  // do not emit dataChanged : otherwise there is a display error because of QSortFilterProxyModel

  return true;
}
}
