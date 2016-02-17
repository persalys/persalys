//                                               -*- C++ -*-
/**
 *  @brief QwtPlot for all the graphics of the interface
 *
 *  Copyright 2015-2016 EDF
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
 *  You should have received a copy of the GNU Lesser General Public
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
#include "otgui/PlotWidget.hxx"

#include "otgui/CustomScaleEngine.hxx"
#include "otgui/GraphConfigurationWidget.hxx"

#include <QMenu>
#include <QFileDialog>
#include <QImageWriter>

#include <qwt_plot_layout.h>
#include <qwt_plot_panner.h>
#include <qwt_plot_histogram.h>
#include <qwt_plot_magnifier.h>
#include <qwt_plot_picker.h>
#include <qwt_legend.h>
#include <qwt_column_symbol.h>
#include <qwt_plot_renderer.h>

using namespace OT;

namespace OTGUI {

class customHorizontalScaleDraw: public QwtScaleDraw
{
public:
  customHorizontalScaleDraw(const OT::Description & labels): labels_(labels){}

  virtual QwtText label(double value) const
  {
    const int index = qRound(value);
    if (index >= 0 && index < (int)labels_.getSize())
      return QwtText(labels_[index].c_str());
    return QwtText();
  }

private:
    const OT::Description labels_;
};


const QColor PlotWidget::DefaultHistogramColor = QColor(127, 172, 210);

PlotWidget::PlotWidget(bool isIndicesPlot, QWidget * parent)
  : QwtPlot(parent)
{
  if (!isIndicesPlot)
  {
    // panning with the left mouse button
    ( void ) new QwtPlotPanner(canvas());

    // zoom in/out with the wheel
    QwtPlotMagnifier * magnifier = new QwtPlotMagnifier(canvas());
    // set to NoButton to have not interference with the right click of the context menu
    magnifier->setMouseButton(Qt::NoButton);
  }

  // show coordinates
  ( void ) new QwtPlotPicker(QwtPlot::xBottom, QwtPlot::yLeft,
                              QwtPlotPicker::CrossRubberBand, QwtPicker::AlwaysOn, canvas());

  setCanvasBackground(Qt::white);
  plotLayout()->setAlignCanvasToScales(true);

  clear();

  // build action
  exportPlotAction_ = new QAction(tr("Export plot"), this);
  connect(exportPlotAction_, SIGNAL(triggered(bool)), this, SLOT(exportPlot()));

  setContextMenuPolicy(Qt::CustomContextMenu);
  connect(this, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(contextMenu(QPoint)));
}


// show the context menu when right clicking
void PlotWidget::contextMenu(const QPoint & pos)
{
  QMenu * contextMenu(new QMenu(this));
  contextMenu->addAction(exportPlotAction_);
  contextMenu->popup(mapToGlobal(pos));
}


void PlotWidget::exportPlot()
{
  QString fileName = QFileDialog::getSaveFileName(this, tr("Export plot"),
                     QDir::homePath(),
                     tr("Images (*.bmp *.jpg *.jpeg *.png *.ppm *.xbm *.xpm *.tiff *.svg *.pdf *.ps)"));

  if (!fileName.isEmpty())
  {
    QString format = QFileInfo(fileName).suffix().toLower();

    if (format == "")
    {
      fileName += ".png";
      format = QString("png");
    }
    if (format == "ps" || format == "pdf" || format == "svg")
    {
      QwtPlotRenderer * renderer = new QwtPlotRenderer();
      renderer->renderDocument(this, fileName, QSizeF(150, 100));
    }
    else
    {
      // QwtPlotRenderer supports all the listed formats
      // but renderDocument doesn't emit error message...
      if (QImageWriter::supportedImageFormats().indexOf(format.toLatin1()) >= 0)
      {
        QPixmap pixmap(200, 200);
        pixmap.fill();
        QwtPlotRenderer renderer;

        renderer.renderTo(this, pixmap);

        bool saveOperationSucceed = pixmap.save(fileName, format.toLatin1());
        if (!saveOperationSucceed)
          std::cerr<<"Export doesn't work\n";
      }
      else
      {
        std::cerr<<"Format not supported\n";
      }
    }
  }
}


void PlotWidget::plotCurve(double * x, double * y, int size, const QPen pen, QwtPlotCurve::CurveStyle style, QwtSymbol* symbol, QString title)
{
  QwtPlotCurve * curve = new QwtPlotCurve;
  curve->setSamples(x, y, size);
  curve->setPen(pen);
  curve->setStyle(style);
  if (symbol)
    curve->setSymbol(symbol);
  if (!title.isEmpty())
    curve->setTitle(title);
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


void PlotWidget::plotScatter(const NumericalSample & input, const NumericalSample & output, const QPen pen)
{
  UnsignedInteger size = input.getSize();

  if (output.getSize()!=size)
    return;
  
  double *xData = new double[size];
  double *yData = new double[size];

  for (UnsignedInteger i=0 ; i<size ; ++i)
  {
    xData[i] = input[i][0];
    yData[i] = output[i][0];
    //qDebug() << "x= " << xData[i] << " , y= " << yData[i];
  }

  plotCurve(xData, yData, size, pen, QwtPlotCurve::Dots);
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
  double sampleMin = sample.getMin()[0];
  double width = (sample.getMax()[0] - sampleMin) / barNumber;
  if (width < 1e-12)
    return;

  NumericalPoint histogramData(barNumber);

  for (int i=0; i<size; ++i)
  {
    int index = static_cast< int >((sample[i][0] - sampleMin) / width);
    // x=xmax -> index=barnumber, so bound it
    index = std::min(index, barNumber-1);
    ++ histogramData[index];
  }
  double inverseArea = 1. / (size*width);
  for (int i=0; i<barNumber; ++i)
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
    QwtInterval interval(sampleMin+i*width, sampleMin+(i+1)*width);
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


void PlotWidget::plotSensitivityIndices(const NumericalPoint firstOrder, const NumericalPoint totalOrder, const Description inputNames)
{
  if (totalOrder.getSize())
    setTitle("Sobol sensitivity indices");
  else
    setTitle("SRC sensitivity indices");

  setAxisTitle(QwtPlot::yLeft, "Indice value");
  setAxisTitle(QwtPlot::xBottom, "Inputs");

  // populate bar chart
  static const char *colors[] = {"DarkOrchid", "SteelBlue"};

  UnsignedInteger size = firstOrder.getSize();
  
  double *xData = new double[size];
  double *yData = new double[size];

  double width = 0.;
  if (totalOrder.getSize())
    width = 0.1;

  for (UnsignedInteger i=0 ; i<size ; ++i)
  {
    xData[i] = (i-width);
    yData[i] = firstOrder[i];
    //qDebug() << "x= " << xData[i] << " , y= " << yData[i];
  }

  plotCurve(xData, yData, size, QPen(Qt::black), QwtPlotCurve::NoCurve, new QwtSymbol(QwtSymbol::Ellipse, QBrush(colors[0]), QPen(colors[0]), QSize(5, 5)), "First order indice");

  if (totalOrder.getSize())
  {
    xData = new double[size];
    yData = new double[size];

    for (UnsignedInteger i=0 ; i<size ; ++i)
    {
      xData[i] = (i+width) ;
      yData[i] = totalOrder[i];
      //qDebug() << "x= " << xData[i] << " , y= " << yData[i];
    }
    plotCurve(xData, yData, size, QPen(Qt::black), QwtPlotCurve::NoCurve, new QwtSymbol(QwtSymbol::Rect, QBrush(colors[1]), QPen(colors[1]), QSize(5, 5)), "Total order indice");

    insertLegend(new QwtLegend(), QwtPlot::BottomLegend);
  }

  // scales
  setAxisScale(QwtPlot::xBottom, -0.5, firstOrder.getSize()-0.5, 1.0);
  setAxisMaxMinor(QwtPlot::xBottom, 0);
  setAxisScaleDraw(QwtPlot::xBottom, new customHorizontalScaleDraw(inputNames));

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


void PlotWidget::replot()
{
  QwtPlot::replot();
  emit plotChanged();
}
}