//                                               -*- C++ -*-
/**
 *  @brief QMdiSubWindow for the analysis of data
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
#include "otgui/DataAnalysisWindow.hxx"

#include "otgui/QtTools.hxx"
#include "otgui/DesignOfExperimentWindow.hxx"
#include "otgui/MinMaxTableGroupBox.hxx"
#include "otgui/MomentsEstimatesTableGroupBox.hxx"

#include <QVBoxLayout>
#include <QScrollArea>
#include <QHeaderView>

using namespace OT;

namespace OTGUI {

DataAnalysisWindow::DataAnalysisWindow(AnalysisItem * item)
  : ResultWindow(item)
  , result_()
  , resultsSampleIsValid_(true)
  , sampleSizeTitle_(tr("Sample size: "))
  , stochInputNames_(QStringList())
  , inAxisTitles_(QStringList())
  , outputNames_(QStringList())
  , outAxisTitles_(QStringList())
  , isConfidenceIntervalRequired_(false)
  , levelConfidenceInterval_(0.)
  , tabWidget_(new QTabWidget)
  , variablesComboBox_(0)
  , pdf_cdfPlotsConfigurationWidget_(0)
  , boxPlotsConfigurationWidget_(0)
  , scatterPlotsConfigurationWidget_(0)
  , plotMatrixConfigurationWidget_(0)
  , probaSpinBox_(0)
  , quantileSpinBox_(0)
  , tabOffset_(0)
{
}


void DataAnalysisWindow::buildInterface()
{
  // first tab: Table --------------------------------
  // table if MonteCarlo result

  // second tab: Summary -----------------------------
  QWidget * tab = new QWidget;
  QVBoxLayout * tabLayout = new QVBoxLayout(tab);

  // if the sample is valid:
  if (resultsSampleIsValid_)
  {
    QScrollArea * scrollArea = new QScrollArea;
    scrollArea->setWidgetResizable(true);
    tabLayout->setSizeConstraint(QLayout::SetFixedSize);

    // -- output name --
    QHBoxLayout * headLayout = new QHBoxLayout;
    QLabel * outputName = new QLabel(tr("Variable"));
    headLayout->addWidget(outputName);
    variablesComboBox_ = new QComboBox;
    variablesComboBox_->addItems(stochInputNames_ + outputNames_);
    connect(variablesComboBox_, SIGNAL(currentIndexChanged(int)), this, SLOT(updateSpinBoxes(int)));
    headLayout->addWidget(variablesComboBox_);
    headLayout->addStretch();
    tabLayout->addLayout(headLayout);

    // -- results --
    QVBoxLayout * vbox = new QVBoxLayout;

    // elapsed time
    if (result_.getElapsedTime() > 0.)
    {
      QLabel * elapsedTimeLabel = new QLabel(tr("Elapsed time:") + " " + QString::number(result_.getElapsedTime()) + " s");
      elapsedTimeLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);
      tabLayout->addWidget(elapsedTimeLabel);
    }

    // sample size
    QLabel * nbSimuLabel = new QLabel(sampleSizeTitle_ + QString::number(result_.getInputSample().getSize()) + "\n");
    nbSimuLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);
    vbox->addWidget(nbSimuLabel);

    // min/max table
    MinMaxTableGroupBox * minMaxTableGroupBox = new MinMaxTableGroupBox(result_, false);
    vbox->addWidget(minMaxTableGroupBox);
    connect(variablesComboBox_, SIGNAL(currentIndexChanged(int)), minMaxTableGroupBox, SLOT(setCurrentIndexStackedWidget(int)));

    // moments estimation
    try
    {
      MomentsEstimatesTableGroupBox * estimatesGroupBox = new MomentsEstimatesTableGroupBox(result_, isConfidenceIntervalRequired_, levelConfidenceInterval_);
      vbox->addWidget(estimatesGroupBox);
      connect(variablesComboBox_, SIGNAL(currentIndexChanged(int)), estimatesGroupBox, SLOT(setCurrentIndexStackedWidget(int)));
    }
    catch (std::exception & ex)
    {
    }

    // quantiles
    QHBoxLayout * quantLayout = new QHBoxLayout;

    // Probability
    QLabel * label = new QLabel(tr("Probability"));
    label->setStyleSheet("font: bold;");
    quantLayout->addWidget(label);
    probaSpinBox_ = new DoubleSpinBox;
    label->setBuddy(probaSpinBox_);
    probaSpinBox_->setMinimum(0.0);
    probaSpinBox_->setMaximum(1.0);
    probaSpinBox_->setSingleStep(0.01);
    quantLayout->addWidget(probaSpinBox_);
    // Quantile
    label = new QLabel(tr("Quantile"));
    label->setStyleSheet("font: bold;");
    quantLayout->addWidget(label);
    quantileSpinBox_ = new DoubleSpinBox;
    label->setBuddy(quantileSpinBox_);
    quantileSpinBox_->setDecimals(8);
    quantLayout->addWidget(quantileSpinBox_);

    connect(probaSpinBox_, SIGNAL(valueChanged(double)), this, SLOT(probaValueChanged(double)));
    connect(quantileSpinBox_, SIGNAL(valueChanged(double)), this, SLOT(quantileValueChanged(double)));

    vbox->addLayout(quantLayout);

    tabLayout->addLayout(vbox);

    updateSpinBoxes();
    scrollArea->setWidget(tab);
    tabWidget_->addTab(scrollArea, tr("Summary"));
  }
  // if the results sample contains NAN
  else
  {
    QLabel * summaryErrorMessage = new QLabel(tr("Computation failed. Some results are not available."));
    summaryErrorMessage->setWordWrap(true);
    tabLayout->addWidget(summaryErrorMessage);
    tabLayout->addStretch();
    tabWidget_->addTab(tab, tr("Summary"));
  }

  // third tab: PDF/CDF ------------------------------

  // if the sample is valid:
  if (resultsSampleIsValid_)
  {
    tab = getPDF_CDFWidget();
  }
  // if the results sample contains NAN
  else
  {
    tab = new QWidget;
    tabLayout = new QVBoxLayout(tab);
    QLabel * summaryErrorMessage = new QLabel(tr("Computation failed. Some results are not available."));
    summaryErrorMessage->setWordWrap(true);
    tabLayout->addWidget(summaryErrorMessage);
    tabLayout->addStretch();
  }
  tabWidget_->addTab(tab, tr("PDF/CDF"));

  // fourth tab: box plots ---------------------------

  // if the sample is valid:
  if (resultsSampleIsValid_)
  {
    tab = getBoxPlotWidget();
  }
  // if the results sample contains NAN
  else
  {
    tab = new QWidget;
    tabLayout = new QVBoxLayout(tab);
    QLabel * summaryErrorMessage = new QLabel(tr("Computation failed. Some results are not available."));
    summaryErrorMessage->setWordWrap(true);
    tabLayout->addWidget(summaryErrorMessage);
    tabLayout->addStretch();
  }
  tabWidget_->addTab(tab, tr("Box plots"));

  // fifth tab: scatter plots ------------------------

  // if the sample is valid:
  if (resultsSampleIsValid_)
  {
    tab = getScatterPlotsWidget();
  }
  // if the results sample contains NAN
  else
  {
    tab = new QWidget;
    tabLayout = new QVBoxLayout(tab);
    QLabel * summaryErrorMessage = new QLabel(tr("Computation failed. Some results are not available."));
    summaryErrorMessage->setWordWrap(true);
    tabLayout->addWidget(summaryErrorMessage);
    tabLayout->addStretch();
  }
  tabWidget_->addTab(tab, tr("Scatter plots"));

  // sixth tab: plot matrix X-X ----------------------

  tab = new PlotMatrixWidget(result_.getInputSample(), result_.getInputSample());
  plotMatrix_X_X_ConfigurationWidget_ = new PlotMatrixConfigurationWidget(dynamic_cast<PlotMatrixWidget*>(tab));

  tabWidget_->addTab(tab, tr("Plot matrix X-X"));

  // seventh tab: plot matrix Y-X ----------------------

  if (result_.getOutputSample().getSize())
  {
    // if the sample is valid:
    if (resultsSampleIsValid_)
    {
      tab = new PlotMatrixWidget(result_.getInputSample(), result_.getOutputSample());
      plotMatrixConfigurationWidget_ = new PlotMatrixConfigurationWidget(dynamic_cast<PlotMatrixWidget*>(tab));
    }
    // if the results sample contains NAN
    else
    {
      tab = new QWidget;
      tabLayout = new QVBoxLayout(tab);
      QLabel * summaryErrorMessage = new QLabel(tr("Computation failed. Some results are not available."));
      summaryErrorMessage->setWordWrap(true);
      tabLayout->addWidget(summaryErrorMessage);
      tabLayout->addStretch();
    }
    tabWidget_->addTab(tab, tr("Plot matrix Y-X"));
  }

  //
  connect(tabWidget_, SIGNAL(currentChanged(int)), this, SLOT(showHideGraphConfigurationWidget(int)));
  setWidget(tabWidget_);
}


QWidget* DataAnalysisWindow::getPDF_CDFWidget()
{
  QWidget * tab = new QWidget;
  QVBoxLayout * plotLayout = new QVBoxLayout(tab);
  ResizableStackedWidget * stackedWidget = new ResizableStackedWidget;

  QVector<PlotWidget*> listPlotWidgets;
  const QStringList variablesNames = stochInputNames_ + outputNames_;
  const QStringList variablesAxisTitles = inAxisTitles_ + outAxisTitles_;

  for (int i=0; i<variablesNames.size(); ++i)
  {
    PlotWidget * plot = new PlotWidget("distributionPDF");

    // PDF
    plot->plotHistogram(result_.getSample().getMarginal(i));
    if (result_.getPDF()[i].getSize())
      plot->plotCurve(result_.getPDF()[i]);
    plot->setTitle(tr("PDF: ") + variablesNames[i]);
    plot->setAxisTitle(QwtPlot::xBottom, variablesAxisTitles[i]);

    stackedWidget->addWidget(plot);
    listPlotWidgets.append(plot);

    // CDF
    plot = new PlotWidget("distributionCDF");
    plot->plotHistogram(result_.getSample().getMarginal(i), 1);
    if (result_.getCDF()[i].getSize())
      plot->plotCurve(result_.getCDF()[i]);
    plot->setTitle(tr("CDF: ") + variablesNames[i]);
    plot->setAxisTitle(QwtPlot::xBottom, variablesAxisTitles[i]);

    stackedWidget->addWidget(plot);
    listPlotWidgets.append(plot);
  }
  plotLayout->addWidget(stackedWidget);

  pdf_cdfPlotsConfigurationWidget_ = new GraphConfigurationWidget(listPlotWidgets, QStringList(), variablesNames, GraphConfigurationWidget::PDFResult);
  connect(pdf_cdfPlotsConfigurationWidget_, SIGNAL(currentPlotChanged(int)), stackedWidget, SLOT(setCurrentIndex(int)));

  return tab;
}


QWidget* DataAnalysisWindow::getBoxPlotWidget()
{
  QWidget * tab = new QWidget;
  QVBoxLayout * boxPlotLayout = new QVBoxLayout(tab);
  ResizableStackedWidget * stackedWidget = new ResizableStackedWidget;

  QVector<PlotWidget*> listBoxPlotWidgets;
  const QStringList variablesNames = stochInputNames_ + outputNames_;
  const QStringList variablesAxisTitles = inAxisTitles_ + outAxisTitles_;

  for (int i=0; i<variablesNames.size(); ++i)
  {
    PlotWidget * plot = new PlotWidget("boxplot");

    const double median = result_.getMedian()[i];
    const double Q1 = result_.getFirstQuartile()[i];
    const double Q3 = result_.getThirdQuartile()[i];
    plot->plotBoxPlot(median, Q1, Q3, Q1 - 1.5*(Q3-Q1), Q3 + 1.5*(Q3-Q1), result_.getOutliers()[i]);
    plot->setTitle(tr("Box plot: ") + variablesNames[i]);
    plot->setAxisTitle(QwtPlot::yLeft, variablesAxisTitles[i]);

    stackedWidget->addWidget(plot);
    listBoxPlotWidgets.append(plot);
  }
  boxPlotLayout->addWidget(stackedWidget);

  boxPlotsConfigurationWidget_ = new GraphConfigurationWidget(listBoxPlotWidgets, QStringList(), variablesNames, GraphConfigurationWidget::BoxPlot);
  connect(boxPlotsConfigurationWidget_, SIGNAL(currentPlotChanged(int)), stackedWidget, SLOT(setCurrentIndex(int)));

  return tab;
}


QWidget* DataAnalysisWindow::getScatterPlotsWidget()
{
  QWidget * tab = new QWidget;
  QVBoxLayout * scatterPlotLayout = new QVBoxLayout(tab);

  QVector<PlotWidget*> listScatterPlotWidgets =
    DesignOfExperimentWindow::GetListScatterPlots(result_.getInputSample(), result_.getOutputSample(),
                                                  stochInputNames_, inAxisTitles_,
                                                  outputNames_, outAxisTitles_);

  ResizableStackedWidget * stackedWidget = new ResizableStackedWidget;
  for (int i=0; i<listScatterPlotWidgets.size(); ++i)
    stackedWidget->addWidget(listScatterPlotWidgets[i]);

  scatterPlotLayout->addWidget(stackedWidget);
  scatterPlotsConfigurationWidget_ = new GraphConfigurationWidget(listScatterPlotWidgets, stochInputNames_, outputNames_, GraphConfigurationWidget::Scatter);
  connect(scatterPlotsConfigurationWidget_, SIGNAL(currentPlotChanged(int)), stackedWidget, SLOT(setCurrentIndex(int)));

  return tab;
}


void DataAnalysisWindow::updateSpinBoxes(int indexOutput)
{
  SignalBlocker blocker(quantileSpinBox_);
  if (result_.getMin().getSize() && result_.getMax().getSize())
  {
    const double min = result_.getMin()[indexOutput];
    const double max = result_.getMax()[indexOutput];

    quantileSpinBox_->setMinimum(min);
    quantileSpinBox_->setMaximum(max);
    quantileSpinBox_->setSingleStep((max-min)/100);
  }
  probaSpinBox_->setValue(0.5);
}


void DataAnalysisWindow::probaValueChanged(double proba)
{
  SignalBlocker blocker(quantileSpinBox_);
  quantileSpinBox_->setValue(result_.getSample().getMarginal(variablesComboBox_->currentIndex()).computeQuantile(proba)[0]);
}


void DataAnalysisWindow::quantileValueChanged(double quantile)
{
  SignalBlocker blocker(probaSpinBox_);
  double cdf = 0.0;
  const double p = 1.0 / double(result_.getSample().getSize());

  for (UnsignedInteger j=0; j<result_.getSample().getSize(); ++j)
    if (result_.getSample()[j][variablesComboBox_->currentIndex()] < quantile)
      cdf += p;

  probaSpinBox_->setValue(cdf);
}


void DataAnalysisWindow::showHideGraphConfigurationWidget(int indexTab)
{
  switch (indexTab-tabOffset_)
  {
    // if a plotWidget is visible
    case 1: // PDF-CDF graph
      if (pdf_cdfPlotsConfigurationWidget_)
        if (!pdf_cdfPlotsConfigurationWidget_->isVisible())
          emit graphWindowActivated(pdf_cdfPlotsConfigurationWidget_);
      break;
    case 2: // box plot
      if (boxPlotsConfigurationWidget_)
        if (!boxPlotsConfigurationWidget_->isVisible())
          emit graphWindowActivated(boxPlotsConfigurationWidget_);
      break;
    case 3: // scatter plot
      if (scatterPlotsConfigurationWidget_)
        if (!scatterPlotsConfigurationWidget_->isVisible())
          emit graphWindowActivated(scatterPlotsConfigurationWidget_);
      break;
    case 4: // plot matrix X-X
      if (plotMatrix_X_X_ConfigurationWidget_)
        if (!plotMatrix_X_X_ConfigurationWidget_->isVisible())
          emit graphWindowActivated(plotMatrix_X_X_ConfigurationWidget_);
      break;
    case 5: // plot matrix Y-X
      if (plotMatrixConfigurationWidget_)
        if (!plotMatrixConfigurationWidget_->isVisible())
          emit graphWindowActivated(plotMatrixConfigurationWidget_);
      break;
    // if no plotWidget is visible
    default:
    {
      emit graphWindowDeactivated();
      break;
    }
  }
}


void DataAnalysisWindow::showHideGraphConfigurationWidget(Qt::WindowStates oldState, Qt::WindowStates newState)
{
  if (oldState == Qt::WindowMaximized)
    return;

  if (newState == Qt::WindowFullScreen || newState == (Qt::WindowActive|Qt::WindowMaximized))
    showHideGraphConfigurationWidget(tabWidget_->currentIndex());
  else if (newState == Qt::WindowNoState || newState == Qt::WindowMinimized || newState == (Qt::WindowActive|Qt::WindowMinimized))
    showHideGraphConfigurationWidget(-1);
}
}