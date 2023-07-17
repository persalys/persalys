//                                               -*- C++ -*-
/**
 *  @brief QwtPlot for all the graphics of the interface
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
#ifndef PERSALYS_PLOTWIDGET_HXX
#define PERSALYS_PLOTWIDGET_HXX

#include "persalys/PersalysPrivate.hxx"

#include <qwt_plot_curve.h>
#include <qwt_plot.h>
#include <qwt_symbol.h>
#include <qwt_scale_draw.h>
#include <qwt_scale_engine.h>
#include <qwt_scale_div.h>
#include <qwt_text.h>

#include <openturns/Distribution.hxx>

namespace PERSALYS
{

// -- custom class CustomHorizontalScaleDraw --
class CustomHorizontalScaleDraw: public QwtScaleDraw
{
public:
  CustomHorizontalScaleDraw(const QStringList& labels)
    : labels_(labels)
  {
  }

  virtual QwtText label(double value) const
  {
    const int index = qRound(value);
    if (index >= 0 && index < labels_.size())
      return QwtText(labels_[index]);
    return QwtText();
  }

private:
  const QStringList labels_;
};


class PERSALYS_PLOT_API PlotWidget : public QwtPlot
{
  Q_OBJECT

public:
  enum HistoType {PDF, CDF, Survival, Other};

  /// constructor
  PlotWidget(const QString &plotTypeName = "", const bool disableZoom = false, QWidget *parent = nullptr);

  /// plot a curve
  void plotCurve(QVector<QPointF> points, const QPen pen = QPen(Qt::black, 2),
                 QwtPlotCurve::CurveStyle style = QwtPlotCurve::Lines, QwtSymbol* symbol = 0, QString title = "", bool isStatic = false);
  void plotCurve(double * x, double * y, int size, const QPen pen = QPen(Qt::black, 2),
                 QwtPlotCurve::CurveStyle style = QwtPlotCurve::Lines, QwtSymbol* symbol = 0, QString title = "");
  void plotCurve(const OT::Sample & data, const QPen pen = QPen(Qt::black, 2),
                 QwtPlotCurve::CurveStyle style = QwtPlotCurve::Lines, QwtSymbol* symbol = 0, QString title = "");

  void plotPDFCurve(const OT::Distribution & distribution, const QPen pen = QPen(Qt::black, 2));
  void plotCDFCurve(const OT::Distribution & distribution, const QPen pen = QPen(Qt::black, 2));
  void plotQuantileCurve(const OT::Distribution & distribution, const QPen pen = QPen(Qt::black, 2));
  void plotSurvivalCurve(const OT::Distribution & distribution, const QPen pen = QPen(Qt::black, 2));
  void plotHistogram(const OT::Sample & sample, const PlotWidget::HistoType graphType = PlotWidget::PDF, int barNumber = 0, QString title = "");
  void plotScatter(const OT::Sample & input, const OT::Sample & output,
                   QPen pen = QPen(Qt::blue, 4), QString Xtitle = "", QString Ytitle = "");

  void plotFronts(const OT::Collection<OT::Sample> & fronts,
                  const OT::UnsignedInteger idx1, const OT::UnsignedInteger idx2);

  void plotContour(const OT::Collection<OT::Drawable>& drawables,
                   const OT::UnsignedInteger drawableIndex,
                   const bool displayGradient,
                   const bool isPDF);

  /// clear plot
  void clear();

  static QVector<PlotWidget*> GetListScatterPlots(const OT::Sample& inS,
      const OT::Sample& notValidInS,
      const OT::Sample& outS,
      const QStringList inNames,
      const QStringList inAxisNames,
      const QStringList outNames,
      const QStringList outAxisNames);

  void setEnableContourColorBar(const bool enable);

public slots:
  void contextMenu(const QPoint & pos);
  virtual void replot();
  void exportPlot();
  void setXLabelOrientation(int);
  void resetAxisRanges();
  void showCurve(const QVariant &itemInfo, bool on, int /*index*/);


signals:
  void plotChanged();

private:
  void updateScaleParameters(const OT::Distribution & distribution);

private:
// TODO  QwtPlotGrid * grid_;
  QString plotTypeName_;
  bool enableContourColorBar_ = true;
};
}
#endif
