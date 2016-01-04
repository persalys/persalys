#include "otgui/GraphConfigurationWidget.hxx"

#include <QGridLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QGroupBox>
#include <QTabWidget>
#include <QLabel>
#include <QFileDialog>
#include <QImageWriter>
#include <QRadioButton>

#include <qwt_plot_renderer.h>

namespace OTGUI {

GraphConfigurationWidget::GraphConfigurationWidget(QVector<PlotWidget *> plotWidgets, QStringList inputNames,
                                                   QStringList outputNames,
                                                   GraphConfigurationWidget::Type plotType)
  : QTabWidget()
  , plotWidgets_(plotWidgets)
  , plotType_(plotType)
  , currentPlotIndex_(0)
{
  for (int i=0; i<plotWidgets_.size(); ++i)
    connect(plotWidgets_[i], SIGNAL(plotChanged()), this, SLOT(updateLineEdits()));

  QGridLayout * mainGridLayout = new QGridLayout(this);
  int rowGrid = 0;

  QLabel * label = new QLabel(tr("Title"));
  mainGridLayout->addWidget(label, rowGrid, 0, 1, 1);
  titleLineEdit_ = new QLineEdit;
  connect(titleLineEdit_, SIGNAL(textChanged(QString)), this, SLOT(updateTitle()));
  mainGridLayout->addWidget(titleLineEdit_, rowGrid, 1, 1, 1);

  xAxisComboBox_ = 0;
  if (plotType_ == GraphConfigurationWidget::Scatter)
  {
    label = new QLabel(tr("X-axis"));
    mainGridLayout->addWidget(label, ++rowGrid, 0, 1, 1);

    xAxisComboBox_ = new QComboBox;
    xAxisComboBox_->addItems(inputNames);
    mainGridLayout->addWidget(xAxisComboBox_, rowGrid, 1, 1, 1);
    connect(xAxisComboBox_, SIGNAL(currentIndexChanged(int)), this, SLOT(plotChanged()));
  }

  label = new QLabel(tr("Y-axis"));
  mainGridLayout->addWidget(label, ++rowGrid, 0, 1, 1);
  yAxisComboBox_ = new QComboBox;
  yAxisComboBox_->addItems(outputNames);
  mainGridLayout->addWidget(yAxisComboBox_, rowGrid, 1, 1, 1);
  connect(yAxisComboBox_, SIGNAL(currentIndexChanged(int)), this, SLOT(plotChanged()));
  emit currentPlotChanged(currentPlotIndex_);

  pdf_cdfGroup_ = 0;
  if (plotType_ == GraphConfigurationWidget::PDF)
  {
    pdf_cdfGroup_ = new QButtonGroup;
    QRadioButton * buttonToChoosePDForCDF = new QRadioButton(tr("PDF"));
    buttonToChoosePDForCDF->setChecked(true);
    pdf_cdfGroup_->addButton(buttonToChoosePDForCDF, GraphConfigurationWidget::PDF);
    mainGridLayout->addWidget(buttonToChoosePDForCDF, ++rowGrid, 0, 1, 1);
    buttonToChoosePDForCDF = new QRadioButton(tr("CDF"));
    pdf_cdfGroup_->addButton(buttonToChoosePDForCDF, GraphConfigurationWidget::CDF);
    mainGridLayout->addWidget(buttonToChoosePDForCDF, rowGrid, 1, 1, 1);
    connect(pdf_cdfGroup_, SIGNAL(buttonClicked(int)), this, SLOT(plotChanged()));
  }
  //TODO: add Legend?
//   QwtLegend *legend = new QwtLegend ;
//   plotWidgets_[currentPlotIndex_]->insertLegend(legend, QwtPlot::BottomLegend);

  QTabWidget * tabWidget = new QTabWidget;

  // --- tab Horizontal Axis
  QWidget * tabHorizontalAxis = new QWidget;
  QGridLayout * gridLayoutTab = new QGridLayout(tabHorizontalAxis);

  label = new QLabel(tr("Title"));
  gridLayoutTab->addWidget(label, 0, 0, 1, 1);

  xlabelLineEdit_ = new QLineEdit;
  connect(xlabelLineEdit_, SIGNAL(textChanged(QString)), this, SLOT(updateXLabel()));
  gridLayoutTab->addWidget(xlabelLineEdit_, 0, 1, 1, 1);

  label = new QLabel(tr("Min"));
  gridLayoutTab->addWidget(label, 1, 0, 1, 1);

  xmin_ = new QLineEdit;
  connect(xmin_, SIGNAL(textChanged(QString)), this, SLOT(updateXrange()));
  gridLayoutTab->addWidget(xmin_, 1, 1, 1, 1);

  label = new QLabel(tr("Max"));
  gridLayoutTab->addWidget(label, 2, 0, 1, 1);

  xmax_ = new QLineEdit;
  connect(xmax_, SIGNAL(textChanged(QString)), this, SLOT(updateXrange()));
  gridLayoutTab->addWidget(xmax_, 2, 1, 1, 1);

  gridLayoutTab->setRowStretch(3, 1);

  tabWidget->addTab(tabHorizontalAxis, tr("X-axis"));

  // --- tab Vertical Axis
  QWidget * tabVerticalAxis = new QWidget;
  gridLayoutTab = new QGridLayout(tabVerticalAxis);
  label = new QLabel(tr("Label"));
  gridLayoutTab->addWidget(label, 0, 0, 1, 1);

  ylabelLineEdit_ = new QLineEdit;
  connect(ylabelLineEdit_, SIGNAL(textChanged(QString)), this, SLOT(updateYLabel()));
  gridLayoutTab->addWidget(ylabelLineEdit_, 0, 1, 1, 1);

  label = new QLabel(tr("Min"));
  gridLayoutTab->addWidget(label, 1, 0, 1, 1);

  ymin_ = new QLineEdit;
  connect(ymin_, SIGNAL(textChanged(QString)), this, SLOT(updateYrange()));
  gridLayoutTab->addWidget(ymin_, 1, 1, 1, 1);

  label = new QLabel(tr("Max"));
  gridLayoutTab->addWidget(label, 2, 0, 1, 1);

  ymax_ = new QLineEdit;
  connect(ymax_, SIGNAL(textChanged(QString)), this, SLOT(updateYrange()));
  gridLayoutTab->addWidget(ymax_, 2, 1, 1, 1);

  gridLayoutTab->setRowStretch(3, 1);

  tabWidget->addTab(tabVerticalAxis, tr("Y-axis"));

  mainGridLayout->addWidget(tabWidget, ++rowGrid, 0, 1, 2);

  // Bottom layout
  QHBoxLayout * hboxForBottomButtons = new QHBoxLayout;
  hboxForBottomButtons->addStretch();
  QPushButton * button = new QPushButton(tr("Export"));
  connect(button, SIGNAL(clicked()), this, SLOT(exportPlot()));
  hboxForBottomButtons->addWidget(button);

  mainGridLayout->addLayout(hboxForBottomButtons, ++rowGrid, 1, 1, 1);

  updateLineEdits();
}


void GraphConfigurationWidget::updateLineEdits()
{
  xlabelLineEdit_->blockSignals(true);
  xmin_->blockSignals(true);
  xmax_->blockSignals(true);
  ymin_->blockSignals(true);
  ymax_->blockSignals(true);
  ylabelLineEdit_->blockSignals(true);
  titleLineEdit_->blockSignals(true);

  titleLineEdit_->setText(plotWidgets_[currentPlotIndex_]->title().text());
  xlabelLineEdit_->setText(plotWidgets_[currentPlotIndex_]->axisTitle(QwtPlot::xBottom).text());
  xmin_->setText(QString::number(plotWidgets_[currentPlotIndex_]->axisInterval(QwtPlot::xBottom).minValue()));
  xmax_->setText(QString::number(plotWidgets_[currentPlotIndex_]->axisInterval(QwtPlot::xBottom).maxValue()));
  ylabelLineEdit_->setText(plotWidgets_[currentPlotIndex_]->axisTitle(QwtPlot::yLeft).text());
  ymin_->setText(QString::number(plotWidgets_[currentPlotIndex_]->axisInterval(QwtPlot::yLeft).minValue()));
  ymax_->setText(QString::number(plotWidgets_[currentPlotIndex_]->axisInterval(QwtPlot::yLeft).maxValue()));

  xlabelLineEdit_->blockSignals(false);
  xmin_->blockSignals(false);
  xmax_->blockSignals(false);
  ymin_->blockSignals(false);
  ymax_->blockSignals(false);
  ylabelLineEdit_->blockSignals(false);
  titleLineEdit_->blockSignals(false);
}


void GraphConfigurationWidget::plotChanged()
{
  int outputIndex = yAxisComboBox_->currentIndex();
  if (plotType_ == GraphConfigurationWidget::Scatter)
  {
    int inputIndex = xAxisComboBox_->currentIndex();
    currentPlotIndex_ = outputIndex * xAxisComboBox_->count() + inputIndex;
  }
  else if (plotType_ == GraphConfigurationWidget::PDF || plotType_ == GraphConfigurationWidget::CDF)
  {
    
    switch (GraphConfigurationWidget::Type(pdf_cdfGroup_->checkedId()))
    {
      case GraphConfigurationWidget::PDF:
      {
        currentPlotIndex_ = 2 * outputIndex;
        break;
      }
      case GraphConfigurationWidget::CDF:
      {
        currentPlotIndex_ = 2 * outputIndex + 1;
        break;
      }
    }
  }
  else
    currentPlotIndex_ = outputIndex;
  updateLineEdits();
  emit currentPlotChanged(currentPlotIndex_);
}


void GraphConfigurationWidget::updateTitle()
{
  plotWidgets_[currentPlotIndex_]->setTitle(titleLineEdit_->text());
  plotWidgets_[currentPlotIndex_]->replot();
}


void GraphConfigurationWidget::updateXLabel()
{
  plotWidgets_[currentPlotIndex_]->setAxisTitle(QwtPlot::xBottom, xlabelLineEdit_->text());
  plotWidgets_[currentPlotIndex_]->replot();
}


void GraphConfigurationWidget::updateYLabel()
{
  plotWidgets_[currentPlotIndex_]->setAxisTitle(QwtPlot::yLeft, ylabelLineEdit_->text());
  plotWidgets_[currentPlotIndex_]->replot();
}


void GraphConfigurationWidget::updateXrange()
{
  bool okMin;
  double valueMin = xmin_->text().toDouble(&okMin);
  bool okMax;
  double valueMax = xmax_->text().toDouble(&okMax);
  
  if (okMin && okMax && valueMin < valueMax)
  {
    plotWidgets_[currentPlotIndex_]->setAxisScale(QwtPlot::xBottom, valueMin, valueMax);
    plotWidgets_[currentPlotIndex_]->replot();
  }
}


void GraphConfigurationWidget::updateYrange()
{
  bool okMin;
  double valueMin = ymin_->text().toDouble(&okMin);
  bool okMax;
  double valueMax = ymax_->text().toDouble(&okMax);

  if (okMin && okMax && valueMin < valueMax)
  {
    plotWidgets_[currentPlotIndex_]->setAxisScale(QwtPlot::yLeft, valueMin, valueMax);
    plotWidgets_[currentPlotIndex_]->replot();
  }
}


void GraphConfigurationWidget::exportPlot()
{
  plotWidgets_[currentPlotIndex_]->exportPlot();
}
}