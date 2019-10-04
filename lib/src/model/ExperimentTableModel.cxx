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
#include "persalys/ExperimentTableModel.hxx"

#include "persalys/StudyTreeViewModel.hxx"

#include <openturns/Box.hxx>

#include <QHeaderView>

using namespace OT;

namespace PERSALYS
{

ExperimentTableModel::ExperimentTableModel(const GridDesignOfExperiment &designOfExperiment, QObject *parent)
  : QAbstractTableModel(parent)
  , designOfExperiment_(designOfExperiment)
{
  designOfExperiment_.updateParameters();

  values_ = designOfExperiment_.getValues();

  const UnsignedInteger nbInputs = designOfExperiment_.getPhysicalModel().getInputDimension();
  fixValues_ = Point(nbInputs);
  interval_ = Interval(nbInputs);
  Tools::ComputeBounds(designOfExperiment_.getPhysicalModel().getInputs(), fixValues_, interval_);

  Point lowerBounds(interval_.getLowerBound());
  Point upperBounds(interval_.getUpperBound());
  levels_ = Indices(nbInputs);
  deltas_ = Point(nbInputs);
  for (UnsignedInteger i = 0; i < nbInputs; ++i)
  {
    if (values_[i].getSize() == 1)
      fixValues_[i] = values_[i][0];
    else
    {
      fixValues_[i] = designOfExperiment_.getPhysicalModel().getInputs()[i].getValue();

      std::pair<Collection<Scalar>::iterator, Collection<Scalar>::iterator> p = std::minmax_element(values_[i].begin(), values_[i].end());
      lowerBounds[i] = (*p.first);
      upperBounds[i] = (*p.second);
      interval_.setLowerBound(lowerBounds);
      interval_.setUpperBound(upperBounds);
    }

    levels_[i] = values_[i].getSize();
    deltas_[i] = levels_[i] < 2 ? 0 : interval_.getMarginal(i).getVolume() / (levels_[i] - 1);
    settings_ << tr("Levels =");
  }
}


int ExperimentTableModel::columnCount(const QModelIndex & /*parent*/) const
{
  return 8;
}


int ExperimentTableModel::rowCount(const QModelIndex & /*parent*/) const
{
  // +1 for the header
  return designOfExperiment_.getPhysicalModel().getInputDimension() + 1;
}


Qt::ItemFlags ExperimentTableModel::flags(const QModelIndex &index) const
{
  Qt::ItemFlags result = QAbstractTableModel::flags(index);
  if (index.column() == 0)
    result |= Qt::ItemIsUserCheckable;

  // header
  if (index.row() == 0)
  {
    return result &= ~Qt::ItemIsEditable;
  }
  // not header
  else
  {
    const int indexInput = index.row() - 1;
    if (index.column() == 0)
    {
      result &= ~Qt::ItemIsEditable;
      result |= Qt::ItemIsSelectable;
    }
    else if (index.column() == 2 && levels_[indexInput] != 1)
      result &= ~Qt::ItemIsEnabled;
    else if (index.column() == 2 && levels_[indexInput] == 1)
      result |= Qt::ItemIsEditable | Qt::ItemIsEnabled;

    else if (index.column() == 6 && settings_[indexInput] == tr("Delta =") && deltas_[indexInput] <= 0)
      result &= ~Qt::ItemIsEnabled;

    else if (index.column() > 2 && levels_[indexInput] != 1)
      result |= Qt::ItemIsEditable | Qt::ItemIsEnabled;
    else if (index.column() > 2 && levels_[indexInput] == 1)
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


QVariant ExperimentTableModel::data(const QModelIndex &index, int role) const
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
          return tr("Discretization");
        default:
          return QVariant();
      }
    }

    else if (role == Qt::CheckStateRole && index.column() == 0)
      return !levels_.contains(1) ? Qt::Checked : Qt::Unchecked;

    else if (role == Qt::BackgroundRole)
      return QHeaderView(Qt::Horizontal).palette().color(QPalette::Window);

    else if (role == Qt::TextAlignmentRole)
      return Qt::AlignCenter;
  }
  // not header
  else
  {
    const int indexInput = index.row() - 1;
    if (role == Qt::DisplayRole || role == Qt::EditRole)
    {
      const int digits = StudyTreeViewModel::DefaultSignificantDigits;
      switch (index.column())
      {
        case 0:
          return QString::fromUtf8(designOfExperiment_.getPhysicalModel().getInputs()[indexInput].getName().c_str());
        case 1:
          return QString::fromUtf8(designOfExperiment_.getPhysicalModel().getInputs()[indexInput].getDescription().c_str());
        case 2:
          return QString::number(fixValues_[indexInput], 'g', digits);
        case 3:
          return QString::number(interval_.getLowerBound()[indexInput], 'g', digits);
        case 4:
          return QString::number(interval_.getUpperBound()[indexInput], 'g', digits);
        case 5:
          return settings_[indexInput];
        case 6:
        {
          if (settings_[indexInput] == tr("Levels ="))
            return QString::number((int)levels_[indexInput]);
          else
            return deltas_[indexInput] > 0 ? QString::number(deltas_[indexInput], 'g', digits) : "-";
        }
        case 7:
          return "...";
        default:
          return QVariant();
      }
    }
    else if (role == Qt::CheckStateRole && index.column() == 0)
      return levels_[indexInput] != 1 ? Qt::Checked : Qt::Unchecked;

    else if (role == Qt::ToolTipRole && index.column() == 0 && levels_[indexInput] != 1 && interval_.getMarginal(indexInput).isNumericallyEmpty())
      return tr("The lower bound must be less than the upper bound");

    else if (role == Qt::ForegroundRole && index.column() == 0 && levels_[indexInput] != 1 && interval_.getMarginal(indexInput).isNumericallyEmpty())
      return QColor(Qt::red);

    else if (role == Qt::UserRole + 1)
      return QStringList() << tr("Levels =") << tr("Delta =");

    else if (role == Qt::UserRole && index.column() == 7)
      return QVariant::fromValue(values_[indexInput]);

    else if (role == Qt::BackgroundRole && index.column() == 5)
      return QHeaderView(Qt::Horizontal).palette().color(QPalette::Window);
  }
  return QVariant();
}


