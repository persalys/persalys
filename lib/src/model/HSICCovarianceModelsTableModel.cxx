//                                               -*- C++ -*-
/**
 *  @brief QAbstractTableModel to list covariance models for HSCI sensitivity analysis
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

#include "persalys/HSICCovarianceModelsTableModel.hxx"

#include <openturns/MaternModel.hxx>
#include <openturns/SquaredExponential.hxx>
#include <openturns/AbsoluteExponential.hxx>
#include <openturns/GeneralizedExponential.hxx>

using namespace OT;

namespace PERSALYS
{

HSICCovarianceModelsTableModel::HSICCovarianceModelsTableModel(const Description & variableNames, QObject *parent)
  : QAbstractTableModel(parent)
  , variableNames_(variableNames)
{
  for (UnsignedInteger i = 0; i < variableNames_.getSize(); ++i)
  {
    modelIndices_ << CovarianceModelType::SquaredExponential;
    nuIndices_ << NuType::ThreeHalf;
    pValues_ << 1.0;
  }
}

void HSICCovarianceModelsTableModel::setVariablesNames(const Description & variableNames)
{
  beginResetModel();
  variableNames_ = variableNames;
  modelIndices_.clear();
  nuIndices_.clear();
  pValues_.clear();
  for (UnsignedInteger i = 0; i < variableNames_.getSize(); ++i)
  {
    modelIndices_ << CovarianceModelType::SquaredExponential;
    nuIndices_ << NuType::ThreeHalf;
    pValues_ << 1.0;
  }
  endResetModel();
}

int HSICCovarianceModelsTableModel::rowCount(const QModelIndex & /*parent*/) const
{
  return variableNames_.getSize();
}

int HSICCovarianceModelsTableModel::columnCount(const QModelIndex & /*parent*/) const
{
  return 5; // variable name, covariance model, ν parameter, p parameter, apply-to-all button
}

QVariant HSICCovarianceModelsTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
  if (role == Qt::DisplayRole && orientation == Qt::Horizontal)
  {
    switch (section)
    {
    case 0:
      return tr("Variable");
    case 1:
      return tr("Covariance model");
    case 2:
      return QString::fromUtf8("\xce\xbd"); // ν
    case 3:
      return tr("p");
    case 4:
      return QVariant();
    default:
      return QVariant();
    }
  }
  return QAbstractTableModel::headerData(section, orientation, role);
}

Qt::ItemFlags HSICCovarianceModelsTableModel::flags(const QModelIndex & index) const
{
  Qt::ItemFlags result = QAbstractTableModel::flags(index);

  // Column 0 (variable name): not editable
  if (index.column() == 0)
    return result;

  // Column 1 (covariance model): editable
  if (index.column() == 1)
    return result | Qt::ItemIsEditable;

  // Column 2 (ν): editable only for Matérn
  if (index.column() == 2)
  {
    if (modelIndices_[index.row()] == CovarianceModelType::Matern)
      return result | Qt::ItemIsEditable;
    else
      return result & ~Qt::ItemIsEnabled;
  }

  // Column 3 (p): editable only for GeneralizedExponential
  if (index.column() == 3)
  {
    if (modelIndices_[index.row()] == CovarianceModelType::GeneralizedExponential)
      return result | Qt::ItemIsEditable;
    else
      return result & ~Qt::ItemIsEnabled;
  }

  // Column 4 (apply-to-all button): widget only, not editable
  if (index.column() == 4)
    return Qt::ItemIsEnabled;

  return result;
}

QVariant HSICCovarianceModelsTableModel::data(const QModelIndex & index, int role) const
{
  if (!index.isValid())
    return QVariant();

  const int row = index.row();

  if (role == Qt::DisplayRole || role == Qt::EditRole)
  {
    switch (index.column())
    {
    case 0:
      return QString::fromStdString(variableNames_[row]);
    case 1:
    {
      const QStringList names = {tr("Matérn"), tr("Squared exponential"),
                                  tr("Absolute exponential"), tr("Generalized exponential")};
      return names[static_cast<int>(modelIndices_[row])];
    }
    case 2:
    {
      if (modelIndices_[row] == CovarianceModelType::Matern)
      {
        const QStringList nuVals = {"1/2", "3/2", "5/2"};
        return nuVals[static_cast<int>(nuIndices_[row])];
      }
      return "-";
    }
    case 3:
    {
      if (modelIndices_[row] == CovarianceModelType::GeneralizedExponential)
        return pValues_[row];
      return "-";
    }
    case 4:
      return QVariant();
    default:
      return QVariant();
    }
  }

  // Provide combo items for ComboBoxDelegate
  if (role == Qt::UserRole + 1)
  {
    if (index.column() == 1)
      return QStringList() << tr("Matérn") << tr("Squared exponential")
                           << tr("Absolute exponential") << tr("Generalized exponential");
    if (index.column() == 2 && modelIndices_[row] == CovarianceModelType::Matern)
      return QStringList() << "1/2" << "3/2" << "5/2";
  }

  return QVariant();
}

