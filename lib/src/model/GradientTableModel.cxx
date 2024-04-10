//                                               -*- C++ -*-
/**
 *  @brief QAbstractTableModel to display gradient values
 *
 *  Copyright 2015-2024 EDF-Phimeca
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
#include "persalys/GradientTableModel.hxx"

#include "persalys/ModelEvaluation.hxx"
#include "persalys/StudyTreeViewModel.hxx"

using namespace OT;

namespace PERSALYS
{

GradientTableModel::GradientTableModel(const PhysicalModel& physicalModel, QObject* parent)
  : QAbstractTableModel(parent)
  , physicalModel_(physicalModel)
  , gradient_(Matrix())
{
}


int GradientTableModel::columnCount(const QModelIndex& /*parent*/) const
{
  return physicalModel_.getSelectedOutputsNames().getSize();
}


int GradientTableModel::rowCount(const QModelIndex& /*parent*/) const
{
  return physicalModel_.getInputDimension();
}


Qt::ItemFlags GradientTableModel::flags(const QModelIndex & index) const
{
  return QAbstractTableModel::flags(index);
}


QVariant GradientTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
  if (role == Qt::DisplayRole && orientation == Qt::Horizontal)
    return QString::fromUtf8(physicalModel_.getSelectedOutputsNames()[section].c_str());
  else if (role == Qt::DisplayRole && orientation == Qt::Vertical)
    return QString::fromUtf8(physicalModel_.getInputs()[section].getName().c_str());

  return QAbstractTableModel::headerData(section, orientation, role);
}


QVariant GradientTableModel::data(const QModelIndex& index, int role) const
{
  if (!index.isValid())
    return QVariant();

  if ((int)gradient_.getNbRows() != rowCount() ||
      (int)gradient_.getNbColumns() != columnCount()) {
    if (role == Qt::DisplayRole)
      return QString("?");
    else
      return QVariant();
  }

  if (role == Qt::DisplayRole)
  {
    return QString::number(gradient_(index.row(), index.column()), 'g', StudyTreeViewModel::DefaultSignificantDigits);
  }
  if (gradient_(index.row(), index.column()) == 0) {
    if (role == Qt::DecorationRole)
      return QIcon(":/images/task-attention.png");
    if (role == Qt::ToolTipRole)
      return QString(tr("Information: Gradient is null"));
  }
  return QVariant();
}


bool GradientTableModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
  if (!index.isValid())
    return false;
  return QAbstractTableModel::setData(index, value, role);
}


void GradientTableModel::updateData()
{
  beginResetModel();
  gradient_ = Matrix();
  endResetModel();
}

void GradientTableModel::evaluateGradient()
{
  errorMessage_.clear();
  // if no outputs do nothing
  if (!physicalModel_.getSelectedOutputsNames().getSize())
    return;

  ModelEvaluation eval("anEval", physicalModel_);
  try
  {
    eval.run();
  }
  catch (const std::exception &)
  {
    // do nothing
  }

  // get result
  Sample outputSample(eval.getResult().getDesignOfExperiment().getOutputSample());

  if (!eval.getErrorMessage().empty())
  {
    emit errorMessageChanged(QString(eval.getErrorMessage().c_str()));
    return;
  }

  try
  {
    Function func = physicalModel_.getFunction(physicalModel_.getSelectedOutputsNames());
    gradient_ = func.gradient(eval.getOriginalInputSample()[0]);
    emit dataChanged(this->index(0,0), this->index(rowCount(), columnCount()));
  }
  catch (const std::exception & ex)
  {
    emit errorMessageChanged(tr("Not possible to evaluate the gradient: %1").arg(ex.what()));
    gradient_ = Matrix();
    return;
  }
}

}
