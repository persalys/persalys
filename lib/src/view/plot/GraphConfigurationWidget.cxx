//                                               -*- C++ -*-
/**
 *  @brief QWidget to configure graphics
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
#include "persalys/GraphConfigurationWidget.hxx"

#include "persalys/TitledComboBox.hxx"
#include "persalys/ListWidgetWithCheckBox.hxx"
#include "persalys/QtTools.hxx"

#include <QToolButton>
#include <QPushButton>
#include <QGroupBox>
#include <QLabel>
#include <QScrollArea>
#include <QDebug>

namespace PERSALYS
{

static const QwtPlot::Axis Axes[2] = {QwtPlot::xBottom, QwtPlot::yLeft};

GraphConfigurationWidget::GraphConfigurationWidget(const QVector<PlotWidget *> &plotWidgets, QWidget *parent)
  : QWidget(parent)
  , plotWidgets_(plotWidgets)
  , plotIndex_(0)
  , frameLayout_(0)
  , propertiesTabWidget_(0)
  , titleLineEdit_(0)
{
  for (int i = 0; i < plotWidgets_.size(); ++i)
    connect(plotWidgets_[i], SIGNAL(plotChanged()), this, SLOT(updateLineEdits()));

  QVBoxLayout * mainLayout = new QVBoxLayout(this);
  mainLayout->setContentsMargins(0, 0, 0, 0);
  setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Maximum);

  QScrollArea * scrollArea = new QScrollArea;
  scrollArea->setWidgetResizable(true);
  QFrame * frame = new QFrame;
  frameLayout_ = new QGridLayout(frame);

  // title
  QLabel * label = new QLabel(tr("Title"));
  frameLayout_->addWidget(label, 0, 0);

  titleLineEdit_ = new QLineEdit;
  connect(titleLineEdit_, &QLineEdit::textChanged, [=](const QString& text) {plotWidgets_[plotIndex_]->setTitle(text);});
  frameLayout_->addWidget(titleLineEdit_, 0, 1);

  propertiesTabWidget_ = new QTabWidget;

  scrollArea->setWidget(frame);
  mainLayout->addWidget(scrollArea);
  setVisible(false);
}


void GraphConfigurationWidget::addXYAxisTabs(const bool xAxisWithLabels)
{
  for (int i = 0; i < 2; ++i)
  {
    // tab widget
    QWidget * tabVerticalAxis = new QWidget;
    QGridLayout * gridLayoutTab = new QGridLayout(tabVerticalAxis);

    // axis title
    QLabel * label = new QLabel(tr("Title"));
    gridLayoutTab->addWidget(label, 0, 0);

    axisLabelLineEdit_[i] = new QLineEdit;
    connect(axisLabelLineEdit_[i], &QLineEdit::textChanged, [=](const QString& text) {plotWidgets_[plotIndex_]->setAxisTitle(Axes[i], text);});
    gridLayoutTab->addWidget(axisLabelLineEdit_[i], 0, 1);

    // min
    label = new QLabel(tr("Min"));
    gridLayoutTab->addWidget(label, 1, 0);

    axisMinValueLineEdit_[i] = new ValueLineEdit;
    connect(axisMinValueLineEdit_[i], &ValueLineEdit::editingFinished, [=]() {updateRange(Axes[i]);});
    gridLayoutTab->addWidget(axisMinValueLineEdit_[i], 1, 1);

    // max
    label = new QLabel(tr("Max"));
    gridLayoutTab->addWidget(label, 2, 0);

    axisMaxValueLineEdit_[i] = new ValueLineEdit;
    connect(axisMaxValueLineEdit_[i], &ValueLineEdit::editingFinished, [=]() {updateRange(Axes[i]);});
    gridLayoutTab->addWidget(axisMaxValueLineEdit_[i], 2, 1);

    // label direction
    if (xAxisWithLabels)
    {
      if (Axes[i] == QwtPlot::xBottom)
      {
        QHBoxLayout * hLayout = new QHBoxLayout;
        label = new QLabel(tr("Labels\norientation"));
        QComboBox * labelOrientation = new QComboBox;
        labelOrientation->addItems(QStringList() << tr("Horizontal") << tr("Slanting") << tr("Vertical"));
        hLayout->addWidget(label);
        hLayout->addWidget(labelOrientation, 1);
        gridLayoutTab->addLayout(hLayout, 3, 0, 1, 2);
        connect(labelOrientation, QOverload<int>::of(&QComboBox::currentIndexChanged), [=](int idx) {plotWidgets_[plotIndex_]->setXLabelOrientation(idx);});
      }
      axisMinValueLineEdit_[i]->setEnabled(false);
      axisMaxValueLineEdit_[i]->setEnabled(false);
      axisMinValueLineEdit_[i]->clear();
      axisMaxValueLineEdit_[i]->clear();
    }
    else
    {
      // reset axis
      QToolButton * resetButton = new QToolButton;
      resetButton->setIcon(QIcon(":/images/view-refresh.svg"));
      resetButton->setToolTip(tr("Reset axis ranges"));
      connect(resetButton, &QToolButton::clicked, [=]() {plotWidgets_[plotIndex_]->resetAxisRanges();});
      gridLayoutTab->addWidget(resetButton, 3, 1, Qt::AlignRight);
    }
    propertiesTabWidget_->addTab(tabVerticalAxis, Axes[i] == QwtPlot::xBottom ? tr("X-axis") : tr("Y-axis"));
  }
}


void GraphConfigurationWidget::updateLineEdits()
{
  SignalBlocker titleLineEditBlocker(titleLineEdit_);
  titleLineEdit_->setText(plotWidgets_[plotIndex_]->title().text());

  for (int i = 0; i < 2; ++i)
  {
    SignalBlocker labelLineEditBlocker(axisLabelLineEdit_[i]);
    axisLabelLineEdit_[i]->setText(plotWidgets_[plotIndex_]->axisTitle(Axes[i]).text());
    if (axisMinValueLineEdit_[i]->isEnabled() && axisMaxValueLineEdit_[i]->isEnabled())
    {
      SignalBlocker minLineEditBlocker(axisMinValueLineEdit_[i]);
      SignalBlocker maxLineEditBlocker(axisMaxValueLineEdit_[i]);
      axisMinValueLineEdit_[i]->setValue(plotWidgets_[plotIndex_]->axisInterval(Axes[i]).minValue());
      axisMaxValueLineEdit_[i]->setValue(plotWidgets_[plotIndex_]->axisInterval(Axes[i]).maxValue());
    }
  }
}


int GraphConfigurationWidget::getCurrentPlotIndex() const
{
  return plotIndex_;
}


void GraphConfigurationWidget::currentPlotIndexChanged(int index)
{
  plotIndex_ = index;
  updateLineEdits();
  emit currentPlotChanged(plotIndex_);
}


void GraphConfigurationWidget::updateRange(QwtPlot::Axis ax)
{
  const int axIndex = (ax == QwtPlot::xBottom ? 0 : 1);
  try
  {
    plotWidgets_[plotIndex_]->setAxisScale(ax, axisMinValueLineEdit_[axIndex]->value(), axisMaxValueLineEdit_[axIndex]->value());
    plotWidgets_[plotIndex_]->replot();
  }
  catch (std::exception & ex)
  {
    updateLineEdits();
    qDebug() << "GraphConfigurationWidget::updateRange: value not valid\n";
  }
}


void GraphConfigurationWidget::addExportLayout()
{
  QHBoxLayout * exportLayout = new QHBoxLayout;
  QPushButton * button = new QPushButton(QIcon(":/images/document-export-table.png"), tr("Export"));
  connect(button, &QPushButton::clicked, [=]() {plotWidgets_[plotIndex_]->exportPlot();});
  exportLayout->addWidget(button);
  exportLayout->addStretch();

  frameLayout_->addLayout(exportLayout, frameLayout_->rowCount(), 0, 1, 2);
}


QSize GraphConfigurationWidget::sizeHint() const
{
  QSize size = QWidget::sizeHint();
  const int hScrollBarHeight = style()->pixelMetric(QStyle::PM_ScrollBarExtent);
  size.setHeight(size.height() + hScrollBarHeight);
  return size;
}


QSize GraphConfigurationWidget::minimumSizeHint() const
{
  QSize size = QWidget::minimumSizeHint();
  size.setHeight(10);
  return size;
}

// ----------- SimpleGraphSetting -----------

SimpleGraphSetting::SimpleGraphSetting(const QVector<PlotWidget *> &plotWidgets, const QStringList &inputNames, QWidget *parent)
  : GraphConfigurationWidget(plotWidgets, parent)
{
  // X-axis combobox
  if (inputNames.size())
  {
    int rowGrid = frameLayout_->rowCount();
    QLabel * label = new QLabel(tr("X-axis"));
    frameLayout_->addWidget(label, rowGrid, 0);

    QComboBox * xAxisComboBox = new QComboBox;
    xAxisComboBox->addItems(inputNames);
    frameLayout_->addWidget(xAxisComboBox, rowGrid, 1);
    connect(xAxisComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(currentPlotIndexChanged(int)));
  }

  // axis, plot properties
  addXYAxisTabs();
  frameLayout_->addWidget(propertiesTabWidget_, frameLayout_->rowCount(), 0, 1, 2);
  // export button
  addExportLayout();
  frameLayout_->setRowStretch(frameLayout_->rowCount(), 1);
}


SimpleGraphSetting::SimpleGraphSetting(PlotWidget *plotWidget, QWidget *parent)
  : SimpleGraphSetting(QVector<PlotWidget *>(1, plotWidget), QStringList(), parent)
{
}

// ----------- ScatterGraphSetting -----------

ScatterGraphSetting::ScatterGraphSetting(const QVector<PlotWidget *> &plotWidgets, const QStringList &inputNames, const QStringList &outputNames, QWidget *parent)
  : GraphConfigurationWidget(plotWidgets, parent)
{
  int rowGrid = frameLayout_->rowCount();
  // X-axis combobox
  QLabel * label = new QLabel(tr("X-axis"));
  frameLayout_->addWidget(label, rowGrid, 0);

  xAxisComboBox_ = new QComboBox;
  for (int i = 0; i < inputNames.size(); ++i)
    xAxisComboBox_->addItem(inputNames[i], true);
  for (int i = 0; i < outputNames.size(); ++i)
    xAxisComboBox_->addItem(outputNames[i], false);

  frameLayout_->addWidget(xAxisComboBox_, rowGrid, 1);
  connect(xAxisComboBox_, SIGNAL(currentIndexChanged(int)), this, SLOT(updateYComboBox()));

  // Y-axis combobox
  label = new QLabel(tr("Y-axis"));
  frameLayout_->addWidget(label, ++rowGrid, 0);

  yAxisComboBox_ = new QComboBox;
  frameLayout_->addWidget(yAxisComboBox_, rowGrid, 1);
  connect(yAxisComboBox_, SIGNAL(currentIndexChanged(int)), this, SLOT(currentPlotIndexChanged(int)));

  // rank check box
  rankCheckBox_ = new QCheckBox(tr("Ranks"));
  frameLayout_->addWidget(rankCheckBox_, ++rowGrid, 0, 1, 2);
  connect(rankCheckBox_, SIGNAL(stateChanged(int)), this, SLOT(currentPlotIndexChanged()));

  // axis, plot properties
  addXYAxisTabs();
  frameLayout_->addWidget(propertiesTabWidget_, ++rowGrid, 0, 1, 2);
  // export button
  addExportLayout();
  frameLayout_->setRowStretch(frameLayout_->rowCount(), 1);

  updateYComboBox();
}


void ScatterGraphSetting::updateYComboBox()
{
  const QString currentYText = yAxisComboBox_->currentText();
  SignalBlocker blocker(yAxisComboBox_);
  yAxisComboBox_->clear();

  QStringList inputNames;
  QStringList outputNames;
  for (int i = 0; i < xAxisComboBox_->count(); ++i)
  {
    if (i != xAxisComboBox_->currentIndex()) // must have x != y
    {
      if (xAxisComboBox_->itemData(i).toBool()) // == input
        inputNames << xAxisComboBox_->itemText(i);
      else // == output
        outputNames << xAxisComboBox_->itemText(i);
    }
  }
  yAxisComboBox_->addItems(outputNames + inputNames);
  yAxisComboBox_->setCurrentText(currentYText);

  currentPlotIndexChanged();
}


void ScatterGraphSetting::currentPlotIndexChanged(int /*i*/)
{
  const int inputIndex = xAxisComboBox_->currentIndex();
  const int outputIndex = yAxisComboBox_->currentIndex();
  const int outputCount = yAxisComboBox_->count();
  plotIndex_ = 2 * (inputIndex * outputCount + outputIndex) + (rankCheckBox_->isChecked() ? 1 : 0);
  GraphConfigurationWidget::currentPlotIndexChanged(plotIndex_);
}

