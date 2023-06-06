//                                               -*- C++ -*-
/**
 *  @brief QAbstractTableModel to list inputs for multi obj optm
 *
 *  Copyright 2015-2023 EDF-Phimeca
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
#ifndef PERSALYS_MULTIOBJECTIVEOPTIMIZATIONTABLEMODEL_HXX
#define PERSALYS_MULTIOBJECTIVEOPTIMIZATIONTABLEMODEL_HXX

#include "persalys/MultiObjectiveOptimizationAnalysis.hxx"
#include "persalys/OptimizationTableModel.hxx"

namespace PERSALYS
{
class PERSALYS_MODEL_API MultiObjectiveOptimizationTableModel : public OptimizationTableModel
{
  Q_OBJECT

public:
  MultiObjectiveOptimizationTableModel(const MultiObjectiveOptimizationAnalysis & analysis, QObject *parent = nullptr);

  int rowCount(const QModelIndex & parent = QModelIndex()) const override;
  QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const override;
  bool setData(const QModelIndex & index, const QVariant & value, int role) override;
  Qt::ItemFlags flags(const QModelIndex & index) const override;
  MultiObjectiveOptimizationAnalysis getAnalysis() const;

private:
  MultiObjectiveOptimizationAnalysis analysis_;
  bool firstColumnChecked_;
};
}
#endif
