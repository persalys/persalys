//                                               -*- C++ -*-
/**
 *  @brief QAbstractTableModel for data model sample
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
#include "otgui/DataModelTableModel.hxx"

#include "otgui/StudyTreeViewModel.hxx"

#include <QColor>
#include <QHeaderView>

using namespace OT;

namespace OTGUI
{

DataModelTableModel::DataModelTableModel(DataModel* dataModel, QObject* parent)
  : QAbstractTableModel(parent)
  , dataModel_(dataModel)
  , names_()
  , inputColumns_()
  , outputColumns_()
{
}


void DataModelTableModel::updateData()
{
  beginResetModel();

  // the sample of dataModel can be smaller than the sample from the file
  // so we can not use getIn/OutputColumns()
  if (!dataModel_->getSampleFromFile().getSize())
  {
    // when opening a data model: we use dataModel.getSample() and not the sample from the file
    // so we can not use getInputColumns and getOutputColumns.
    // In dataModel.getSample(): we put first the input sample, then the output sample
    // so we can retrieve easily the indices
    names_ = dataModel_->getSample().getDescription();
    inputColumns_ = Indices(dataModel_->getInputSample().getSize() > 0 ? dataModel_->getInputSample().getDimension() : 0);
    inputColumns_.fill();
    outputColumns_ = Indices(dataModel_->getOutputSample().getSize() > 0 ? dataModel_->getOutputSample().getDimension() : 0);
    outputColumns_.fill(inputColumns_.getSize());
  }
  else
  {
    // the sample comes from the file
    // so we can use getIn/OutputColumns()
    names_ = dataModel_->getSampleFromFile().getDescription();
    inputColumns_ = dataModel_->getInputColumns();
    outputColumns_ = dataModel_->getOutputColumns();

    // update sample description
    Description dataDescription(names_);
    if (inputColumns_.getSize())
    {
      if (dataModel_->getInputNames().getSize() == inputColumns_.getSize())
      {
        for (UnsignedInteger i = 0; i < inputColumns_.getSize(); ++i)
          dataDescription[inputColumns_[i]] = dataModel_->getInputNames()[i];
      }
    }
    if (outputColumns_.getSize())
    {
      if (dataModel_->getOutputNames().getSize() == outputColumns_.getSize())
      {
        for (UnsignedInteger i = 0; i < outputColumns_.getSize(); ++i)
          dataDescription[outputColumns_[i]] = dataModel_->getOutputNames()[i];
      }
    }
    names_ = dataDescription;
  }

  // emit signal to update the sample table
  Sample sample(dataModel_->getSampleFromFile().getSize() ? dataModel_->getSampleFromFile() : dataModel_->getSample());
  sample.setDescription(names_);

  emit sampleChanged(sample);

  if (!dataModel_->isValid())
  {
    emit errorMessageChanged(tr("The sample contains invalid values"));
  }

  endResetModel();
}


int DataModelTableModel::rowCount(const QModelIndex& parent) const
{
  return 2;
}


int DataModelTableModel::columnCount(const QModelIndex& parent) const
{
  return names_.getSize();
}


Qt::ItemFlags DataModelTableModel::flags(const QModelIndex & index) const
{
  Qt::ItemFlags result = QAbstractTableModel::flags(index);

  // variables names
  if (index.row() == 0 && dataModel_->getSampleFromFile().getSize())
  {
    result |= Qt::ItemIsEditable | Qt::ItemIsUserCheckable;
  }
  // variables types
  else if (index.row() == 1 && !outputColumns_.contains(index.column()) && !inputColumns_.contains(index.column()))
  {
    result &= ~Qt::ItemIsSelectable;
    result &= ~Qt::ItemIsEnabled;
    result &= ~Qt::ItemIsEditable;
  }

  return result;
}


QVariant DataModelTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
  if (orientation == Qt::Vertical)
  {
    // text
    if (role == Qt::DisplayRole)
    {
      if (section == 0)
        return tr("Variable");
      else
        return tr("Type");
    }
    // alignment
    else if (role == Qt::TextAlignmentRole)
      return Qt::AlignCenter;
    // toolTip
    else if (role == Qt::ToolTipRole && section == 0 && dataModel_->getSampleFromFile().getSize())
      return (outputColumns_.isEmpty() && inputColumns_.isEmpty()) ? tr("Enable all") : tr("Disable all");
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
    {
      return QString::fromUtf8(names_[index.column()].c_str());
    }
    // check state
    else if (role == Qt::CheckStateRole && dataModel_->getSampleFromFile().getSize())
    {
      return (inputColumns_.contains(index.column()) || outputColumns_.contains(index.column())) ? Qt::Checked : Qt::Unchecked;
    }
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
    }

    // combobox content
    else if (role == Qt::UserRole + 1)
      return QStringList() << tr("Input") << tr("Output");

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

    // background
    else if (role == Qt::BackgroundRole)
      return QHeaderView(Qt::Horizontal).palette().color(QPalette::Window);
  }
  return QVariant();
}


bool DataModelTableModel::setData(const QModelIndex & index, const QVariant & value, int role)
{
  if (!index.isValid())
    return false;

  if (index.row() == 0)
  {
    // update variable name
    if (role == Qt::EditRole)
    {
      // if name not valid : reset
      if (value.toString().isEmpty())
        return false;

      // if the variable is already named like value : do nothing
      if (names_[index.column()] == value.toString().toUtf8().constData())
        return true;
      // if another variable is named like value : reset
      if (names_.contains(value.toString().toUtf8().constData()))
      {
        emit temporaryErrorMessageChanged(tr("The name %2 is already used by another variable").arg(value.toString()));
        return false;
      }
      names_[index.column()] = value.toString().toUtf8().constData();

      // update dataModel_ variables names
      Description inNames;
      for (UnsignedInteger i = 0; i < inputColumns_.getSize(); ++i)
        inNames.add(names_[inputColumns_[i]]);

      Description outNames;
      for (UnsignedInteger i = 0; i < outputColumns_.getSize(); ++i)
        outNames.add(names_[outputColumns_[i]]);

      dataModel_->blockNotification("DataModelDefinition");
      dataModel_->setNames(inNames, outNames);
      dataModel_->blockNotification();

      emit sampleDescriptionChanged(names_);
      return true;
    }
    // update list of variables
    else if (role == Qt::CheckStateRole)
    {
      if (value.toInt() == Qt::Unchecked)
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
      else
      {
        // by default the variable is an input
        inputColumns_.add(index.column());
      }
      // emit signal to update the sample table header
      emit headerDataChanged(Qt::Vertical, 0, 0);
    }
  }
  // change type
  else if (index.row() == 1 && role == Qt::EditRole)
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
  }
  // update dataModel_
  Description inNames;
  for (UnsignedInteger i = 0; i < inputColumns_.getSize(); ++i)
    inNames.add(names_[inputColumns_[i]]);

  Description outNames;
  for (UnsignedInteger i = 0; i < outputColumns_.getSize(); ++i)
    outNames.add(names_[outputColumns_[i]]);

  dataModel_->blockNotification("DataModelDefinition");
  dataModel_->setColumns(inputColumns_, outputColumns_, inNames, outNames);

  // emit error if all variables are disabled
  emit errorMessageChanged("");
  if (!inputColumns_.getSize() && !outputColumns_.getSize())
    emit errorMessageChanged(tr("Define at least one variable"));

  if (!dataModel_->isValid())
    emit errorMessageChanged(tr("The sample contains invalid values"));

  dataModel_->blockNotification();

  emit dataChanged(index, this->index(1, index.column()));

  return true;
}
}
