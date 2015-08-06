#include "PlotWidget.hxx"
#include "DistributionScaleEngine.hxx"

namespace OTGUI {

const QColor PlotWidget::DefaultCurveColor = QColor(240, 140, 0);//dark orange

PlotWidget::PlotWidget(QWidget * parent)
: QwtPlot( parent )
{
  setCanvasBackground( Qt::white );
  setMinimumSize( 200, 150 );
  clear();
}


void PlotWidget::clear()
{
  detachItems();
  setAxisAutoScale(QwtPlot::xBottom);
  enableAxis(QwtPlot::xBottom);  

  // initialize grid
  grid_ = new QwtPlotGrid;

  replot();
}


void PlotWidget::plotCurve(double * x, double * y, int size, const QColor & color, int width, QwtPlotCurve::CurveStyle style)
{
  QwtPlotCurve * curve = new QwtPlotCurve;
  QPen pen;
  pen.setWidth(width);
  pen.setColor(color);
  curve->setPen(pen);
  curve->setSamples(x, y, size);
  curve->setStyle(style);
  curve->attach(this);
  replot();
}


void PlotWidget::plotCurve(const OT::NumericalSample & data, const QColor& color, int width)
{
  const int size = data.getSize();
  double * x = new double[size];
  double * y = new double[size];

  for (int i = 0; i < size; ++ i)
  {
    x[i] = data[i][0];
    y[i] = data[i][1];
  }
  plotCurve(x, y, size, color, width);
  delete[] x;
  delete[] y;
}


void PlotWidget::plotScatter(const OT::NumericalSample & input, const OT::NumericalSample & output)
{
  int size = input.getSize();

  if (output.getSize()!=size)
    return;
  
  double *xData = new double[size];
  double *yData = new double[size];

  for (int i = 0 ; i < size ; ++i)
  {
    xData[i] = input[i][0];
    yData[i] = output[i][0];
    qDebug() << "x= " << xData[i] << " , y= " << yData[i];
  }

  plotCurve(xData, yData, size, Qt::GlobalColor(16), 4, QwtPlotCurve::Dots);
}


void PlotWidget::plotPDFCurve(const OT::Distribution & distribution, const QColor& color, int width)
{
  double mean = distribution.getMean()[0];
  double stepSize = 0.0;
  const double qmin = OT::ResourceMap::GetAsNumericalScalar("DistributionImplementation-QMin");
  const double qmax = OT::ResourceMap::GetAsNumericalScalar("DistributionImplementation-QMax");
  double x1 = distribution.computeQuantile(qmin)[0];
  double x2 = distribution.computeQuantile(qmax)[0];
  const double delta = 2.0 *(x2 - x1) *(1.0 - 0.5 *(qmax - qmin));
  x1 -= delta;
  x2 -= delta;

  DistributionScaleEngine xScaleEngine;
  xScaleEngine.setReference(mean);
  xScaleEngine.setAttribute(QwtScaleEngine::Symmetric, true);
  xScaleEngine.autoScale(3, x1, x2, stepSize);
  setAxisScale(QwtPlot::xBottom, x1, x2, stepSize);

  plotCurve(distribution.drawPDF().getDrawable(0).getData(), color, width);
}

}
