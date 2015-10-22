#include "PlotWidget.hxx"
#include "DistributionScaleEngine.hxx"
#include "ImageEditionDialog.hxx"
#include <qwt_plot_panner.h>
#include <qwt_plot_layout.h>
#include <QMenu>
#include <qwt_plot_renderer.h>
#include <qwt_plot_magnifier.h>
#include <qwt_plot_shapeitem.h>
#include <qpainterpath.h>
#include <qwt_legend.h>
#include <QHBoxLayout>

using namespace OT;

namespace OTGUI {

PlotWidget::PlotWidget(QWidget * parent)
: QwtPlot(parent)
, plotLabel_(new QLabel(this))
, dialog_(new ImageEditionDialog(this))
{
  setCanvasBackground(Qt::white);
  setMinimumSize(200, 150);

  plotLayout()->setAlignCanvasToScales(true);

  // panning with the left mouse button
  ( void ) new QwtPlotPanner(canvas());

  // zoom in/out with the wheel
  ( void ) new QwtPlotMagnifier(canvas());
  clear();

  // build actions
  copyImageAction_ = new QAction(tr("Edit image"), this);
  connect(copyImageAction_, SIGNAL(triggered(bool)), this, SLOT(editImage()));
//   saveAsAction_ = new QAction( tr( "Save image as..." ), this) ;
//   connect( saveAsAction_, SIGNAL(triggered( bool )), this, SLOT(saveAs()) );

  plotLabel_->setContextMenuPolicy(Qt::CustomContextMenu);
  connect(plotLabel_, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(contextMenu(QPoint)));
}

void PlotWidget::clear()
{
  detachItems();
  setAxisAutoScale(QwtPlot::xBottom);
  enableAxis(QwtPlot::xBottom); 
  setAxisAutoScale(QwtPlot::yLeft);
  enableAxis(QwtPlot::yLeft);
  // initialize grid
  grid_ = new QwtPlotGrid;
  replot();
}


void PlotWidget::plotCurve(double * x, double * y, int size, const QPen pen, QwtPlotCurve::CurveStyle style, QwtSymbol* symbol)
{
  QwtPlotCurve * curve = new QwtPlotCurve;
  curve->setSamples(x, y, size);
  curve->setPen(pen);
  curve->setStyle(style);
  if (symbol)
    curve->setSymbol(symbol);
  curve->attach(this);
  replot();
}


void PlotWidget::plotCurve(const NumericalSample & data, const QPen pen)
{
  const int size = data.getSize();
  double * x = new double[size];
  double * y = new double[size];

  for (int i = 0; i < size; ++ i)
  {
    x[i] = data[i][0];
    y[i] = data[i][1];
  }
  plotCurve(x, y, size, pen);
  delete[] x;
  delete[] y;
}


void PlotWidget::plotScatter(const NumericalSample & input, const NumericalSample & output)
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
    //qDebug() << "x= " << xData[i] << " , y= " << yData[i];
  }

  plotCurve(xData, yData, size, QPen(Qt::GlobalColor(16), 4), QwtPlotCurve::Dots);
}


void PlotWidget::plotPDFCurve(const Distribution & distribution, const QPen pen)
{
  double mean = distribution.getMean()[0];
  double stepSize = 0.0;
  const double qmin = ResourceMap::GetAsNumericalScalar("DistributionImplementation-QMin");
  const double qmax = ResourceMap::GetAsNumericalScalar("DistributionImplementation-QMax");
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

  setTitle(tr("PDF"));
  setAxisTitle(QwtPlot::yLeft, tr("Density"));
  setAxisTitle(QwtPlot::xBottom, tr("X"));

  plotCurve(distribution.drawPDF().getDrawable(0).getData(), pen);
}


void PlotWidget::drawBoxPlot(double median, double lowerQuartile, double upperQuartile,
                             double lowerBound, double upperBound, NumericalPoint outliers_)
{
  // draw median
  double xMedian[2] = {0.9, 1.1};
  double yMedian[2] = {median, median};
  plotCurve(xMedian, yMedian, 2, QPen(Qt::red));

  // draw box
  QwtPlotShapeItem *item = new QwtPlotShapeItem;

  QRectF rect(QPointF(0.9, lowerQuartile), QSizeF(0.2, upperQuartile-lowerQuartile));
  
  QPainterPath path;
  path.addRect(rect);
  item->setShape(path);
  item->setPen(QPen(Qt::blue));

  item->attach(this);

  // draw whiskers
  double xWhiskers[2] = {1., 1.};
  double yLower[2] = {lowerBound, lowerQuartile};
  plotCurve(xWhiskers, yLower, 2, QPen(Qt::black, 2, Qt::DashLine));

  double yUpper[2] = {upperQuartile, upperBound};
  plotCurve(xWhiskers, yUpper, 2, QPen(Qt::black, 2, Qt::DashLine));


  double xWhiskersBars[2] = {0.95, 1.05};
  double yLowerWhiskersBar[2] = {lowerBound, lowerBound};
  plotCurve(xWhiskersBars, yLowerWhiskersBar, 2);

  double yUpperWhiskersBar[2] = {upperBound, upperBound};
  plotCurve(xWhiskersBars, yUpperWhiskersBar, 2);

  // draw outliers
  const int dim = outliers_.getDimension();
  double * xOutliers = new double[dim];
  double * yOutliers = new double[dim];

  for (int i=0; i<dim; ++i)
  {
    xOutliers[i] = 1.;
    yOutliers[i] = outliers_[i];
  }

  plotCurve(xOutliers, yOutliers, dim, QPen(Qt::blue), QwtPlotCurve::NoCurve, new QwtSymbol(QwtSymbol::Cross, Qt::NoBrush, QPen(Qt::blue), QSize(5, 5)));

//   TODO think about:
//   QwtScaleDiv scaleDiv(0.5, 1.5);
//   QList< double > ticks;
//   scaleDiv.setTicks(QwtScaleDiv::NoTick, ticks);
//   setAxisScaleDiv(QwtPlot::xBottom, scaleDiv);

  setAxisScale(QwtPlot::xBottom, 0.5, 1.5, 0.5);
  replot();
}


// show the context menu when right clicking
void PlotWidget::contextMenu(const QPoint & pos)
{
  QMenu * contextMenu(new QMenu(this));
  contextMenu->addAction(copyImageAction_);
//   contextMenu->addAction(saveAsAction_);
  contextMenu->popup(plotLabel_->mapToGlobal(pos));
}


void PlotWidget::editImage()
{
  dialog_->setInitParameters();
  if (dialog_->exec() == QDialog::Rejected)
  {
    dialog_->resetParameters();
    replot();
  }
}


void PlotWidget::replot()
{
  QwtPlot::replot();
  updatePlotLabel();
  dialog_->updateLineEdits();
}


void PlotWidget::updatePlotLabel()
{
  QPixmap pixmap(200,200);
  pixmap.fill();
  QwtPlotRenderer renderer;

  renderer.renderTo(this, pixmap);

  plotLabel_->setPixmap(pixmap);
}


QLabel * PlotWidget::getPlotLabel() const
{
  return plotLabel_;
}


}
