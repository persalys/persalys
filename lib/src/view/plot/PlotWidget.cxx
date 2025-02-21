//                                               -*- C++ -*-
/**
 *  @brief QwtPlot for all the graphics of the interface
 *
 *  Copyright 2015-2025 EDF-Phimeca
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
#include "persalys/PlotWidget.hxx"

#include "persalys/CustomScaleEngine.hxx"
#include "persalys/CustomScaleDraw.hxx"
#include "persalys/GraphConfigurationWidget.hxx"
#include "persalys/UIntSpinBox.hxx"
#include "persalys/FileTools.hxx"
#include "persalys/QtTools.hxx"

#include <openturns/Contour.hxx>
#include <openturns/HistogramFactory.hxx>

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
#include <qwt_picker_machine.h>
#include <qwt_scale_widget.h>
#include <qwt_plot_marker.h>
#include <qwt_legend_label.h>
#include <qwt_plot_spectrogram.h>
#include <qwt_matrix_raster_data.h>
#include <qwt_color_map.h>

using namespace OT;

namespace PERSALYS
{

// -- custom QwtPlotCurve with Rtti_PlotUserItem = 1001
class PERSALYS_PLOT_API StaticPlotCurveItem : public QwtPlotCurve
{
public:
  StaticPlotCurveItem() {};
  virtual int rtti() const
  {
    return 1001;
  };
};


// -- class PlotWidget --

PlotWidget::PlotWidget(const QString &plotTypeName, const bool disableZoom, QWidget *parent)
  : QwtPlot(parent)
  , plotTypeName_(plotTypeName)
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
  sizeSelectionWizard.setWindowIcon(QIcon(":/images/Ps-icon-16.png"));
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
                     FileTools::GetCurrentDir() + QDir::separator() + plotTypeName_ + ".png",
                     tr("Images (*.bmp *.jpg *.jpeg *.png *.pdf *.svg *.ppm *.xbm *.xpm)"));

  if (!fileName.isEmpty())
  {
    QString format = QFileInfo(fileName).suffix().toLower();
    FileTools::SetCurrentDir(fileName);

    if (format == "")
    {
      fileName += ".png";
      format = QString("png");
    }
    if (format == "pdf" || format == "svg")
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


void PlotWidget::plotCurve(const Sample & sample, const QPen pen, QwtPlotCurve::CurveStyle style, QwtSymbol* symbol, QString title)
{
  const int size = sample.getSize();
  double * x = new double[size];
  double * y = new double[size];

  for (int i = 0; i < size; ++ i)
  {
    x[i] = sample(i, 0);
    y[i] = sample(i, 1);
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
  const Sample dataPDF(distribution.drawPDF().getDrawable(0).getData());
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
  Drawable drawable(distribution.drawCDF().getDrawable(0));
  Sample dataCDF(drawable.getData());
  if (drawable.getImplementation()->getClassName() == "Staircase")
  {
    Sample newDataCDF(0, 2);
    for (UnsignedInteger i = 0; i < dataCDF.getSize() - 1; ++i)
    {
      newDataCDF.add(dataCDF[i]);
      Point interPt(2);
      interPt[0] = dataCDF(i + 1, 0);
      interPt[1] = dataCDF(i, 1);
      newDataCDF.add(interPt);
    }
    newDataCDF.add(dataCDF[dataCDF.getSize() - 1]);
    dataCDF = newDataCDF;
  }
  plotCurve(dataCDF, pen);
  // Add margin at the top to avoid to cut the curve
  const double yMax = dataCDF.getMax()[1];
  if (axisInterval(QwtPlot::yLeft).maxValue() < (yMax * (1 + 0.02)))
    setAxisScale(QwtPlot::yLeft, 0, yMax * (1 + 0.02));
  replot();
}


void PlotWidget::plotQuantileCurve(const Distribution & distribution, const QPen pen)
{
  setTitle(tr("Quantile"));
  setAxisTitle(QwtPlot::yLeft, tr("Quantile"));
  setAxisTitle(QwtPlot::xBottom, tr("X"));

  updateScaleParameters(distribution);
  const Sample dataQuantile(distribution.drawQuantile().getDrawable(0).getData());
  plotCurve(dataQuantile, pen);
  // Add margin at the top to avoid to cut the curve
  double yMax = dataQuantile.getMax()[1];
  if (axisInterval(QwtPlot::yLeft).maxValue() < (yMax * (1 + 0.02)))
    yMax = yMax * (1 + 0.02);
  setAxisScale(QwtPlot::yLeft, dataQuantile.getMin()[1], yMax * (1 + 0.02));
  setAxisScale(QwtPlot::xBottom, -0.01, 1.01);
  replot();
}


void PlotWidget::plotSurvivalCurve(const Distribution & distribution, const QPen pen)
{
  setTitle(tr("Survival function"));
  setAxisTitle(QwtPlot::yLeft, tr("Survival function"));
  setAxisTitle(QwtPlot::xBottom, tr("X"));
  updateScaleParameters(distribution);
  Drawable drawable(distribution.drawSurvivalFunction().getDrawable(0));
  Sample dataSurv(drawable.getData());
  if (drawable.getImplementation()->getClassName() == "Staircase")
  {
    Sample newDataSurv(0, 2);
    for (UnsignedInteger i = 0; i < dataSurv.getSize() - 1; ++i)
    {
      newDataSurv.add(dataSurv[i]);
      Point interPt(2);
      interPt[0] = dataSurv(i, 0);
      interPt[1] = dataSurv(i + 1, 1);
      newDataSurv.add(interPt);
    }
    newDataSurv.add(dataSurv[dataSurv.getSize() - 1]);
    dataSurv = newDataSurv;
  }

  plotCurve(dataSurv, pen);
  // Add margin at the top to avoid to cut the curve
  double yMax = dataSurv.getMax()[1];
  if (axisInterval(QwtPlot::yLeft).maxValue() < (yMax * (1 + 0.02)))
    yMax = yMax * (1 + 0.02);
  setAxisScale(QwtPlot::yLeft, dataSurv.getMin()[1], yMax * (1 + 0.02));
  replot();
}


// graphType = 0 -> PDF
// graphType = 1 -> CDF
// graphType = 2 -> survival function
// graphType = 3 -> other
void PlotWidget::plotHistogram(const Sample & sample, const PlotWidget::HistoType graphType, int barNumber, QString title)
{
  if (graphType > PlotWidget::Other)
    throw InvalidArgumentException(HERE) << "Type of graph not known " << graphType;

  const int size = (int)sample.getSize();
  if (size == 0) throw InvalidArgumentException(HERE) << "Cannot draw an histogram based on an empty sample.";

  // compute bar number
  const double sampleMin = sample.getMin()[0];
  const double sampleMax = sample.getMax()[0];
  if (sampleMax - sampleMin < 1e-12)
    return;

  const double Q1 = sample.computeQuantile(0.25)[0];
  const double Q3 = sample.computeQuantile(0.75)[0];
  const double width = HistogramFactory().computeBandwidth(sample, Q3 - Q1 > 1e-12);

  if (barNumber <= 0)
    barNumber = ceil((sampleMax - sampleMin)/width);

  Point histogramData(barNumber);

  for (int i = 0; i < size; ++i)
  {
    int index = static_cast< int >((sample(i, 0) - sampleMin) / width);
    // x=xmax -> index=barnumber, so bound it
    index = std::min(index, barNumber - 1);
    if (!(index > barNumber || index < 0))
      ++ histogramData[index];
  }

  // if PDF, CDF or Survival
  if (graphType < PlotWidget::Other)
  {
    double inverseArea = 1. / (size * width);
    for (int i = 0; i < barNumber; ++i)
      histogramData[i] *= inverseArea;
  }

  double sum = 1.;
  //  if CDF / Survival
  switch(graphType)
  {
    case PlotWidget::PDF:
      break;
    case PlotWidget::CDF:
      sum = histogramData[0];
      for (int i = 1; i < barNumber; i++)
      {
        sum += histogramData[i];
        histogramData[i] += histogramData[i - 1];
      }
      break;
    case PlotWidget::Survival:
      sum = histogramData[barNumber - 1];
      for (int i = barNumber - 2; i >= 0; i--)
      {
        sum += histogramData[i];
        histogramData[i] += histogramData[i + 1];
      }
      break;
    case PlotWidget::Other:
      break;
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

void PlotWidget::plotFronts(const OT::Collection<OT::Sample> & allFronts,
                            const UnsignedInteger idx1, const UnsignedInteger idx2)
{

  const Description colors(Drawable().BuildRainbowPalette(allFronts.getSize()));
  Sample sample;
  Indices indices;
  indices.add(idx1);
  indices.add(idx2);
  // Loop over all fronts
  for (UnsignedInteger j = 0; j < allFronts.getSize(); ++j)
  {
    if (allFronts[j].getDimension() == 2)
    {
      Sample front = allFronts[j].getMarginal(indices).sortAccordingToAComponent(0);
      sample = Sample(2 * front.getSize() - 1, 2);
      // Build front points and link them
      for (UnsignedInteger i = 0; i < front.getSize(); ++i)
      {
        sample[2 * i] = front[i];
        if (i != front.getSize() - 1)
        {
          sample(2 * i + 1, 0) = front(i + 1, 0);
          sample(2 * i + 1, 1) = front(i, 1);
        }
      }
    }
    else
    {
      // Get side view marginals sub samples and plot only front points
      sample = allFronts[j].getMarginal(indices).sortAccordingToAComponent(0);
    }
    // Draw
    if (j < MaxVisibleVariableNumber)
    {
      QwtSymbol * symbol = new QwtSymbol(QwtSymbol::Ellipse,
                                         QBrush(colors[j].c_str()).color(),
                                         QPen(colors[j].c_str()).color(),
                                         QSize(7, 7));
      setAxisTitle(QwtPlot::xBottom, QString(allFronts[j].getDescription()[idx1].c_str()));
      setAxisTitle(QwtPlot::yLeft, QString(allFronts[j].getDescription()[idx2].c_str()));

      plotCurve(sample, QPen(colors[j].c_str()).color(),
                allFronts[j].getDimension() == 2 ? QwtPlotCurve::Lines : QwtPlotCurve::Dots,
                symbol, QString("front" + QString::number(j)));
    }
  }

  QwtLegend *legend = new QwtLegend();
  legend->setDefaultItemMode(QwtLegendData::Checkable);
  insertLegend(legend, QwtPlot::RightLegend);

  // Loop over legend items ans set them checked
  QwtPlotItemList items = itemList();
  for (int i = 0; i < items.size(); i++)
  {
    const QVariant itemInfo = itemToInfo(items[i]);
    QwtLegendLabel* legendLabel =
      qobject_cast< QwtLegendLabel* >(legend->legendWidget(itemInfo));
    if (legendLabel)
      legendLabel->setChecked(true );
  }

  connect(legend, SIGNAL(checked(const QVariant &, bool, int)),
          this, SLOT(showCurve(const QVariant &, bool, int)));
}

void PlotWidget::showCurve(const QVariant &itemInfo, bool on, int /*index*/)
{
  infoToItem(itemInfo)->setVisible(on);
  replot();
}