// ----------- PDFGraphSetting -----------

PDFGraphSetting::PDFGraphSetting(const QVector<PlotWidget *> &plotWidgets, const QStringList &inputNames, const PDFType type, QWidget *parent)
  : GraphConfigurationWidget(plotWidgets, parent)
  , xAxisComboBox_(0)
  , yAxisComboBox_(0)
{
  int rowGrid = frameLayout_->rowCount();
  // distribution representation choice
  QStringList reprs = QStringList() << tr("PDF") << tr("CDF");
  if (type == PDFGraphSetting::Distribution)
    reprs << tr("Quantile function") << tr("Survival function");
  reprComboBox_ = new QComboBox;
  reprComboBox_->addItems(reprs);
  frameLayout_->addWidget(reprComboBox_, rowGrid, 0, 1, 2);
  connect(reprComboBox_, SIGNAL(currentIndexChanged(int)), this, SLOT(currentPlotIndexChanged()));
  // X-axis combobox
  if (inputNames.size())
  {
    QLabel * label = new QLabel(tr("X-axis"));
    frameLayout_->addWidget(label, ++rowGrid, 0);

    xAxisComboBox_ = new QComboBox;
    xAxisComboBox_->addItems(inputNames);
    frameLayout_->addWidget(xAxisComboBox_, rowGrid, 1);
    if (type == PDFGraphSetting::Ksi)
      connect(xAxisComboBox_, SIGNAL(currentIndexChanged(int)), this, SLOT(currentPlotIndexChanged(int)));
    else
    {
      // Y-axis combobox
      label = new QLabel(tr("Y-axis"));
      frameLayout_->addWidget(label, ++rowGrid, 0);

      yAxisComboBox_ = new QComboBox;
      frameLayout_->addWidget(yAxisComboBox_, rowGrid, 1);
      connect(xAxisComboBox_, SIGNAL(currentIndexChanged(int)), this, SLOT(updateYComboBox()));
      connect(yAxisComboBox_, SIGNAL(currentIndexChanged(int)), this, SLOT(currentPlotIndexChanged(int)));
    }
  }
  // axis, plot properties
  addXYAxisTabs();
  frameLayout_->addWidget(propertiesTabWidget_, ++rowGrid, 0, 1, 2);
  // export button
  addExportLayout();
  frameLayout_->setRowStretch(frameLayout_->rowCount(), 1);

  if (yAxisComboBox_)
    updateYComboBox();
}


