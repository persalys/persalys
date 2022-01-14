//                                               -*- C++ -*-
/**
 *  @brief QWidget to configure XYChart
 *
 *  Copyright 2015-2022 EDF-Phimeca
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
#include "persalys/PVXYChartSettingWidget.hxx"

#include "persalys/QtTools.hxx"

#include <vtkPlotPoints.h>

#include <QGroupBox>
#include <QLabel>
#include <QColorDialog>
#include <QSpinBox>

using namespace OT;

namespace PERSALYS
{

static const vtkAxis::Location Axes[2] = {vtkAxis::BOTTOM, vtkAxis::LEFT};

PVXYChartSettingWidget::PVXYChartSettingWidget(PVXYChartViewWidget *pvViewWidget, const QStringList &outputNames, const bool isScatter, QWidget *parent)
  : PVPlotSettingWidget(pvViewWidget, parent)
  , pvXYViewWidget_(pvViewWidget)
  , propertiesTabWidget_(0)
  , xAxisComboBox_(0)
  , yAxisComboBox_(0)
  , titleLineEdit_(0)
  , colorButton_(0)
{
  plotNames_ = outputNames;

  connect(pvXYViewWidget_, &PVXYChartViewWidget::axisHasBeenModified, this, &PVXYChartSettingWidget::updateLineEdits);

  int rowGrid = 0;

  // title
  QLabel * label = new QLabel(tr("Title"));
  frameLayout_->addWidget(label, rowGrid, 0, 1, 1);

  titleLineEdit_ = new QLineEdit;
  connect(titleLineEdit_, &QLineEdit::textChanged, [=](const QString &newValue) {pvXYViewWidget_->setChartTitle(newValue, xAxisComboBox_->currentText(), yAxisComboBox_->currentText());});
  frameLayout_->addWidget(titleLineEdit_, rowGrid, 1, 1, 1);

  // Axis comboboxes : default hidden
  xAxisComboBox_ = new QComboBox(this);
  xAxisComboBox_->setVisible(isScatter);
  yAxisComboBox_ = new QComboBox(this);
  yAxisComboBox_->setVisible(isScatter);

  // axis, plot properties
  propertiesTabWidget_ = new QTabWidget;
  addXYAxisTabs();
}


void PVXYChartSettingWidget::addXYAxisTabs()
{
  for (int i = 0; i < 2; ++i)
  {
    // tab widget
    QWidget * tabVerticalAxis = new QWidget;
    QGridLayout * gridLayoutTab = new QGridLayout(tabVerticalAxis);

    // axis title
    QLabel * label = new QLabel(tr("Title"));
    gridLayoutTab->addWidget(label, 0, 0, 1, 1);

    axisLabelLineEdit_[i] = new QLineEdit;
    if (!yAxisComboBox_->currentText().isEmpty() || plotNames_.size() == 0)
      connect(axisLabelLineEdit_[i], &QLineEdit::editingFinished, [=]() {pvXYViewWidget_->setAxisTitle(Axes[i], axisLabelLineEdit_[i]->text(), xAxisComboBox_->currentText(), yAxisComboBox_->currentText());});
    else
      connect(axisLabelLineEdit_[i], &QLineEdit::editingFinished, [=]() {pvXYViewWidget_->setAxisTitle(Axes[i], axisLabelLineEdit_[i]->text(), xAxisComboBox_->currentText(), plotNames_[0]);});
    gridLayoutTab->addWidget(axisLabelLineEdit_[i], 0, 1, 1, 1);

    // min
    label = new QLabel(tr("Min"));
    gridLayoutTab->addWidget(label, 1, 0, 1, 1);

    axisMinValueLineEdit_[i] = new ValueLineEdit;
    connect(axisMinValueLineEdit_[i], &ValueLineEdit::editingFinished, [=]() {pvXYViewWidget_->setAxisRange(Axes[i], axisMinValueLineEdit_[i]->value(), axisMaxValueLineEdit_[i]->value());});
    gridLayoutTab->addWidget(axisMinValueLineEdit_[i], 1, 1, 1, 1);

    // max
    label = new QLabel(tr("Max"));
    gridLayoutTab->addWidget(label, 2, 0, 1, 1);

    axisMaxValueLineEdit_[i] = new ValueLineEdit;
    connect(axisMaxValueLineEdit_[i], &ValueLineEdit::editingFinished, [=]() {pvXYViewWidget_->setAxisRange(Axes[i], axisMinValueLineEdit_[i]->value(), axisMaxValueLineEdit_[i]->value());});
    gridLayoutTab->addWidget(axisMaxValueLineEdit_[i], 2, 1, 1, 1);

    QHBoxLayout * hLayout = new QHBoxLayout;
    // log scale
    axisLogScaleCheckBox_[i] = new QCheckBox(tr("Log scale"));
    connect(axisLogScaleCheckBox_[i], &QCheckBox::clicked, [=](bool checked) {pvXYViewWidget_->setLogScale(Axes[i], checked);});
    hLayout->addWidget(axisLogScaleCheckBox_[i]);

    // reset axis
    QToolButton * resetButton = new QToolButton;
    resetButton->setIcon(QIcon(":/images/view-refresh.svg"));
    resetButton->setToolTip(tr("Reset axis ranges"));
    connect(resetButton, SIGNAL(clicked()), pvViewWidget_, SLOT(resetDisplay()));
    hLayout->addStretch();
    hLayout->addWidget(resetButton);
    gridLayoutTab->addLayout(hLayout, 3, 0, 1, 2);

    propertiesTabWidget_->addTab(tabVerticalAxis, Axes[i] == vtkAxis::BOTTOM ? tr("X-axis") : tr("Y-axis"));
  }
}


void PVXYChartSettingWidget::addMarkerTab(const bool canChangeColor)
{
  QWidget * widget = new QWidget;
  QGridLayout * widgetLayout = new QGridLayout(widget);

  // color
  if (canChangeColor)
  {
    QLabel * label = new QLabel(tr("Color"));
    widgetLayout->addWidget(label, 0, 0);

    colorButton_ = new QToolButton;
    QPixmap px(20, 20);
    px.fill(pvXYViewWidget_->getRepresentationColor());
    colorButton_->setIcon(px);
    connect(colorButton_, SIGNAL(clicked()), this, SLOT(setColor()));
    widgetLayout->addWidget(colorButton_, 0, 1, Qt::AlignLeft);
  }

  // marker style
  QLabel * label = new QLabel(tr("Style"));
  widgetLayout->addWidget(label, 1, 0);

  QComboBox * markerStyles = new QComboBox;
  markerStyles->addItem(tr("None"), vtkPlotPoints::NONE);
  markerStyles->addItem(tr("Cross"), vtkPlotPoints::CROSS);
  markerStyles->addItem(tr("Plus"), vtkPlotPoints::PLUS);
  markerStyles->addItem(tr("Square"), vtkPlotPoints::SQUARE);
  markerStyles->addItem(tr("Circle"), vtkPlotPoints::CIRCLE);
  markerStyles->addItem(tr("Diamond"), vtkPlotPoints::DIAMOND);
  markerStyles->setCurrentIndex(pvXYViewWidget_->getMarkerStyle());
  connect(markerStyles, &QComboBox::currentTextChanged, [=]() {pvXYViewWidget_->setMarkerStyle(markerStyles->currentData().toInt());});
  widgetLayout->addWidget(markerStyles, 1, 1);

  // marker size
  label = new QLabel(tr("Size"));
  widgetLayout->addWidget(label, 2, 0);

  QSpinBox * markerSize = new QSpinBox;
  markerSize->setMinimum(1);
  markerSize->setMaximum(10);
  markerSize->setValue(2);
  connect(markerSize, SIGNAL(valueChanged(int)), pvXYViewWidget_, SLOT(setMarkerSize(int)));
  widgetLayout->addWidget(markerSize, 2, 1);

  widgetLayout->setRowStretch(3, 1);
  widgetLayout->setColumnStretch(1, 2);

  propertiesTabWidget_->addTab(widget, tr("Marker"));
}


void PVXYChartSettingWidget::updateLineEdits()
{
  SignalBlocker titleLineEditBlocker(titleLineEdit_);
  titleLineEdit_->setText(pvXYViewWidget_->getChartTitle(xAxisComboBox_->currentText(), yAxisComboBox_->currentText()));

  for (int i = 0; i < 2; ++i)
  {
    SignalBlocker minLineEditBlocker(axisMinValueLineEdit_[i]);
    SignalBlocker maxLineEditBlocker(axisMaxValueLineEdit_[i]);
    SignalBlocker labelLineEditBlocker(axisLabelLineEdit_[i]);
    axisMinValueLineEdit_[i]->setValue(pvXYViewWidget_->getAxisRangeMinimum(Axes[i]));
    axisMaxValueLineEdit_[i]->setValue(pvXYViewWidget_->getAxisRangeMaximum(Axes[i]));
    axisLabelLineEdit_[i]->setText(pvXYViewWidget_->getAxisTitle(Axes[i], xAxisComboBox_->currentText(), yAxisComboBox_->currentText()));
    axisLogScaleCheckBox_[i]->setEnabled(pvXYViewWidget_->logScalingValidForAxis(Axes[i]));
    if (axisLogScaleCheckBox_[i]->isEnabled())
      axisLogScaleCheckBox_[i]->setToolTip(tr("Axis bounds must be positive"));
  }
}


void PVXYChartSettingWidget::setColor()
{
  const QColor color(QColorDialog::getColor(pvXYViewWidget_->getRepresentationColor(), this, tr("Select Color")));

  if (!color.isValid())
    return;

  pvXYViewWidget_->setRepresentationColor(color);

  // update button color
  QPixmap px(20, 20);
  px.fill(color);
  colorButton_->setIcon(px);
}


// ------------- SimpleSettingWidget -------------

XYChartSettingWidget::XYChartSettingWidget(PVXYChartViewWidget *pvViewWidget, QWidget *parent)
  : PVXYChartSettingWidget(pvViewWidget, QStringList(), false, parent)
{
  // axis, plot properties
  addMarkerTab(true);
  frameLayout_->addWidget(propertiesTabWidget_, frameLayout_->rowCount(), 0, 1, 2);

  // export button
  addExportLayout();
  frameLayout_->setRowStretch(frameLayout_->rowCount(), 1);
}


// ------------- BagChartSettingWidget -------------

BagChartSettingWidget::BagChartSettingWidget(PVXYChartViewWidget *pvViewWidget, QWidget *parent)
  : PVXYChartSettingWidget(pvViewWidget, QStringList(), false, parent)
{
// does not work for now in Paraview
//  if (plotType_ == PVXYChartSettingWidget::BagChart)
//  {
//    topLayout->addWidget(new QLabel(tr("Quantile")), ++rowGrid, 0);
//    quantileSpinBox_ = new UIntSpinBox;
//    quantileSpinBox_->setMinimum(10);
//    quantileSpinBox_->setMaximum(100);
//    quantileSpinBox_->setValue(dynamic_cast<PVBagChartViewWidget*>(pvXYViewWidget_)->getUserQuantile());
//    connect(quantileSpinBox_, SIGNAL(valueChanged(double)), this, SLOT(updateQuantile()));
//    frameLayout_->addWidget(quantileSpinBox_, rowGrid, 1);
//  }
  // axis, plot properties
  addMarkerTab(false);
  frameLayout_->addWidget(propertiesTabWidget_, frameLayout_->rowCount(), 0, 1, 2);

  // export button
  addExportLayout();
  frameLayout_->setRowStretch(frameLayout_->rowCount(), 1);
}

// void BagChartSettingWidget::updateQuantile()
// {
//   Q_ASSERT(quantileSpinBox_);
//   dynamic_cast<PVBagChartViewWidget*>(pvXYViewWidget_)->setUserQuantile(quantileSpinBox_->value());
// }


// ------------- TrajectoriesSettingWidget -------------

TrajectoriesSettingWidget::TrajectoriesSettingWidget(PVXYChartViewWidget *pvViewWidget, const QStringList &xAxisNames, const QStringList &dataNames, QWidget *parent)
  : PVXYChartSettingWidget(pvViewWidget, dataNames, false, parent)
{
  if (xAxisNames.size())
  {
    const int rowGrid = frameLayout_->rowCount();
    // X-axis combobox
    QLabel * label = new QLabel(tr("X-axis"));
    frameLayout_->addWidget(label, rowGrid, 0, 1, 1);
    xAxisComboBox_->setVisible(true);
    xAxisComboBox_->addItems(xAxisNames);
    connect(xAxisComboBox_, &QComboBox::currentTextChanged, [=]() {pvXYViewWidget_->setXAxisData(xAxisComboBox_->currentText());});
    frameLayout_->addWidget(xAxisComboBox_, rowGrid, 1, 1, 1);
  }

  addSelectDataWidget(tr("Data"));

  // axis, plot properties
  addMarkerTab(false);
  frameLayout_->addWidget(propertiesTabWidget_, frameLayout_->rowCount(), 0, 1, 2);

  // export button
  addExportLayout();
  frameLayout_->setRowStretch(frameLayout_->rowCount(), 1);
}


TrajectoriesSettingWidget::TrajectoriesSettingWidget(PVXYChartViewWidget *pvViewWidget, const QStringList &dataNames, QWidget *parent)
  : TrajectoriesSettingWidget(pvViewWidget, QStringList(), dataNames, parent)
{
}


// ------------- ScatterSettingWidget -------------

ScatterSettingWidget::ScatterSettingWidget(PVXYChartViewWidget *pvViewWidget,
                                           const Collection<Sample> &samples,
                                           const Collection<Sample> &rankSamples,
                                           const QStringList &inputNames,
                                           const QStringList &outputNames,
                                           QWidget *parent)
  : PVXYChartSettingWidget(pvViewWidget, QStringList(), true, parent)
{
  samples_ = samples;
  rankSamples_ = rankSamples;

  Q_ASSERT(samples_.getSize());
  if (rankSamples_.getSize())
    Q_ASSERT(samples_.getSize() == rankSamples_.getSize());

  int rowGrid = frameLayout_->rowCount();

  // X-axis combobox
  QLabel * label = new QLabel(tr("X-axis"));
  frameLayout_->addWidget(label, rowGrid, 0, 1, 1);

  for (int i = 0; i < inputNames.size(); ++i)
    xAxisComboBox_->addItem(inputNames[i], true);

  for (int i = 0; i < outputNames.size(); ++i)
    xAxisComboBox_->addItem(outputNames[i], false);

  frameLayout_->addWidget(xAxisComboBox_, rowGrid, 1, 1, 1);

  // Y-axis combobox
  label = new QLabel(tr("Y-axis"));
  frameLayout_->addWidget(label, ++rowGrid, 0, 1, 1);

  connect(xAxisComboBox_, SIGNAL(currentIndexChanged(int)), this, SLOT(updateYComboBox()));
  frameLayout_->addWidget(yAxisComboBox_, rowGrid, 1, 1, 1);
  connect(yAxisComboBox_, &QComboBox::currentTextChanged, [=]() {pvXYViewWidget_->showChart(xAxisComboBox_->currentText(), yAxisComboBox_->currentText());});

  if (pvXYViewWidget_->getNumberOfRepresentations() > 1)
    addSelectDataWidget(tr("Data"));

  // rank checkBox
  if (rankSamples_.getSize())
    addRankWidget(false);

  // axis, plot properties
  addMarkerTab(pvXYViewWidget_->getNumberOfRepresentations() == 1);
  frameLayout_->addWidget(propertiesTabWidget_, frameLayout_->rowCount(), 0, 1, 2);

  // export button
  addExportLayout();
  frameLayout_->setRowStretch(frameLayout_->rowCount(), 1);

  updateYComboBox();
}


ScatterSettingWidget::ScatterSettingWidget(PVXYChartViewWidget *pvViewWidget,
                                           const Sample &sample,
                                           const Sample &rankSample,
                                           const QStringList &inputNames,
                                           const QStringList &outputNames,
                                           QWidget *parent)
  : ScatterSettingWidget(pvViewWidget, Collection<Sample>(1, sample), Collection<Sample>(1, rankSample), inputNames, outputNames, parent)
{
}


ScatterSettingWidget::ScatterSettingWidget(PVXYChartViewWidget *pvViewWidget,
                                           const Collection<Sample> &samples,
                                           const Collection<Sample> &rankSamples,
                                           QWidget *parent)
  : ScatterSettingWidget(pvViewWidget, samples, rankSamples, QtOT::DescriptionToStringList(samples[0].getDescription()), QStringList(), parent)
{
}


void ScatterSettingWidget::updateYComboBox()
{
  if (!xAxisComboBox_ || !yAxisComboBox_)
    return;

  const QString currentYText = yAxisComboBox_->currentText();
  SignalBlocker blocker(yAxisComboBox_);
  // remove all items of yAxisComboBox_
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
  // update yAxisComboBox_ items
  yAxisComboBox_->addItems(outputNames + inputNames);
  yAxisComboBox_->setCurrentText(currentYText);

  pvXYViewWidget_->showChart(xAxisComboBox_->currentText(), yAxisComboBox_->currentText());
}


// ------------- ScatterSettingWidget -------------

MultiPDFSettingWidget::MultiPDFSettingWidget(PVXYChartViewWidget *pvViewWidget,
                                           const QStringList &inputNames,
                                           const QStringList &outputNames,
                                           QWidget *parent)
  : PVXYChartSettingWidget(pvViewWidget, QStringList(), false, parent)
{
  int rowGrid = frameLayout_->rowCount();

  // X-axis combobox
  QLabel * label = new QLabel(tr("X-axis"));
  frameLayout_->addWidget(label, rowGrid, 0, 1, 1);

  for (int i = 0; i < inputNames.size(); ++i)
    xAxisComboBox_->addItem(inputNames[i]);
  xAxisComboBox_->setVisible(true);

  for (int i = 0; i < outputNames.size(); ++i)
    yAxisComboBox_->addItem(outputNames[i]);

  frameLayout_->addWidget(xAxisComboBox_, rowGrid, 1, 1, 1);

  connect(xAxisComboBox_, SIGNAL(currentIndexChanged(int)), yAxisComboBox_, SLOT(setCurrentIndex(int)));
  connect(yAxisComboBox_, &QComboBox::currentTextChanged, [=]() {pvXYViewWidget_->showChart(xAxisComboBox_->currentText(), yAxisComboBox_->currentText());});

  if (pvXYViewWidget_->getNumberOfRepresentations() > 1)
    addSelectDataWidget(tr("Data"));

  // axis, plot properties
  addMarkerTab(pvXYViewWidget_->getNumberOfRepresentations() == 1);
  frameLayout_->addWidget(propertiesTabWidget_, frameLayout_->rowCount(), 0, 1, 2);

  // export button
  addExportLayout();
  frameLayout_->setRowStretch(frameLayout_->rowCount(), 1);

  pvXYViewWidget_->showChart(xAxisComboBox_->currentText(), yAxisComboBox_->currentText());
}
}
