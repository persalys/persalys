// PlotWidget.hxx


#ifndef PLOTWIDGET_HXX
#define PLOTWIDGET_HXX

#include <qwt_plot_curve.h>
#include <qwt_plot.h>
#include <qwt_plot_grid.h>
#include <qwt_symbol.h>
#include <qwt_scale_draw.h>

#include "Description.hxx"
#include "NumericalPoint.hxx"
#include "Distribution.hxx"

#include <QLabel>

namespace OTGUI {

class ImageEditionDialog;

class PlotWidget : public QwtPlot
{
  Q_OBJECT

public:

  static const QColor DefaultHistogramColor;

  /// constructor
  PlotWidget(bool isBarChart = false, QWidget * parent = 0);

  /// plot a curve
  void plotCurve(double * x, double * y, int size, const QPen pen=QPen(Qt::black, 2),
                 QwtPlotCurve::CurveStyle style=QwtPlotCurve::Lines, QwtSymbol* symbol=0);
  void plotCurve(const OT::NumericalSample & data, const QPen pen=QPen(Qt::black, 2));

  void plotPDFCurve(const OT::Distribution & distribution, const QPen pen=QPen(Qt::black, 2));
  void plotCDFCurve(const OT::Distribution & distribution, const QPen pen=QPen(Qt::black, 2));
  void plotHistogram(const OT::NumericalSample & sample, bool cdf=false, int barNumber=0);
  void plotScatter(const OT::NumericalSample & input, const OT::NumericalSample & output);
  void plotBoxPlot(double median, double lowerQuartile, double upperQuartile,
                   double lowerBound, double upperBound, OT::NumericalPoint outliers_);
  void plotBarChart(const OT::NumericalPoint firstOrder, const OT::NumericalPoint totalOrder,
                    const OT::Description inputNames);

  /// clear plot
  void clear();
  void replot();
  void updatePlotLabel();
  QLabel * getPlotLabel() const;

public slots:
  void contextMenu(const QPoint & pos);
  void editImage();

private:
  void updateScaleParameters(const OT::Distribution & distribution);

private:
  QLabel * plotLabel_;
  ImageEditionDialog * dialog_;
  QwtPlotGrid * grid_;
  QAction * copyImageAction_;
};
}
#endif
