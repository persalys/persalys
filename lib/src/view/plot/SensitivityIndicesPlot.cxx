//                                               -*- C++ -*-
/**
 *  @brief QwtPlot for sensitivity indices
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
#include "persalys/SensitivityIndicesPlot.hxx"

#include "persalys/QtTools.hxx"

#include <qwt_legend.h>
#include <qwt_plot_marker.h>

using namespace OT;

namespace PERSALYS
{

SensitivityIndicesPlot::SensitivityIndicesPlot(const QString &plotTypeName,
                                               const Point &firstOrderIndices,
                                               const Point &totalIndices,
                                               const Interval &firstOrderIndicesIntervals,
                                               const Interval &totalIndicesIntervals,
                                               const Description &inputNames,
                                               const QStringList &legendNames,
                                               QWidget *parent)
  : PlotWidget(plotTypeName, true, parent)
  , legendNames_(legendNames)
  , yMin_(0.)
  , yMax_(0.)
{
  Q_ASSERT(legendNames.size() == 2);

  // set axis titles
  setAxisTitle(QwtPlot::xBottom, tr("Inputs"));
  setAxisTitle(QwtPlot::yLeft, tr("Index"));

  // insert legend
  if (totalIndices.getSize())
    insertLegend(new QwtLegend(), QwtPlot::BottomLegend);

  // plot indices
  updatePlot(firstOrderIndices, totalIndices, firstOrderIndicesIntervals, totalIndicesIntervals, inputNames);

  // update x axis
  setAxisScale(QwtPlot::xBottom, -0.5, firstOrderIndices.getSize() - 0.5, 1.0);
  setAxisMaxMinor(QwtPlot::xBottom, 0);

  // rescale to avoid to cut points
  yMin_ = yMin_ - (std::abs(0.05 * yMin_) < 0.01 ? 0.05 : std::abs(0.05 * yMin_));
  yMax_ = yMax_ + std::abs(0.05 * yMax_);
  setAxisScale(QwtPlot::yLeft, yMin_, yMax_);

  // add horizontal line y = 0
  QwtPlotMarker * hMarker = new QwtPlotMarker;
  hMarker->setLineStyle(QwtPlotMarker::HLine);
  hMarker->setLinePen(QPen(Qt::darkGray, 1));
  hMarker->attach(this);
}


void SensitivityIndicesPlot::updatePlot(const Point &firstOrderIndices,
                                        const Point &totalIndices,
                                        const Interval &firstOrderIndicesIntervals,
                                        const Interval &totalIndicesIntervals,
                                        const Description &inputNames)
{
  detachItems(QwtPlotItem::Rtti_PlotCurve);

  // x offset
  double xOffset = 0.;
  if (totalIndices.getSize())
    xOffset = 0.1;

  const char *colors[] = {"DarkOrchid", "SteelBlue"};

  // first order indices
  QwtSymbol * symbol = new QwtSymbol(QwtSymbol::Ellipse, QBrush(colors[0]), QPen(colors[0]), QSize(5, 5));
  plotIndices(firstOrderIndices, firstOrderIndicesIntervals, -xOffset, symbol, legendNames_[0]);

  // total order indices
  if (totalIndices.getSize())
  {
    QwtSymbol * symbol2 = new QwtSymbol(QwtSymbol::Rect, QBrush(colors[1]), QPen(colors[1]), QSize(5, 5));
    plotIndices(totalIndices, totalIndicesIntervals, xOffset, symbol2, legendNames_[1]);
  }
  // update x axis
  setAxisScaleDraw(QwtPlot::xBottom, new CustomHorizontalScaleDraw(QtOT::DescriptionToStringList(inputNames)));

  replot();
}


void SensitivityIndicesPlot::plotIndices(const Point &indices, const Interval &interval, const double xOffSet, QwtSymbol *symbol, const QString &legend)
{
  const UnsignedInteger size = indices.getSize();

  double *xData = new double[size];
  double *yData = new double[size];

  for (UnsignedInteger i = 0; i < size; ++i)
  {
    xData[i] = (i + xOffSet) ;
    yData[i] = indices[i];
    // - update y range
    yMin_ = std::min(yMin_, indices[i]);
    yMax_ = std::max(yMax_, indices[i]);

    // plot confidence interval
    if (interval.getDimension() == size)
    {
      double xInterval[2] = {(i + xOffSet), (i + xOffSet)};
      double yInterval[2] = {interval.getLowerBound()[i], interval.getUpperBound()[i]};
      plotCurve(xInterval, yInterval, 2, symbol->pen().color());

      // - update y range
      yMin_ = std::min(yMin_, interval.getLowerBound()[i]);
      yMax_ = std::max(yMax_, interval.getUpperBound()[i]);
    }
  }
  plotCurve(xData, yData, size, QPen(Qt::black), QwtPlotCurve::NoCurve, symbol, legend);

  delete[] xData;
  delete[] yData;
}
}
