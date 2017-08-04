//                                               -*- C++ -*-
/**
 *  @brief QWidget to configure XYChart
 *
 *  Copyright 2015-2017 EDF-Phimeca
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
#include "otgui/PVXYChartSettingWidget.hxx"

#include "otgui/QtTools.hxx"
#include "otgui/NoWheelEventComboBox.hxx"

#include <vtkPlotPoints.h>

#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QScrollArea>
#include <QToolButton>
#include <QPushButton>
#include <QColorDialog>
#include <QSpinBox>

using namespace OT;

namespace OTGUI {

PVXYChartSettingWidget::PVXYChartSettingWidget(PVXYChartViewWidget * pvViewWidget,
                                               PVXYChartViewWidget * rankPvViewWidget,
                                               QStringList inputNames,
                                               QStringList outputNames,
                                               PVXYChartSettingWidget::Type plotType,
                                               QWidget * parent)
  : QWidget(parent)
  , pvViewWidget_(pvViewWidget)
  , rankPvViewWidget_(rankPvViewWidget)
  , plotType_(plotType)
  , xAxisComboBox_(0)
  , yAxisComboBox_(0)
  , titleLineEdit_(0)
  , xlabelLineEdit_(0)
  , xmin_(0)
  , xmax_(0)
  , xlogScaleCheckBox_(0)
  , ylabelLineEdit_(0)
  , ymin_(0)
  , ymax_(0)
  , ylogScaleCheckBox_(0)
  , colorButton_(0)
  , markerStyles_(0)
  , reprListWidget_(0)
{
  if (!pvViewWidget_)
    throw InvalidArgumentException(HERE) << "PVXYChartSettingWidget: No pvViewWidget !";

  connect(pvViewWidget_, SIGNAL(plotChanged()), this, SLOT(updateLineEdits()));

  if (rankPvViewWidget_)
    connect(rankPvViewWidget_, SIGNAL(plotChanged()), this, SLOT(updateLineEdits()));

  // buildInterface
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
  if (plotType_ == PVXYChartSettingWidget::Scatter)
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

  // rank checkbox
  QCheckBox * rankCheckBox = 0;
  if (plotType_ == PVXYChartSettingWidget::Scatter && rankPvViewWidget_)
  {
    rankPvViewWidget_->setVisible(false);
    rankCheckBox = new QCheckBox(tr("Ranks"));
    mainGridLayout->addWidget(rankCheckBox, ++rowGrid, 0, 1, 2);
    connect(rankCheckBox, SIGNAL(clicked(bool)), rankPvViewWidget_, SLOT(setVisible(bool)));
    connect(rankCheckBox, SIGNAL(clicked(bool)), pvViewWidget_, SLOT(setHidden(bool)));
  }

  // representation
  if (pvViewWidget_->getNumberOfRepresentations() > 1)
  {
    label = new QLabel(tr("Data"));
    mainGridLayout->addWidget(label, ++rowGrid, 0, 1, 1);

    // combobox to select the reprensations to display
    NoWheelEventComboBox * reprComboBox = new NoWheelEventComboBox;
    QStringList reprNames;
    for (int i = 0; i < pvViewWidget_->getNumberOfRepresentations(); ++i)
    {
      reprNames << pvViewWidget_->getRepresentationLabels(i)[0];
    }
    reprListWidget_ = new ListWidgetWithCheckBox("-- " + tr("Select") + " --", reprNames, reprNames, this);
    connect(reprListWidget_, SIGNAL(checkedItemsChanged(QStringList)), this, SLOT(setRepresentationToDisplay()));
    reprComboBox->setModel(reprListWidget_->model());
    reprComboBox->setView(reprListWidget_);

    mainGridLayout->addWidget(reprComboBox, rowGrid, 1, 1, 1);
  }

  // axis - style
  QTabWidget * tabWidget = new QTabWidget;

  // --- tab Horizontal Axis
  QWidget * tabHorizontalAxis = new QWidget;
  QGridLayout * gridLayoutTab = new QGridLayout(tabHorizontalAxis);

  // axis title
  label = new QLabel(tr("Title"));
  gridLayoutTab->addWidget(label, 0, 0, 1, 1);

  xlabelLineEdit_ = new QLineEdit;
  connect(xlabelLineEdit_, SIGNAL(textChanged(QString)), this, SLOT(updateXLabel()));
  gridLayoutTab->addWidget(xlabelLineEdit_, 0, 1, 1, 1);

  // min
  label = new QLabel(tr("Min"));
  gridLayoutTab->addWidget(label, 1, 0, 1, 1);

  xmin_ = new ValueLineEdit;
  connect(xmin_, SIGNAL(editingFinished()), this, SLOT(updateXrange()));
  gridLayoutTab->addWidget(xmin_, 1, 1, 1, 1);

  // max
  label = new QLabel(tr("Max"));
  gridLayoutTab->addWidget(label, 2, 0, 1, 1);

  xmax_ = new ValueLineEdit;
  connect(xmax_, SIGNAL(editingFinished()), this, SLOT(updateXrange()));
  gridLayoutTab->addWidget(xmax_, 2, 1, 1, 1);

  // log scale
  xlogScaleCheckBox_ = new QCheckBox(tr("Log scale"));
  connect(xlogScaleCheckBox_, SIGNAL(clicked(bool)), pvViewWidget_, SLOT(setXLogScale(bool)));
  if (rankCheckBox)
    connect(rankCheckBox, SIGNAL(clicked(bool)), xlogScaleCheckBox_, SLOT(setHidden(bool)));
  gridLayoutTab->addWidget(xlogScaleCheckBox_, 3, 0, 1, 2);

  gridLayoutTab->setRowStretch(4, 1);
  tabWidget->addTab(tabHorizontalAxis, tr("X-axis"));

  // --- tab Vertical Axis
  QWidget * tabVerticalAxis = new QWidget;
  gridLayoutTab = new QGridLayout(tabVerticalAxis);

  // axis title
  label = new QLabel(tr("Title"));
  gridLayoutTab->addWidget(label, 0, 0, 1, 1);

  ylabelLineEdit_ = new QLineEdit;
  connect(ylabelLineEdit_, SIGNAL(textChanged(QString)), this, SLOT(updateYLabel()));
  gridLayoutTab->addWidget(ylabelLineEdit_, 0, 1, 1, 1);

  // min
  label = new QLabel(tr("Min"));
  gridLayoutTab->addWidget(label, 1, 0, 1, 1);

  ymin_ = new ValueLineEdit;
  connect(ymin_, SIGNAL(editingFinished()), this, SLOT(updateYrange()));
  gridLayoutTab->addWidget(ymin_, 1, 1, 1, 1);

  // max
  label = new QLabel(tr("Max"));
  gridLayoutTab->addWidget(label, 2, 0, 1, 1);

  ymax_ = new ValueLineEdit;
  connect(ymax_, SIGNAL(editingFinished()), this, SLOT(updateYrange()));
  gridLayoutTab->addWidget(ymax_, 2, 1, 1, 1);

  // log scale
  ylogScaleCheckBox_ = new QCheckBox(tr("Log scale"));
  connect(ylogScaleCheckBox_, SIGNAL(clicked(bool)), pvViewWidget_, SLOT(setYLogScale(bool)));
  if (rankCheckBox)
    connect(rankCheckBox, SIGNAL(clicked(bool)), ylogScaleCheckBox_, SLOT(setHidden(bool)));
  gridLayoutTab->addWidget(ylogScaleCheckBox_, 3, 0, 1, 2);

  gridLayoutTab->setRowStretch(4, 1);
  tabWidget->addTab(tabVerticalAxis, tr("Y-axis"));

  // --- tab Properties
  QWidget * propertiesTab = new QWidget;
  QGridLayout * propertiesTabLayout = new QGridLayout(propertiesTab);

  // color
  // if only one representation
  if (pvViewWidget_->getNumberOfRepresentations() == 1)
  {
    label = new QLabel(tr("Plot color"));
    propertiesTabLayout->addWidget(label, 0, 0, 1, 1);

    colorButton_ = new QToolButton;
    QPixmap px(20, 20);
    px.fill(pvViewWidget_->getRepresentationColor());
    colorButton_->setIcon(px);
    connect(colorButton_, SIGNAL(clicked()), this, SLOT(setColor()));
    propertiesTabLayout->addWidget(colorButton_, 0, 1, 1, 1);
  }

  // marker style
  label = new QLabel(tr("Marker style"));
  propertiesTabLayout->addWidget(label, 1, 0, 1, 1);

  markerStyles_ = new QComboBox;
  markerStyles_->addItem(tr("Cross"), vtkPlotPoints::CROSS);
  markerStyles_->addItem(tr("Plus"), vtkPlotPoints::PLUS);
  markerStyles_->addItem(tr("Square"), vtkPlotPoints::SQUARE);
  markerStyles_->addItem(tr("Circle"), vtkPlotPoints::CIRCLE);
  markerStyles_->addItem(tr("Diamond"), vtkPlotPoints::DIAMOND);
  markerStyles_->setCurrentIndex(3);
  connect(markerStyles_, SIGNAL(currentIndexChanged(int)), this, SLOT(setMarkerStyle(int)));
  propertiesTabLayout->addWidget(markerStyles_, 1, 1, 1, 1);

  tabWidget->addTab(propertiesTab, tr("Plot style"));

  // marker size
  label = new QLabel(tr("Marker size"));
  propertiesTabLayout->addWidget(label, 2, 0, 1, 1);

  QSpinBox * markerSize = new QSpinBox;
  markerSize->setMinimum(1);
  markerSize->setMaximum(10);
  markerSize->setValue(2);
  connect(markerSize, SIGNAL(valueChanged(int)), this, SLOT(setMarkerSize(int)));
  propertiesTabLayout->addWidget(markerSize, 2, 1, 1, 1);
  
  propertiesTabLayout->setRowStretch(3, 1);
  tabWidget->addTab(propertiesTab, tr("Plot style"));

  mainGridLayout->addWidget(tabWidget, ++rowGrid, 0, 1, 2);

  // Bottom layout
  // export button
  QHBoxLayout * hboxForBottomButtons = new QHBoxLayout;
  hboxForBottomButtons->addStretch();
  QPushButton * button = new QPushButton(tr("Export"));
  connect(button, SIGNAL(clicked()), this, SLOT(exportPlot()));
  hboxForBottomButtons->addWidget(button);

  mainGridLayout->addLayout(hboxForBottomButtons, ++rowGrid, 1, 1, 1);

  mainGridLayout->setRowStretch(++rowGrid, 1);

  // update widgets
  updateYComboBox();

  //
  scrollArea->setWidget(frame);
  mainLayout->addWidget(scrollArea);
  setVisible(false);
}


void PVXYChartSettingWidget::updateLineEdits()
{
  PVXYChartViewWidget * chartWidget = 0;
  if (pvViewWidget_)
    if (pvViewWidget_->isVisible())
      chartWidget = pvViewWidget_;
  if (rankPvViewWidget_)
    if (rankPvViewWidget_->isVisible())
      chartWidget = rankPvViewWidget_;

  if (!chartWidget || !xAxisComboBox_ || !yAxisComboBox_)
    return;

  SignalBlocker xlabelLineEditBlocker(xlabelLineEdit_);
  SignalBlocker xminBlocker(xmin_);
  SignalBlocker xmaxBlocker(xmax_);
  SignalBlocker yminBlocker(ymin_);
  SignalBlocker ymaxBlocker(ymax_);
  SignalBlocker ylabelLineEditBlocker(ylabelLineEdit_);
  SignalBlocker titleLineEditBlocker(titleLineEdit_);

  titleLineEdit_->setText(chartWidget->getChartTitle(xAxisComboBox_->currentText(), yAxisComboBox_->currentText()));
  xlabelLineEdit_->setText(chartWidget->getXAxisTitle(xAxisComboBox_->currentText(), yAxisComboBox_->currentText()));
  xmin_->setValue(chartWidget->getXAxisRangeMinimum());
  xmax_->setValue(chartWidget->getXAxisRangeMaximum());
  ylabelLineEdit_->setText(chartWidget->getYAxisTitle(xAxisComboBox_->currentText(), yAxisComboBox_->currentText()));
  ymin_->setValue(chartWidget->getYAxisRangeMinimum());
  ymax_->setValue(chartWidget->getYAxisRangeMaximum());

  if (xlogScaleCheckBox_ && ylogScaleCheckBox_)
  {
    xlogScaleCheckBox_->setEnabled(chartWidget->logScalingValidForXAxis());
    ylogScaleCheckBox_->setEnabled(chartWidget->logScalingValidForYAxis());
    if (!xlogScaleCheckBox_->isEnabled())
      xlogScaleCheckBox_->setToolTip(tr("X-Axis bounds must be positive"));
    if (!ylogScaleCheckBox_->isEnabled())
      ylogScaleCheckBox_->setToolTip(tr("Y-Axis bounds must be positive"));
  }
}


void PVXYChartSettingWidget::updateYComboBox()
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

  if (plotType_ == PVXYChartSettingWidget::Scatter)
    yAxisComboBox_->addItems(outputNames + inputNames);

  plotChanged();
}


void PVXYChartSettingWidget::plotChanged()
{
  if (!xAxisComboBox_ || !yAxisComboBox_)
    return;

  xlogScaleCheckBox_->setChecked(false);
  ylogScaleCheckBox_->setChecked(false);

  if (pvViewWidget_)
    pvViewWidget_->showChart(xAxisComboBox_->currentText(), yAxisComboBox_->currentText());
  if (rankPvViewWidget_)
    rankPvViewWidget_->showChart(xAxisComboBox_->currentText(), yAxisComboBox_->currentText());
}


void PVXYChartSettingWidget::updateTitle()
{
  if (!xAxisComboBox_ || !yAxisComboBox_ || !titleLineEdit_)
    return;
  if (pvViewWidget_)
    pvViewWidget_->setChartTitle(xAxisComboBox_->currentText(), yAxisComboBox_->currentText(), titleLineEdit_->text());
  if (rankPvViewWidget_)
    rankPvViewWidget_->setChartTitle(xAxisComboBox_->currentText(), yAxisComboBox_->currentText(), titleLineEdit_->text());
}


void PVXYChartSettingWidget::updateXLabel()
{
  if (!xAxisComboBox_ || !yAxisComboBox_ || !xlabelLineEdit_)
    return;
  if (pvViewWidget_ && xAxisComboBox_ && yAxisComboBox_ && xlabelLineEdit_)
    pvViewWidget_->setXAxisTitle(xAxisComboBox_->currentText(), yAxisComboBox_->currentText(), xlabelLineEdit_->text());
  if (rankPvViewWidget_ )
    rankPvViewWidget_->setXAxisTitle(xAxisComboBox_->currentText(), yAxisComboBox_->currentText(), xlabelLineEdit_->text());
}


void PVXYChartSettingWidget::updateYLabel()
{
  if (!xAxisComboBox_ || !yAxisComboBox_ || !ylabelLineEdit_)
    return;
  if (pvViewWidget_)
    pvViewWidget_->setYAxisTitle(xAxisComboBox_->currentText(), yAxisComboBox_->currentText(), ylabelLineEdit_->text());
  if (rankPvViewWidget_)
    rankPvViewWidget_->setYAxisTitle(xAxisComboBox_->currentText(), yAxisComboBox_->currentText(), ylabelLineEdit_->text());
}


void PVXYChartSettingWidget::updateXrange()
{
  if (pvViewWidget_)
    pvViewWidget_->setXAxisRange(xmin_->value(), xmax_->value());
  if (rankPvViewWidget_)
    rankPvViewWidget_->setXAxisRange(xmin_->value(), xmax_->value());
}


void PVXYChartSettingWidget::updateYrange()
{
  if (pvViewWidget_)
    pvViewWidget_->setYAxisRange(ymin_->value(), ymax_->value());
  if (rankPvViewWidget_)
    rankPvViewWidget_->setYAxisRange(ymin_->value(), ymax_->value());
}


void PVXYChartSettingWidget::exportPlot()
{
  if (pvViewWidget_)
  {
    if (pvViewWidget_->isVisible())
      pvViewWidget_->exportPlot();
  }
  if (rankPvViewWidget_)
  {
    if (rankPvViewWidget_->isVisible())
      rankPvViewWidget_->exportPlot();
  }
}


void PVXYChartSettingWidget::setColor()
{
  const QColor color = QColorDialog::getColor(pvViewWidget_->getRepresentationColor(), this, tr("Select Color"));

  if (color.isValid())
  {
    if (pvViewWidget_)
      pvViewWidget_->setRepresentationColor(color);
    if (rankPvViewWidget_)
      rankPvViewWidget_->setRepresentationColor(color);

    // update button color
    QPixmap px(20, 20);
    px.fill(color);
    colorButton_->setIcon(px);
  }
}


void PVXYChartSettingWidget::setMarkerStyle(const int index)
{
  const int style = markerStyles_->itemData(index).toInt();
  if (pvViewWidget_)
    pvViewWidget_->setMarkerStyle(style);
  if (rankPvViewWidget_)
    rankPvViewWidget_->setMarkerStyle(style);
}


void PVXYChartSettingWidget::setMarkerSize(const int size)
{
  if (pvViewWidget_)
    pvViewWidget_->setMarkerSize(size);
  if (rankPvViewWidget_)
    rankPvViewWidget_->setMarkerSize(size);
}


void PVXYChartSettingWidget::setRepresentationToDisplay()
{
  QStringList checkedItemNames = reprListWidget_->getCheckedItemNames();
  for (int i = 0; i < reprListWidget_->getItemNames().size(); ++i)
  {
    const QString itemName = reprListWidget_->getItemNames()[i];
    pvViewWidget_->setRepresentationVisibility(checkedItemNames.contains(itemName), i);
    if (rankPvViewWidget_)
      rankPvViewWidget_->setRepresentationVisibility(checkedItemNames.contains(itemName), i);
  }
}
}
