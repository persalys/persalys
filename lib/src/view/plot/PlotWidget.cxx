#include "otgui/PlotWidget.hxx"

#include "otgui/CustomScaleEngine.hxx"
#include "otgui/ImageEditionDialog.hxx"

#include <qwt_plot_layout.h>
#include <qwt_plot_panner.h>
#include <qwt_plot_histogram.h>
#include <qwt_plot_magnifier.h>
#include <qwt_legend.h>
#include <qwt_column_symbol.h>
#include <qwt_plot_renderer.h>

#include <QHBoxLayout>
#include <QMenu>

using namespace OT;

namespace OTGUI {

class BarChartHorizontalScaleDraw: public QwtScaleDraw
{
public:
  BarChartHorizontalScaleDraw(const OT::Description & labels): labels_(labels){}

  virtual QwtText label(double value) const
  {
    const int index = qRound(value);
    if ( index>=0 && index<=labels_.getSize())
    {
      return QwtText(labels_[index].c_str());
    }
    return QwtText();
  }

private:
    const OT::Description labels_;
};


const QColor PlotWidget::DefaultHistogramColor = QColor(127, 172, 210);

PlotWidget::PlotWidget(bool isBarChart, QWidget * parent)
  : QwtPlot(parent)
  , plotLabel_(new QLabel(this))
  , dialog_(new ImageEditionDialog(this, isBarChart))
{
  if (!isBarChart)
  {
    // panning with the left mouse button
    ( void ) new QwtPlotPanner(canvas());

    // zoom in/out with the wheel
    ( void ) new QwtPlotMagnifier(canvas());
  }

  setCanvasBackground(Qt::white);
  setMinimumSize(200, 150);

  plotLayout()->setAlignCanvasToScales(true);

  clear();

  // build actions
  copyImageAction_ = new QAction(tr("Edit image"), this);
  connect(copyImageAction_, SIGNAL(triggered(bool)), this, SLOT(editImage()));
//   saveAsAction_ = new QAction( tr( "Save image as..." ), this) ;
//   connect( saveAsAction_, SIGNAL(triggered( bool )), this, SLOT(saveAs()) );

  plotLabel_->setContextMenuPolicy(Qt::CustomContextMenu);
  connect(plotLabel_, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(contextMenu(QPoint)));
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
  setTitle(tr("PDF"));
  setAxisTitle(QwtPlot::yLeft, tr("Density"));
  setAxisTitle(QwtPlot::xBottom, tr("X"));

  updateScaleParameters(distribution);
  plotCurve(distribution.drawPDF().getDrawable(0).getData(), pen);
}


void PlotWidget::plotCDFCurve(const Distribution & distribution, const QPen pen)
{
  setTitle(tr("CDF"));
  setAxisTitle(QwtPlot::yLeft, tr("CDF"));
  setAxisTitle(QwtPlot::xBottom, tr("X"));

  updateScaleParameters(distribution);
  plotCurve(distribution.drawCDF().getDrawable(0).getData(), pen);
}


void PlotWidget::plotHistogram(const NumericalSample & sample, bool cdf, int barNumber)
{
  const int size = (int)sample.getSize();

  // compute bar number
  if (barNumber <= 0)
    barNumber = ceil(1.0 + log(std::max(size, 1)) / log(2));

  // compute data

  double width = (sample.getMax()[0] - sample.getMin()[0]) / barNumber;
  if (width < 1e-12)
    return;

  NumericalPoint histogramData(barNumber);
  for (int i=0; i<size; ++i)
  {
    int index = static_cast< int >((sample[i][0] - sample.getMin()[0]) / width);
    // x=xmax -> index=barnumber, so bound it
    index = std::min(index, barNumber-1);
    
    ++ histogramData[index];
  }
  double inverseArea = 1. / (size*width);
  for (UnsignedInteger i = 0; i < barNumber; ++i)
    histogramData[i] *= inverseArea;

  //  if CDF
  double sum = 1.;
  if (cdf)
  {
    sum = histogramData[0];
    for (int i=1; i<barNumber; i++)
    {
      sum += histogramData[i];
      histogramData[i] += histogramData[i-1];
    }
  }

  // create histogram
  QwtPlotHistogram * histogram = new QwtPlotHistogram;
  histogram->setBrush(QBrush(DefaultHistogramColor));

  QVector<QwtIntervalSample> samples(barNumber);
  for (int i=0; i<barNumber; i++)
  {
    QwtInterval interval(sample.getMin()[0]+i*width, sample.getMin()[0]+(i+1)*width);
    samples[i] = QwtIntervalSample(histogramData[i]/sum, interval);
  }

  histogram->setData(new QwtIntervalSeriesData(samples));

  // draw
  histogram->attach(this);
  replot();
}

void PlotWidget::plotBoxPlot(double median, double lowerQuartile, double upperQuartile,
                             double lowerBound, double upperBound, NumericalPoint outliers_)
{
  // draw median
  double xMedian[2] = {0.9, 1.1};
  double yMedian[2] = {median, median};
  plotCurve(xMedian, yMedian, 2, QPen(Qt::red));

  // draw box
  double yUpperQuartile[2] = {upperQuartile, upperQuartile};
  plotCurve(xMedian, yUpperQuartile, 2, QPen(Qt::blue));
  double yLowerQuartile[2] = {lowerQuartile, lowerQuartile};
  plotCurve(xMedian, yLowerQuartile, 2, QPen(Qt::blue));
  double xLeftSide[2] = {0.9, 0.9};
  double yBoxSides[2] = {lowerQuartile, upperQuartile};
  plotCurve(xLeftSide, yBoxSides, 2, QPen(Qt::blue));
  double xRightSide[2] = {1.1, 1.1};
  plotCurve(xRightSide, yBoxSides, 2, QPen(Qt::blue));

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


void PlotWidget::plotBarChart(const NumericalPoint firstOrder, const NumericalPoint totalOrder, const Description inputNames)
{
  setTitle("Sensitivity indices");

  setAxisTitle(QwtPlot::yLeft, "Sensitivity indice");
  setAxisTitle(QwtPlot::xBottom, "Inputs");

  // populate bar chart
  static const char *colors[] = {"DarkOrchid", "SteelBlue"};

  QVector<QwtIntervalSample> samples(firstOrder.getSize());

  double width = 0.4;
  if (totalOrder.getSize())
    width = 0.;

  for (int i=0; i<firstOrder.getSize(); i++)
  {
    QwtInterval interval(i-0.4, i+width);
    samples[i] = QwtIntervalSample(firstOrder[i], interval);
  }
  QwtPlotHistogram * histogram = new QwtPlotHistogram;
  histogram->setData(new QwtIntervalSeriesData(samples));
  histogram->setBrush(QBrush("DarkOrchid"));
  histogram->setTitle("First order indice");
  histogram->attach(this);

  if (totalOrder.getSize())
  {
    for (int i=0; i<totalOrder.getSize(); i++)
    {
      QwtInterval interval(i, i+0.4);
      samples[i] = QwtIntervalSample(totalOrder[i], interval);
    }
    QwtPlotHistogram * histogram2 = new QwtPlotHistogram;
    histogram2->setData(new QwtIntervalSeriesData(samples));
    histogram2->setBrush(QBrush("SteelBlue"));
    histogram2->setTitle("Total order indice");
    histogram2->attach(this);

    insertLegend(new QwtLegend(), QwtPlot::BottomLegend);
  }

  // scales
  setAxisScale(QwtPlot::xBottom, -0.5, firstOrder.getSize()-0.5, 1.0);
  setAxisMaxMinor(QwtPlot::xBottom, 0);
  setAxisScaleDraw(QwtPlot::xBottom, new BarChartHorizontalScaleDraw(inputNames));

  setAxisAutoScale(QwtPlot::yLeft);

  replot();
}


void PlotWidget::updateScaleParameters(const Distribution & distribution)
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

  CustomScaleEngine xScaleEngine;
  xScaleEngine.setReference(mean);
  xScaleEngine.setAttribute(QwtScaleEngine::Symmetric, true);
  xScaleEngine.autoScale(3, x1, x2, stepSize);
  setAxisScale(QwtPlot::xBottom, x1, x2, stepSize);
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