PDFGraphSetting::PDFGraphSetting(const QVector<PlotWidget *> &plotWidgets, const PDFType type, QWidget *parent)
  : PDFGraphSetting(plotWidgets, QStringList(), type, parent)
{
}


void PDFGraphSetting::updateYComboBox()
{
  const QString currentYText = yAxisComboBox_->currentText();
  SignalBlocker blocker(yAxisComboBox_);
  yAxisComboBox_->clear();

  QStringList names;
  for (int i = 0; i < xAxisComboBox_->count(); ++i)
    if (i != xAxisComboBox_->currentIndex()) // must have x != y
      names << xAxisComboBox_->itemText(i);

  yAxisComboBox_->addItems(names);
  yAxisComboBox_->setCurrentText(currentYText);

  currentPlotIndexChanged();
}


void PDFGraphSetting::currentPlotIndexChanged(int /*i*/)
{
  const int inputIndex = xAxisComboBox_ ? xAxisComboBox_->currentIndex() : 0;
  const int outputIndex = yAxisComboBox_ ? yAxisComboBox_->currentIndex() : 0;
  const int outputCount = yAxisComboBox_ ? yAxisComboBox_->count() : 1;
  plotIndex_ = 2 * (inputIndex * outputCount + outputIndex) + reprComboBox_->currentIndex();
  GraphConfigurationWidget::currentPlotIndexChanged(plotIndex_);
}

