#include "PlotWidget.hxx"
#include "DistributionScaleEngine.hxx"
#include "ImageEditionDialog.hxx"
#include <qwt_plot_panner.h>
#include <qwt_plot_layout.h>
#include <QMenu>
#include <qwt_plot_renderer.h>
#include <qwt_legend.h>
#include <QHBoxLayout>

namespace OTGUI {

const QColor PlotWidget::DefaultCurveColor = QColor(240, 140, 0);//dark orange

PlotWidget::PlotWidget(QWidget * parent)
: QwtPlot(parent)
, plotLabel_(new QLabel(this))
, dialog_(new ImageEditionDialog(this))
{
  setCanvasBackground(Qt::white);
  setMinimumSize(200, 150);

  plotLayout()->setAlignCanvasToScales(true);

  zoomer_ = new QwtPlotZoomer(canvas());
  zoomer_->setMousePattern( QwtEventPattern::MouseSelect2,
    Qt::RightButton, Qt::ControlModifier );
  zoomer_->setMousePattern( QwtEventPattern::MouseSelect3,
      Qt::RightButton );

  zoomer_->setRubberBandPen(QPen(Qt::black, 2, Qt::DotLine));
  zoomer_->setTrackerPen(QPen(Qt::black));

  QwtPlotPanner *panner = new QwtPlotPanner(canvas());
  panner->setAxisEnabled(QwtPlot::yRight, false);
  panner->setMouseButton(Qt::MidButton);

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
  zoomer_->setZoomBase();
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

  setTitle(tr("PDF"));
  setAxisTitle(QwtPlot::yLeft, tr("Density"));
  setAxisTitle(QwtPlot::xBottom, tr("X"));

  plotCurve(distribution.drawPDF().getDrawable(0).getData(), color, width);
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
