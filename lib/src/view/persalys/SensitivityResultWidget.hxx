//                                               -*- C++ -*-
/**
 *  @brief QWidget for sensitivity analysis result
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
#ifndef PERSALYS_SENSITIVITYRESULTWIDGET_HXX
#define PERSALYS_SENSITIVITYRESULTWIDGET_HXX

#include "persalys/DesignOfExperiment.hxx"

#include "persalys/CustomStandardItemModel.hxx"
#include "persalys/SensitivityIndicesPlot.hxx"

#include <QSortFilterProxyModel>

namespace PERSALYS
{

// custom QSortFilterProxyModel
class PERSALYS_VIEW_API IndicesProxyModel : public QSortFilterProxyModel
{
public:
  explicit IndicesProxyModel(QObject *parent = nullptr)
    : QSortFilterProxyModel(parent)
  {
  }

  bool lessThan(const QModelIndex& left, const QModelIndex& right) const override
  {
    if (!sourceModel()->data(left, Qt::UserRole).toBool())
      return false;
    else
      return QSortFilterProxyModel::lessThan(left, right);
  }
};


class PERSALYS_VIEW_API SensitivityResultWidget : public QWidget
{
  Q_OBJECT

public:
  enum Type {
    Sobol, 
    SRC, 
    GlobalHSICIndices, GlobalHSICR2Indices, GlobalHSICPValuesPermutation, GlobalHSICPValuesAsymptotic, 
    TargetHSICIndices, TargetHSICR2Indices, TargetHSICPValuesPermutation, TargetHSICPValuesAsymptotic, 
    ConditionalHSICIndices, ConditionalHSICR2Indices, ConditionalHSICPValuesPermutation, ConditionalHSICPValuesAsymptotic
  };

  enum ColumnRole {
    InputColumn = 0,
    FirstOrderIndexColumn,
    FirstOrderIntervalColumn,
    TotalIndexColumn,
    TotalIntervalColumn
  };

  SensitivityResultWidget(const OT::Point& firstIndices,
                          const OT::Interval& firstIndicesIntervals,
                          const OT::Point& totalIndices,
                          const OT::Interval& totalIndicesIntervals,
                          const OT::Description& inputNames,
                          const OT::String& outputName,
                          const Type type,
                          const DesignOfExperiment::Type doeType = DesignOfExperiment::Type::MC,
                          QWidget *parent = nullptr);

public slots:
  void updateIndicesPlot(int, Qt::SortOrder);

private:
  SensitivityIndicesPlot * plot_  = nullptr;
  IndicesProxyModel * proxyModel_ = nullptr;
};
} // namespace PERSALYS

#if QT_VERSION < 0x060000
Q_DECLARE_METATYPE(PERSALYS::SensitivityResultWidget::ColumnRole)
#endif

#endif
