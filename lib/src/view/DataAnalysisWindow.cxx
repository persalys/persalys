//                                               -*- C++ -*-
/**
 *  @brief QMdiSubWindow for the analysis of data
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
#include "otgui/DataAnalysisWindow.hxx"

#include "otgui/QtTools.hxx"
#include "otgui/DesignOfExperimentWindow.hxx"
#include "otgui/MinMaxTableGroupBox.hxx"
#include "otgui/MomentsEstimatesTableGroupBox.hxx"
#include "otgui/PlotWidgetWithGraphSetting.hxx"
#include "otgui/ParametersTableView.hxx"
#include "otgui/ExportableTableView.hxx"
#include "otgui/SampleTableModel.hxx"

#include <QVBoxLayout>
#include <QScrollArea>
#include <QHeaderView>
#include <QSplitter>
#include <QSortFilterProxyModel>

using namespace OT;

namespace OTGUI {

DataAnalysisWindow::DataAnalysisWindow(AnalysisItem * item)
  : ResultWindow(item)
  , result_()
  , resultsSampleIsValid_(true)
  , sampleSizeTitle_(tr("Sample size"))
  , stochInputNames_(QStringList())
  , inAxisTitles_(QStringList())
  , outputNames_(QStringList())
  , outAxisTitles_(QStringList())
  , isConfidenceIntervalRequired_(false)
  , levelConfidenceInterval_(0.)
  , showTable_(false)
  , variablesGroupBox_(0)
  , variablesListWidget_(0)
  , tabWidget_(new QTabWidget)
  , scatterPlotsTabWidget_(0)
  , scatterPlotsConfigurationWidget_(0)
  , plotMatrixConfigurationWidget_(0)
  , plotMatrix_X_X_ConfigurationWidget_(0)
  , probaSpinBox_(0)
  , quantileSpinBox_(0)
{
}


DataAnalysisWindow::~DataAnalysisWindow()
{
  delete plotMatrixConfigurationWidget_;
  delete plotMatrix_X_X_ConfigurationWidget_;
  delete scatterPlotsConfigurationWidget_;
  plotMatrixConfigurationWidget_ = 0;
  plotMatrix_X_X_ConfigurationWidget_ = 0;
  scatterPlotsConfigurationWidget_ = 0;
}


void DataAnalysisWindow::buildInterface()
{
  // get output info
  QStringList variablesNames = outputNames_ + stochInputNames_;

  // main splitter
  QSplitter * mainWidget = new QSplitter(Qt::Horizontal);

  // - list outputs
  variablesGroupBox_ = new QGroupBox(tr("Variables"));
  QVBoxLayout * outputsLayoutGroupBox = new QVBoxLayout(variablesGroupBox_);

  variablesListWidget_ = new OTguiListWidget;
  variablesListWidget_->addItems(variablesNames);
  variablesListWidget_->setCurrentRow(0);
  connect(variablesListWidget_, SIGNAL(currentRowChanged(int)), this, SLOT(updateSpinBoxes(int)));
  outputsLayoutGroupBox->addWidget(variablesListWidget_);

  mainWidget->addWidget(variablesGroupBox_);
  variablesGroupBox_->hide();
  mainWidget->setStretchFactor(0, 1);

  // first tab: Summary -----------------------------
  QWidget * tab = new QWidget;
  QVBoxLayout * tabLayout = new QVBoxLayout(tab);

  // if the sample is valid:
  if (resultsSampleIsValid_)
  {
    QScrollArea * scrollArea = new QScrollArea;
    scrollArea->setWidgetResizable(true);
    tabLayout->setSizeConstraint(QLayout::SetFixedSize);

    // -- results --
    QVBoxLayout * vbox = new QVBoxLayout;

    // stop criteria
    QString groupBoxTitle = (result_.getElapsedTime() > 0.)? tr("Stop criteria") : tr("");
    QGroupBox * parametersGroupBox = new QGroupBox(groupBoxTitle);
    QVBoxLayout * parametersGroupBoxLayout = new QVBoxLayout(parametersGroupBox);

    QStringList namesList;
    // elapsed time
    if (result_.getElapsedTime() > 0.)
      namesList << tr("Elapsed time");
    // sample size
    namesList << sampleSizeTitle_;

    QStringList valuesList;
    if (result_.getElapsedTime() > 0.)
      valuesList << QString::number(result_.getElapsedTime()) + " s";
    valuesList << QString::number(result_.getSample().getSize());

    ParametersTableView * table = new ParametersTableView(namesList, valuesList, true, true);
    parametersGroupBoxLayout->addWidget(table);
    parametersGroupBoxLayout->addStretch();
    vbox->addWidget(parametersGroupBox);

    // min/max table
    MinMaxTableGroupBox * minMaxTableGroupBox = new MinMaxTableGroupBox(result_, false);
    vbox->addWidget(minMaxTableGroupBox);
    connect(variablesListWidget_, SIGNAL(currentRowChanged(int)), minMaxTableGroupBox, SLOT(setCurrentIndexStackedWidget(int)));

    // moments estimation
    try
    {
      MomentsEstimatesTableGroupBox * estimatesGroupBox = new MomentsEstimatesTableGroupBox(result_, isConfidenceIntervalRequired_, levelConfidenceInterval_);
      vbox->addWidget(estimatesGroupBox);
      connect(variablesListWidget_, SIGNAL(currentRowChanged(int)), estimatesGroupBox, SLOT(setCurrentIndexStackedWidget(int)));
    }
    catch (std::exception & ex)
    {
      qDebug() << "Error: In DataAnalysisWindow::buildInterface when creating the MomentsEstimatesTableGroupBox " << ex.what();
    }

    // quantiles
    QGridLayout * quantLayout = new QGridLayout;

    // Probability
    QLabel * label = new QLabel(tr("Probability"));
    label->setStyleSheet("font: bold;");
    quantLayout->addWidget(label, 0, 0);
    probaSpinBox_ = new DoubleSpinBox;
    label->setBuddy(probaSpinBox_);
    probaSpinBox_->setMinimum(0.0);
    probaSpinBox_->setMaximum(1.0);
    probaSpinBox_->setSingleStep(0.01);
    quantLayout->addWidget(probaSpinBox_, 0, 1);
    // Quantile
    label = new QLabel(tr("Quantile"));
    label->setStyleSheet("font: bold;");
    quantLayout->addWidget(label, 1, 0);
    quantileSpinBox_ = new DoubleSpinBox;
    label->setBuddy(quantileSpinBox_);
    quantileSpinBox_->setDecimals(8);
    quantLayout->addWidget(quantileSpinBox_, 1, 1);

    connect(probaSpinBox_, SIGNAL(valueChanged(double)), this, SLOT(probaValueChanged(double)));
    connect(quantileSpinBox_, SIGNAL(valueChanged(double)), this, SLOT(quantileValueChanged(double)));

    vbox->addLayout(quantLayout);
    updateSpinBoxes();

    tabLayout->addLayout(vbox);
    tabLayout->addStretch();

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

  // second tab: PDF/CDF ------------------------------

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

  // third tab: box plots ---------------------------

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

  // fourth tab: scatter plots ------------------------

  scatterPlotsTabWidget_ = new QTabWidget;

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
  scatterPlotsTabWidget_->addTab(tab, tr("Scatter plots"));

  // fourth tab: plot matrix X-X ----------------------

  if (result_.getInputSample().getSize())
  {
    tab = new PlotMatrixWidget(result_.getInputSample(), result_.getInputSample());
    plotMatrix_X_X_ConfigurationWidget_ = new PlotMatrixConfigurationWidget(dynamic_cast<PlotMatrixWidget*>(tab));

    scatterPlotsTabWidget_->addTab(tab, tr("Plot matrix X-X"));

  // fourth tab: plot matrix Y-X ----------------------

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
      scatterPlotsTabWidget_->addTab(tab, tr("Plot matrix Y-X"));
    }
    connect(scatterPlotsTabWidget_, SIGNAL(currentChanged(int)), this, SLOT(scatterPlotsTabWidgetIndexChanged()));
    tabWidget_->addTab(scatterPlotsTabWidget_, tr("Scatter plots"));
  }

  // fifth tab: Table --------------------------------
  // table if MonteCarlo result
  if (showTable_)
  {
    QWidget * tab = new QWidget;
    QVBoxLayout * tabLayout = new QVBoxLayout(tab);

    ExportableTableView * tabResultView = new ExportableTableView;
    tabResultView->setSortingEnabled(true);

    SampleTableModel * tabResultModel = new SampleTableModel(result_.getSample(), tabResultView);
    QSortFilterProxyModel * proxyModel = new QSortFilterProxyModel(tabResultView);
    proxyModel->setSourceModel(tabResultModel);

    tabResultView->setModel(proxyModel);
    tabResultView->sortByColumn(0, Qt::AscendingOrder);

    tabLayout->addWidget(tabResultView);

    tabWidget_->addTab(tab, tr("Table"));
  }

  // sixth tab --------------------------------
  if (parametersWidget_)
    tabWidget_->addTab(parametersWidget_, tr("Parameters"));

  //
  connect(tabWidget_, SIGNAL(currentChanged(int)), this, SLOT(showHideGraphConfigurationWidget(int)));

  mainWidget->addWidget(tabWidget_);
  mainWidget->setStretchFactor(1, 10);

  setWidget(mainWidget);
}


QWidget* DataAnalysisWindow::getPDF_CDFWidget()
{
  ResizableStackedWidget * tabStackedWidget = new ResizableStackedWidget;
  connect(variablesListWidget_, SIGNAL(currentRowChanged(int)), tabStackedWidget, SLOT(setCurrentIndex(int)));

  const QStringList variablesNames = outputNames_ + stochInputNames_;
  const QStringList variablesAxisTitles = outAxisTitles_ + inAxisTitles_;

  // we want to display output results before the input results
  // input indices
  Indices inInd(stochInputNames_.size());
  inInd.fill();
  // ouput indices
  Indices ind(outputNames_.size());
  ind.fill(stochInputNames_.size());
  // indices with good order
  ind.add(inInd);

  for (int i=0; i<variablesNames.size(); ++i)
  {
    QVector<PlotWidget*> listPlotWidgets;

    PlotWidget * plot = new PlotWidget(tr("distributionPDF"));
    // PDF
    plot->plotHistogram(result_.getSample().getMarginal(ind[i]));
    if (result_.getPDF()[ind[i]].getSize())
      plot->plotCurve(result_.getPDF()[ind[i]]);
    plot->setTitle(tr("PDF:") + " " + variablesNames[i]);
    plot->setAxisTitle(QwtPlot::xBottom, variablesAxisTitles[i]);
    plot->setAxisTitle(QwtPlot::yLeft, tr("Density"));

    listPlotWidgets.append(plot);

    // CDF
    plot = new PlotWidget(tr("distributionCDF"));
    plot->plotHistogram(result_.getSample().getMarginal(ind[i]), 1);
    if (result_.getCDF()[ind[i]].getSize())
      plot->plotCurve(result_.getCDF()[ind[i]]);
    plot->setTitle(tr("CDF:") + " " + variablesNames[i]);
    plot->setAxisTitle(QwtPlot::xBottom, variablesAxisTitles[i]);
    plot->setAxisTitle(QwtPlot::yLeft, tr("CDF"));

    listPlotWidgets.append(plot);

    // PlotWidgetWithGraphSetting
    PlotWidgetWithGraphSetting * plotWidget = new PlotWidgetWithGraphSetting(i, listPlotWidgets, GraphConfigurationWidget::PDFResult);

    connect(plotWidget, SIGNAL(graphWindowActivated(QWidget*)), this, SIGNAL(graphWindowActivated(QWidget*)));
    connect(variablesListWidget_, SIGNAL(currentRowChanged(int)), plotWidget, SLOT(showHideGraphConfigurationWidget(int)));
    connect(this, SIGNAL(stateChanged(int)), plotWidget, SLOT(showHideGraphConfigurationWidget(int)));

    tabStackedWidget->addWidget(plotWidget);
  }

  return tabStackedWidget;
}


QWidget* DataAnalysisWindow::getBoxPlotWidget()
{
  ResizableStackedWidget * tabStackedWidget = new ResizableStackedWidget;
  connect(variablesListWidget_, SIGNAL(currentRowChanged(int)), tabStackedWidget, SLOT(setCurrentIndex(int)));

  const QStringList variablesNames = outputNames_ + stochInputNames_;
  const QStringList variablesAxisTitles = outAxisTitles_ + inAxisTitles_;

  // we want to display output results before the input results
  // input indices
  Indices inInd(stochInputNames_.size());
  inInd.fill();
  // ouput indices
  Indices ind(outputNames_.size());
  ind.fill(stochInputNames_.size());
  // indices with good order
  ind.add(inInd);

  for (int i=0; i<variablesNames.size(); ++i)
  {
    QVector<PlotWidget*> listBoxPlotWidgets;

    PlotWidget * plot = new PlotWidget(tr("boxplot"));

    const double median = result_.getMedian()[ind[i]][0];
    const double Q1 = result_.getFirstQuartile()[ind[i]][0];
    const double Q3 = result_.getThirdQuartile()[ind[i]][0];
    plot->plotBoxPlot(median, Q1, Q3, Q1 - 1.5*(Q3-Q1), Q3 + 1.5*(Q3-Q1), result_.getOutliers()[ind[i]]);
    plot->setTitle(tr("Box plot:") + " " + variablesNames[i]);
    plot->setAxisTitle(QwtPlot::yLeft, variablesAxisTitles[i]);

    listBoxPlotWidgets.append(plot);

    // PlotWidgetWithGraphSetting
    PlotWidgetWithGraphSetting * plotWidget = new PlotWidgetWithGraphSetting(i, listBoxPlotWidgets, GraphConfigurationWidget::NoType);
    connect(plotWidget, SIGNAL(graphWindowActivated(QWidget*)), this, SIGNAL(graphWindowActivated(QWidget*)));
    connect(variablesListWidget_, SIGNAL(currentRowChanged(int)), plotWidget, SLOT(showHideGraphConfigurationWidget(int)));
    connect(this, SIGNAL(stateChanged(int)), plotWidget, SLOT(showHideGraphConfigurationWidget(int)));

    tabStackedWidget->addWidget(plotWidget);
  }

  return tabStackedWidget;
}


QWidget* DataAnalysisWindow::getScatterPlotsWidget()
{
  QWidget * tab = new QWidget;
  QVBoxLayout * scatterPlotLayout = new QVBoxLayout(tab);

  QVector<PlotWidget*> listScatterPlotWidgets =
    DesignOfExperimentWindow::GetListScatterPlots(result_.getInputSample(),
                                                  result_.getOutputSample(),
                                                  Sample(),
                                                  stochInputNames_,
                                                  inAxisTitles_,
                                                  outputNames_,
                                                  outAxisTitles_);

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
    const double min = result_.getMin()[indexOutput][0];
    const double max = result_.getMax()[indexOutput][0];

    quantileSpinBox_->setMinimum(min);
    quantileSpinBox_->setMaximum(max);
    quantileSpinBox_->setSingleStep((max-min)/100);
  }
  probaSpinBox_->setValue(0.5);
}


void DataAnalysisWindow::probaValueChanged(double proba)
{
  SignalBlocker blocker(quantileSpinBox_);
  quantileSpinBox_->setValue(result_.getSample().getMarginal(variablesListWidget_->currentRow()).computeQuantile(proba)[0]);
}


void DataAnalysisWindow::quantileValueChanged(double quantile)
{
  SignalBlocker blocker(probaSpinBox_);
  double cdf = 0.0;
  const double p = 1.0 / double(result_.getSample().getSize());

  for (UnsignedInteger j=0; j<result_.getSample().getSize(); ++j)
    if (result_.getSample()[j][variablesListWidget_->currentRow()] < quantile)
      cdf += p;

  probaSpinBox_->setValue(cdf);
}


void DataAnalysisWindow::scatterPlotsTabWidgetIndexChanged()
{
  showHideGraphConfigurationWidget(tabWidget_->currentIndex());
}


void DataAnalysisWindow::showHideGraphConfigurationWidget(int indexTab)
{
  switch (indexTab)
  {
    // if a plotWidget is visible
    case 1: // PDF-CDF graph
    case 2: // box plot
      emit stateChanged(variablesListWidget_->currentRow());
      variablesGroupBox_->show();
      break;
    case 3: // scatter plots
      if (scatterPlotsTabWidget_->currentIndex() == 0) // scatter plots
      {
        if (scatterPlotsConfigurationWidget_)
          if (!scatterPlotsConfigurationWidget_->isVisible())
            emit graphWindowActivated(scatterPlotsConfigurationWidget_);
      }
      else if (scatterPlotsTabWidget_->currentIndex() == 1) // plot matrix X-X
      {
        if (plotMatrix_X_X_ConfigurationWidget_)
          if (!plotMatrix_X_X_ConfigurationWidget_->isVisible())
            emit graphWindowActivated(plotMatrix_X_X_ConfigurationWidget_);
      }
      else if (scatterPlotsTabWidget_->currentIndex() == 2) // plot matrix Y-X
      {
        if (plotMatrixConfigurationWidget_)
          if (!plotMatrixConfigurationWidget_->isVisible())
            emit graphWindowActivated(plotMatrixConfigurationWidget_);
      }
      variablesGroupBox_->hide();
      break;
    // if no plotWidget is visible. Summary; Table; Parameters
    default:
    {
      emit graphWindowDeactivated();
      variablesGroupBox_->show();
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