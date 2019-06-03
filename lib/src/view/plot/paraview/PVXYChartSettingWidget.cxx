//                                               -*- C++ -*-
/**
 *  @brief QWidget to configure XYChart
 *
 *  Copyright 2015-2019 EDF-Phimeca
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

#include "persalys/PVBagChartViewWidget.hxx"

#include "persalys/QtTools.hxx"
#include "persalys/TitledComboBox.hxx"

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

namespace PERSALYS
{

PVXYChartSettingWidget::PVXYChartSettingWidget(PVXYChartViewWidget * pvViewWidget,
    const Sample& sample,
    const Sample& sampleRank,
    QStringList inputNames,
    QStringList outputNames,
    Type plotType,
    QWidget * parent)
  : QWidget(parent)
  , pvViewWidget_(pvViewWidget)
  , sample_(sample)
  , sampleRank_(sampleRank)
  , failedSample_()
  , failedSampleRank_()
  , notEvalSample_()
  , notEvalSampleRank_()
  , inputNames_(inputNames)
  , outputNames_(outputNames)
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

  buildInterface();
}


PVXYChartSettingWidget::PVXYChartSettingWidget(PVXYChartViewWidget* pvViewWidget,
    const Sample& sample,
    const Sample& sampleRank,
    const Sample& failedSample,
    const Sample& failedSampleRank,
    const Sample& notEvalSample,
    const Sample& notEvalSampleRank,
    Type plotType,
    QWidget * parent)
  : QWidget(parent)
  , pvViewWidget_(pvViewWidget)
  , sample_(sample)
  , sampleRank_(sampleRank)
  , failedSample_(failedSample)
  , failedSampleRank_(failedSampleRank)
  , notEvalSample_(notEvalSample)
  , notEvalSampleRank_(notEvalSampleRank)
  , inputNames_()
  , outputNames_()
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
  , quantileSpinBox_(0)
  , colorButton_(0)
  , markerStyles_(0)
  , reprListWidget_(0)
{
  inputNames_ = QtOT::DescriptionToStringList(sample_.getDescription());
  connect(pvViewWidget_, SIGNAL(plotChanged()), this, SLOT(updateLineEdits()));
  buildInterface();
}


PVXYChartSettingWidget::PVXYChartSettingWidget(PVXYChartViewWidget * pvViewWidget, const QStringList& outputNames, Type plotType, QWidget * parent)
  : QWidget(parent)
  , pvViewWidget_(pvViewWidget)
  , sample_()
  , sampleRank_()
  , failedSample_()
  , failedSampleRank_()
  , notEvalSample_()
  , notEvalSampleRank_()
  , inputNames_()
  , outputNames_(outputNames)
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

  buildInterface();
}


void PVXYChartSettingWidget::buildInterface()
{
  QVBoxLayout * mainLayout = new QVBoxLayout(this);
  mainLayout->setContentsMargins(0, 0, 0, 0);
  setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Maximum);

  QScrollArea * scrollArea = new QScrollArea;
  scrollArea->setWidgetResizable(true);
  QFrame * frame = new QFrame;
  QVBoxLayout * frameLayout = new QVBoxLayout(frame);

  QGridLayout * topLayout = new QGridLayout;
  int rowGrid = 0;

  // title
  QLabel * label = new QLabel(tr("Title"));
  topLayout->addWidget(label, rowGrid, 0, 1, 1);

  titleLineEdit_ = new QLineEdit;
  connect(titleLineEdit_, SIGNAL(textChanged(QString)), this, SLOT(updateTitle()));
  topLayout->addWidget(titleLineEdit_, rowGrid, 1, 1, 1);

  // Axis comboboxes
  xAxisComboBox_ = new QComboBox(this);
  yAxisComboBox_ = new QComboBox(this);
  if (plotType_ == PVXYChartSettingWidget::Scatter)
  {
    // X-axis combobox
    label = new QLabel(tr("X-axis"));
    topLayout->addWidget(label, ++rowGrid, 0, 1, 1);

    for (int i = 0; i < inputNames_.size(); ++i)
      xAxisComboBox_->addItem(inputNames_[i], true);

    for (int i = 0; i < outputNames_.size(); ++i)
      xAxisComboBox_->addItem(outputNames_[i], false);

    topLayout->addWidget(xAxisComboBox_, rowGrid, 1, 1, 1);
    connect(xAxisComboBox_, SIGNAL(currentIndexChanged(int)), this, SLOT(updateYComboBox()));

    // Y-axis combobox
    label = new QLabel(tr("Y-axis"));
    topLayout->addWidget(label, ++rowGrid, 0, 1, 1);

    topLayout->addWidget(yAxisComboBox_, rowGrid, 1, 1, 1);
    connect(yAxisComboBox_, SIGNAL(currentIndexChanged(int)), this, SLOT(plotChanged()));
  }
  else
  {
    xAxisComboBox_->addItem("");
    yAxisComboBox_->addItem("");
    xAxisComboBox_->setVisible(false);
    yAxisComboBox_->setVisible(false);
  }

  // representation
  if (pvViewWidget_->getNumberOfRepresentations() > 1)
  {
    label = new QLabel(tr("Data"));
    topLayout->addWidget(label, ++rowGrid, 0, 1, 1);

    // combobox to select the representations to display
    TitledComboBox * reprComboBox = new TitledComboBox("-- " + tr("Select") + " --");
    QStringList reprNames;
    QStringList visibleReprNames;
    for (int i = 0; i < pvViewWidget_->getNumberOfRepresentations(); ++i)
    {
      reprNames << pvViewWidget_->getRepresentationLabels(i)[0];
      if (pvViewWidget_->getRepresentationVisibility(i))
        visibleReprNames << pvViewWidget_->getRepresentationLabels(i)[0];
    }
    reprListWidget_ = new ListWidgetWithCheckBox("-- " + tr("Select") + " --", reprNames, visibleReprNames, this);
    connect(reprListWidget_, SIGNAL(checkedItemsChanged(QList<int>)), pvViewWidget_, SLOT(setRepresentationVisibility(QList<int>)));
    connect(pvViewWidget_, SIGNAL(selectedReprChanged(QStringList)), reprListWidget_, SLOT(setCheckedNames(QStringList)));
    reprComboBox->setModel(reprListWidget_->model());
    reprComboBox->setView(reprListWidget_);

    topLayout->addWidget(reprComboBox, rowGrid, 1, 1, 1);
  }

  if (plotType_ == PVXYChartSettingWidget::Trajectories)
  {
    label = new QLabel(tr("Data"));
    topLayout->addWidget(label, ++rowGrid, 0, 1, 1);

    // combobox to select the representations to display
    TitledComboBox * dataComboBox = new TitledComboBox("-- " + tr("Select") + " --");

    reprListWidget_ = new ListWidgetWithCheckBox("-- " + tr("Select") + " --", outputNames_, outputNames_, this);
    connect(reprListWidget_, SIGNAL(checkedItemsChanged(QStringList)), pvViewWidget_, SLOT(setAxisToShow(QStringList)));
    dataComboBox->setModel(reprListWidget_->model());
    dataComboBox->setView(reprListWidget_);
    reprListWidget_->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);

    topLayout->addWidget(dataComboBox, rowGrid, 1, 1, 1);
  }
  frameLayout->addLayout(topLayout);

  // rank checkbox
  if (sampleRank_.getSize())
  {
    QCheckBox * rankCheckBox = new QCheckBox(tr("Ranks"));
    frameLayout->addWidget(rankCheckBox);
    connect(rankCheckBox, SIGNAL(clicked(bool)), this, SLOT(modifyData(bool)));
  }

  // TODO: does not work for now
//  if (plotType_ == PVXYChartSettingWidget::BagChart)
//  {
//    topLayout->addWidget(new QLabel(tr("Quantile")), ++rowGrid, 0);
//    quantileSpinBox_ = new UIntSpinBox;
//    quantileSpinBox_->setMinimum(10);
//    quantileSpinBox_->setMaximum(100);
//    quantileSpinBox_->setValue(dynamic_cast<PVBagChartViewWidget*>(pvViewWidget_)->getUserQuantile());
//    connect(quantileSpinBox_, SIGNAL(valueChanged(double)), this, SLOT(updateQuantile()));
//    topLayout->addWidget(quantileSpinBox_, rowGrid, 1);
//  }

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
  gridLayoutTab->addWidget(xlogScaleCheckBox_, 3, 0, 1, 2);

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
  gridLayoutTab->addWidget(ylogScaleCheckBox_, 3, 0, 1, 2);

  tabWidget->addTab(tabVerticalAxis, tr("Y-axis"));

  // --- tab Properties
  if (plotType_ == PVXYChartSettingWidget::Scatter ||
      plotType_ == PVXYChartSettingWidget::Trajectories ||
      plotType_ == PVXYChartSettingWidget::Simple)
  {
    QWidget * propertiesTab = new QWidget;
    QGridLayout * propertiesTabLayout = new QGridLayout(propertiesTab);

    // color
    // if only one representation
    if (pvViewWidget_->getNumberOfRepresentations() == 1 &&
        (plotType_ == PVXYChartSettingWidget::Scatter || plotType_ == PVXYChartSettingWidget::Simple))
    {
      label = new QLabel(tr("Plot color"));
      propertiesTabLayout->addWidget(label, 0, 0);

      colorButton_ = new QToolButton;
      QPixmap px(20, 20);
      px.fill(pvViewWidget_->getRepresentationColor());
      colorButton_->setIcon(px);
      connect(colorButton_, SIGNAL(clicked()), this, SLOT(setColor()));
      propertiesTabLayout->addWidget(colorButton_, 0, 1, Qt::AlignLeft);
    }

    // marker style
    label = new QLabel(tr("Marker style"));
    propertiesTabLayout->addWidget(label, 1, 0);

    markerStyles_ = new QComboBox;
    markerStyles_->addItem(tr("None"), vtkPlotPoints::NONE);
    markerStyles_->addItem(tr("Cross"), vtkPlotPoints::CROSS);
    markerStyles_->addItem(tr("Plus"), vtkPlotPoints::PLUS);
    markerStyles_->addItem(tr("Square"), vtkPlotPoints::SQUARE);
    markerStyles_->addItem(tr("Circle"), vtkPlotPoints::CIRCLE);
    markerStyles_->addItem(tr("Diamond"), vtkPlotPoints::DIAMOND);
    markerStyles_->setCurrentIndex(pvViewWidget_->getMarkerStyle());
    connect(markerStyles_, SIGNAL(currentIndexChanged(int)), this, SLOT(setMarkerStyle(int)));
    propertiesTabLayout->addWidget(markerStyles_, 1, 1);

    // marker size
    label = new QLabel(tr("Marker size"));
    propertiesTabLayout->addWidget(label, 2, 0);

    QSpinBox * markerSize = new QSpinBox;
    markerSize->setMinimum(1);
    markerSize->setMaximum(10);
    markerSize->setValue(2);
    connect(markerSize, SIGNAL(valueChanged(int)), pvViewWidget_, SLOT(setMarkerSize(int)));
    propertiesTabLayout->addWidget(markerSize, 2, 1);

    propertiesTabLayout->setRowStretch(3, 1);
    propertiesTabLayout->setColumnStretch(1, 2);
    tabWidget->addTab(propertiesTab, tr("Plot style"));
  }

  frameLayout->addWidget(tabWidget);

  // Bottom layout
  // export button
  QHBoxLayout * hboxForBottomButtons = new QHBoxLayout;
  QPushButton * button = new QPushButton(QIcon(":/images/document-export-table.png"), tr("Export"));
  connect(button, SIGNAL(clicked()), pvViewWidget_, SLOT(exportPlot()));
  hboxForBottomButtons->addWidget(button);
  hboxForBottomButtons->addStretch();

  frameLayout->addLayout(hboxForBottomButtons);
  frameLayout->addStretch();

  // update widgets
  if (plotType_ == PVXYChartSettingWidget::Scatter)
    updateYComboBox();

  //
  scrollArea->setWidget(frame);
  mainLayout->addWidget(scrollArea);
  setVisible(false);
}


void PVXYChartSettingWidget::modifyData(bool isRankRequired)
{
  pvViewWidget_->updateTable(isRankRequired ? sampleRank_ : sample_, 0);
  if (failedSample_.getSize())
    pvViewWidget_->updateTable(isRankRequired ? failedSampleRank_ : failedSample_, 1);
  if (notEvalSample_.getSize())
    pvViewWidget_->updateTable(isRankRequired ? notEvalSampleRank_ : notEvalSample_, failedSample_.getSize() ? 2 : 1);
}


void PVXYChartSettingWidget::updateLineEdits()
{
  Q_ASSERT(xAxisComboBox_ && yAxisComboBox_ && titleLineEdit_);
  Q_ASSERT(xlabelLineEdit_ && xmin_ && xmax_);
  Q_ASSERT(ylabelLineEdit_ && ymin_ && ymax_);

  SignalBlocker xlabelLineEditBlocker(xlabelLineEdit_);
  SignalBlocker xminBlocker(xmin_);
  SignalBlocker xmaxBlocker(xmax_);
  SignalBlocker yminBlocker(ymin_);
  SignalBlocker ymaxBlocker(ymax_);
  SignalBlocker ylabelLineEditBlocker(ylabelLineEdit_);
  SignalBlocker titleLineEditBlocker(titleLineEdit_);

  titleLineEdit_->setText(pvViewWidget_->getChartTitle(xAxisComboBox_->currentText(), yAxisComboBox_->currentText()));
  xlabelLineEdit_->setText(pvViewWidget_->getXAxisTitle(xAxisComboBox_->currentText(), yAxisComboBox_->currentText()));
  xmin_->setValue(pvViewWidget_->getXAxisRangeMinimum());
  xmax_->setValue(pvViewWidget_->getXAxisRangeMaximum());
  ylabelLineEdit_->setText(pvViewWidget_->getYAxisTitle(xAxisComboBox_->currentText(), yAxisComboBox_->currentText()));
  ymin_->setValue(pvViewWidget_->getYAxisRangeMinimum());
  ymax_->setValue(pvViewWidget_->getYAxisRangeMaximum());

  if (xlogScaleCheckBox_ && ylogScaleCheckBox_)
  {
    xlogScaleCheckBox_->setEnabled(pvViewWidget_->logScalingValidForXAxis());
    ylogScaleCheckBox_->setEnabled(pvViewWidget_->logScalingValidForYAxis());
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

  const QString currentYText = yAxisComboBox_->currentText();
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

  if (plotType_ == PVXYChartSettingWidget::Scatter)
    yAxisComboBox_->addItems(outputNames + inputNames);
  yAxisComboBox_->setCurrentText(currentYText);

  plotChanged();
}


void PVXYChartSettingWidget::plotChanged()
{
  Q_ASSERT(xAxisComboBox_ && yAxisComboBox_);
  xlogScaleCheckBox_->setChecked(false);
  ylogScaleCheckBox_->setChecked(false);

  pvViewWidget_->showChart(xAxisComboBox_->currentText(), yAxisComboBox_->currentText());
  updateLineEdits();
}


void PVXYChartSettingWidget::updateTitle()
{
  Q_ASSERT(xAxisComboBox_ && yAxisComboBox_ && titleLineEdit_);
  pvViewWidget_->setChartTitle(xAxisComboBox_->currentText(), yAxisComboBox_->currentText(), titleLineEdit_->text());
}


void PVXYChartSettingWidget::updateXLabel()
{
  Q_ASSERT(xAxisComboBox_ && yAxisComboBox_ && xlabelLineEdit_);
  pvViewWidget_->setXAxisTitle(xAxisComboBox_->currentText(), yAxisComboBox_->currentText(), xlabelLineEdit_->text());
}


void PVXYChartSettingWidget::updateYLabel()
{
  Q_ASSERT(xAxisComboBox_ && yAxisComboBox_ && ylabelLineEdit_);
  pvViewWidget_->setYAxisTitle(xAxisComboBox_->currentText(), yAxisComboBox_->currentText(), ylabelLineEdit_->text());
}


void PVXYChartSettingWidget::updateXrange()
{
  Q_ASSERT(xmin_ && xmax_);
  pvViewWidget_->setXAxisRange(xmin_->value(), xmax_->value());
}


void PVXYChartSettingWidget::updateYrange()
{
  Q_ASSERT(ymin_ && ymax_);
  pvViewWidget_->setYAxisRange(ymin_->value(), ymax_->value());
}


void PVXYChartSettingWidget::updateQuantile()
{
  Q_ASSERT(quantileSpinBox_);
  dynamic_cast<PVBagChartViewWidget*>(pvViewWidget_)->setUserQuantile(quantileSpinBox_->value());
}


void PVXYChartSettingWidget::setColor()
{
  const QColor color = QColorDialog::getColor(pvViewWidget_->getRepresentationColor(), this, tr("Select Color"));

  if (color.isValid())
  {
    pvViewWidget_->setRepresentationColor(color);

    // update button color
    QPixmap px(20, 20);
    px.fill(color);
    colorButton_->setIcon(px);
  }
}


void PVXYChartSettingWidget::setMarkerStyle(const int index)
{
  const int style = markerStyles_->itemData(index).toInt();
  pvViewWidget_->setMarkerStyle(style);
}


QSize PVXYChartSettingWidget::sizeHint() const
{
  QSize size = QWidget::sizeHint();
  const int hScrollBarHeight = style()->pixelMetric(QStyle::PM_ScrollBarExtent);
  size.setHeight(size.height() + hScrollBarHeight);
  return size;
}


QSize PVXYChartSettingWidget::minimumSizeHint() const
{
  QSize size = QWidget::minimumSizeHint();
  size.setHeight(10);
  return size;
}
}