void PlotWidget::clear()
{
  detachItems();
  enableAxis(QwtPlot::xBottom);
  enableAxis(QwtPlot::yLeft);
  // TODO initialize grid
//   grid_ = new QwtPlotGrid;
  resetAxisRanges();
}


void PlotWidget::replot()
{
  QwtPlot::replot();
  emit plotChanged();
}


void PlotWidget::setXLabelOrientation(int orientation)
{
  // orientation == 0 : Horizontal
  // orientation == 1 : Slanting
  // orientation == 2 : Vertical

  setAxisLabelAlignment(QwtPlot::xBottom, orientation == 0 ? Qt::AlignBottom : Qt::AlignLeft);
  const Scalar angle = orientation * -45.;
  setAxisLabelRotation(QwtPlot::xBottom, angle);
}


void PlotWidget::resetAxisRanges()
{
  setAxisAutoScale(QwtPlot::xBottom);
  setAxisAutoScale(QwtPlot::yLeft);
  replot();
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


// Custom QwtMatrixRasterData
class RasterData : public QwtMatrixRasterData
{
public:
  RasterData(Contour *contour)
    : QwtMatrixRasterData()
    , dx_(contour->getX()(1, 0) - contour->getX()(0, 0))
    , dy_(contour->getY()(1, 0) - contour->getY()(0, 0))
  {
    setResampleMode(QwtMatrixRasterData::BilinearInterpolation);

    QVector<double> zValues(contour->getData().getSize());
    Sample sample(contour->getData());
    std::copy(&sample(0, 0), &sample(contour->getData().getSize() - 1, 0) + 1, zValues.begin());
    setValueMatrix(zValues, contour->getX().getSize());
  };


  QRectF pixelHint(const QRectF & /*area*/) const
  {
    const QwtInterval intervalX(interval(Qt::XAxis));
    const QwtInterval intervalY(interval(Qt::YAxis));
    if (intervalX.isValid() && intervalY.isValid())
      return QRectF(intervalX.minValue(), intervalY.minValue(), dx_, dy_);
    return QRectF();
  }

private:
  double dx_;
  double dy_;
};



// Custom QwtLinearColorMap
class ColorMap : public QwtLinearColorMap
{
public:
  ColorMap(const QList<double> &levels)
    : QwtLinearColorMap(Qt::darkBlue, Qt::darkRed)
  {
    const double maxLevel = levels[levels.size() - 1];
    const double minLevel = levels[0];
    const double intervalWidth = maxLevel - minLevel;

    if (levels.size() > 2) addColorStop((levels[1] - minLevel) / intervalWidth, Qt::blue);
    if (levels.size() > 3) addColorStop((levels[2] - minLevel) / intervalWidth, Qt::cyan);
    if (levels.size() > 4) addColorStop((levels[3] - minLevel) / intervalWidth, Qt::green);
    if (levels.size() > 9) addColorStop((levels[levels.size() - 5] - minLevel) / intervalWidth, Qt::yellow);
    if (levels.size() > 7) addColorStop((levels[levels.size() - 3] - minLevel) / intervalWidth, "#ffb000"); // orange
    if (levels.size() > 6) addColorStop((levels[levels.size() - 2] - minLevel) / intervalWidth, Qt::red);
  }
};

void PlotWidget::plotContour(const Collection<Drawable>& drawables,
                             const UnsignedInteger drawableIndex,
                             const bool displayGradient,
                             const bool isPDF)
{
  // contour
  Contour * contour = dynamic_cast<Contour*>(drawables[drawableIndex].getImplementation().get());
  if (!contour)
    throw InvalidArgumentException(HERE) << "In plotContour: the drawable is not a Contour";

  // spectrogram
  QwtPlotSpectrogram * spectrogram = new QwtPlotSpectrogram;

  spectrogram->setRenderThreadCount(0); // use system specific thread count
  spectrogram->setCachePolicy(QwtPlotRasterItem::PaintCache);
  spectrogram->setDisplayMode(QwtPlotSpectrogram::ContourMode, true);
  spectrogram->setRenderHint(QwtPlotItem::RenderAntialiased, true);
  spectrogram->setDisplayMode(QwtPlotSpectrogram::ImageMode, displayGradient); // color gradient
  if (!displayGradient)
    spectrogram->setDefaultContourPen(QPen(Qt::NoPen));

  // intervals
  const QwtInterval xInterval(contour->getX().getMin()[0], contour->getX().getMax()[0]);
  const QwtInterval yInterval(contour->getY().getMin()[0], contour->getY().getMax()[0]);
  const double maxZ = isPDF ? contour->getData().computeQuantile(0.97)[0] : contour->getData().getMax()[0];
  const QwtInterval zInterval(contour->getData().getMin()[0], maxZ);
  // data
  RasterData * rasterData = new RasterData(contour);
  rasterData->setInterval(Qt::XAxis, xInterval);
  rasterData->setInterval(Qt::YAxis, yInterval);
  rasterData->setInterval(Qt::ZAxis, zInterval);
  spectrogram->setData(rasterData);

  // levels
  QList<double> levels;
  for (UnsignedInteger i = 0; i < drawables.getSize(); ++i)
  {
    Contour * contour2 = dynamic_cast<Contour*>(drawables[i].getImplementation().get());
    if (contour2)
    {
      const Point levelsI(drawables[i].getLevels());
      for (UnsignedInteger j = 0; j < levelsI.getSize(); ++ j)
        levels.append(levelsI[j]);
    }
  }
  std::sort(levels.begin(), levels.end());
  spectrogram->setContourLevels(levels);

  // color map
  spectrogram->setColorMap(new ColorMap(levels));

  spectrogram->attach(this);


  setAxisScale(QwtPlot::xBottom, xInterval.minValue(), xInterval.maxValue());
  setAxisScale(QwtPlot::yLeft, yInterval.minValue(), yInterval.maxValue());

  if (enableContourColorBar_)
  {
    // A color bar on the right axis
    QwtScaleWidget * rightAxis = axisWidget(QwtPlot::yRight);
    rightAxis->setTitle(tr("Density"));
    rightAxis->setColorBarEnabled(true);
    rightAxis->setScaleDraw(new CustomScaleDraw);
    // - color map
    rightAxis->setColorMap(zInterval, new ColorMap(levels));
    setAxisScale(QwtPlot::yRight, zInterval.minValue(), zInterval.maxValue());
    enableAxis(QwtPlot::yRight);
  }

  plotLayout()->setAlignCanvasToScales(true);
}

void PlotWidget::setEnableContourColorBar(const bool enable)
{
  enableContourColorBar_ = enable;
}

}
