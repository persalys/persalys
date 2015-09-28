// PlotWidget.hxx


#ifndef PLOTWIDGET_HXX
#define PLOTWIDGET_HXX

#include <qwt_plot_curve.h>
#include <qwt_plot.h>
#include <qwt_plot_grid.h>
#include <qwt_plot_zoomer.h>
#include <QLabel>

#include "NumericalSample.hxx"
#include "Distribution.hxx"


namespace OTGUI {

class ImageEditionDialog;

class PlotWidget : public QwtPlot
{
  Q_OBJECT

public:

  static const int DefaultCurveWidth = 2;
  static const QColor DefaultCurveColor;

  /// constructor
  PlotWidget(QWidget * parent = 0);
//   PlotWidget(const PlotWidget & other);
//   virtual PlotWidget * clone() const;

  /// plot a curve
  void plotCurve(double * x, double * y, int size, const QColor & color = DefaultCurveColor, int width = DefaultCurveWidth, QwtPlotCurve::CurveStyle style=QwtPlotCurve::Lines);
  void plotCurve(const OT::NumericalSample & data, const QColor & color = DefaultCurveColor, int width = DefaultCurveWidth);

  void plotPDFCurve(const OT::Distribution & distribution, const QColor & color = DefaultCurveColor, int width = DefaultCurveWidth);
  void plotScatter(const OT::NumericalSample & input, const OT::NumericalSample & output);

  /// clear plot
  void clear();
  void replot();
  void updatePlotLabel();
  QLabel * getPlotLabel() const;

public slots:
  void contextMenu(const QPoint & pos);
  void editImage();

protected:
  QLabel * plotLabel_;
  ImageEditionDialog * dialog_;
  /// the grid
  QwtPlotGrid * grid_;
  QwtPlotZoomer * zoomer_;
  QAction * copyImageAction_;
};
}
#endif