bool ExperimentTableModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
  if (role == Qt::DisplayRole)
    return true;

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
  }
  // not header
  else
  {
    const int indexInput = index.row() - 1;
    // when user check a line
    if (role == Qt::CheckStateRole && index.column() == 0)
    {
      if (value.toInt() == Qt::Checked)
      {
        levels_[indexInput] = 2;
        deltas_[indexInput] = interval_.getMarginal(indexInput).getVolume();
        values_[indexInput] = Box(Indices(1, levels_[indexInput] - 2), interval_.getMarginal(indexInput)).generate().asPoint();
      }
      else
      {
        levels_[indexInput] = 1;
        deltas_[indexInput] = 0.;
        values_[indexInput] = Point(1, fixValues_[indexInput]);
      }

      emit dataChanged(index, this->index(index.row(), 7));
      designOfExperiment_.setValues(values_);
    }
    // when user edit a value
    else if (role == Qt::EditRole)
    {
      switch (index.column())
      {
        case 0: // names
        case 1: // descriptions
          return false;
        case 2: // values
        {
          fixValues_[indexInput] = value.toDouble();
          values_[indexInput] = Point(1, fixValues_[indexInput]);
          deltas_[indexInput] = 0.;

          break;
        }
        case 3: // lower bounds
        {
          Point lowerBounds(interval_.getLowerBound());
          lowerBounds[indexInput] = value.toDouble();
          interval_.setLowerBound(lowerBounds);
          values_[indexInput] = Box(Indices(1, levels_[indexInput] - 2), interval_.getMarginal(indexInput)).generate().asPoint();
          deltas_[indexInput] = values_[indexInput][1] - values_[indexInput][0];

          break;
        }
        case 4: // upper bounds
        {
          Point upperBounds(interval_.getUpperBound());
          upperBounds[indexInput] = value.toDouble();
          interval_.setUpperBound(upperBounds);
          values_[indexInput] = Box(Indices(1, levels_[indexInput] - 2), interval_.getMarginal(indexInput)).generate().asPoint();
          deltas_[indexInput] = values_[indexInput][1] - values_[indexInput][0];

          break;
        }
        case 5:
        {
          settings_[indexInput] = value.toString();
          break;
        }
        case 6: // levels or deltas
        {
          // levels
          if (settings_[indexInput] == tr("Levels ="))
          {
            levels_[indexInput] = value.toUInt();
            values_[indexInput] = Box(Indices(1, levels_[indexInput] - 2), interval_.getMarginal(indexInput)).generate().asPoint();
            deltas_[indexInput] = levels_[indexInput] < 2 ? 0 : values_[indexInput][1] - values_[indexInput][0];
          }
          // deltas
          else
          {
            if (value.toDouble() > interval_.getMarginal(indexInput).getVolume() || value.toDouble() <= 0.)
            {
              emit errorMessageChanged(tr("The delta must be greater or equal to 0 and less than the interval length."));
              return false;
            }
            deltas_[indexInput] = value.toDouble();
            Point values;
            values.add(interval_.getMarginal(indexInput).getLowerBound()[0]);
            Scalar value = values[0] + deltas_[indexInput];
            while (value < interval_.getMarginal(indexInput).getUpperBound()[0])
            {
              values.add(value);
              value += deltas_[indexInput];
            }
            values.add(interval_.getMarginal(indexInput).getUpperBound()[0]);
            values_[indexInput] = values;
            levels_[indexInput] = values.getSize();
          }
          break;
        }
        default:
          return false;
      }

      designOfExperiment_.setValues(values_);
      emit dataChanged(this->index(index.row(), 6), this->index(index.row(), 7));
    }
    else if (role == Qt::UserRole)
    {
      Point values(qvariant_cast<Point>(value));
      values_[indexInput] = values;
      levels_[indexInput] = values.getSize();
      std::pair<Collection<Scalar>::iterator, Collection<Scalar>::iterator> p = std::minmax_element(values.begin(), values.end());
      Point lowerBounds(interval_.getLowerBound());
      Point upperBounds(interval_.getUpperBound());
      lowerBounds[indexInput] = *p.first;
      upperBounds[indexInput] = *p.second;
      interval_ = Interval(lowerBounds, upperBounds);

      // compute delta if possible
      const double delta = interval_.getMarginal(indexInput).getVolume() / (levels_[indexInput] - 1);
      const Point boxValues(Box(Indices(1, levels_[indexInput] - 2), interval_.getMarginal(indexInput)).generate().asPoint());
      deltas_[indexInput] = (values == boxValues ? delta : -1);
      if (values != boxValues)
        settings_[indexInput] = tr("Levels =");

      emit dataChanged(this->index(index.row(), 0), this->index(index.row(), 7));
      designOfExperiment_.setValues(values_);
    }

    emit errorMessageChanged("");
    int nbPoints = 1;
    for (UnsignedInteger i = 0; i < values_.getSize(); ++i)
      nbPoints *= values_[i].getSize();
    emit doeSizeChanged(QString::number(nbPoints));

  }
  return true;
}


GridDesignOfExperiment ExperimentTableModel::getDesignOfExperiment() const
{
  return designOfExperiment_;
}


Interval ExperimentTableModel::getInterval() const
{
  return interval_;
}
}
