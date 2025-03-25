//                                               -*- C++ -*-
/**
 *  @brief QAbstractTableModel to list quantiles
 *
 *  Copyright 2015-2025 EDF-Phimeca
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
#include "persalys/QuantileTableModel.hxx"

using namespace OT;

namespace PERSALYS
{

  QuantileTableModel::QuantileTableModel(QObject * parent)
    : QAbstractTableModel(parent)
  {

  }

  QuantileTableModel::QuantileTableModel(const QuantileAnalysis & analysis, QObject * parent)
    : QAbstractTableModel(parent)
    , analysis_(analysis)
  {
    types_ = Sample(analysis_.getInterestVariables().getSize(), 3);
    Collection<Point> values;
    for (UnsignedInteger iMarg=0; iMarg<analysis_.getInterestVariables().getSize(); ++iMarg)
    {
      if (analysis_.getTailTypes()[iMarg] & QuantileAnalysisResult::Lower)
        types_(iMarg, 0) = 1;
      if (analysis_.getTailTypes()[iMarg] & QuantileAnalysisResult::Upper)
        types_(iMarg, 1) = 1;
      if (analysis_.getTailTypes()[iMarg] & QuantileAnalysisResult::Bilateral)
        types_(iMarg, 2) = 1;

      if (analysis_.getTargetProbabilities()[iMarg].getDimension() == analysis_.getInterestVariables().getSize())
        values.add(analysis_.getTargetProbabilities()[iMarg]);
      else
        values.add(analysis_.getDefaultTargetProbability());
    }
    setValues(values);
  }

  int QuantileTableModel::columnCount(const QModelIndex & /*parent*/) const
  {
    return 6;
  }


  int QuantileTableModel::rowCount(const QModelIndex & /*parent*/) const
  {
    // +1 for the header
    return analysis_.getInterestVariables().getSize() + 1;
  }

  Qt::ItemFlags QuantileTableModel::flags(const QModelIndex & index) const
  {
    Qt::ItemFlags result = QAbstractTableModel::flags(index);

    // header
    if (index.row() == 0)
    {
      result &= ~Qt::ItemIsEditable;
    }

    if (index.column() == 1 || index.column() == 2 || index.column() == 3)
    {
      result |= Qt::ItemIsUserCheckable;
    }
    return result;
  }


  QVariant QuantileTableModel::headerData(int section, Qt::Orientation orientation, int role) const
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


  QVariant QuantileTableModel::data(const QModelIndex & index, int role) const
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
          return tr("Variable");
        case 1:
          return tr("Lower");
        case 2:
          return tr("Upper");
        case 3:
          return tr("Bilateral");
        case 4:
          return tr("Target probability");
        default:
          return QVariant();
        }
      }
      else if (role == Qt::CheckStateRole)
      {
        if (index.column() >= 1 && index.column() <= 3)
        {
          for (UnsignedInteger i=0; i<types_.getMarginal(index.column()-1).getSize(); ++i)
            if (!types_.getMarginal(index.column()-1)(i,0))
              return Qt::Unchecked;
          return Qt::Checked;
        }
      }
    }
    // not header
    else
    {
      const int varIndex = index.row() - 1;
      if (role == Qt::CheckStateRole)
      {
        if (index.column() == 1)
        {
          if (types_(varIndex, 0))
            return Qt::Checked;
          else
            return Qt::Unchecked;
        }
        else if (index.column() == 2)
        {
          if (types_(varIndex, 1))
            return Qt::Checked;
          else
            return Qt::Unchecked;
        }
        else if (index.column() == 3)
        {
          if (types_(varIndex, 2))
            return Qt::Checked;
          else
            return Qt::Unchecked;
        }
      }

      if (role == Qt::DisplayRole && index.column() == 0)
        return QString::fromUtf8(analysis_.getInterestVariables()[varIndex].c_str());
      if ((role == Qt::DisplayRole || role == Qt::EditRole || role == Qt::UserRole) && index.column() == 4)
        return QtOT::PointToString(values_[varIndex]);
      else if ((role == Qt::DisplayRole || role == Qt::EditRole || role == Qt::UserRole) && index.column() == 5)
        return QVariant::fromValue(values_[varIndex]);

      if (role == Qt::TextAlignmentRole && index.column() > 0 && index.column() < 4)
        return Qt::AlignCenter;
    }
    return QVariant();
  }


  bool QuantileTableModel::setData(const QModelIndex & index, const QVariant & value, int role)
  {
    // header
    if (index.row() == 0)
    {
      if (role == Qt::CheckStateRole && index.column() == 1)
      {
        for (int i = 1; i < rowCount(); ++i)
        {
          if (data(this->index(i, 1), role).toInt() != (value.toBool() ? Qt::Checked : Qt::Unchecked))
            setData(this->index(i, 1), value.toBool() ? Qt::Checked : Qt::Unchecked, role);
        }
      }
      else if (role == Qt::CheckStateRole && index.column() == 2)
      {
        for (int i = 1; i < rowCount(); ++i)
        {
          if (data(this->index(i, 2), role).toInt() != (value.toBool() ? Qt::Checked : Qt::Unchecked))
            setData(this->index(i, 2), value.toBool() ? Qt::Checked : Qt::Unchecked, role);
        }
      }
      else if (role == Qt::CheckStateRole && index.column() == 3)
      {
        for (int i = 1; i < rowCount(); ++i)
        {
          if (data(this->index(i, 3), role).toInt() != (value.toBool() ? Qt::Checked : Qt::Unchecked))
            setData(this->index(i, 3), value.toBool() ? Qt::Checked : Qt::Unchecked, role);
        }
      }
      emit dataChanged(this->index(0, index.column()), this->index(this->rowCount(), index.column()));
    }
    // not header
    else
    {
      const int varIndex = index.row() - 1;
      if (role == Qt::CheckStateRole)
      {
        switch (index.column())
        {
        case 1: // lower
          types_(varIndex, 0) = value.toInt();
          break;
        case 2: // upper
          types_(varIndex, 1) = value.toInt();
          break;
        case 3: // bilateral
          types_(varIndex, 2) = value.toInt();
          break;
        default:
          return false;
        }
        emit dataChanged(this->index(index.row(), 0), this->index(index.row(), 5));
      }
      else
      {
        if (index.column() == 4)
        {
          QStringList valueList = value.toString().split(";");
          Point values;
          foreach (QString val, valueList)
          {
            values.add(val.toDouble());
          }
          values_[varIndex] = values;
          emit dataChanged(this->index(index.row(), index.column()),
                           this->index(index.row(), index.column()+1));
        }
        else if (index.column() == 5)
        {
          Point values(qvariant_cast<Point>(value));
          values_[varIndex] = values;
          emit dataChanged(this->index(index.row(), index.column()-1),
                           this->index(index.row(), index.column()));
        }
        for (UnsignedInteger i=0; i<values_[varIndex].getSize(); ++i)
          if (!(values_[varIndex][i] > 0. && values_[varIndex][i] < 1.))
            emit errorMessageChanged("Target probabilities are invalid for variable "
                                     + QString(analysis_.getInterestVariables()[varIndex].c_str()));
      }
    }
    checkTable();
    return true;
  }

  void QuantileTableModel::setValues(const Collection<Point> values)
  {
    values_ = values;
    emit dataChanged(this->index(1, 5), this->index(this->rowCount(), 5));
    checkTable();
  }

  Collection<Point> QuantileTableModel::getTargetProbabilities() const
  {
    return values_;
  }

  Indices QuantileTableModel::getTailTypes() const
  {
    Indices types;
    for (UnsignedInteger iMarg=0; iMarg<types_.getSize(); ++iMarg)
    {
      UnsignedInteger typeMarg = 0;
      if (types_(iMarg, 0))
        typeMarg |= QuantileAnalysisResult::Lower;
      if (types_(iMarg, 1))
        typeMarg |= QuantileAnalysisResult::Upper;
      if (types_(iMarg, 2))
        typeMarg |= QuantileAnalysisResult::Bilateral;
      types.add(typeMarg);
    }
    return types;
  }

  Bool QuantileTableModel::checkTable()
  {
    for (UnsignedInteger varIndex=0; varIndex<types_.getSize(); varIndex++)
    {
      if (Point(types_[varIndex]).norm() == 0)
      {
        emit errorMessageChanged(tr("Select at least one tail for variable ")
                                  + QString(analysis_.getInterestVariables()[varIndex].c_str()));
        return false;
      }
      if (analysis_.getType() == QuantileAnalysisResult::MonteCarlo)
      {
        const UnsignedInteger size = analysis_.getDesignOfExperiment().getSample().getSize();
        const Indices validity = analysis_.computeWilksValidity(values_[varIndex], size, static_cast<QuantileAnalysisResult::TailType>(getTailTypes()[varIndex]));
        for (UnsignedInteger i=0; i<validity.getSize(); ++i)
        {
          if (!validity[i])
          {
            emit errorMessageChanged(tr("Sample size is too small to ensure quantile validity at specified target probability for variable ") + QString::fromUtf8(analysis_.getInterestVariables()[varIndex].c_str()) + tr("\nYou must increase target probability or use excess with Generalized pareto distribution"));
            return false;
          }
        }
      }
    }
    emit errorMessageChanged("");
    return true;
  }
}
