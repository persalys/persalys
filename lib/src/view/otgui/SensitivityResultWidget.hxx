//                                               -*- C++ -*-
/**
 *  @brief QWidget for sensitivity analysis result
 *
 *  Copyright 2015-2018 EDF-Phimeca
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
#ifndef OTGUI_SENSITIVITYRESULTWIDGET_HXX
#define OTGUI_SENSITIVITYRESULTWIDGET_HXX

#include "otgui/CustomStandardItemModel.hxx"
#include "otgui/PlotWidget.hxx"

#include <openturns/Point.hxx>

#include <QSortFilterProxyModel>

namespace OTGUI
{

// custom QSortFilterProxyModel
class OTGUI_API IndicesProxyModel : public QSortFilterProxyModel
{
public:
  IndicesProxyModel(QObject * parent = 0)
    : QSortFilterProxyModel(parent)
  {
  }

  bool lessThan(const QModelIndex& left, const QModelIndex& right) const
  {
    if (!sourceModel()->data(left, Qt::UserRole).toBool())
      return false;
    else
      return QSortFilterProxyModel::lessThan(left, right);
  }
};


class OTGUI_API SensitivityResultWidget : public QWidget
{
  Q_OBJECT

public:
  enum Type {Sobol, SRC};

  SensitivityResultWidget(const OT::Point& firstIndices,
                          const OT::Interval& firstIndicesIntervals,
                          const OT::Point& totalIndices,
                          const OT::Interval& totalIndicesIntervals,
                          const OT::Description& inputNames,
                          const OT::String& outputName,
                          const Type type,
                          QWidget * parent = 0);

public slots:
  void updateIndicesPlot(int, Qt::SortOrder);

private:
  PlotWidget * plot_;
  IndicesProxyModel * proxyModel_;
};
}
#endif