// ----------- BoxPlotGraphSetting -----------

BoxPlotGraphSetting::BoxPlotGraphSetting(BoxPlot *plotWidget, const QStringList &inputNames, const QList<bool> & checked, QWidget *parent)
  : GraphConfigurationWidget(QVector<PlotWidget *>(1, plotWidget), parent)
{
  int rowGrid = frameLayout_->rowCount();

  // combobox to select the variables to display
  QLabel * label = new QLabel(tr("Variables"));
  frameLayout_->addWidget(label, rowGrid, 0);

  TitledComboBox * varComboBox = new TitledComboBox("-- " + tr("Select") + " --");
  QStringList visibleReprNames;
  for (int i=0; i<inputNames.size(); ++i) {
    if (visibleReprNames.size() < MaxVisibleVariableNumber && checked[i])
      visibleReprNames << qAsConst(inputNames[i]);}

  ListWidgetWithCheckBox * varListWidget = new ListWidgetWithCheckBox("-- " + tr("Select") + " --", inputNames, visibleReprNames, this);
  connect(varListWidget, SIGNAL(checkedItemsChanged(QStringList)), plotWidget, SLOT(setVariablesToShow(QStringList)));

  varComboBox->setModel(varListWidget->model());
  varComboBox->setView(varListWidget);
  frameLayout_->addWidget(varComboBox, rowGrid, 1);

  // axis, plot properties
  addXYAxisTabs(true);
  frameLayout_->addWidget(propertiesTabWidget_, ++rowGrid, 0, 1, 2);
  // export button
  addExportLayout();
  frameLayout_->setRowStretch(frameLayout_->rowCount(), 1);

  if (inputNames != visibleReprNames) plotWidget->setVariablesToShow(visibleReprNames);
}

// ----------- SensitivityIndicesGraphSetting -----------

SensitivityIndicesGraphSetting::SensitivityIndicesGraphSetting(PlotWidget *plotWidget, QWidget *parent)
  : GraphConfigurationWidget(QVector<PlotWidget *>(1, plotWidget), parent)
{
  // axis, plot properties
  addXYAxisTabs(true);
  frameLayout_->addWidget(propertiesTabWidget_, frameLayout_->rowCount(), 0, 1, 2);
  // export button
  addExportLayout();
  frameLayout_->setRowStretch(frameLayout_->rowCount(), 1);
}
}
