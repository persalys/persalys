//                                               -*- C++ -*-
/**
 *  @brief QwtPlot for box plot
 *
 *  Copyright 2015-2020 EDF-Phimeca
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
#include "persalys/BoxPlot.hxx"

using namespace OT;

namespace PERSALYS
{

// -- custom QwtPlotCurve with Rtti_PlotUserItem = 1000 + index_
class PlotCurveItem : public QwtPlotCurve
{
public:
  PlotCurveItem(int index)
    : QwtPlotCurve()
    , index_(index)
  {
  };

  virtual int rtti() const
  {
    return QwtPlotItem::Rtti_PlotUserItem + index_;
  };

private:
  int index_;
};


// Custom ScaleEngine For Boxplot
class ScaleEngine: public QwtLinearScaleEngine
{
public:
  ScaleEngine(const int nbVariables)
    : QwtLinearScaleEngine()
    , varIndices_()
  {
    for (int i = 0; i < nbVariables; ++i)
      varIndices_ << i;
  }

  ScaleEngine(const QList<int> &varIndices)
    : QwtLinearScaleEngine()
    , varIndices_(varIndices)
  {
  }

  virtual QwtScaleDiv divideScale(double /*x1*/, double /*x2*/, int /*numMajorSteps*/, int /*numMinorSteps*/, double /*stepSize*/) const
  {
    QList<double> ticks[QwtScaleDiv::NTickTypes];
    for (int i = 0; i < varIndices_.size(); ++i)
      ticks[QwtScaleDiv::MajorTick] << varIndices_[i];

    QwtScaleDiv scDiv(ticks[QwtScaleDiv::MajorTick].first() - 0.2, ticks[QwtScaleDiv::MajorTick].last() + 0.2, ticks);
    return scDiv;
  }

private:
  QList<int> varIndices_;
};


// BoxPlot --------------------------------------------------------

BoxPlot::BoxPlot(const QStringList &variableNames, QWidget *parent)
  : PlotWidget(tr("boxplot"), false, parent)
  , variableNames_(variableNames)
  , boxesCounter_(0)
{
  setTitle(tr("Box plots"));
  setAxisScaleDraw(QwtPlot::xBottom, new CustomHorizontalScaleDraw(variableNames));
  setAxisScaleEngine(QwtPlot::xBottom, new ScaleEngine(variableNames.size()));
}


void BoxPlot::addBoxPlot(const DataAnalysisResult &result, const UnsignedInteger resuIndex)
{
  const double mean = result.getMean()[resuIndex][0];
  const double indexStd = result.getStandardDeviation()[resuIndex][0];
  const double indexMedian = result.getMedian()[resuIndex][0];
  const double Q1 = result.getFirstQuartile()[resuIndex][0];
  const double Q3 = result.getThirdQuartile()[resuIndex][0];
  const Point indexOutlier(result.getOutliers()[resuIndex]);

  const int index = boxesCounter_;
  const double x = boxesCounter_;
  const double std = indexStd > 0 ? indexStd : 1;

  const double median = (indexMedian - mean) / std;
  const double lowerQuartile = (Q1 - mean) / std;
  const double upperQuartile = (Q3 - mean) / std;
  const double lowerBound = ((Q1 - 1.5 * (Q3 - Q1)) - mean) / std;
  const double upperBound = ((Q3 + 1.5 * (Q3 - Q1)) - mean) / std;
  const Point outliers((indexOutlier - Point(indexOutlier.getSize(), mean)) / std);

  // draw median
  double xMedian[2] = {x - 0.1, x + 0.1};
  double yMedian[2] = {median, median};
  plotCurve(index, xMedian, yMedian, 2, QPen(Qt::red));

  // draw box
  double yUpperQuartile[2] = {upperQuartile, upperQuartile};
  plotCurve(index, xMedian, yUpperQuartile, 2, QPen(Qt::blue));
  double yLowerQuartile[2] = {lowerQuartile, lowerQuartile};
  plotCurve(index, xMedian, yLowerQuartile, 2, QPen(Qt::blue));
  double xLeftSide[2] = {x - 0.1, x - 0.1};
  double yBoxSides[2] = {lowerQuartile, upperQuartile};
  plotCurve(index, xLeftSide, yBoxSides, 2, QPen(Qt::blue));
  double xRightSide[2] = {x + 0.1, x + 0.1};
  plotCurve(index, xRightSide, yBoxSides, 2, QPen(Qt::blue));

  // draw whiskers
  double xWhiskers[2] = {x, x};
  double yLower[2] = {lowerBound, lowerQuartile};
  plotCurve(index, xWhiskers, yLower, 2, QPen(Qt::black, 2, Qt::DashLine));

  double yUpper[2] = {upperQuartile, upperBound};
  plotCurve(index, xWhiskers, yUpper, 2, QPen(Qt::black, 2, Qt::DashLine));


  double xWhiskersBars[2] = {x - 0.05, x + 0.05};
  double yLowerWhiskersBar[2] = {lowerBound, lowerBound};
  plotCurve(index, xWhiskersBars, yLowerWhiskersBar, 2);

  double yUpperWhiskersBar[2] = {upperBound, upperBound};
  plotCurve(index, xWhiskersBars, yUpperWhiskersBar, 2);

  // draw outliers
  const int dim = outliers.getDimension();
  double * xOutliers = new double[dim];
  double * yOutliers = new double[dim];

  for (int i = 0; i < dim; ++i)
  {
    xOutliers[i] = x;
    yOutliers[i] = outliers[i];
  }

  plotCurve(index, xOutliers, yOutliers, dim, QPen(Qt::blue), QwtPlotCurve::NoCurve, new QwtSymbol(QwtSymbol::Cross, Qt::NoBrush, QPen(Qt::blue), QSize(5, 5)));
  delete[] xOutliers;
  delete[] yOutliers;

  setAxisMaxMinor(QwtPlot::xBottom, 0);
  replot();

  ++boxesCounter_;
}


