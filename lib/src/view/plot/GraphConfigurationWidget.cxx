//                                               -*- C++ -*-
/**
 *  @brief QWidget to configure graphics
 *
 *  Copyright 2015-2016 EDF-Phimeca
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

#include <qwt_plot_renderer.h>

namespace OTGUI {

GraphConfigurationWidget::GraphConfigurationWidget(QVector<PlotWidget *> plotWidgets,
                                                   QStringList inputNames,
                                                   QStringList outputNames,
                                                   GraphConfigurationWidget::Type plotType,
                                                   QWidget * parent)
  : QWidget(parent)
  , plotWidgets_(plotWidgets)
  , plotType_(plotType)
  , currentPlotIndex_(0)
  , pdf_cdfGroup_(0)
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
  for (int i=0; i<plotWidgets_.size(); ++i)
    connect(plotWidgets_[i], SIGNAL(plotChanged()), this, SLOT(updateLineEdits()));

  QVBoxLayout * mainLayout = new QVBoxLayout(this);
  QScrollArea * scrollArea = new QScrollArea;
  scrollArea->setWidgetResizable(true);
  QFrame * frame = new QFrame;
  QGridLayout * mainGridLayout = new QGridLayout(frame);
  int rowGrid = 0;

  // title
  QLabel * label = new QLabel(tr("Title"));
  mainGridLayout->addWidget(label, rowGrid, 0, 1, 1);

  titleLineEdit_ = new QLineEdit;
  connect(titleLineEdit_, SIGNAL(textChanged(QString)), this, SLOT(updateTitle()));
  mainGridLayout->addWidget(titleLineEdit_, rowGrid, 1, 1, 1);

  // Axis comboboxes
  if (plotType_ == GraphConfigurationWidget::Scatter || plotType_ == GraphConfigurationWidget::Copula)
  {
    // X-axis combobox
    label = new QLabel(tr("X-axis"));
    mainGridLayout->addWidget(label, ++rowGrid, 0, 1, 1);

    xAxisComboBox_ = new QComboBox;

    for (int i=0; i<inputNames.size(); ++i)
      xAxisComboBox_->addItem(inputNames[i], true);

    for (int i=0; i<outputNames.size(); ++i)
      xAxisComboBox_->addItem(outputNames[i], false);

    mainGridLayout->addWidget(xAxisComboBox_, rowGrid, 1, 1, 1);
    connect(xAxisComboBox_, SIGNAL(currentIndexChanged(int)), this, SLOT(updateYComboBox()));

    // Y-axis combobox
    label = new QLabel(tr("Y-axis"));
    mainGridLayout->addWidget(label, ++rowGrid, 0, 1, 1);

    yAxisComboBox_ = new QComboBox;
    mainGridLayout->addWidget(yAxisComboBox_, rowGrid, 1, 1, 1);
    connect(yAxisComboBox_, SIGNAL(currentIndexChanged(int)), this, SLOT(plotChanged()));
  }

  // buttons PDF - CDF
  if (plotType_ == GraphConfigurationWidget::PDF ||
      plotType_ == GraphConfigurationWidget::PDFResult ||
      plotType_ == GraphConfigurationWidget::Copula)
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

  // update widgets
  updateLineEdits();
  updateYComboBox();

  //
  scrollArea->setWidget(frame);
  mainLayout->addWidget(scrollArea);
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

  for (int i=0; i<xAxisComboBox_->count(); ++i)
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
    yAxisComboBox_->addItems(outputNames+inputNames);
  else if (plotType_ == GraphConfigurationWidget::Copula)
    yAxisComboBox_->addItems(inputNames+outputNames);

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

  if (plotType_ == GraphConfigurationWidget::Scatter && xAxisComboBox_ && yAxisComboBox_)
  {
    currentPlotIndex_ = xAxisComboBox_->currentIndex() * yAxisComboBox_->count() + outputIndex;
  }
  else if (plotType_ == GraphConfigurationWidget::Copula && pdf_cdfGroup_ && xAxisComboBox_ && yAxisComboBox_)
  {
    if (pdf_cdfGroup_->checkedId() == GraphConfigurationWidget::PDF)
      currentPlotIndex_ = 2 * (xAxisComboBox_->currentIndex() * yAxisComboBox_->count() + outputIndex);
    else
      currentPlotIndex_ = 2 * (xAxisComboBox_->currentIndex() * yAxisComboBox_->count() + outputIndex) + 1;
  }
  else if (pdf_cdfGroup_ && (plotType_ == GraphConfigurationWidget::PDF ||
                             plotType_ == GraphConfigurationWidget::PDFResult))
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
      default:
        break;
    }
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


void GraphConfigurationWidget::exportPlot()
{
  if (!plotWidgets_.size())
    return;

  plotWidgets_[currentPlotIndex_]->exportPlot();
}
}