//                                               -*- C++ -*-
/**
 *  @brief QWidget to configure graphics
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
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
#include "otgui/GraphConfigurationWidget.hxx"

#include "otgui/QtTools.hxx"

#include <QGridLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QGroupBox>
#include <QLabel>
#include <QFileDialog>
#include <QImageWriter>
#include <QRadioButton>
#include <QScrollArea>
#include <QCheckBox>

#include <qwt_plot_renderer.h>

namespace OTGUI
{

GraphConfigurationWidget::GraphConfigurationWidget(QVector<PlotWidget *> plotWidgets,
    QStringList inputNames,
    QStringList outputNames,
    GraphConfigurationWidget::Type plotType,
    QWidget * parent)
  : QWidget(parent)
  , plotWidgets_(plotWidgets)
  , plotType_(plotType)
  , currentPlotIndex_(0)
  , inputNames_(inputNames)
  , outputNames_(outputNames)
  , rankCheckBox_(0)
  , distReprComboBox_(0)
  , xAxisComboBox_(0)
  , yAxisComboBox_(0)
  , titleLineEdit_(0)
  , xlabelLineEdit_(0)
  , xmin_(0)
  , xmax_(0)
  , ylabelLineEdit_(0)
  , ymin_(0)
  , ymax_(0)
{
  buildInterface();
}


GraphConfigurationWidget::GraphConfigurationWidget(PlotWidget * plotWidget,
    QStringList inputNames,
    QStringList outputNames,
    GraphConfigurationWidget::Type plotType,
    QWidget * parent)
  : QWidget(parent)
  , plotWidgets_(1, plotWidget)
  , plotType_(plotType)
  , currentPlotIndex_(0)
  , inputNames_(inputNames)
  , outputNames_(outputNames)
  , rankCheckBox_(0)
  , distReprComboBox_(0)
  , xAxisComboBox_(0)
  , yAxisComboBox_(0)
  , titleLineEdit_(0)
  , xlabelLineEdit_(0)
  , xmin_(0)
  , xmax_(0)
  , ylabelLineEdit_(0)
  , ymin_(0)
  , ymax_(0)
{
  buildInterface();
}


void GraphConfigurationWidget::buildInterface()
{
  for (int i = 0; i < plotWidgets_.size(); ++i)
    connect(plotWidgets_[i], SIGNAL(plotChanged()), this, SLOT(updateLineEdits()));

  QVBoxLayout * mainLayout = new QVBoxLayout(this);
  setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Maximum);

  QScrollArea * scrollArea = new QScrollArea;
  scrollArea->setWidgetResizable(true);
  QFrame * frame = new QFrame;
  QGridLayout * mainGridLayout = new QGridLayout(frame);
  int rowGrid = 0;

  // combobox PDF - CDF - quantile function - Survival function
  if (plotType_ == GraphConfigurationWidget::PDF ||
      plotType_ == GraphConfigurationWidget::PDF_Inference ||
      plotType_ == GraphConfigurationWidget::PDFResult ||
      plotType_ == GraphConfigurationWidget::Copula)
  {
    QStringList distReprs = QStringList() << tr("PDF") << tr("CDF");
    if (plotType_ == GraphConfigurationWidget::PDF)
      distReprs << tr("Quantile function") << tr("Survival function");
    distReprComboBox_ = new QComboBox;
    distReprComboBox_->addItems(distReprs);
    mainGridLayout->addWidget(distReprComboBox_, rowGrid, 0, 1, 2);
    connect(distReprComboBox_, SIGNAL(currentIndexChanged(int)), this, SLOT(plotChanged()));
  }

  // title
  QLabel * label = new QLabel(tr("Title"));
  mainGridLayout->addWidget(label, ++rowGrid, 0, 1, 1);

  titleLineEdit_ = new QLineEdit;
  connect(titleLineEdit_, SIGNAL(textChanged(QString)), this, SLOT(updateTitle()));
  mainGridLayout->addWidget(titleLineEdit_, rowGrid, 1, 1, 1);

  // Axis comboboxes
  if (plotType_ == GraphConfigurationWidget::Scatter ||
      plotType_ == GraphConfigurationWidget::Copula  ||
      plotType_ == GraphConfigurationWidget::Kendall)
  {
    // X-axis combobox
    label = new QLabel(tr("X-axis"));
    mainGridLayout->addWidget(label, ++rowGrid, 0, 1, 1);

    xAxisComboBox_ = new QComboBox;

    for (int i = 0; i < inputNames_.size(); ++i)
      xAxisComboBox_->addItem(inputNames_[i], true);

    for (int i = 0; i < outputNames_.size(); ++i)
      xAxisComboBox_->addItem(outputNames_[i], false);

    mainGridLayout->addWidget(xAxisComboBox_, rowGrid, 1, 1, 1);
    if (plotType_ != GraphConfigurationWidget::Kendall)
    {
      connect(xAxisComboBox_, SIGNAL(currentIndexChanged(int)), this, SLOT(updateYComboBox()));

      // Y-axis combobox
      label = new QLabel(tr("Y-axis"));
      mainGridLayout->addWidget(label, ++rowGrid, 0, 1, 1);

      yAxisComboBox_ = new QComboBox;
      mainGridLayout->addWidget(yAxisComboBox_, rowGrid, 1, 1, 1);
      connect(yAxisComboBox_, SIGNAL(currentIndexChanged(int)), this, SLOT(plotChanged()));
    }
    else
      connect(xAxisComboBox_, SIGNAL(currentIndexChanged(int)), this, SLOT(plotChanged()));
  }

  // radio button ranks
  if (plotType_ == GraphConfigurationWidget::Scatter)
  {
    rankCheckBox_ = new QCheckBox(tr("Ranks"));
    mainGridLayout->addWidget(rankCheckBox_, ++rowGrid, 0, 1, 2);
    connect(rankCheckBox_, SIGNAL(stateChanged(int)), this, SLOT(plotChanged()));
  }

  // label direction
  if (plotType_ == GraphConfigurationWidget::SensitivityIndices)
  {
    label = new QLabel(tr("X-axis labels\norientation"));
    QComboBox * labelOrientation = new QComboBox;
    labelOrientation->addItems(QStringList() << tr("Horizontal") << tr("Vertical"));
    mainGridLayout->addWidget(label, ++rowGrid, 0, 1, 1);
    mainGridLayout->addWidget(labelOrientation, rowGrid, 1, 1, 1);
    connect(labelOrientation, SIGNAL(currentIndexChanged(int)), this, SLOT(changeLabelOrientation(int)));
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

  xmin_ = new ValueLineEdit;
  connect(xmin_, SIGNAL(editingFinished()), this, SLOT(updateXrange()));
  gridLayoutTab->addWidget(xmin_, 1, 1, 1, 1);

  label = new QLabel(tr("Max"));
  gridLayoutTab->addWidget(label, 2, 0, 1, 1);

  xmax_ = new ValueLineEdit;
  connect(xmax_, SIGNAL(editingFinished()), this, SLOT(updateXrange()));
  gridLayoutTab->addWidget(xmax_, 2, 1, 1, 1);

  gridLayoutTab->setRowStretch(3, 1);

  tabWidget->addTab(tabHorizontalAxis, tr("X-axis"));

  // --- tab Vertical Axis
  QWidget * tabVerticalAxis = new QWidget;
  gridLayoutTab = new QGridLayout(tabVerticalAxis);
  label = new QLabel(tr("Title"));
  gridLayoutTab->addWidget(label, 0, 0, 1, 1);

  ylabelLineEdit_ = new QLineEdit;
  connect(ylabelLineEdit_, SIGNAL(textChanged(QString)), this, SLOT(updateYLabel()));
  gridLayoutTab->addWidget(ylabelLineEdit_, 0, 1, 1, 1);

  label = new QLabel(tr("Min"));
  gridLayoutTab->addWidget(label, 1, 0, 1, 1);

  ymin_ = new ValueLineEdit;
  connect(ymin_, SIGNAL(editingFinished()), this, SLOT(updateYrange()));
  gridLayoutTab->addWidget(ymin_, 1, 1, 1, 1);

  label = new QLabel(tr("Max"));
  gridLayoutTab->addWidget(label, 2, 0, 1, 1);

  ymax_ = new ValueLineEdit;
  connect(ymax_, SIGNAL(editingFinished()), this, SLOT(updateYrange()));
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

  mainGridLayout->setRowStretch(++rowGrid, 1);

  // update widgets
  updateLineEdits();
  updateYComboBox();

  //
  scrollArea->setWidget(frame);
  mainLayout->addWidget(scrollArea);
  setVisible(false);
}


void GraphConfigurationWidget::updateLineEdits()
{
  if (!plotWidgets_.size())
    return;

  SignalBlocker xlabelLineEditBlocker(xlabelLineEdit_);
  SignalBlocker xminBlocker(xmin_);
  SignalBlocker xmaxBlocker(xmax_);
  SignalBlocker yminBlocker(ymin_);
  SignalBlocker ymaxBlocker(ymax_);
  SignalBlocker ylabelLineEditBlocker(ylabelLineEdit_);
  SignalBlocker titleLineEditBlocker(titleLineEdit_);

  titleLineEdit_->setText(plotWidgets_[currentPlotIndex_]->title().text());
  xlabelLineEdit_->setText(plotWidgets_[currentPlotIndex_]->axisTitle(QwtPlot::xBottom).text());
  xmin_->setValue(plotWidgets_[currentPlotIndex_]->axisInterval(QwtPlot::xBottom).minValue());
  xmax_->setValue(plotWidgets_[currentPlotIndex_]->axisInterval(QwtPlot::xBottom).maxValue());
  ylabelLineEdit_->setText(plotWidgets_[currentPlotIndex_]->axisTitle(QwtPlot::yLeft).text());
  ymin_->setValue(plotWidgets_[currentPlotIndex_]->axisInterval(QwtPlot::yLeft).minValue());
  ymax_->setValue(plotWidgets_[currentPlotIndex_]->axisInterval(QwtPlot::yLeft).maxValue());

  if (plotType_ == GraphConfigurationWidget::SensitivityIndices)
  {
    xmin_->setEnabled(false);
    xmax_->setEnabled(false);
    ymin_->setEnabled(false);
    ymax_->setEnabled(false);
    xmin_->clear();
    xmax_->clear();
    ymin_->clear();
    ymax_->clear();
  }
}


void GraphConfigurationWidget::updateYComboBox()
{
  if (!xAxisComboBox_ || !yAxisComboBox_)
    return;

  SignalBlocker blocker(yAxisComboBox_);
  yAxisComboBox_->clear();

  QStringList inputNames;
  QStringList outputNames;

  for (int i = 0; i < xAxisComboBox_->count(); ++i)
  {
    if (i != xAxisComboBox_->currentIndex()) // must have x != y
    {
      // == input
      if (xAxisComboBox_->itemData(i).toBool())
        inputNames << xAxisComboBox_->itemText(i);
      // == output
      else
        outputNames << xAxisComboBox_->itemText(i);
    }
  }

  if (plotType_ == GraphConfigurationWidget::Scatter)
    yAxisComboBox_->addItems(outputNames + inputNames);
  else if (plotType_ == GraphConfigurationWidget::Copula)
    yAxisComboBox_->addItems(inputNames + outputNames);

  plotChanged();
}


int GraphConfigurationWidget::getCurrentPlotIndex() const
{
  return currentPlotIndex_;
}


void GraphConfigurationWidget::plotChanged()
{
  int outputIndex = 0;
  if (yAxisComboBox_)
    outputIndex = yAxisComboBox_->currentIndex();

  currentPlotIndex_ = outputIndex;

  if (plotType_ == GraphConfigurationWidget::Scatter && xAxisComboBox_ && yAxisComboBox_ && rankCheckBox_)
  {
    if (!rankCheckBox_->isChecked())
      currentPlotIndex_ = 2 * (xAxisComboBox_->currentIndex() * yAxisComboBox_->count() + outputIndex);
    else
      currentPlotIndex_ = 2 * (xAxisComboBox_->currentIndex() * yAxisComboBox_->count() + outputIndex) + 1;
  }
  else if (plotType_ == GraphConfigurationWidget::Copula && distReprComboBox_ && xAxisComboBox_ && yAxisComboBox_)
  {
    if (distReprComboBox_->currentIndex() == 0)
      currentPlotIndex_ = 2 * (xAxisComboBox_->currentIndex() * yAxisComboBox_->count() + outputIndex);
    else
      currentPlotIndex_ = 2 * (xAxisComboBox_->currentIndex() * yAxisComboBox_->count() + outputIndex) + 1;
  }
  else if (plotType_ == GraphConfigurationWidget::Kendall && xAxisComboBox_)
  {
    currentPlotIndex_ = xAxisComboBox_->currentIndex();
  }
  else if (distReprComboBox_ && (plotType_ == GraphConfigurationWidget::PDF ||
                                 plotType_ == GraphConfigurationWidget::PDF_Inference ||
                                 plotType_ == GraphConfigurationWidget::PDFResult))
  {
    currentPlotIndex_ = 2 * outputIndex + distReprComboBox_->currentIndex();
  }

  updateLineEdits();
  emit currentPlotChanged(currentPlotIndex_);
}


void GraphConfigurationWidget::updateTitle()
{
  if (!plotWidgets_.size())
    return;

  plotWidgets_[currentPlotIndex_]->setTitle(titleLineEdit_->text());
  plotWidgets_[currentPlotIndex_]->replot();
}


void GraphConfigurationWidget::updateXLabel()
{
  if (!plotWidgets_.size())
    return;

  plotWidgets_[currentPlotIndex_]->setAxisTitle(QwtPlot::xBottom, xlabelLineEdit_->text());
  plotWidgets_[currentPlotIndex_]->replot();
}


void GraphConfigurationWidget::updateYLabel()
{
  if (!plotWidgets_.size())
    return;

  plotWidgets_[currentPlotIndex_]->setAxisTitle(QwtPlot::yLeft, ylabelLineEdit_->text());
  plotWidgets_[currentPlotIndex_]->replot();
}


void GraphConfigurationWidget::updateXrange()
{
  if (!plotWidgets_.size())
    return;

  try
  {
    plotWidgets_[currentPlotIndex_]->setAxisScale(QwtPlot::xBottom, xmin_->value(), xmax_->value());
    plotWidgets_[currentPlotIndex_]->replot();
  }
  catch (std::exception & ex)
  {
    updateLineEdits();
    qDebug() << "GraphConfigurationWidget::updateXrange: value not valid\n";
  }
}


void GraphConfigurationWidget::updateYrange()
{
  if (!plotWidgets_.size())
    return;

  try
  {
    plotWidgets_[currentPlotIndex_]->setAxisScale(QwtPlot::yLeft, ymin_->value(), ymax_->value());
    plotWidgets_[currentPlotIndex_]->replot();
  }
  catch (std::exception & ex)
  {
    updateLineEdits();
    qDebug() << "GraphConfigurationWidget::updateYrange: value not valid\n";
  }
}


void GraphConfigurationWidget::changeLabelOrientation(int index)
{
  if (index == 0)
  {
    plotWidgets_[currentPlotIndex_]->setAxisLabelAlignment(QwtPlot::xBottom, Qt::AlignBottom);
    plotWidgets_[currentPlotIndex_]->setAxisLabelRotation(QwtPlot::xBottom, 0);
  }
  else
  {
    plotWidgets_[currentPlotIndex_]->setAxisLabelAlignment(QwtPlot::xBottom, Qt::AlignLeft);
    plotWidgets_[currentPlotIndex_]->setAxisLabelRotation(QwtPlot::xBottom, -90);
  }
  plotWidgets_[currentPlotIndex_]->replot();
}


void GraphConfigurationWidget::exportPlot()
{
  if (!plotWidgets_.size())
    return;

  plotWidgets_[currentPlotIndex_]->exportPlot();
}


QSize GraphConfigurationWidget::minimumSizeHint() const
{
  QSize size = QWidget::minimumSizeHint();
  size.setHeight(10);
  return size;
}
}
