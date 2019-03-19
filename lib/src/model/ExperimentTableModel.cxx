//                                               -*- C++ -*-
/**
 *  @brief QAbstractTableModel to list the inputs
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
#include "otgui/ExperimentTableModel.hxx"

#include "otgui/StudyTreeViewModel.hxx"

#include <QHeaderView>

using namespace OT;

namespace OTGUI
{

ExperimentTableModel::ExperimentTableModel(const GridDesignOfExperiment & designOfExperiment, QObject * parent)
  : QAbstractTableModel(parent)
  , designOfExperiment_(designOfExperiment)
  , firstColumnChecked_(!designOfExperiment.getLevels().contains(1))
{
  designOfExperiment_.updateParameters();
}


int ExperimentTableModel::columnCount(const QModelIndex & parent) const
{
  return 6;
}


int ExperimentTableModel::rowCount(const QModelIndex & parent) const
{
  // +1 for the header
  return designOfExperiment_.getPhysicalModel().getInputDimension() + 1;
}


Qt::ItemFlags ExperimentTableModel::flags(const QModelIndex & index) const
{
  Qt::ItemFlags result = QAbstractTableModel::flags(index);
  if (index.column() == 0)
    result |= Qt::ItemIsUserCheckable;

  // header
  if (index.row() == 0)
  {
    if (index.column() == 5)
      return result |= Qt::ItemIsEditable | Qt::ItemIsEnabled;
    else if (index.column() != 5)
      return result &= ~Qt::ItemIsEditable;
  }
  // not header
  else
  {
    int indexInput = index.row() - 1;
    if (index.column() == 0)
    {
      result &= ~Qt::ItemIsEditable;
      result |= Qt::ItemIsSelectable;
    }
    else if (index.column() == 2 && designOfExperiment_.getLevels()[indexInput] != 1)
      result &= ~Qt::ItemIsEnabled;
    else if (index.column() == 2 && designOfExperiment_.getLevels()[indexInput] == 1)
      result |= Qt::ItemIsEditable | Qt::ItemIsEnabled;
    else if (index.column() > 2 && designOfExperiment_.getLevels()[indexInput] != 1)
      result |= Qt::ItemIsEditable | Qt::ItemIsEnabled;
    else if (index.column() > 2 && designOfExperiment_.getLevels()[indexInput] == 1)
      result &= ~Qt::ItemIsEnabled;
  }
  return result;

}


QVariant ExperimentTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
  if (orientation == Qt::Vertical && role == Qt::DisplayRole)
  {
    if (section != 0)
      return section;
    else // first line is considered as the header of the table: do not set a line number
      return QVariant();
  }
  return QAbstractItemModel::headerData(section, orientation, role);
}


QVariant ExperimentTableModel::data(const QModelIndex & index, int role) const
{
  if (!index.isValid())
    return QVariant();

  // header
  if (index.row() == 0)
  {
    if (role == Qt::DisplayRole)
    {
      switch (index.column())
      {
        case 0:
          return tr("Name");
        case 1:
          return tr("Description");
        case 2:
          return tr("Value");
        case 3:
          return tr("Lower bound");
        case 4:
          return tr("Upper bound");
        case 5:
          if (designOfExperiment_.getTypeDesignOfExperiment() == GridDesignOfExperiment::FromBoundsAndLevels)
            return tr("Levels");
          else
            return tr("Delta");
        default:
          return QVariant();
      }
    }
    // combobox content
    else if (role == Qt::UserRole + 1)
    {
      return QStringList() << tr("Levels") << tr("Delta");
    }
    else if (role == Qt::CheckStateRole && index.column() == 0)
    {
      return firstColumnChecked_ ? Qt::Checked : Qt::Unchecked;
    }
    else if (role == Qt::BackgroundRole)
    {
      return QHeaderView(Qt::Horizontal).palette().color(QPalette::Window);
    }
    else if (role == Qt::TextAlignmentRole)
    {
      return Qt::AlignCenter;
    }
  }
  // not header
  else
  {
    if (role == Qt::DisplayRole || role == Qt::EditRole)
    {
      int indexInput = index.row() - 1;
      switch (index.column())
      {
        case 0:
          return QString::fromUtf8(designOfExperiment_.getPhysicalModel().getInputs()[indexInput].getName().c_str());
        case 1:
          return QString::fromUtf8(designOfExperiment_.getPhysicalModel().getInputs()[indexInput].getDescription().c_str());
        case 2:
          return QString::number(designOfExperiment_.getValues()[indexInput], 'g', StudyTreeViewModel::DefaultSignificantDigits);
        case 3:
          return QString::number(designOfExperiment_.getLowerBounds()[indexInput], 'g', StudyTreeViewModel::DefaultSignificantDigits);
        case 4:
          return QString::number(designOfExperiment_.getUpperBounds()[indexInput], 'g', StudyTreeViewModel::DefaultSignificantDigits);
        case 5:
        {
          if (designOfExperiment_.getTypeDesignOfExperiment() == GridDesignOfExperiment::FromBoundsAndLevels)
            return QString::number(int(designOfExperiment_.getLevels()[indexInput]));
          else
            return QString::number(designOfExperiment_.getDeltas()[indexInput], 'g', StudyTreeViewModel::DefaultSignificantDigits);
        }
        default:
          return QVariant();
      }
    }
    else if (role == Qt::CheckStateRole && index.column() == 0)
    {
      return designOfExperiment_.getLevels()[index.row() - 1] == 1 ? Qt::Unchecked : Qt::Checked;
    }
  }
  return QVariant();
}


bool ExperimentTableModel::setData(const QModelIndex & index, const QVariant & value, int role)
{
  // header
  if (index.row() == 0)
  {
    // first column : select variables
    if (role == Qt::CheckStateRole && index.column() == 0)
    {
      for (int i = 1; i < rowCount(); ++i)
        if (data(this->index(i, 0), role).toInt() != (value.toBool() ? Qt::Checked : Qt::Unchecked))
          setData(this->index(i, 0), value.toBool() ? Qt::Checked : Qt::Unchecked, role);
    }
    // fifth column : select levels or deltas
    else if (role == Qt::EditRole && index.column() == 5)
    {
      try
      {
        if (value.toString() == tr("Levels") && designOfExperiment_.getTypeDesignOfExperiment() == GridDesignOfExperiment::FromBoundsAndDeltas)
          designOfExperiment_.setLevels(designOfExperiment_.getLevels());
        else if (value.toString() == tr("Delta") && designOfExperiment_.getTypeDesignOfExperiment() == GridDesignOfExperiment::FromBoundsAndLevels)
          designOfExperiment_.setDeltas(designOfExperiment_.getDeltas());
        else
          return false;
      }
      catch (std::exception & ex)
      {
        emit errorMessageChanged(ex.what());
      }
      emit dataChanged(this->index(1, 5), this->index(rowCount() - 1, 5));
    }
  }
  // not header
  else
  {
    // when user check a line
    if (role == Qt::CheckStateRole && index.column() == 0)
    {
      const int indexInput = index.row() - 1;
      // doe defined with levels
      if (designOfExperiment_.getTypeDesignOfExperiment() == GridDesignOfExperiment::FromBoundsAndLevels)
      {
        Indices levels = designOfExperiment_.getLevels();
        if (value.toInt() == Qt::Checked)
          levels[indexInput] = 2;
        else if (value.toInt() == Qt::Unchecked)
          levels[indexInput] = 1;
        else
          return false;
        try
        {
          designOfExperiment_.setLevels(levels);
        }
        catch (std::exception & ex)
        {
          emit errorMessageChanged(ex.what());
        }
      }
      // doe defined with deltas
      else
      {
        Point deltas = designOfExperiment_.getDeltas();
        if (value.toDouble() == Qt::Checked)
          deltas[indexInput] = designOfExperiment_.getUpperBounds()[indexInput] - designOfExperiment_.getLowerBounds()[indexInput];
        else if (value.toDouble() == Qt::Unchecked)
          deltas[indexInput] = 0;
        else
          return false;
        try
        {
          designOfExperiment_.setDeltas(deltas);
        }
        catch (std::exception & ex)
        {
          emit errorMessageChanged(ex.what());
        }
      }
      firstColumnChecked_ = !designOfExperiment_.getLevels().contains(1);

      emit dataChanged(index, this->index(indexInput, 6));
    }
    // when user edit a value
    else if (role == Qt::EditRole)
    {
      int indexInput = index.row() - 1;
      switch (index.column())
      {
        case 0: // names
        case 1: // descriptions
          return false;
        case 2: // values
        {
          Point values = designOfExperiment_.getValues();
          if (values[indexInput] == value.toDouble())
            return false;
          values[indexInput] = value.toDouble();
          try
          {
            designOfExperiment_.setValues(values);
          }
          catch (std::exception & ex)
          {
            emit errorMessageChanged(ex.what());
          }
          break;
        }
        case 3: // lower bounds
        {
          if (value.toDouble() >= designOfExperiment_.getUpperBounds()[indexInput])
          {
            emit errorMessageChanged(tr("The lower bound must be less than the upper bound"));
            return false;
          }
          Point lowerBounds = designOfExperiment_.getLowerBounds();
          if (lowerBounds[indexInput] == value.toDouble())
            return false;
          lowerBounds[indexInput] = value.toDouble();
          try
          {
            designOfExperiment_.setLowerBounds(lowerBounds);
          }
          catch (std::exception & ex)
          {
            emit errorMessageChanged(ex.what());
          }
          break;
        }
        case 4: // upper bounds
        {
          if (value.toDouble() <= designOfExperiment_.getLowerBounds()[indexInput])
          {
            emit errorMessageChanged(tr("The upper bound must be greater than the lower bound"));
            return false;
          }
          Point upperBounds = designOfExperiment_.getUpperBounds();
          if (upperBounds[indexInput] == value.toDouble())
            return false;
          upperBounds[indexInput] = value.toDouble();
          try
          {
            designOfExperiment_.setUpperBounds(upperBounds);
          }
          catch (std::exception & ex)
          {
            emit errorMessageChanged(ex.what());
          }
          break;
        }
        case 5: // levels or deltas
        {
          // levels
          if (designOfExperiment_.getTypeDesignOfExperiment() == GridDesignOfExperiment::FromBoundsAndLevels)
          {
            if (value.toInt() < 2)
              return false;
            Indices nbValues = designOfExperiment_.getLevels();
            if (nbValues[indexInput] == value.toUInt())
              return false;
            nbValues[indexInput] = value.toUInt();
            try
            {
              designOfExperiment_.setLevels(nbValues);
            }
            catch (std::exception & ex)
            {
              emit errorMessageChanged(ex.what());
            }
            break;
          }
          // deltas
          else
          {
            Point deltas = designOfExperiment_.getDeltas();
            if (deltas[indexInput] == value.toDouble())
              return false;
            deltas[indexInput] = value.toDouble();
            if (deltas[indexInput] <= (designOfExperiment_.getUpperBounds()[indexInput] - designOfExperiment_.getLowerBounds()[indexInput]))
            {
              try
              {
                designOfExperiment_.setDeltas(deltas);
              }
              catch (std::exception & ex)
              {
                emit errorMessageChanged(ex.what());
              }
            }
            else
            {
              emit errorMessageChanged(tr("Delta must be less than (the upper bound - the lower bound)"));
              return false;
            }
            break;
          }
        }
        default:
          return false;
      }
    }
  }
  emit doeSizeChanged(QString::number(designOfExperiment_.getOriginalInputSample().getSize()));
  return true;
}


GridDesignOfExperiment ExperimentTableModel::getDesignOfExperiment() const
{
  return designOfExperiment_;
}
}
