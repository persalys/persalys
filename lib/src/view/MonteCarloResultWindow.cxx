//                                               -*- C++ -*-
/**
 *  @brief QMdiSubWindow for the results of Monte Carlo
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
#include "otgui/MonteCarloResultWindow.hxx"
#include "otgui/DataTableView.hxx"
#include "otgui/DataTableModel.hxx"
#include "otgui/MonteCarloAnalysis.hxx"

#include <QVBoxLayout>
#include <QStackedLayout>
#include <QScrollArea>
#include <QHeaderView>
#include <QGroupBox>

#include <limits>

using namespace OT;

namespace OTGUI {

MonteCarloResultWindow::MonteCarloResultWindow(AnalysisItem * item)
  : OTguiSubWindow(item)
  , result_(dynamic_cast<MonteCarloAnalysis*>(&*item->getAnalysis().getImplementation())->getResult())
  , physicalModel_(item->getAnalysis().getPhysicalModel())
  , isConfidenceIntervalRequired_(dynamic_cast<MonteCarloAnalysis*>(&*item->getAnalysis().getImplementation())->isConfidenceIntervalRequired())
  , levelConfidenceInterval_(dynamic_cast<MonteCarloAnalysis*>(&*item->getAnalysis().getImplementation())->getLevelConfidenceInterval())
  , tabWidget_(0)
  , outputsComboBoxFirstTab_(0)
  , pdf_cdfPlotsConfigurationWidget_(0)
  , boxPlotsConfigurationWidget_(0)
  , scatterPlotsConfigurationWidget_(0)
  , plotMatrixConfigurationWidget_(0)
  , plotMatrix_X_X_ConfigurationWidget_(0)
  , minMaxTable_(0)
  , momentsEstimationsTable_(0)
  , probaSpinBox_(0)
  , quantileSpinBox_(0)
{
  buildInterface();
  connect(this, SIGNAL(windowStateChanged(Qt::WindowStates, Qt::WindowStates)), this, SLOT(showHideGraphConfigurationWidget(Qt::WindowStates, Qt::WindowStates)));
}


void MonteCarloResultWindow::buildInterface()
{
  // data
  int nbInputs = result_.getInputSample().getDimension();
  QStringList inputNames;
  for (int i=0; i<nbInputs; ++i)
    inputNames << QString::fromUtf8(physicalModel_.getStochasticInputNames()[i].c_str());

  int nbOutputs = result_.getOutputSample().getDimension();
  OutputCollection outputs = physicalModel_.getOutputs();
  QStringList outputNames;
  for (int i=0; i<nbOutputs; ++i)
    outputNames << QString::fromUtf8(result_.getOutputSample().getDescription()[i].c_str());

  // tabWidget
  tabWidget_ = new QTabWidget;

  // first tab: Table --------------------------------
  QWidget * tab = new QWidget;
  QVBoxLayout * tabLayout = new QVBoxLayout(tab);

  NumericalSample sample = result_.getInputSample();
  sample.stack(result_.getOutputSample());
  DataTableView * tabResultView = new DataTableView(sample);
  tabLayout->addWidget(tabResultView);

  tabWidget_->addTab(tab, tr("Result table"));

  const bool resultsSampleIsValid = dynamic_cast<DataTableModel*>(tabResultView->model())->sampleIsValid();

  // second tab: Summary -----------------------------

  tab = new QWidget;
  tabLayout = new QVBoxLayout(tab);

  // if the sample is valid:
  if (resultsSampleIsValid)
  {
    QScrollArea * scrollArea = new QScrollArea;
    scrollArea->setWidgetResizable(true);
    tabLayout->setSizeConstraint(QLayout::SetFixedSize);

    // -- output name --
    QHBoxLayout * headLayout = new QHBoxLayout;
    QLabel * outputName = new QLabel(tr("Output"));
    headLayout->addWidget(outputName);
    outputsComboBoxFirstTab_ = new QComboBox;
    outputsComboBoxFirstTab_->addItems(outputNames);
    connect(outputsComboBoxFirstTab_, SIGNAL(currentIndexChanged(int)), this, SLOT(outputFirstTabChanged(int)));
    headLayout->addWidget(outputsComboBoxFirstTab_);
    headLayout->addStretch();
    tabLayout->addLayout(headLayout);

    // -- results --
    QVBoxLayout * vbox = new QVBoxLayout;

    // number of simulations
    QLabel * nbSimuLabel = new QLabel(tr("Number of simulations: ") + QString::number(result_.getInputSample().getSize()) + "\n");
    nbSimuLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);
    vbox->addWidget(nbSimuLabel);

    // min/max table
    QGroupBox * minMaxGroupBox = new QGroupBox(tr("Minimum and Maximum"));
    QVBoxLayout * minMaxVbox = new QVBoxLayout(minMaxGroupBox);

    minMaxTable_ = new NotEditableTableWidget(nbInputs+1, 4);

    // horizontal header
    minMaxTable_->setHorizontalHeaderLabels(QStringList() << tr("") << tr("Variable") << tr("Minimum") << tr("Maximum"));

    // vertical header
    minMaxTable_->createHeaderItem(0, 0, tr("Output"));
    minMaxTable_->createHeaderItem(1, 0, tr("Inputs at\nextremum"));
    minMaxTable_->setSpan(1, 0, nbInputs, 1);

    // inputs names
    for (int i=0; i<nbInputs; ++i)
      minMaxTable_->createItem(i+1, 1, inputNames[i]);

    minMaxVbox->addWidget(minMaxTable_);
    vbox->addWidget(minMaxGroupBox);

    // moments estimation
    QGroupBox * momentsGroupBox = new QGroupBox(tr("Moments estimate"));
    QVBoxLayout * momentsVbox = new QVBoxLayout(momentsGroupBox);

    momentsEstimationsTable_ = new NotEditableTableWidget(8, 4);
    momentsEstimationsTable_->horizontalHeader()->hide();

    // vertical header
    momentsEstimationsTable_->createHeaderItem(0, 0, tr("Estimate"));
    momentsEstimationsTable_->setSpan(0, 0, 2, 1);
    momentsEstimationsTable_->createHeaderItem(2, 0, tr("Mean"));
    momentsEstimationsTable_->createHeaderItem(3, 0, tr("Standard deviation"));
    momentsEstimationsTable_->createHeaderItem(4, 0, tr("Skewness"));
    momentsEstimationsTable_->createHeaderItem(5, 0, tr("Kurtosis"));
    momentsEstimationsTable_->createHeaderItem(6, 0, tr("First quartile"));
    momentsEstimationsTable_->createHeaderItem(7, 0, tr("Third quartile"));

    // horizontal header
    momentsEstimationsTable_->createHeaderItem(0, 1, tr("Value"));
    momentsEstimationsTable_->setSpan(0, 1, 2, 1);
    if (isConfidenceIntervalRequired_)
    {
      momentsEstimationsTable_->createHeaderItem(1, 2, tr("Lower bound"));
      momentsEstimationsTable_->createHeaderItem(1, 3, tr("Upper bound"));
    }

    momentsVbox->addWidget(momentsEstimationsTable_);
    vbox->addWidget(momentsGroupBox);

    // quantiles
    QHBoxLayout * quantLayout = new QHBoxLayout;

    // Probability
    QLabel * label = new QLabel(tr("Probability"));
    label->setStyleSheet("font: bold;");
    quantLayout->addWidget(label);
    probaSpinBox_ = new QDoubleSpinBox;
    label->setBuddy(probaSpinBox_);
    probaSpinBox_->setMinimum(0.0);
    probaSpinBox_->setMaximum(1.0);
    probaSpinBox_->setSingleStep(0.01);
    quantLayout->addWidget(probaSpinBox_);
    // Quantile
    label = new QLabel(tr("Quantile"));
    label->setStyleSheet("font: bold;");
    quantLayout->addWidget(label);
    quantileSpinBox_ = new QDoubleSpinBox;
    label->setBuddy(quantileSpinBox_);
    quantileSpinBox_->setDecimals(8);
    quantLayout->addWidget(quantileSpinBox_);

    connect(probaSpinBox_, SIGNAL(valueChanged(double)), this, SLOT(probaValueChanged(double)));
    connect(quantileSpinBox_, SIGNAL(valueChanged(double)), this, SLOT(quantileValueChanged(double)));

    quantLayout->addStretch();
    vbox->addLayout(quantLayout);

    vbox->addStretch();
    tabLayout->addLayout(vbox);

    updateResultWidgets();
    tab->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Preferred);
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
  tab = new QWidget;

  // if the sample is valid:
  if (resultsSampleIsValid)
  {
    QStackedLayout * plotLayout = new QStackedLayout(tab);

    QVector<PlotWidget*> listPlotWidgets;

    for (int i=0; i<nbOutputs; ++i)
    {
      PlotWidget * plot = new PlotWidget;
      plot->plotHistogram(result_.getOutputSample().getMarginal(i));
      plot->plotPDFCurve(result_.getFittedDistribution()[i]);
      plot->setTitle(tr("PDF: ") + QString::fromUtf8(outputs[i].getName().c_str()));
      if (outputs[i].getDescription().size())
        plot->setAxisTitle(QwtPlot::xBottom, QString::fromUtf8(outputs[i].getDescription().c_str()));
      else
        plot->setAxisTitle(QwtPlot::xBottom, QString::fromUtf8(outputs[i].getName().c_str()));
      plotLayout->addWidget(plot);
      listPlotWidgets.append(plot);

      plot = new PlotWidget;
      plot->plotHistogram(result_.getOutputSample().getMarginal(i), 1);
      plot->plotCDFCurve(result_.getFittedDistribution()[i]);
      plot->setTitle(tr("CDF: ") + QString::fromUtf8(outputs[i].getName().c_str()));
      if (outputs[i].getDescription().size())
        plot->setAxisTitle(QwtPlot::xBottom, QString::fromUtf8(outputs[i].getDescription().c_str()));
      else
        plot->setAxisTitle(QwtPlot::xBottom, QString::fromUtf8(outputs[i].getName().c_str()));
      plotLayout->addWidget(plot);
      listPlotWidgets.append(plot);
    }

    pdf_cdfPlotsConfigurationWidget_ = new GraphConfigurationWidget(listPlotWidgets, QStringList(), outputNames, GraphConfigurationWidget::PDFResult);
    connect(pdf_cdfPlotsConfigurationWidget_, SIGNAL(currentPlotChanged(int)), plotLayout, SLOT(setCurrentIndex(int)));
  }
  // if the results sample contains NAN
  else
  {
    tabLayout = new QVBoxLayout(tab);
    QLabel * summaryErrorMessage = new QLabel(tr("Computation failed. Some results are not available."));
    summaryErrorMessage->setWordWrap(true);
    tabLayout->addWidget(summaryErrorMessage);
    tabLayout->addStretch();
  }
  tabWidget_->addTab(tab, tr("PDF/CDF"));

  // fourth tab: box plots ---------------------------
  tab = new QWidget;

  // if the sample is valid:
  if (resultsSampleIsValid)
  {
    QStackedLayout * boxPlotLayout = new QStackedLayout(tab);

    QVector<PlotWidget*> listBoxPlotWidgets;

    for (int i=0; i<nbOutputs; ++i)
    {
      PlotWidget * plot = new PlotWidget;
      double median = result_.getMedian()[i];
      double Q1 = result_.getFirstQuartile()[i];
      double Q3 = result_.getThirdQuartile()[i];

      plot->plotBoxPlot(median, Q1, Q3, Q1 - 1.5*(Q3-Q1), Q3 + 1.5*(Q3-Q1), result_.getOutliers()[i]);
      plot->setTitle(tr("Box plot: ") + QString::fromUtf8(outputs[i].getName().c_str()));
      if (outputs[i].getDescription().size())
        plot->setAxisTitle(QwtPlot::yLeft, QString::fromUtf8(outputs[i].getDescription().c_str()));
      else
        plot->setAxisTitle(QwtPlot::yLeft, QString::fromUtf8(outputs[i].getName().c_str()));
      boxPlotLayout->addWidget(plot);
      listBoxPlotWidgets.append(plot);
    }

    boxPlotsConfigurationWidget_ = new GraphConfigurationWidget(listBoxPlotWidgets, QStringList(), outputNames, GraphConfigurationWidget::BoxPlot);
    connect(boxPlotsConfigurationWidget_, SIGNAL(currentPlotChanged(int)), boxPlotLayout, SLOT(setCurrentIndex(int)));
  }
  // if the results sample contains NAN
  else
  {
    tabLayout = new QVBoxLayout(tab);
    QLabel * summaryErrorMessage = new QLabel(tr("Computation failed. Some results are not available."));
    summaryErrorMessage->setWordWrap(true);
    tabLayout->addWidget(summaryErrorMessage);
    tabLayout->addStretch();
  }
  tabWidget_->addTab(tab, tr("Box plots"));

  // fifth tab: scatter plots ------------------------
  tab = new QWidget;

  // if the sample is valid:
  if (resultsSampleIsValid)
  {
    QStackedLayout * scatterPlotLayout = new QStackedLayout(tab);

    QVector<PlotWidget*> listScatterPlotWidgets;

    for (int j=0; j<nbInputs; ++j)
    {
      for (int i=0; i<nbOutputs; ++i)
      {
        PlotWidget * plot = new PlotWidget;
        plot->plotScatter(result_.getInputSample().getMarginal(j), result_.getOutputSample().getMarginal(i));
        plot->setTitle(tr("Scatter plot: ") + QString::fromUtf8(outputs[i].getName().c_str()) + tr(" vs ") + inputNames[j]);
        String inputDescription = physicalModel_.getInputByName(inputNames[j].toStdString()).getDescription();
        if (!inputDescription.empty())
          plot->setAxisTitle(QwtPlot::xBottom, QString::fromUtf8(inputDescription.c_str()));
        else
          plot->setAxisTitle(QwtPlot::xBottom, inputNames[j]);
        if (outputs[i].getDescription().size())
          plot->setAxisTitle(QwtPlot::yLeft, QString::fromUtf8(outputs[i].getDescription().c_str()));
        else
          plot->setAxisTitle(QwtPlot::yLeft, QString::fromUtf8(outputs[i].getName().c_str()));
        scatterPlotLayout->addWidget(plot);
        listScatterPlotWidgets.append(plot);
      }
      for (int i=0; i<nbInputs; ++i)
      {
        PlotWidget * plot = new PlotWidget;
        plot->plotScatter(result_.getInputSample().getMarginal(j), result_.getInputSample().getMarginal(i));
        plot->setTitle(tr("Scatter plot: ") + inputNames[i] + tr(" vs ") + inputNames[j]);
        String inputDescription = physicalModel_.getInputByName(inputNames[j].toStdString()).getDescription();
        if (!inputDescription.empty())
          plot->setAxisTitle(QwtPlot::xBottom, QString::fromUtf8(inputDescription.c_str()));
        else
          plot->setAxisTitle(QwtPlot::xBottom, inputNames[j]);
        inputDescription = physicalModel_.getInputByName(inputNames[i].toStdString()).getDescription();
        if (!inputDescription.empty())
          plot->setAxisTitle(QwtPlot::yLeft, QString::fromUtf8(inputDescription.c_str()));
        else
          plot->setAxisTitle(QwtPlot::yLeft, inputNames[i]);
        scatterPlotLayout->addWidget(plot);
        listScatterPlotWidgets.append(plot);
      }
    }
    for (int j=0; j<nbOutputs; ++j)
    {
      for (int i=0; i<nbOutputs; ++i)
      {
        PlotWidget * plot = new PlotWidget;
        plot->plotScatter(result_.getOutputSample().getMarginal(j), result_.getOutputSample().getMarginal(i));
        plot->setTitle(tr("Scatter plot: ") + QString::fromUtf8(outputs[i].getName().c_str()) + tr(" vs ") + QString::fromUtf8(outputs[j].getName().c_str()));
        if (outputs[j].getDescription().size())
          plot->setAxisTitle(QwtPlot::xBottom, QString::fromUtf8(outputs[j].getDescription().c_str()));
        else
          plot->setAxisTitle(QwtPlot::xBottom, QString::fromUtf8(outputs[j].getName().c_str()));
        if (outputs[i].getDescription().size())
          plot->setAxisTitle(QwtPlot::yLeft, QString::fromUtf8(outputs[i].getDescription().c_str()));
        else
          plot->setAxisTitle(QwtPlot::yLeft, QString::fromUtf8(outputs[i].getName().c_str()));
        scatterPlotLayout->addWidget(plot);
        listScatterPlotWidgets.append(plot);
      }
      for (int i=0; i<nbInputs; ++i)
      {
        PlotWidget * plot = new PlotWidget;
        plot->plotScatter(result_.getOutputSample().getMarginal(j), result_.getInputSample().getMarginal(i));
        plot->setTitle(tr("Scatter plot: ") + inputNames[i] + tr(" vs ") + QString::fromUtf8(outputs[j].getName().c_str()));
        if (outputs[j].getDescription().size())
          plot->setAxisTitle(QwtPlot::xBottom, QString::fromUtf8(outputs[j].getDescription().c_str()));
        else
          plot->setAxisTitle(QwtPlot::xBottom, QString::fromUtf8(outputs[j].getName().c_str()));
        String inputDescription = physicalModel_.getInputByName(inputNames[i].toStdString()).getDescription();
        if (!inputDescription.empty())
          plot->setAxisTitle(QwtPlot::yLeft, QString::fromUtf8(inputDescription.c_str()));
        else
          plot->setAxisTitle(QwtPlot::yLeft, inputNames[i]);

        scatterPlotLayout->addWidget(plot);
        listScatterPlotWidgets.append(plot);
      }
    }

    scatterPlotsConfigurationWidget_ = new GraphConfigurationWidget(listScatterPlotWidgets, inputNames, outputNames, GraphConfigurationWidget::Scatter);
    connect(scatterPlotsConfigurationWidget_, SIGNAL(currentPlotChanged(int)), scatterPlotLayout, SLOT(setCurrentIndex(int)));
  }
  // if the results sample contains NAN
  else
  {
    tabLayout = new QVBoxLayout(tab);
    QLabel * summaryErrorMessage = new QLabel(tr("Computation failed. Some results are not available."));
    summaryErrorMessage->setWordWrap(true);
    tabLayout->addWidget(summaryErrorMessage);
    tabLayout->addStretch();
  }
  tabWidget_->addTab(tab, tr("Scatter plots"));

  // sixth tab: plot matrix Y-X ----------------------

  // if the sample is valid:
  if (resultsSampleIsValid)
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

  // seventh tab: plot matrix X-X ----------------------
  tab = new PlotMatrixWidget(result_.getInputSample(), result_.getInputSample());
  plotMatrix_X_X_ConfigurationWidget_ = new PlotMatrixConfigurationWidget(dynamic_cast<PlotMatrixWidget*>(tab));

  tabWidget_->addTab(tab, tr("Plot matrix X-X"));

  //
  connect(tabWidget_, SIGNAL(currentChanged(int)), this, SLOT(showHideGraphConfigurationWidget(int)));
  setWidget(tabWidget_);
}


void MonteCarloResultWindow::updateResultWidgets(int indexOutput)
{
  // -- minMaxTable_ --

  // output name
  minMaxTable_->createItem(0, 1, outputsComboBoxFirstTab_->currentText());
  // min
  const double min = result_.getOutputSample().getMin()[indexOutput];
  minMaxTable_->createItem(0, 2, min);
  // max
  const double max = result_.getOutputSample().getMax()[indexOutput];
  minMaxTable_->createItem(0, 3, max);

  // Xmin/XMax
  if (result_.getListXMin()[indexOutput].getSize() > 1)
  {
    minMaxTable_->horizontalHeaderItem(2)->setIcon(QIcon(":/images/task-attention.png"));
    minMaxTable_->horizontalHeaderItem(2)->setToolTip(tr("Information: The output is minimum at another point."));
  }
  if (result_.getListXMax()[indexOutput].getSize() > 1)
  {
    minMaxTable_->horizontalHeaderItem(3)->setIcon(QIcon(":/images/task-attention.png"));
    minMaxTable_->horizontalHeaderItem(3)->setToolTip(tr("Information: The output is maximum at another point."));
  }
  for (UnsignedInteger i=0; i<result_.getInputSample().getDimension(); ++i)
  {
    // XMin
    minMaxTable_->createItem(i+1, 2, result_.getListXMin()[indexOutput][0][i]);
    // XMax
    minMaxTable_->createItem(i+1, 3, result_.getListXMax()[indexOutput][0][i]);
  }

  // resize table
  minMaxTable_->resizeToContents();

  // -- momentsEstimationsTable_ --

  // Mean
  momentsEstimationsTable_->createItem(2, 1, result_.getMean()[indexOutput]);

  if (isConfidenceIntervalRequired_)
  {
    double meanCILowerBound = result_.getMeanConfidenceInterval(levelConfidenceInterval_).getLowerBound()[indexOutput];
    momentsEstimationsTable_->createItem(2, 2, meanCILowerBound);
    double meanCIUpperBound = result_.getMeanConfidenceInterval(levelConfidenceInterval_).getUpperBound()[indexOutput];
    momentsEstimationsTable_->createItem(2, 3, meanCIUpperBound);
  }
  // Standard Deviation
  momentsEstimationsTable_->createItem(3, 1, result_.getStandardDeviation()[indexOutput]);

  if (isConfidenceIntervalRequired_)
  {
    double stdCILowerBound = result_.getStdConfidenceInterval(levelConfidenceInterval_).getLowerBound()[indexOutput];
    momentsEstimationsTable_->createItem(3, 2, stdCILowerBound);
    double stdCIUpperBound = result_.getStdConfidenceInterval(levelConfidenceInterval_).getUpperBound()[indexOutput];
    momentsEstimationsTable_->createItem(3, 3, stdCIUpperBound);
  }
  // Skewness
  momentsEstimationsTable_->createItem(4, 1, result_.getSkewness()[indexOutput]);
  // Kurtosis
  momentsEstimationsTable_->createItem(5, 1, result_.getKurtosis()[indexOutput]);
  // First quartile
  momentsEstimationsTable_->createItem(6, 1, result_.getFirstQuartile()[indexOutput]);
  // Third quartile
  momentsEstimationsTable_->createItem(7, 1, result_.getThirdQuartile()[indexOutput]);

  // resize table

  // first: clear item at (0,2) because the text is to wide:
  // resizeColumnsToContents takes into account the text of item at (0,2)
  // to resize the column 2, even if there is a setSpan(0, 2, 1, 2)
  momentsEstimationsTable_->setItem(0, 2, new QTableWidgetItem);

  momentsEstimationsTable_->resizeToContents();

  if (isConfidenceIntervalRequired_)
  {
    momentsEstimationsTable_->createHeaderItem(0, 2, tr("Confidence interval at ") + QString::number(levelConfidenceInterval_*100) + "%");
    momentsEstimationsTable_->setSpan(0, 2, 1, 2);
  }

  // -- quantiles --
  quantileSpinBox_->setMinimum(min);
  quantileSpinBox_->setMaximum(max);
  quantileSpinBox_->setSingleStep((max-min)/100);
  probaSpinBox_->setValue(0.5);
  probaValueChanged(0.5);
}


void MonteCarloResultWindow::probaValueChanged(double proba)
{
  quantileSpinBox_->blockSignals(true);
  quantileSpinBox_->setValue(result_.getOutputSample().getMarginal(outputsComboBoxFirstTab_->currentIndex()).computeQuantile(proba)[0]);
  quantileSpinBox_->blockSignals(false);
}


void MonteCarloResultWindow::quantileValueChanged(double quantile)
{
  probaSpinBox_->blockSignals(true);
  double cdf = 0.0;
  double p = 1.0 / double(result_.getOutputSample().getSize());

  for (UnsignedInteger j=0; j<result_.getOutputSample().getSize(); ++j)
    if (result_.getOutputSample()[j][outputsComboBoxFirstTab_->currentIndex()] < quantile)
      cdf += p;

  probaSpinBox_->setValue(cdf);
  probaSpinBox_->blockSignals(false);
}


void MonteCarloResultWindow::outputFirstTabChanged(int indexOutput)
{
  updateResultWidgets(indexOutput);
  probaValueChanged(0.5);
}


void MonteCarloResultWindow::showHideGraphConfigurationWidget(int indexTab)
{
  emit graphWindowDeactivated(pdf_cdfPlotsConfigurationWidget_);
  emit graphWindowDeactivated(boxPlotsConfigurationWidget_);
  emit graphWindowDeactivated(scatterPlotsConfigurationWidget_);
  emit graphWindowDeactivated(plotMatrixConfigurationWidget_);
  emit graphWindowDeactivated(plotMatrix_X_X_ConfigurationWidget_);

  switch (indexTab)
  {
    // if a plotWidget is visible
    case 2:
      if (pdf_cdfPlotsConfigurationWidget_)
        emit graphWindowActivated(pdf_cdfPlotsConfigurationWidget_);
      break;
    case 3:
      if (boxPlotsConfigurationWidget_)
        emit graphWindowActivated(boxPlotsConfigurationWidget_);
      break;
    case 4:
      if (scatterPlotsConfigurationWidget_)
        emit graphWindowActivated(scatterPlotsConfigurationWidget_);
      break;
    case 5:
      if (plotMatrixConfigurationWidget_)
        emit graphWindowActivated(plotMatrixConfigurationWidget_);
      break;
    case 6:
      if (plotMatrix_X_X_ConfigurationWidget_)
        emit graphWindowActivated(plotMatrix_X_X_ConfigurationWidget_);
      break;
    // if no plotWidget is visible
    default:
    {
      break;
    }
  }
}


void MonteCarloResultWindow::showHideGraphConfigurationWidget(Qt::WindowStates oldState, Qt::WindowStates newState)
{
  if (newState == 4 || newState == 8 || newState == 10)
    showHideGraphConfigurationWidget(tabWidget_->currentIndex());
  else if (newState == 0 || newState == 1 || newState == 2 || newState == 9)
    showHideGraphConfigurationWidget(-1);
}
}
