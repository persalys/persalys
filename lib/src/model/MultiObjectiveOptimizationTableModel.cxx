//                                               -*- C++ -*-
/**
 *  @brief QAbstractTableModel to list the inputs for multi obj optm
 *
 *  Copyright 2015-2022 EDF-Phimeca
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
#include "persalys/MultiObjectiveOptimizationTableModel.hxx"

#include "persalys/StudyTreeViewModel.hxx"
#include <QHeaderView>

using namespace OT;

namespace PERSALYS
{

  MultiObjectiveOptimizationTableModel::MultiObjectiveOptimizationTableModel(const MultiObjectiveOptimizationAnalysis & analysis, QObject * parent)
    : OptimizationTableModel(parent)
    , analysis_(analysis)
  {
    analysis_.updateParameters();
    types_.clear();
    const UnsignedInteger nbInputs = analysis_.getPhysicalModel().getInputs().getSize();
    for (UnsignedInteger i = 0; i < nbInputs; ++i)
      switch (analysis_.getVariablesType()[i])
      {
      case OptimizationProblemImplementation::CONTINUOUS:
        types_ << tr("Continuous");
        break;
      case OptimizationProblemImplementation::INTEGER:
        types_ << tr("Integer");
        break;
      case OptimizationProblemImplementation::BINARY:
        types_ << tr("Binary");
        break;
      default:
        throw InvalidArgumentException(HERE) << "Unknown variable type";
      }
  }


  int MultiObjectiveOptimizationTableModel::rowCount(const QModelIndex & /*parent*/) const
  {
    return analysis_.getPhysicalModel().getInputs().getSize() + 1;
  }


  Qt::ItemFlags MultiObjectiveOptimizationTableModel::flags(const QModelIndex & index) const
  {
    Qt::ItemFlags result = QAbstractTableModel::flags(index);

    if (index.column() == 0)
    {
      result |= Qt::ItemIsUserCheckable;
      result &= ~Qt::ItemIsEditable;
      result |= Qt::ItemIsSelectable;
    }

    // header
    if (index.row() == 0)
    {
      return result &= ~Qt::ItemIsEditable;
    }
    // not header
    else
    {
      const int inputIndex = index.row() - 1;
      const String currentInputName = analysis_.getPhysicalModel().getInputNames()[inputIndex];
      if (index.column() == 2) {
        if (analysis_.getVariableInputs().contains(currentInputName))
          result |= Qt::ItemIsEditable | Qt::ItemIsEnabled;
        else
          result &= ~Qt::ItemIsEnabled;
      }
      else if (index.column() == 3)
      {
        if (analysis_.getVariableInputs().contains(currentInputName))
          result &= ~Qt::ItemIsEnabled;
        else
          result |= Qt::ItemIsEditable | Qt::ItemIsEnabled;
      }
      else if (index.column() == 4 || index.column() == 5)
      {
        if (analysis_.getVariableInputs().contains(analysis_.getPhysicalModel().getInputNames()[index.row() - 1]))
          result |= Qt::ItemIsEditable | Qt::ItemIsEnabled;
        else
          result &= ~Qt::ItemIsEnabled;
      }
    }
    return result;
  }


  QVariant MultiObjectiveOptimizationTableModel::data(const QModelIndex & ind, int role) const
  {
    // header
    if (ind.row() == 0)
    {
      if (role == Qt::DisplayRole || role == Qt::EditRole)
      {
        switch (ind.column())
        {
        case 0:
          return tr("Name");
        case 1:
          return tr("Description");
        case 2:
          return tr("Type");
        case 3:
          return tr("Fixed\nvalue");
        case 4:
          return tr("Lower\nbound");
        case 5:
          return tr("Upper\nbound");
        default:
          return QVariant();
        }
      }
      else if (role == Qt::CheckStateRole && ind.column() == 0)
      {
        const UnsignedInteger nbInputs = analysis_.getPhysicalModel().getInputNames().getSize();
        return analysis_.getVariableInputs().getSize() == nbInputs ? Qt::Checked : Qt::Unchecked;
      }
      else if (role == Qt::BackgroundRole)
      {
        return QBrush("#f2f1f0");
      }
      else if (role == Qt::TextAlignmentRole)
      {
        return Qt::AlignCenter;
      }
      return QVariant();
    }

    // not header

    if (role == Qt::DisplayRole || role == Qt::EditRole)
    {
      const int inputIndex = ind.row() - 1;
      const String currentInputName = analysis_.getPhysicalModel().getInputNames()[inputIndex];

      switch (ind.column())
      {
      case 0:
        return QString::fromUtf8(analysis_.getPhysicalModel().getInputs()[inputIndex].getName().c_str());
      case 1:
        return QString::fromUtf8(analysis_.getPhysicalModel().getInputs()[inputIndex].getDescription().c_str());
      case 2:
        switch(analysis_.getVariablesType()[inputIndex])
        {
        case OptimizationProblemImplementation::CONTINUOUS:
          return tr("Continuous");
        case OptimizationProblemImplementation::INTEGER:
          return tr("Integer");
        case OptimizationProblemImplementation::BINARY:
          return tr("Binary");
        default:
          throw InvalidArgumentException(HERE) << "Unknown variable type";
        }
      case 3:
        return QString::number(analysis_.getStartingPoint()[inputIndex], 'g', StudyTreeViewModel::DefaultSignificantDigits);
      case 4:
        return QString::number(analysis_.getBounds().getLowerBound()[inputIndex], 'g', StudyTreeViewModel::DefaultSignificantDigits);
      case 5:
        return QString::number(analysis_.getBounds().getUpperBound()[inputIndex], 'g', StudyTreeViewModel::DefaultSignificantDigits);
      default:
        return QVariant();
      }
    }
    else if (role == Qt::CheckStateRole)
    {
      const int inputIndex = ind.row() - 1;

      switch (ind.column())
      {
      case 0:
        const String currentInputName = analysis_.getPhysicalModel().getInputNames()[inputIndex];
        return analysis_.getVariableInputs().contains(currentInputName) ? Qt::Checked : Qt::Unchecked;
      }
    }
    else if ((role == Qt::ForegroundRole || role == Qt::ToolTipRole) && ind.column() == 0)
    {
      const int inputIndex = ind.row() - 1;
      const String currentInputName = analysis_.getPhysicalModel().getInputNames()[inputIndex];
      // check bounds
      if (analysis_.getVariableInputs().contains(currentInputName))
      {
        const Interval bounds(analysis_.getBounds().getMarginal(inputIndex));
        if (role == Qt::ForegroundRole && (bounds.isEmpty()))
          return QColor(Qt::red);
        if (role == Qt::ToolTipRole && bounds.isEmpty())
          return tr("The lower bound must be less than the upper bound");
      }
    }
    else if (role == Qt::UserRole + 1 && ind.column() == 2)
      return QStringList() << tr("Continuous") << tr("Integer") << tr("Binary");
    else if (role == Qt::BackgroundRole && ind.column() == 2)
      return QHeaderView(Qt::Horizontal).palette().color(QPalette::Window);
    return QVariant();
  }


  bool MultiObjectiveOptimizationTableModel::setData(const QModelIndex & index, const QVariant & value, int role)
  {
    emit errorMessageChanged("");

    // header
    if (index.row() == 0)
    {
      // first column : select variables
      if (role == Qt::CheckStateRole && index.column() == 0)
      {
        for (int i = 1; i < rowCount(); ++i)
          if (data(this->index(i, 0), role).toInt() != (value.toBool() ? Qt::Checked : Qt::Unchecked))
            setData(this->index(i, 0), value.toBool() ? Qt::Checked : Qt::Unchecked, role);

        return true;
      }
      return false;
    }

    // not header

    if (role == Qt::EditRole)
    {
      const int inputIndex = index.row() - 1;
      const QString currentInputName = analysis_.getPhysicalModel().getInputNames()[inputIndex].c_str();

      switch (index.column())
      {
      case 2: // type
      {
        Indices values = analysis_.getVariablesType();
        if (value.toString() == tr("Continuous"))
          values[inputIndex] = OptimizationProblemImplementation::CONTINUOUS;
        else if (value.toString() == tr("Integer"))
          values[inputIndex] = OptimizationProblemImplementation::INTEGER;
        else if (value.toString() == tr("Binary"))
          values[inputIndex] = OptimizationProblemImplementation::BINARY;
        analysis_.setVariablesType(values);
        break;
      }

      case 3: // starting point
      {
        Point values = analysis_.getStartingPoint();
        if (values[inputIndex] == value.toDouble())
          return false;

        values[inputIndex] = value.toDouble();
        analysis_.setStartingPoint(values);

        break;
      }
      case 4: // lower bounds
      {
        Point lowerBounds = analysis_.getBounds().getLowerBound();
        if (lowerBounds[inputIndex] == value.toDouble())
          return false;
        lowerBounds[inputIndex] = value.toDouble();
        Interval newInterval(analysis_.getBounds());
        newInterval.setLowerBound(lowerBounds);

        analysis_.setBounds(newInterval);

        break;
      }
      case 5: // upper bounds
      {
        Point upperBounds = analysis_.getBounds().getUpperBound();
        if (upperBounds[inputIndex] == value.toDouble())
          return false;
        upperBounds[inputIndex] = value.toDouble();
        Interval newInterval(analysis_.getBounds());
        newInterval.setUpperBound(upperBounds);

        analysis_.setBounds(newInterval);

        break;
      }
      default:
        return false;
      }
    }
    else if (role == Qt::CheckStateRole)
    {
      const int inputIndex = index.row() - 1;
      String currentInputName = analysis_.getPhysicalModel().getInputNames()[inputIndex];

      switch (index.column())
      {
      case 0: // input of interest
      {
        Description variableInputs = analysis_.getVariableInputs();
        if (value.toBool() && !variableInputs.contains(currentInputName))
          variableInputs.add(currentInputName);
        else if (!value.toBool() && variableInputs.contains(currentInputName))
          variableInputs.erase(std::remove(variableInputs.begin(), variableInputs.end(), currentInputName), variableInputs.end());
        else
          return false;

        analysis_.setVariableInputs(variableInputs);
        emit variablesChanged();
        break;
      }
      default:
        return false;
      }
      emit dataChanged(this->index(index.row(), 0), this->index(index.row(), 4));
    }
    return true;
  }


  MultiObjectiveOptimizationAnalysis MultiObjectiveOptimizationTableModel::getAnalysis() const
  {
    return analysis_;
  }
}