void BoxPlot::plotCurve(const int index, double *x, double *y, int size, const QPen pen, QwtPlotCurve::CurveStyle style, QwtSymbol *symbol)
{
  PlotCurveItem * curve = new PlotCurveItem(index);
  curve->setSamples(x, y, size);
  curve->setPen(pen);
  curve->setStyle(style);
  if (symbol)
    curve->setSymbol(symbol);
  curve->setItemAttribute(QwtPlotItem::Legend, false);
  curve->attach(this);

  replot();
}


void BoxPlot::updateVariableOrder(const QStringList &variableNames, const QList<int> &indices)
{
  setAxisScaleDraw(QwtPlot::xBottom, indices.size() ? new CustomHorizontalScaleDraw(variableNames) : 0);
  setAxisScaleEngine(QwtPlot::xBottom, indices.size() ? new ScaleEngine(indices) : 0);
}


void BoxPlot::setVariablesToShow(const QStringList &varNames)
{
  QStringList orderedVarList;
  QList<int> indices;
  int newX = -1;

  for (int i = 0; i < variableNames_.size(); ++i)
  {
    if (varNames.contains(variableNames_[i]))
    {
      orderedVarList << variableNames_[i];
      ++newX;
      indices << newX;
    }
    // get current X axis coord
    QwtPlotCurve * curve = dynamic_cast<QwtPlotCurve*>(itemList(QwtPlotItem::Rtti_PlotUserItem + i)[5]);
    const int x = curve->data()->sample(0).x();

    // update all curves of the i_th boxplot
    for (int j = 0; j < itemList(QwtPlotItem::Rtti_PlotUserItem + i).size(); ++j)
    {
      // change visibility
      itemList(QwtPlotItem::Rtti_PlotUserItem + i)[j]->setVisible(varNames.contains(variableNames_[i]));

      // translate curve
      if (varNames.contains(variableNames_[i]) && (x != newX))
      {
        QwtPlotCurve * curve2 = dynamic_cast<QwtPlotCurve*>(itemList(QwtPlotItem::Rtti_PlotUserItem + i)[j]);
        QVector< QPointF > series(curve2->data()->size());
        for (size_t k = 0; k < curve2->data()->size(); ++k)
        {
          QPointF pt = curve2->data()->sample(k);
          pt.setX(pt.x() + abs(x - newX) * (newX > x ? 1 : -1));
          series[k] = pt;
        }
        curve2->setSamples(series);
      }
    }
  }

  updateVariableOrder(orderedVarList, indices);

  replot();
}
}