bool HSICCovarianceModelsTableModel::setData(const QModelIndex & index, const QVariant & value, int role)
{
  if (!index.isValid() || role != Qt::EditRole)
    return false;

  const int row = index.row();

  if (index.column() == 1)
  {
    const QStringList names = {tr("Matérn"), tr("Squared exponential"),
                                tr("Absolute exponential"), tr("Generalized exponential")};
    const int idx = names.indexOf(value.toString());
    if (idx >= 0)
      modelIndices_[row] = static_cast<CovarianceModelType>(idx);
    // Refresh ν and p columns when model changes
    emit dataChanged(this->index(row, 2), this->index(row, 3));
    emit dataChanged(index, index);
    return true;
  }

  if (index.column() == 2)
  {
    const QStringList nuVals = {"1/2", "3/2", "5/2"};
    const int idx = nuVals.indexOf(value.toString());
    if (idx >= 0)
      nuIndices_[row] = static_cast<NuType>(idx);
    emit dataChanged(index, index);
    return true;
  }

  if (index.column() == 3)
  {
    bool ok;
    const double p = value.toDouble(&ok);
    if (ok && p > 0.0 && p <= 2.0)
    {
      pValues_[row] = p;
      emit dataChanged(index, index);
      return true;
    }
    return false;
  }

  return false;
}

Collection<CovarianceModel> HSICCovarianceModelsTableModel::getCovarianceModels() const
{
  static const double nuValues[] = {0.5, 1.5, 2.5};
  Collection<CovarianceModel> models;

  for (int i = 0; i < modelIndices_.size(); ++i)
  {
    switch (modelIndices_[i])
    {
    case CovarianceModelType::Matern:
      models.add(MaternModel(Point(1, 1.0), Point(1, 1.0), nuValues[static_cast<int>(nuIndices_[i])]));
      break;
    case CovarianceModelType::SquaredExponential:
      models.add(SquaredExponential());
      break;
    case CovarianceModelType::AbsoluteExponential:
      models.add(AbsoluteExponential());
      break;
    case CovarianceModelType::GeneralizedExponential:
      models.add(GeneralizedExponential(Point(1, 1.0), Point(1, 1.0), pValues_[i]));
      break;
    default:
      models.add(SquaredExponential());
      break;
    }
  }

  return models;
}

void HSICCovarianceModelsTableModel::setCovarianceModels(const Collection<CovarianceModel> & models)
{
  if (static_cast<UnsignedInteger>(modelIndices_.size()) != models.getSize())
    return;

  beginResetModel();
  for (int i = 0; i < (int) models.getSize(); ++i)
  {
    const String className = models[i].getImplementation()->getClassName();
    if (className == "MaternModel")
    {
      modelIndices_[i] = CovarianceModelType::Matern;
      const double nu = dynamic_cast<const MaternModel *>(models[i].getImplementation().get())->getNu();
      if (nu <= 1.0)
        nuIndices_[i] = NuType::OneHalf;
      else if (nu <= 2.0)
        nuIndices_[i] = NuType::ThreeHalf;
      else
        nuIndices_[i] = NuType::FiveHalf;
    }
    else if (className == "SquaredExponential")
      modelIndices_[i] = CovarianceModelType::SquaredExponential;
    else if (className == "AbsoluteExponential")
      modelIndices_[i] = CovarianceModelType::AbsoluteExponential;
    else if (className == "GeneralizedExponential")
    {
      modelIndices_[i] = CovarianceModelType::GeneralizedExponential;
      pValues_[i] = dynamic_cast<const GeneralizedExponential *>(models[i].getImplementation().get())->getP();
    }
  }
  endResetModel();
}

void HSICCovarianceModelsTableModel::applyToAll(int sourceRow)
{
  if (sourceRow < 0 || sourceRow >= modelIndices_.size())
    return;

  const CovarianceModelType srcModel = modelIndices_[sourceRow];
  const NuType srcNu = nuIndices_[sourceRow];
  const double srcP = pValues_[sourceRow];

  for (int i = 0; i < modelIndices_.size(); ++i)
  {
    modelIndices_[i] = srcModel;
    nuIndices_[i] = srcNu;
    pValues_[i] = srcP;
  }
  emit dataChanged(index(0, 1), index(rowCount() - 1, 3));
}

} // namespace PERSALYS