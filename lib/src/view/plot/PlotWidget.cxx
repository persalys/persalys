//                                               -*- C++ -*-
/**
 *  @brief QwtPlot for all the graphics of the interface
 *
 *  Copyright 2015-2018 EDF-Phimeca
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
#include "otgui/CustomScaleDraw.hxx"
#include "otgui/GraphConfigurationWidget.hxx"
#include "otgui/ContourData.hxx"
#include "otgui/UIntSpinBox.hxx"
#include "otgui/FileTools.hxx"

#include <QMenu>
#include <QFileDialog>
#include <QImageWriter>
#include <QMessageBox>
#include <QApplication>
#include <QWizard>
#include <QGroupBox>
#include <QGridLayout>
#include <QLabel>

#include <qwt_plot_layout.h>
#include <qwt_plot_panner.h>
#include <qwt_plot_histogram.h>
#include <qwt_plot_magnifier.h>
#include <qwt_plot_picker.h>
#include <qwt_legend.h>
#include <qwt_column_symbol.h>
#include <qwt_plot_renderer.h>
#include <qwt_plot_spectrogram.h>
#include <qwt_picker_machine.h>
#include <qwt_scale_widget.h>
#include <qwt_scale_draw.h>
#include <qwt_color_map.h>

using namespace OT;

namespace OTGUI
{

// -- custom class CustomHorizontalScaleDraw --
class CustomHorizontalScaleDraw: public QwtScaleDraw
{
public:
  CustomHorizontalScaleDraw(const Description& labels)
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
  const Description labels_;
};


// -- custom QwtPlotCurve with Rtti_PlotUserItem = 1001
class OTGUI_API StaticPlotCurveItem : public QwtPlotCurve
{
public:
  StaticPlotCurveItem(){};
  virtual int rtti() const {return 1001;};
};


// -- class PlotWidget --

const QColor PlotWidget::DefaultHistogramColor = QColor(127, 172, 210);

PlotWidget::PlotWidget(const QString &plotTypeName, const bool disableZoom, QWidget *parent)
  : QwtPlot(parent)
  , plotTypeName_(plotTypeName)
  , verticalMarker_(0)
{
  if (!disableZoom)
  {
    // panning with the left mouse button
    (void) new QwtPlotPanner(canvas());

    // zoom in/out with the wheel
    QwtPlotMagnifier * magnifier = new QwtPlotMagnifier(canvas());
    // set to NoButton to have not interference with the right click of the context menu
    magnifier->setMouseButton(Qt::NoButton);
    // to zoom with the right direction of the wheel
    magnifier->setWheelFactor(1.1);
  }

  // show coordinates
  ( void ) new QwtPlotPicker(QwtPlot::xBottom, QwtPlot::yLeft,
                             QwtPlotPicker::CrossRubberBand, QwtPicker::AlwaysOn, canvas());

  setCanvasBackground(Qt::white);
  plotLayout()->setAlignCanvasToScales(true);

  // QwtScaleDraw with locale insensitive labels
  setAxisScaleDraw(xBottom, new CustomScaleDraw);
  setAxisScaleDraw(yLeft, new CustomScaleDraw);

  clear();

  // context menu
  setContextMenuPolicy(Qt::CustomContextMenu);
  connect(this, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(contextMenu(QPoint)));
}


// show the context menu when right clicking
void PlotWidget::contextMenu(const QPoint & pos)
{
  QMenu contextMenu(this);
  QAction * exportPlotAction = new QAction(tr("Export plot"), this);
  connect(exportPlotAction, SIGNAL(triggered(bool)), this, SLOT(exportPlot()));
  contextMenu.addAction(new QAction(tr("Export plot"), &contextMenu));
  contextMenu.popup(mapToGlobal(pos));
}


void PlotWidget::exportPlot()
{
  // wizard to choose image size
  QWizard sizeSelectionWizard(this);
  sizeSelectionWizard.setWindowIcon(QIcon(":/images/OT_icon16x16.png"));
  sizeSelectionWizard.setButtonText(QWizard::CancelButton, tr("Cancel"));
  sizeSelectionWizard.setButtonText(QWizard::FinishButton, tr("OK"));
  sizeSelectionWizard.setOption(QWizard::NoDefaultButton, true);
  sizeSelectionWizard.setOption(QWizard::NoBackButtonOnStartPage, true);
  QWizardPage * page = new QWizardPage;
  QVBoxLayout * pageLayout = new QVBoxLayout(page);
  QGroupBox * groupBox = new QGroupBox(tr("Size"));
  QGridLayout * groupBoxLayout = new QGridLayout(groupBox);
  groupBoxLayout->addWidget(new QLabel(tr("Image resolution")), 0, 0);
  UIntSpinBox * spinBoxW = new UIntSpinBox;
  spinBoxW->setSingleStep(100);
  spinBoxW->setValue(512);
  groupBoxLayout->addWidget(spinBoxW, 0, 1);
  groupBoxLayout->addWidget(new QLabel("x"), 0, 2, Qt::AlignHCenter);
  UIntSpinBox * spinBoxH = new UIntSpinBox;
  spinBoxH->setSingleStep(100);
  spinBoxH->setValue(512);
  groupBoxLayout->addWidget(spinBoxH, 0, 3);
  pageLayout->addWidget(groupBox);
  sizeSelectionWizard.addPage(page);

  // get image size
  if (!sizeSelectionWizard.exec())
    return;
  const int imageWidth = spinBoxW->value();
  const int imageHeight = spinBoxH->value();

  // save image
  QString fileName = QFileDialog::getSaveFileName(this, tr("Export plot"),
                     FileTools::GetCurrentDir() + QDir::separator() + plotTypeName_,
                     tr("PNG (*.png);; JPEG (*.jpg *.jpeg);; BMP (*.bmp);; PPM (*.ppm);; XBM (*.xbm);; XPM (*.xpm);; TIFF (*.tiff);; SVG (*.svg);; PDF (*.pdf);; PS (*.ps)"));

  if (!fileName.isEmpty())
  {
    QString format = QFileInfo(fileName).suffix().toLower();
    FileTools::SetCurrentDir(fileName);

    if (format == "")
    {
      fileName += ".png";
      format = QString("png");
    }
    if (format == "ps" || format == "pdf" || format == "svg")
    {
      QwtPlotRenderer * renderer = new QwtPlotRenderer();
      renderer->renderDocument(this, fileName, QSizeF(imageWidth, imageHeight));
    }
    else
    {
      // QwtPlotRenderer supports all the listed formats
      // but renderDocument doesn't emit error message...
      if (QImageWriter::supportedImageFormats().indexOf(format.toLatin1()) >= 0)
      {
        QPixmap pixmap(imageWidth, imageHeight);
        pixmap.fill();
        QwtPlotRenderer renderer;

        renderer.renderTo(this, pixmap);

        bool saveOperationSucceed = pixmap.save(fileName, format.toLatin1());
        if (!saveOperationSucceed)
          QMessageBox::warning(QApplication::activeWindow(), tr("Warning"), tr("Impossible to export the plot."));
      }
      else
      {
        QMessageBox::warning(QApplication::activeWindow(), tr("Warning"), tr("Format not supported."));
      }
    }
  }
}


void PlotWidget::plotCurve(QVector<QPointF> points, const QPen pen, QwtPlotCurve::CurveStyle style, QwtSymbol* symbol, QString title, bool isStatic)
{
  QwtPlotCurve * curve;
  if (isStatic)
    curve = new StaticPlotCurveItem;
  else
    curve = new QwtPlotCurve;
  curve->setSamples(points);
  curve->setPen(pen);
  curve->setStyle(style);
  if (symbol)
    curve->setSymbol(symbol);
  if (!title.isEmpty())
  {
    curve->setTitle(title);
    if (symbol)
      curve->setLegendAttribute(QwtPlotCurve::LegendShowSymbol, true);
    else
      curve->setLegendAttribute(QwtPlotCurve::LegendShowLine, true);
  }
  else
    curve->setItemAttribute(QwtPlotItem::Legend, false);
  curve->attach(this);
  replot();
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
  {
    curve->setTitle(title);
  }
  else
  {
    curve->setItemAttribute(QwtPlotItem::Legend, false);
  }
  curve->attach(this);

  replot();
}


void PlotWidget::plotCurve(const Sample & data, const QPen pen, QwtPlotCurve::CurveStyle style, QwtSymbol* symbol, QString title)
{
  const int size = data.getSize();
  double * x = new double[size];
  double * y = new double[size];

  for (int i = 0; i < size; ++ i)
  {
    x[i] = data(i, 0);
    y[i] = data(i, 1);
  }
  plotCurve(x, y, size, pen, style, symbol, title);
  delete[] x;
  delete[] y;
}


void PlotWidget::plotScatter(const Sample & input, const Sample & output,
                             const QPen pen, QString Xtitle, QString Ytitle)
{
  if (input.getDimension()*input.getSize()*output.getDimension()*output.getSize() == 0)
  {
    qDebug() << "In plotScatter: a sample is empty";
    return;
  }
  if (input.getDimension()*output.getDimension() != 1)
  {
    qDebug() << "In plotScatter: the samples must have a dimension of 1";
    return;
  }
  if (output.getSize() != input.getSize())
  {
    qDebug() << "In plotScatter: the 2 samples must have the same size";
    return;
  }

  double *xData = const_cast<double*>(&(input[0][0]));
  double *yData = const_cast<double*>(&(output[0][0]));

  plotCurve(xData, yData, input.getSize(), pen, QwtPlotCurve::Dots);

  setAxisTitle(QwtPlot::xBottom, Xtitle);
  setAxisTitle(QwtPlot::yLeft, Ytitle);
}


void PlotWidget::plotPDFCurve(const Distribution & distribution, const QPen pen)
{
  setTitle(tr("PDF"));
  setAxisTitle(QwtPlot::yLeft, tr("Density"));
  setAxisTitle(QwtPlot::xBottom, tr("X"));

  updateScaleParameters(distribution);
  const Sample dataPDF = distribution.drawPDF().getDrawable(0).getData();
  plotCurve(dataPDF, pen);
  // Add margin at the top to avoid to cut the curve
  const double yMax = dataPDF.getMax()[1];
  if (axisInterval(QwtPlot::yLeft).maxValue() < (yMax * (1 + 0.02)))
    setAxisScale(QwtPlot::yLeft, 0, yMax * (1 + 0.02));
  replot();
}


void PlotWidget::plotCDFCurve(const Distribution & distribution, const QPen pen)
{
  setTitle(tr("CDF"));
  setAxisTitle(QwtPlot::yLeft, tr("CDF"));
  setAxisTitle(QwtPlot::xBottom, tr("X"));

  updateScaleParameters(distribution);
  const Sample dataCDF = distribution.drawCDF().getDrawable(0).getData();
  plotCurve(dataCDF, pen);
  // Add margin at the top to avoid to cut the curve
  const double yMax = dataCDF.getMax()[1];
  if (axisInterval(QwtPlot::yLeft).maxValue() < (yMax * (1 + 0.02)))
    setAxisScale(QwtPlot::yLeft, 0, yMax * (1 + 0.02));
  replot();
}


// graphType = 0 -> PDF
// graphType = 1 -> CDF
// graphType = 2 -> other
void PlotWidget::plotHistogram(const Sample & sample, const UnsignedInteger graphType, int barNumber, QString title)
{
  if (graphType > 2)
    throw InvalidArgumentException(HERE) << "Type of graph not known " << graphType;

  const int size = (int)sample.getSize();
  if (size == 0) throw InvalidArgumentException(HERE) << "Cannot draw an histogram based on an empty sample.";

  // compute bar number
  if (barNumber <= 0)
    barNumber = ceil(1.0 + log(std::max(size, 1)) / log(2));

  // compute data
  double sampleMin = sample.getMin()[0];
  double width = (sample.getMax()[0] - sampleMin) / barNumber;
  if (width < 1e-12)
    return;

  Point histogramData(barNumber);

  for (int i = 0; i < size; ++i)
  {
    int index = static_cast< int >((sample(i, 0) - sampleMin) / width);
    // x=xmax -> index=barnumber, so bound it
    index = std::min(index, barNumber - 1);
    if (!(index > barNumber || index < 0))
      ++ histogramData[index];
  }

  // if PDF or CDF
  if (graphType < 2)
  {
    double inverseArea = 1. / (size * width);
    for (int i = 0; i < barNumber; ++i)
      histogramData[i] *= inverseArea;
  }

  //  if CDF
  double sum = 1.;
  if (graphType == 1)
  {
    sum = histogramData[0];
    for (int i = 1; i < barNumber; i++)
    {
      sum += histogramData[i];
      histogramData[i] += histogramData[i - 1];
    }
  }

  // create histogram
  QwtPlotHistogram * histogram = new QwtPlotHistogram;
  histogram->setBrush(QBrush(DefaultHistogramColor));

  QVector<QwtIntervalSample> samples(barNumber);
  for (int i = 0; i < barNumber; i++)
  {
    QwtInterval interval(sampleMin + i * width, sampleMin + (i + 1)*width);
    samples[i] = QwtIntervalSample(histogramData[i] / sum, interval);
  }

  histogram->setData(new QwtIntervalSeriesData(samples));
  histogram->setTitle(title);

  // draw
  histogram->attach(this);
  replot();
}


void PlotWidget::plotBoxPlot(const double median,
                             const double lowerQuartile,
                             const double upperQuartile,
                             const double lowerBound,
                             const double upperBound,
                             const Point& outliers)
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
  const int dim = outliers.getDimension();
  double * xOutliers = new double[dim];
  double * yOutliers = new double[dim];

  for (int i = 0; i < dim; ++i)
  {
    xOutliers[i] = 1.;
    yOutliers[i] = outliers[i];
  }

  plotCurve(xOutliers, yOutliers, dim, QPen(Qt::blue), QwtPlotCurve::NoCurve, new QwtSymbol(QwtSymbol::Cross, Qt::NoBrush, QPen(Qt::blue), QSize(5, 5)));
  delete[] xOutliers;
  delete[] yOutliers;

//   TODO think about:
//   QwtScaleDiv scaleDiv(0.5, 1.5);
//   QList< double > ticks;
//   scaleDiv.setTicks(QwtScaleDiv::NoTick, ticks);
//   setAxisScaleDiv(QwtPlot::xBottom, scaleDiv);

  setAxisScale(QwtPlot::xBottom, 0.5, 1.5, 0.5);
  replot();
}


void PlotWidget::plotSensitivityIndices(const Point& firstOrderIndices,
                                        const Point& totalIndices,
                                        const Description& inputNames,
                                        const Interval& firstOrderIndicesIntervals,
                                        const Interval& totalIndicesIntervals)
{
  setAxisTitle(QwtPlot::yLeft, tr("Index"));
  setAxisTitle(QwtPlot::xBottom, tr("Inputs"));

  // populate bar chart
  static const char *colors[] = {"DarkOrchid", "SteelBlue"};

  const UnsignedInteger size = firstOrderIndices.getSize();

  double *xData = new double[size];
  double *yData = new double[size];

  double width = 0.;
  if (totalIndices.getSize())
    width = 0.1;

  double yMin = 0.;
  double yMax = 0.;
  for (UnsignedInteger i = 0 ; i < size ; ++i)
  {
    xData[i] = (i - width);
    yData[i] = firstOrderIndices[i];
    yMin = std::min(yMin, firstOrderIndices[i]);
    yMax = std::max(yMax, firstOrderIndices[i]);
    //qDebug() << "x= " << xData[i] << " , y= " << yData[i];
    if (firstOrderIndicesIntervals.getDimension() == size)
    {
      double xInterval[2] = {(i - width), (i - width)};
      double yInterval[2] = {firstOrderIndicesIntervals.getLowerBound()[i], firstOrderIndicesIntervals.getUpperBound()[i]};
      plotCurve(xInterval, yInterval, 2, QPen(colors[0]));

      yMin = std::min(yMin, firstOrderIndicesIntervals.getLowerBound()[i]);
      yMax = std::max(yMax, firstOrderIndicesIntervals.getUpperBound()[i]);
    }
  }

  plotCurve(xData, yData, size, QPen(Qt::black), QwtPlotCurve::NoCurve, new QwtSymbol(QwtSymbol::Ellipse, QBrush(colors[0]), QPen(colors[0]), QSize(5, 5)), tr("First order index"));
  delete[] xData;
  delete[] yData;

  if (totalIndices.getSize())
  {
    xData = new double[size];
    yData = new double[size];

    for (UnsignedInteger i = 0 ; i < size ; ++i)
    {
      xData[i] = (i + width) ;
      yData[i] = totalIndices[i];
      yMin = std::min(yMin, totalIndices[i]);
      yMax = std::max(yMax, totalIndices[i]);
      //qDebug() << "x= " << xData[i] << " , y= " << yData[i];
      if (totalIndicesIntervals.getDimension() == size)
      {
        double xInterval[2] = {(i + width), (i + width)};
        double yInterval[2] = {totalIndicesIntervals.getLowerBound()[i], totalIndicesIntervals.getUpperBound()[i]};
        plotCurve(xInterval, yInterval, 2, QPen(colors[1]));

        yMin = std::min(yMin, totalIndicesIntervals.getLowerBound()[i]);
        yMax = std::max(yMax, totalIndicesIntervals.getUpperBound()[i]);
      }
    }
    plotCurve(xData, yData, size, QPen(Qt::black), QwtPlotCurve::NoCurve, new QwtSymbol(QwtSymbol::Rect, QBrush(colors[1]), QPen(colors[1]), QSize(5, 5)), tr("Total index"));

    insertLegend(new QwtLegend(), QwtPlot::BottomLegend);
    delete[] xData;
    delete[] yData;
  }

  // scales
  setAxisScale(QwtPlot::xBottom, -0.5, firstOrderIndices.getSize() - 0.5, 1.0);
  setAxisMaxMinor(QwtPlot::xBottom, 0);
  setAxisScaleDraw(QwtPlot::xBottom, new CustomHorizontalScaleDraw(inputNames));

  // rescale to avoid to cut points
  yMin = yMin - (std::abs(0.05 * yMin) < 0.01 ? 0.05 : std::abs(0.05 * yMin));
  yMax = yMax + std::abs(0.05 * yMax);
  setAxisScale(QwtPlot::yLeft, yMin, yMax);

  replot();
}


void PlotWidget::plotContour(const Distribution& distribution, const bool isPdf)
{
  if (distribution.getDimension() != 2)
  {
    qDebug() << "In plotContour: distribution dimension must be 2";
    return;
  }

  QwtPlotSpectrogram * spectrogram = new QwtPlotSpectrogram;

  spectrogram->setRenderThreadCount(0); // use system specific thread count
  spectrogram->setCachePolicy(QwtPlotRasterItem::PaintCache);
  spectrogram->setDisplayMode(QwtPlotSpectrogram::ContourMode, true);
  spectrogram->setDisplayMode(QwtPlotSpectrogram::ImageMode, true);

  // drawable
  Drawable aDrawable;
  if (isPdf)
    aDrawable = distribution.drawPDF(Indices(2, 70)).getDrawable(1);
  else
    aDrawable = distribution.drawCDF(Indices(2, 70)).getDrawable(0);
  Contour contour = *dynamic_cast<Contour*>(aDrawable.getImplementation().get());

  // data
  ContourData * contourData = new ContourData;
  contourData->setData(contour);
  spectrogram->setData(contourData);

  const QwtInterval zInterval = contourData->interval(Qt::ZAxis);

  // build levels
  const Sample sortedData(contour.getData().sort(0));
  const UnsignedInteger size = sortedData.getSize();
  QList<double> rank = QList<double>() << 0.1 << 0.2 << 0.3 << 0.4 << 0.5 << 0.6 << 0.7 << 0.8 << 0.9 << 0.95 << 0.97 << 0.99;

  QList<double> levels;
  for (int i = 0; i < rank.size(); ++i)
  {
    levels += sortedData[rank[i] * size - 1][0];
  }
  levels += (sortedData[size - 1][0] - sortedData[0.99 * size - 1][0]) * 0.5;
  qSort(levels);
  spectrogram->setContourLevels(levels);

  // color map
  const double maxV = levels[levels.size() - 1];
  const double minV = levels[0];

  QwtLinearColorMap * colorMap = new QwtLinearColorMap(Qt::darkCyan, Qt::darkRed);
  colorMap->addColorStop((levels[1] - minV) / (maxV - minV), Qt::cyan);
  colorMap->addColorStop((levels[2] - minV) / (maxV - minV), Qt::green);
  colorMap->addColorStop((levels[levels.size() - 5] - minV) / (maxV - minV), Qt::yellow);
  colorMap->addColorStop((levels[levels.size() - 2] - minV) / (maxV - minV), Qt::red);
  spectrogram->setColorMap(colorMap);

  spectrogram->attach(this);

  // A color bar on the right axis
  QwtScaleWidget * rightAxis = axisWidget(QwtPlot::yRight);
  rightAxis->setTitle(tr("Density"));
  rightAxis->setColorBarEnabled(true);
  rightAxis->setScaleDraw(new CustomScaleDraw);

  // - color map
  colorMap = new QwtLinearColorMap(Qt::darkCyan, Qt::darkRed);
  colorMap->addColorStop((levels[1] - minV) / (maxV - minV), Qt::cyan);
  colorMap->addColorStop((levels[2] - minV) / (maxV - minV), Qt::green);
  colorMap->addColorStop((levels[levels.size() - 5] - minV) / (maxV - minV), Qt::yellow);
  colorMap->addColorStop((levels[levels.size() - 2] - minV) / (maxV - minV), Qt::red);
  rightAxis->setColorMap(zInterval, colorMap);

  setAxisScale(QwtPlot::yRight, zInterval.minValue(), zInterval.maxValue());
  enableAxis(QwtPlot::yRight);

  plotLayout()->setAlignCanvasToScales(true);

  replot();
}


void PlotWidget::setMorrisPlotType(const QPointF& initialMarkersCoord)
{
  // vertical marker: no effect boundary
  verticalMarker_ = new QwtPlotMarker;
  verticalMarker_->setValue(initialMarkersCoord);
  verticalMarker_->setLineStyle(QwtPlotMarker::VLine);
  verticalMarker_->setTitle(tr("No effect boundary"));
  verticalMarker_->setLinePen(QPen(Qt::darkGreen, 2, Qt::DashLine));
  verticalMarker_->setItemAttribute(QwtPlotItem::Legend, true);
  verticalMarker_->attach(this);

  // mouse right click event filter to change the vertical marker position
  QwtScaleWidget * scaleWidget = axisWidget(QwtPlot::xBottom);
  scaleWidget->installEventFilter(this);
  scaleWidget->setToolTip(tr("Selecting a value at the scale will move the vertical marker."));

  // picker to select points with the right button
  QwtPlotPicker * picker = new QwtPlotPicker(QwtPlot::xBottom, QwtPlot::yLeft, QwtPlotPicker::RectRubberBand, QwtPicker::AlwaysOn, canvas());
  picker->setStateMachine(new QwtPickerDragRectMachine);
  picker->setMousePattern(QwtEventPattern::MouseSelect1, Qt::RightButton);

  connect(picker, SIGNAL(selected(QRectF)), this , SLOT(selectPoints(QRectF)));
}


bool PlotWidget::eventFilter(QObject *obj, QEvent *event)
{
  if (event->type() == QEvent::MouseButtonPress)
  {
    if (obj == axisWidget(QwtPlot::xBottom))
    {
      QMouseEvent * mEvent = dynamic_cast<QMouseEvent *>(event);
      if (mEvent->button() == Qt::LeftButton)
      {
        const double newX = invTransform(QwtPlot::xBottom, mEvent->pos().x());
        verticalMarker_->setValue(QPointF(newX, 0));
        replot();
        emit verticalMarkerPositionChanged(newX);
      }
    }
  }
  return QwtPlot::eventFilter(obj, event);
}


void PlotWidget::selectPoints(const QRectF& rect)
{
  const double left = std::min(rect.left(), rect.right());
  const double right = std::max(rect.left(), rect.right());
  const double top = std::max(rect.top(), rect.bottom());
  const double bottom = std::min(rect.top(), rect.bottom());

  const QwtPlotItemList& plotItemList = itemList();
  QVector< QPointF > selectedPoints;
  QVector< QwtPlotMarker* > selectedMarkers;

  for (QwtPlotItemIterator it = plotItemList.begin(); it != plotItemList.end(); ++it)
  {
    if ((*it)->rtti() == QwtPlotItem::Rtti_PlotMarker)
    {
      QwtPlotMarker *m = static_cast<QwtPlotMarker *>(*it);
      const double xP = m->xValue();
      const double yP = m->yValue();

      // if the point is in rect
      if (yP <= top && yP >= bottom && xP <= right && xP >= left)
      {
        selectedMarkers.append(m);
        selectedPoints.append(m->value());
      }
    }
  }
  // if there is at least one point in the rectangle
  if (selectedMarkers.size())
  {
    // hightlight the selected points
    QwtPlotCurve selectedPointCurve;
    selectedPointCurve.setSymbol(new QwtSymbol(QwtSymbol::Ellipse, Qt::magenta, QPen(Qt::magenta), QSize(7, 7)));
    selectedPointCurve.setStyle(QwtPlotCurve::NoCurve);
    selectedPointCurve.setItemAttribute(QwtPlotItem::Legend, false);

    selectedPointCurve.setSamples(selectedPoints);
    selectedPointCurve.attach(this);

    replot();

    // context menu to choose to de/select the points
    QMenu contextMenu(this);
    // actions of the context menu
    QAction * deselectPointsAction = new QAction(tr("Deselect the points"), &contextMenu);
    QAction * selectPointsAction= new QAction(tr("Select the points"), &contextMenu);
    contextMenu.addAction(deselectPointsAction);
    contextMenu.addAction(selectPointsAction);

    const QPoint pos(transform(QwtPlot::xBottom, left), transform(QwtPlot::yLeft, bottom));
    QAction * action = contextMenu.exec(mapToGlobal(pos));

    // emit signal to the window to update the data
    if (action == deselectPointsAction || action == selectPointsAction)
    {
      const QPen markerPen = (action == deselectPointsAction ? QPen(Qt::red) : QPen(Qt::blue));
      for (int i = 0; i < selectedMarkers.size(); ++i)
        selectedMarkers[i]->setSymbol(new QwtSymbol(QwtSymbol::Cross, Qt::NoBrush, markerPen, QSize(5, 5)));
      emit selectedPointsChanged();
    }
    selectedPointCurve.detach();

    replot();
  }
}


void PlotWidget::updateScaleParameters(const Distribution & distribution)
{
  double mean = distribution.getMean()[0];
  double stepSize = 0.0;
  const double qmin = ResourceMap::GetAsScalar("Distribution-QMin");
  const double qmax = ResourceMap::GetAsScalar("Distribution-QMax");
  double x1 = distribution.computeQuantile(qmin)[0];
  double x2 = distribution.computeQuantile(qmax)[0];
  const double delta = 2.0 * (x2 - x1) * (1.0 - 0.5 * (qmax - qmin));
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
  // TODO initialize grid
//   grid_ = new QwtPlotGrid;
  replot();
}


void PlotWidget::replot()
{
  QwtPlot::replot();
  emit plotChanged();
}


QVector<PlotWidget*> PlotWidget::GetListScatterPlots(const Sample& inS,
    const Sample& outS,
    const Sample& notValidInS,
    const QStringList inNames,
    const QStringList inAxisNames,
    const QStringList outNames,
    const QStringList outAxisNames
                                                   )
{
  QVector<PlotWidget*> listScatterPlotWidgets;

  const UnsignedInteger nbInputs = inS.getSize() ? inS.getDimension() : 0;
  const UnsignedInteger nbOutputs = outS.getSize() ? outS.getDimension() : 0;
  const QPen pen(Qt::blue, 4);
  const QPen notValidPen(Qt::red, 4);

  // in rank
  Sample inSrank;
  if (nbInputs)
    inSrank = inS.rank() / inS.getSize();
  Sample notValidInSrank;
  if (notValidInS.getSize())
    notValidInSrank = notValidInS.rank() / notValidInS.getSize();
  // out rank
  Sample outSrank;
  if (nbOutputs)
    outSrank = outS.rank() / outS.getSize();

  for (UnsignedInteger j = 0; j < nbInputs; ++j)
  {
    for (UnsignedInteger i = 0; i < nbOutputs; ++i)
    {
      PlotWidget * plot = new PlotWidget(tr("scatterplot"));
      plot->plotScatter(inS.getMarginal(j), outS.getMarginal(i), pen, inAxisNames[j], outAxisNames[i]);
      plot->setTitle(tr("Scatter plot:") + " " + outNames[i] + " " + tr("vs") + " " + inNames[j]);
      listScatterPlotWidgets.append(plot);

      // ranks
      plot = new PlotWidget(tr("scatterplot"));
      plot->plotScatter(inSrank.getMarginal(j), outSrank.getMarginal(i), pen, inAxisNames[j], outAxisNames[i]);
      plot->setTitle(tr("Scatter plot:") + " " + outNames[i] + " " + tr("vs") + " " + inNames[j]);
      listScatterPlotWidgets.append(plot);
    }
    for (UnsignedInteger i = 0; i < nbInputs; ++i)
    {
      if (i != j)
      {
        PlotWidget * plot = new PlotWidget(tr("scatterplot"));
        plot->plotScatter(inS.getMarginal(j), inS.getMarginal(i), pen, inAxisNames[j], inAxisNames[i]);
        if (notValidInS.getSize())
          plot->plotScatter(notValidInS.getMarginal(j), notValidInS.getMarginal(i), notValidPen, inAxisNames[j], inAxisNames[i]);
        plot->setTitle(tr("Scatter plot:") + " " + inNames[i] + " " + tr("vs") + " " + inNames[j]);
        listScatterPlotWidgets.append(plot);

        // ranks
        plot = new PlotWidget(tr("scatterplot"));
        plot->plotScatter(inSrank.getMarginal(j), inSrank.getMarginal(i), pen, inAxisNames[j], inAxisNames[i]);
        if (notValidInS.getSize())
          plot->plotScatter(notValidInSrank.getMarginal(j), notValidInSrank.getMarginal(i), notValidPen, inAxisNames[j], inAxisNames[i]);
        plot->setTitle(tr("Scatter plot:") + " " + inNames[i] + " " + tr("vs") + " " + inNames[j]);
        listScatterPlotWidgets.append(plot);
      }
    }
  }
  for (UnsignedInteger j = 0; j < nbOutputs; ++j)
  {
    for (UnsignedInteger i = 0; i < nbOutputs; ++i)
    {
      if (i != j)
      {
        PlotWidget * plot = new PlotWidget(tr("scatterplot"));
        plot->plotScatter(outS.getMarginal(j), outS.getMarginal(i), pen, outAxisNames[j], outAxisNames[i]);
        plot->setTitle(tr("Scatter plot:") + " " + outNames[i] + " " + tr("vs") + " " + outNames[j]);
        listScatterPlotWidgets.append(plot);

        // ranks
        plot = new PlotWidget(tr("scatterplot"));
        plot->plotScatter(outSrank.getMarginal(j), outSrank.getMarginal(i), pen, outAxisNames[j], outAxisNames[i]);
        plot->setTitle(tr("Scatter plot:") + " " + outNames[i] + " " + tr("vs") + " " + outNames[j]);
        listScatterPlotWidgets.append(plot);
      }
    }
    for (UnsignedInteger i = 0; i < nbInputs; ++i)
    {
      PlotWidget * plot = new PlotWidget(tr("scatterplot"));
      plot->plotScatter(outS.getMarginal(j), inS.getMarginal(i), pen, outAxisNames[j], inAxisNames[i]);
      plot->setTitle(tr("Scatter plot:") + " " + inNames[i] + " " + tr("vs") + " " + outNames[j]);
      listScatterPlotWidgets.append(plot);

      // ranks
      plot = new PlotWidget(tr("scatterplot"));
      plot->plotScatter(outSrank.getMarginal(j), inSrank.getMarginal(i), pen, outAxisNames[j], inAxisNames[i]);
      plot->setTitle(tr("Scatter plot:") + " " + inNames[i] + " " + tr("vs") + " " + outNames[j]);
      listScatterPlotWidgets.append(plot);
    }
  }
  return listScatterPlotWidgets;
}
}
