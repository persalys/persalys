#include "otgui/ImageEditionDialog.hxx"

#include <QGridLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QGroupBox>
#include <QTabWidget>
#include <QLabel>
#include <QFileDialog>
#include <QImageWriter>

#include <qwt_plot_renderer.h>

namespace OTGUI {

ImageEditionDialog::ImageEditionDialog(PlotWidget* plotWidget, bool plotWidgetIsBarChart)
: QDialog()
, plotWidget_(plotWidget)
, plotWidgetIsBarChart_(plotWidgetIsBarChart)
{
  QGridLayout * dialogMainGridLayout = new QGridLayout(this);

  // Bottom layout
  QHBoxLayout * hboxForBottomButtons = new QHBoxLayout;
  hboxForBottomButtons->addStretch();
  QPushButton * button = new QPushButton(tr("Export"));
  connect(button, SIGNAL(clicked()), this, SLOT(exportPlot()));
  hboxForBottomButtons->addWidget(button);
  button = new QPushButton(tr("Cancel"));
  connect(button, SIGNAL(clicked()), this, SLOT(reject()));
  hboxForBottomButtons->addWidget(button);
  button = new QPushButton(tr("Ok"));
  connect(button, SIGNAL(clicked()), this, SLOT(accept()));
  hboxForBottomButtons->addWidget(button);

  dialogMainGridLayout->addLayout(hboxForBottomButtons, 1, 0, 1, 1);

  // Top layout
  QHBoxLayout * mainHbox = new QHBoxLayout;

  // - preview zone
  QGroupBox * previewGroupBox = new QGroupBox(tr("Preview"));
  QHBoxLayout * plotHbox = new QHBoxLayout(previewGroupBox);
  plotHbox->addWidget(plotWidget);

  mainHbox->addWidget(previewGroupBox);

  // - configuration zone
  QVBoxLayout * vbox = new QVBoxLayout;

  // -- Graph groupbox
  QGroupBox * graphGroupBox = new QGroupBox(tr("Graph"));
  QGridLayout * gridLayoutTab = new QGridLayout(graphGroupBox);

  QLabel * label = new QLabel(tr("Title"));
  gridLayoutTab->addWidget(label, 0, 0, 1, 1);

  titleLineEdit_ = new QLineEdit;
  connect(titleLineEdit_, SIGNAL(textChanged(QString)), this, SLOT(updateTitle()));
  gridLayoutTab->addWidget(titleLineEdit_, 0, 1, 1, 1);

  //TODO: add Legend?
//   QwtLegend *legend = new QwtLegend ;
//   plotWidget_->insertLegend(legend, QwtPlot::BottomLegend);

  vbox->addWidget(graphGroupBox);

  // -- Columns groupbox
  QGroupBox * columnsGroupBox = new QGroupBox(tr("Columns"));
  QHBoxLayout * columnsGroupBoxHbox = new QHBoxLayout(columnsGroupBox);
  QTabWidget * tabWidget = new QTabWidget;

  // --- tab Horizontal Axis
  QWidget * tabHorizontalAxis = new QWidget;
  gridLayoutTab = new QGridLayout(tabHorizontalAxis);

  label = new QLabel(tr("X-label"));
  gridLayoutTab->addWidget(label, 0, 0, 1, 1);

  xlabelLineEdit_ = new QLineEdit;
  connect(xlabelLineEdit_, SIGNAL(textChanged(QString)), this, SLOT(updateXLabel()));
  gridLayoutTab->addWidget(xlabelLineEdit_, 0, 1, 1, 1);

  label = new QLabel(tr("Xmin"));
  gridLayoutTab->addWidget(label, 1, 0, 1, 1);

  xmin_ = new QLineEdit;
  connect(xmin_, SIGNAL(textChanged(QString)), this, SLOT(updateXrange()));
  gridLayoutTab->addWidget(xmin_, 1, 1, 1, 1);

  label = new QLabel(tr("Xmax"));
  gridLayoutTab->addWidget(label, 2, 0, 1, 1);

  xmax_ = new QLineEdit;
  connect(xmax_, SIGNAL(textChanged(QString)), this, SLOT(updateXrange()));
  gridLayoutTab->addWidget(xmax_, 2, 1, 1, 1);

  gridLayoutTab->setRowStretch(3, 1);

  if (plotWidgetIsBarChart_)
  {
    xmin_->setEnabled(false);
    xmax_->setEnabled(false);
  }

  tabWidget->addTab(tabHorizontalAxis, tr("Horizontal axis"));

  // --- tab Vertical Axis
  QWidget * tabVerticalAxis = new QWidget;
  gridLayoutTab = new QGridLayout(tabVerticalAxis);
  label = new QLabel(tr("Y-label"));
  gridLayoutTab->addWidget(label, 0, 0, 1, 1);

  ylabelLineEdit_ = new QLineEdit;
  connect(ylabelLineEdit_, SIGNAL(textChanged(QString)), this, SLOT(updateYLabel()));
  gridLayoutTab->addWidget(ylabelLineEdit_, 0, 1, 1, 1);

  label = new QLabel(tr("Ymin"));
  gridLayoutTab->addWidget(label, 1, 0, 1, 1);

  ymin_ = new QLineEdit;
  connect(ymin_, SIGNAL(textChanged(QString)), this, SLOT(updateYrange()));
  gridLayoutTab->addWidget(ymin_, 1, 1, 1, 1);

  label = new QLabel(tr("Ymax"));
  gridLayoutTab->addWidget(label, 2, 0, 1, 1);

  ymax_ = new QLineEdit;
  connect(ymax_, SIGNAL(textChanged(QString)), this, SLOT(updateYrange()));
  gridLayoutTab->addWidget(ymax_, 2, 1, 1, 1);

  gridLayoutTab->setRowStretch(3, 1);

  tabWidget->addTab(tabVerticalAxis, tr("Vertical axis"));

  columnsGroupBoxHbox->addWidget(tabWidget);
  vbox->addWidget(columnsGroupBox);

  mainHbox->addLayout(vbox);
  dialogMainGridLayout->addLayout(mainHbox, 0, 0, 1, 1);

  updateLineEdits();
  setInitParameters();
}


void ImageEditionDialog::updateLineEdits()
{
  xlabelLineEdit_->blockSignals(true);
  xmin_->blockSignals(true);
  xmax_->blockSignals(true);
  ymin_->blockSignals(true);
  ymax_->blockSignals(true);
  ylabelLineEdit_->blockSignals(true);
  titleLineEdit_->blockSignals(true);

  titleLineEdit_->setText(plotWidget_->title().text());
  xlabelLineEdit_->setText(plotWidget_->axisTitle(QwtPlot::xBottom).text());
  xmin_->setText(QString::number(plotWidget_->axisInterval(QwtPlot::xBottom).minValue()));
  xmax_->setText(QString::number(plotWidget_->axisInterval(QwtPlot::xBottom).maxValue()));
  ylabelLineEdit_->setText(plotWidget_->axisTitle(QwtPlot::yLeft).text());
  ymin_->setText(QString::number(plotWidget_->axisInterval(QwtPlot::yLeft).minValue()));
  ymax_->setText(QString::number(plotWidget_->axisInterval(QwtPlot::yLeft).maxValue()));

  xlabelLineEdit_->blockSignals(false);
  xmin_->blockSignals(false);
  xmax_->blockSignals(false);
  ymin_->blockSignals(false);
  ymax_->blockSignals(false);
  ylabelLineEdit_->blockSignals(false);
  titleLineEdit_->blockSignals(false);
}


void ImageEditionDialog::setInitParameters()
{
  initTitle_ = plotWidget_->title().text();
  initXlabel_ = plotWidget_->axisTitle(QwtPlot::xBottom).text();
  initXmin_ = plotWidget_->axisInterval(QwtPlot::xBottom).minValue();
  initXmax_ = plotWidget_->axisInterval(QwtPlot::xBottom).maxValue();
  initXstep_ = plotWidget_->axisStepSize(QwtPlot::xBottom);
  initYlabel_ = plotWidget_->axisTitle(QwtPlot::yLeft).text();
  initYmin_ = plotWidget_->axisInterval(QwtPlot::yLeft).minValue();
  initYmax_ = plotWidget_->axisInterval(QwtPlot::yLeft).maxValue();
}


void ImageEditionDialog::resetParameters()
{
  plotWidget_->setTitle(initTitle_);
  plotWidget_->setAxisTitle(QwtPlot::xBottom, initXlabel_);
  plotWidget_->setAxisTitle(QwtPlot::yLeft, initYlabel_);
  plotWidget_->setAxisScale(QwtPlot::xBottom, initXmin_, initXmax_, initXstep_);
  plotWidget_->setAxisScale(QwtPlot::yLeft, initYmin_, initYmax_);
}


void ImageEditionDialog::updateTitle()
{
  plotWidget_->setTitle(titleLineEdit_->text());
  plotWidget_->replot();
}


void ImageEditionDialog::updateXLabel()
{
  plotWidget_->setAxisTitle(QwtPlot::xBottom, xlabelLineEdit_->text());
  plotWidget_->replot();
}


void ImageEditionDialog::updateYLabel()
{
  plotWidget_->setAxisTitle(QwtPlot::yLeft, ylabelLineEdit_->text());
  plotWidget_->replot();
}


void ImageEditionDialog::updateXrange()
{
  bool okMin;
  double valueMin = xmin_->text().toDouble(&okMin);
  bool okMax;
  double valueMax = xmax_->text().toDouble(&okMax);
  
  if (okMin && okMax && valueMin < valueMax)
  {
    plotWidget_->setAxisScale(QwtPlot::xBottom, valueMin, valueMax);
    plotWidget_->replot();
  }
}


void ImageEditionDialog::updateYrange()
{
  bool okMin;
  double valueMin = ymin_->text().toDouble(&okMin);
  bool okMax;
  double valueMax = ymax_->text().toDouble(&okMax);

  if (okMin && okMax && valueMin < valueMax)
  {
    plotWidget_->setAxisScale(QwtPlot::yLeft, valueMin, valueMax);
    plotWidget_->replot();
  }
}


void ImageEditionDialog::exportPlot()
{
  QString fileName = QFileDialog::getSaveFileName(this, tr("Export plot"),
                     QDir::homePath(),
                     tr("Images (*.png *.svg *.pdf *.ps *.bmp *.ppm)"));

  if (!fileName.isEmpty())
  {
    const QString format = QFileInfo(fileName).suffix().toLower();

    if (format == "")
    {
      fileName += ".png";
      format == QString("png");
    }
    if (format == "png" || format == "ps" || format == "pdf" )
    {
      QwtPlotRenderer * renderer = new QwtPlotRenderer();
      renderer->renderDocument(plotWidget_, fileName, QSizeF(150, 100));
    }
    else
    {
      if (QImageWriter::supportedImageFormats().indexOf(format.toLatin1() ) >= 0)
      {
        QPixmap pixmap(200,200);
        pixmap.fill();
        QwtPlotRenderer renderer;

        renderer.renderTo(plotWidget_, pixmap);

        bool saveOperationSucceed = pixmap.save(fileName, format.toLatin1());
//         if (!saveOperationSucceed)
//           std::cout<<"Export doesn't work\n";
      }
      else
      {
//         std::cout<<"Format not supported\n";
      }
    }
  }
}


}
