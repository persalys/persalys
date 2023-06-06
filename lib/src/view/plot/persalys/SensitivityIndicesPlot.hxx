//                                               -*- C++ -*-
/**
 *  @brief QwtPlot for sensitivity indices
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
#ifndef PERSALYS_SENSITIVITYINDICESPLOT_HXX
#define PERSALYS_SENSITIVITYINDICESPLOT_HXX

#include "persalys/PlotWidget.hxx"

namespace PERSALYS
{

class PERSALYS_PLOT_API SensitivityIndicesPlot : public PlotWidget
{
  Q_OBJECT

public:
  SensitivityIndicesPlot(const QString &plotTypeName,
                         const OT::Point &firstOrderIndices,
                         const OT::Point &totalIndices,
                         const OT::Interval &firstOrderIndicesIntervals,
                         const OT::Interval &totalIndicesIntervals,
                         const OT::Description &inputNames,
                         const QStringList &legendNames,
                         QWidget *parent = nullptr);

  void updatePlot(const OT::Point &firstOrderIndices,
                  const OT::Point &totalIndices,
                  const OT::Interval &firstOrderIndicesIntervals,
                  const OT::Interval &totalIndicesIntervals,
                  const OT::Description &inputNames);

private:
  void plotIndices(const OT::Point &indices, const OT::Interval &interval, const double xOffSet, QwtSymbol *symbol, const QString &legend);

private:
  QStringList legendNames_;
  double yMin_;
  double yMax_;
};
}
#endif
