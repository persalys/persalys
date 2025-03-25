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
#include "persalys/QuantileThresholdTableModel.hxx"

#include <QColor>

using namespace OT;

namespace PERSALYS
{

  QuantileThresholdTableModel::QuantileThresholdTableModel(QObject * parent)
    : QAbstractTableModel(parent)
  {

  }

  QuantileThresholdTableModel::QuantileThresholdTableModel(const QuantileAnalysis & analysis, QObject * parent)
    : QAbstractTableModel(parent)
    , analysis_(analysis)
  {
    cdfThreshold_ = analysis_.getCDFThreshold().getMarginal(analysis_.getInterestVariables());
    threshold_ = analysis_.getThreshold().getMarginal(analysis_.getInterestVariables());
  }

  int QuantileThresholdTableModel::columnCount(const QModelIndex & /*parent*/) const
  {
    return 7;
  }


  int QuantileThresholdTableModel::rowCount(const QModelIndex & /*parent*/) const
  {
    // +1 for the header
    return analysis_.getInterestVariables().getSize() + 2;
  }

  Qt::ItemFlags QuantileThresholdTableModel::flags(const QModelIndex & index) const
  {
    Qt::ItemFlags result = QAbstractTableModel::flags(index);

    // headers
    if (index.row() < 2 || index.column() == 0)
      result &= ~Qt::ItemIsEditable;
    else
    {
      const int varIndex = index.row() - 2;
      switch (analysis_.getTailTypes()[varIndex])
      {
      case QuantileAnalysisResult::Upper:
        if (index.column() == 1 || index.column() == 3 || index.column() == 5)
          result &= ~Qt::ItemIsEnabled;
        else
          result |= Qt::ItemIsEditable;
        break;
      case QuantileAnalysisResult::Lower:
        if (index.column() == 2 || index.column() == 4 || index.column() == 6)
          result &= ~Qt::ItemIsEnabled;
        else
          result |= Qt::ItemIsEditable;
        break;
      default:
        result |= Qt::ItemIsEditable;
      }
      if (index.column() == 5 || index.column() == 6)
        result &= ~Qt::ItemIsEditable;
    }
    return result;
  }


  QVariant QuantileThresholdTableModel::headerData(int section, Qt::Orientation orientation, int role) const
  {
    if (orientation == Qt::Vertical && role == Qt::DisplayRole)
      return QVariant();
    return QAbstractItemModel::headerData(section, orientation, role);
  }


  QVariant QuantileThresholdTableModel::data(const QModelIndex & index, int role) const
  {
    if (!index.isValid())
      return QVariant();

    // header
    if (index.row() < 2)
    {
      if (role == Qt::DisplayRole)
      {
        if (index.row() == 0)
        {
          switch (index.column())
          {
          case 0:
            return tr("Variable");
          case 1:
            return tr("CDF threshold");
          case 3:
            return tr("Threshold");
          case 5:
            return tr("Excess sample size");
          default:
            return QVariant();
          }
        }
        else
        {
          if (index.column() > 0)
          {
            switch (index.column()%2)
            {
            case 0:
              return tr("Upper");
            case 1:
              return tr("Lower");
            default:
              return QVariant();
            }
          }
          else
            return QVariant();
        }
      }
    }
    // not header
    else
    {
      const int varIndex = index.row() - 2;
      if (role == Qt::DisplayRole && index.column() == 0)
        return QString::fromUtf8(analysis_.getInterestVariables()[varIndex].c_str());
      else if (role == Qt::DisplayRole || role == Qt::EditRole || role == Qt::UserRole)
      {
        if (index.column() == 1)
          return QString::number(cdfThreshold_(0, varIndex));
        if (index.column() == 2)
          return QString::number(cdfThreshold_(1, varIndex));
        if (index.column() == 3)
          return QString::number(threshold_(0, varIndex));
        if (index.column() == 4)
          return QString::number(threshold_(1, varIndex));
        if (index.column() == 5)
          return QString::number((int)(cdfThreshold_(0, varIndex)
                                       * analysis_.getDesignOfExperiment().getSample().getSize()));
        if (index.column() == 6)
          return QString::number((int)((1. - cdfThreshold_(1, varIndex))
                                       * analysis_.getDesignOfExperiment().getSample().getSize()));
      }
      else if (role == Qt::ForegroundRole && flags(index) & Qt::ItemIsEnabled)
      {
        if ((index.column() == 1 || index.column() == 3) && analysis_.getTargetProbabilities()[varIndex].normInf() >= cdfThreshold_(0, varIndex))
          return QColor(Qt::red);
        else if ((index.column() == 2 || index.column() == 4) && analysis_.getTargetProbabilities()[varIndex].normInf() >= 1 - cdfThreshold_(1, varIndex))
          return QColor(Qt::red);
      }
    }
    return QVariant();
  }


  bool QuantileThresholdTableModel::setData(const QModelIndex & index, const QVariant & value, int role)
  {
    if (role == Qt::DisplayRole)
      return true;

    if (index.row() > 1)
    {
      const int varIndex = index.row() - 2;
      const Description varName = Description(1, analysis_.getInterestVariables()[varIndex]);
      if (role == Qt::EditRole || role == Qt::DisplayRole)
      {
        const Sample marginalSample = analysis_.getDesignOfExperiment().getSample().getMarginal(varName);
        switch (index.column())
        {
        case 1: // lower cdf
          cdfThreshold_(0, varIndex) = value.toDouble();
          threshold_(0, varIndex) = marginalSample.computeQuantile(value.toDouble())[0];
          emit dataChanged(this->index(index.row(), 3), this->index(index.row(), 3));
          emit dataChanged(this->index(index.row(), 5), this->index(index.row(), 5));
          break;
        case 2: // upper cdf
          cdfThreshold_(1, varIndex) = value.toDouble();
          threshold_(1, varIndex) = marginalSample.computeQuantile(value.toDouble())[0];
          emit dataChanged(this->index(index.row(), 4), this->index(index.row(), 4));
          emit dataChanged(this->index(index.row(), 6), this->index(index.row(), 6));
          break;
        case 3:  // lower thr
          threshold_(0, varIndex) = value.toDouble();
          cdfThreshold_(0, varIndex) = marginalSample.computeEmpiricalCDF(Point(1, value.toDouble()));
          emit dataChanged(this->index(index.row(), 1), this->index(index.row(), 1));
          emit dataChanged(this->index(index.row(), 5), this->index(index.row(), 5));
          break;
        case 4:  // upper thr
          threshold_(1, varIndex) = value.toDouble();
          cdfThreshold_(1, varIndex) = marginalSample.computeEmpiricalCDF(Point(1, value.toDouble()));
          emit dataChanged(this->index(index.row(), 2), this->index(index.row(), 2));
          emit dataChanged(this->index(index.row(), 6), this->index(index.row(), 6));
          break;
        default:
          return false;
        }
        checkValues(index);
      }
      return true;
    }
    return true;
  }

  void QuantileThresholdTableModel::checkValues(const QModelIndex & index)
  {
    try
    {
      analysis_.setThreshold(threshold_);
      analysis_.checkThresholdCompatibility();
      emit errorMessageChanged("");
    }
    catch (const InvalidArgumentException&)
    {
      setData(this->index(index.row(), index.column()), QColor(Qt::red), Qt::ForegroundRole);
      if (index.column() == 1 || index.column() == 3)
        emit errorMessageChanged(tr("Chosen CDF threshold must be greater than the target probability."));
      else if (index.column() == 2 || index.column() == 4)
        emit errorMessageChanged(tr("Chosen CDF threshold must be lower than the target probability."));
    }
  }

}
