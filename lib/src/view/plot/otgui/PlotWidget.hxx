//                                               -*- C++ -*-
/**
 *  @brief QwtPlot for all the graphics of the interface
 *
 *  Copyright 2015-2019 EDF-Phimeca
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
#ifndef OTGUI_PLOTWIDGET_HXX
#define OTGUI_PLOTWIDGET_HXX

#include <qwt_plot_curve.h>
#include <qwt_plot.h>
#include <qwt_symbol.h>
#include <qwt_plot_marker.h>
#include <qwt_scale_draw.h>
#include <qwt_scale_engine.h>
#include <qwt_scale_div.h>
#include "qwt_global.h"
#include "qwt_interval.h"

#include <openturns/Distribution.hxx>
#include "otgui/OTGuiprivate.hxx"

namespace OTGUI
{

// -- custom class CustomHorizontalScaleDraw --
class CustomHorizontalScaleDraw: public QwtScaleDraw
{
public:
  CustomHorizontalScaleDraw(const OT::Description& labels)
    : labels_(labels)
  {
  }

  virtual QwtText label(double value) const
  {
    const int index = qRound(value);
    if (index >= 0 && index < (int)labels_.getSize())
      return QwtText(QString::fromUtf8(labels_[index].c_str()));
    return QwtText();
  }

private:
  const OT::Description labels_;
};


// Custom ScaleEngine For Boxplot
class CustomScaleEngineForBoxplot: public QwtLinearScaleEngine
{
public:
  CustomScaleEngineForBoxplot(const int nbVariables)
  : QwtLinearScaleEngine()
  , varIndices_()
  {
    for (int i = 0; i < nbVariables; ++i)
      varIndices_ << i;
  }

  CustomScaleEngineForBoxplot(const QList<int> varIndices)
  : QwtLinearScaleEngine()
  , varIndices_(varIndices)
  {
  }

  virtual QwtScaleDiv divideScale(double x1, double x2, int numMajorSteps, int numMinorSteps, double stepSize) const
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


class OTGUI_API PlotWidget : public QwtPlot
{
  Q_OBJECT

public:

  static const QColor DefaultHistogramColor;

  /// constructor
  PlotWidget(const QString &plotTypeName = "", const bool disableZoom = false, QWidget *parent = 0);

  /// plot a curve
  void plotCurve(QVector<QPointF> points, const QPen pen = QPen(Qt::black, 2),
                 QwtPlotCurve::CurveStyle style = QwtPlotCurve::Lines, QwtSymbol* symbol = 0, QString title = "", bool isStatic = false);
  void plotCurve(double * x, double * y, int size, const QPen pen = QPen(Qt::black, 2),
                 QwtPlotCurve::CurveStyle style = QwtPlotCurve::Lines, QwtSymbol* symbol = 0, QString title = "");
  void plotCurve(const int index, double * x, double * y, int size, const QPen pen = QPen(Qt::black, 2),
                 QwtPlotCurve::CurveStyle style = QwtPlotCurve::Lines, QwtSymbol* symbol = 0);
  void plotCurve(const OT::Sample & data, const QPen pen = QPen(Qt::black, 2),
                 QwtPlotCurve::CurveStyle style = QwtPlotCurve::Lines, QwtSymbol* symbol = 0, QString title = "");

  void plotPDFCurve(const OT::Distribution & distribution, const QPen pen = QPen(Qt::black, 2));
  void plotCDFCurve(const OT::Distribution & distribution, const QPen pen = QPen(Qt::black, 2));
  void plotQuantileCurve(const OT::Distribution & distribution, const QPen pen = QPen(Qt::black, 2));
  void plotSurvivalCurve(const OT::Distribution & distribution, const QPen pen = QPen(Qt::black, 2));
  void plotHistogram(const OT::Sample & sample, const OT::UnsignedInteger graphType = 0, int barNumber = 0, QString title = "");
  void plotScatter(const OT::Sample & input, const OT::Sample & output,
                   QPen pen = QPen(Qt::blue, 4), QString Xtitle = "", QString Ytitle = "");
  void plotBoxPlot(const double mean,
                   const double std,
                   const double median,
                   const double lowerQuartile,
                   const double upperQuartile,
                   const double lowerBound,
                   const double upperBound,
                   const OT::Point& outliers,
                   const int index);
  void plotSensitivityIndices(const OT::Point& firstOrderIndices,
                              const OT::Point& totalIndices,
                              const OT::Description& inputNames,
                              const OT::Interval& firstOrderIndicesIntervals = OT::Interval(),
                              const OT::Interval& totalIndicesIntervals = OT::Interval());
  void plotContour(const OT::Distribution& distribution, const bool isPdf = true);
  void plotContour(const OT::Drawable& drawable, const bool displayContour = true);
  void setMorrisPlotType(const QPointF& initialMarkersCoord);

  /// clear plot
  void clear();

  static QVector<PlotWidget*> GetListScatterPlots(const OT::Sample& inS,
      const OT::Sample& notValidInS,
      const OT::Sample& outS,
      const QStringList inNames,
      const QStringList inAxisNames,
      const QStringList outNames,
      const QStringList outAxisNames);

protected:
  virtual bool eventFilter(QObject *obj, QEvent *event);

public slots:
  void contextMenu(const QPoint & pos);
  virtual void replot();
  void exportPlot();
  void selectPoints(const QRectF&);
  void updateVerticalMarkerValue(const QPointF&);

signals:
  void plotChanged();
  void verticalMarkerPositionChanged(double);
  void selectedPointsChanged();
  void selectedPointsChanged(const OT::Indices& ind);

private:
  void updateScaleParameters(const OT::Distribution & distribution);

private:
// TODO  QwtPlotGrid * grid_;
  QString plotTypeName_;
  QwtPlotMarker *  verticalMarker_;
};
}
#endif
