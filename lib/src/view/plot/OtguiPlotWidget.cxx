#include "PlotWidget.hxx"
#include "ImageEditionDialog.hxx"
#include <qwt_plot_layout.h>
#include <QMenu>
#include <qwt_plot_renderer.h>
#include <qwt_legend.h>
#include <QHBoxLayout>

namespace OTGUI {

OtguiPlotWidget::OtguiPlotWidget(QWidget * parent)
: QwtPlot(parent)
, plotLabel_(new QLabel(this))
, dialog_(new ImageEditionDialog(this))
{
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

void OtguiPlotWidget::clear()
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
void OtguiPlotWidget::contextMenu(const QPoint & pos)
{
  QMenu * contextMenu(new QMenu(this));
  contextMenu->addAction(copyImageAction_);
//   contextMenu->addAction(saveAsAction_);
  contextMenu->popup(plotLabel_->mapToGlobal(pos));
}


void OtguiPlotWidget::editImage()
{
  dialog_->setInitParameters();
  if (dialog_->exec() == QDialog::Rejected)
  {
    dialog_->resetParameters();
    replot();
  }
}


void OtguiPlotWidget::replot()
{
  QwtPlot::replot();
  updatePlotLabel();
  dialog_->updateLineEdits();
}


void OtguiPlotWidget::updatePlotLabel()
{
  QPixmap pixmap(200,200);
  pixmap.fill();
  QwtPlotRenderer renderer;
  renderer.renderTo(this, pixmap);
  plotLabel_->setPixmap(pixmap);
}


QLabel * OtguiPlotWidget::getPlotLabel() const
{
  return plotLabel_;
}


}
