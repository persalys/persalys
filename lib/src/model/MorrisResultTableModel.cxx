//                                               -*- C++ -*-
/**
 *  @brief QAbstractTableModel for Morris result
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
#include "persalys/MorrisResultTableModel.hxx"

#include "persalys/StudyTreeViewModel.hxx"

using namespace OT;

namespace PERSALYS
{

MorrisResultTableModel::MorrisResultTableModel(MorrisResult & result, UnsignedInteger outputIndex, const Mode mode, QObject * parent)
  : QAbstractTableModel(parent)
  , mode_(mode)
  , result_(result)
  , outputIndex_(outputIndex)
{
}


int MorrisResultTableModel::columnCount(const QModelIndex & /*parent*/) const
{
  return 7;
}


int MorrisResultTableModel::rowCount(const QModelIndex & /*parent*/) const
{
  return result_.getDesignOfExperiment().getInputSample().getDimension();
}


Qt::ItemFlags MorrisResultTableModel::flags(const QModelIndex & index) const
{
  Qt::ItemFlags result = QAbstractTableModel::flags(index);

  result &= ~Qt::ItemIsEditable;

  if (mode_ == Edition && index.column() == 0)
  {
    result |= Qt::ItemIsUserCheckable;
  }
  else if (mode_ == Display && result_.getInputsSelection(outputIndex_)[index.row()] == 0)
  {
    result &= ~Qt::ItemIsEnabled;
  }

  return result;
}


QVariant MorrisResultTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
  if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
  {
    switch (section)
    {
      case 0:
        return tr("Variable");
      case 1:
        return tr("No effect");
      case 2:
        return tr("Linear effect");
      case 3:
        return tr("Non-linear effect\nor interaction");
      case 4:
        return tr("µ*");
      case 5:
        return tr("µ");
      case 6:
        return tr("σ");
      default:
        return QVariant();
    }
  }
  return QAbstractItemModel::headerData(section, orientation, role);
}


QVariant MorrisResultTableModel::data(const QModelIndex & index, int role) const
{
  if (!index.isValid())
    return QVariant();

  const UnsignedInteger inIndex = index.row();
  if (role == Qt::DisplayRole)
  {
    const Scalar meanAbsEE = result_.getMeanAbsoluteElementaryEffects(outputIndex_)[inIndex];
    const Scalar stdEE = result_.getStandardDeviationElementaryEffects(outputIndex_)[inIndex];
    switch (index.column())
    {
      case 0:
        return QString::fromUtf8(result_.getDesignOfExperiment().getInputSample().getDescription()[inIndex].c_str());
      case 1:
      {
        if (meanAbsEE < result_.getNoEffectBoundary(outputIndex_))
          return "X";
        return QVariant();
      }
      case 2:
      {
        if (meanAbsEE > result_.getNoEffectBoundary(outputIndex_) && stdEE <= meanAbsEE * 0.5)
          return "X";
        return QVariant();
      }
      case 3:
      {
        if (meanAbsEE > result_.getNoEffectBoundary(outputIndex_) && stdEE > meanAbsEE * 0.5)
          return "X";
        return QVariant();
      }
      case 4:
        return QString::number(meanAbsEE, 'g', StudyTreeViewModel::DefaultSignificantDigits);
      case 5:
        return QString::number(result_.getMeanElementaryEffects(outputIndex_)[inIndex], 'g', StudyTreeViewModel::DefaultSignificantDigits);
      case 6:
        return QString::number(result_.getStandardDeviationElementaryEffects(outputIndex_)[inIndex], 'g', StudyTreeViewModel::DefaultSignificantDigits);
      default:
        return QVariant();
    }
  }
  else if (role == Qt::UserRole)
  {
    switch (index.column())
    {
      case 4:
        return result_.getMeanAbsoluteElementaryEffects(outputIndex_)[inIndex];
      case 5:
        return result_.getMeanElementaryEffects(outputIndex_)[inIndex];
      case 6:
        return result_.getStandardDeviationElementaryEffects(outputIndex_)[inIndex];
      default:
        return data(index, Qt::DisplayRole);
    }
  }
  else if (role == Qt::CheckStateRole && index.column() == 0)
  {
    return result_.getInputsSelection(outputIndex_)[inIndex] > 0 ? Qt::Checked : Qt::Unchecked;
  }
  else if (role == Qt::TextAlignmentRole && index.column() > 0 && index.column() < 4)
  {
    return Qt::AlignCenter;
  }
  return QVariant();
}


bool MorrisResultTableModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
  // when user check a line
  if (role == Qt::CheckStateRole && index.column() == 0)
  {
    Indices selection = result_.getInputsSelection(outputIndex_);
    selection[index.row()] = value.toInt();
    result_.setInputsSelection(outputIndex_, selection);
    emit selectionChanged();
    emit dataChanged(index, index);
    emit headerDataChanged(Qt::Horizontal, 0, 0);
  }
  return true;
}


void MorrisResultTableModel::updateData()
{
  beginResetModel();
  endResetModel();
  emit headerDataChanged(Qt::Horizontal, 0, 0);
}


void MorrisResultTableModel::updateData(MorrisResult& result, const UnsignedInteger outIndex)
{
  beginResetModel();
  result_ = result;
  outputIndex_ = outIndex;
  endResetModel();
}
}
