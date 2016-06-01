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
#include "otgui/SampleTableModel.hxx"
#include "otgui/MonteCarloAnalysis.hxx"
#include "otgui/QtTools.hxx"
#include "otgui/CustomStandardItemModel.hxx"
#include "otgui/ResizableTableViewWithoutScrollBar.hxx"

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
  , probaSpinBox_(0)
  , quantileSpinBox_(0)
{
  buildInterface();
  connect(this, SIGNAL(windowStateChanged(Qt::WindowStates, Qt::WindowStates)), this, SLOT(showHideGraphConfigurationWidget(Qt::WindowStates, Qt::WindowStates)));
}


void MonteCarloResultWindow::buildInterface()
{
  // outputs
  OutputCollection outputs = physicalModel_.getOutputs();
  QStringList outputNames;
  for (int i=0; i<outputs.getSize(); ++i)
    outputNames << QString::fromUtf8(outputs[i].getName().c_str());

  // tabWidget
  tabWidget_ = new QTabWidget;

  // first tab: Table --------------------------------
  QWidget * tab = new QWidget;
  QVBoxLayout * tabLayout = new QVBoxLayout(tab);

  NumericalSample sample = result_.getInputSample();
  sample.stack(result_.getOutputSample());
  DataTableView * tabResultView = new DataTableView;
  SampleTableModel * tabResultModel = new SampleTableModel(sample);
  tabResultView->setModel(tabResultModel);
  tabLayout->addWidget(tabResultView);

  tabWidget_->addTab(tab, tr("Result table"));

  const bool resultsSampleIsValid = tabResultModel->sampleIsValid();

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
    connect(outputsComboBoxFirstTab_, SIGNAL(currentIndexChanged(int)), this, SLOT(updateSpinBoxes(int)));
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
    vbox->addWidget(getMinMaxTableWidget());

    // moments estimation
    vbox->addWidget(getMomentsEstimatesTableWidget());

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

    updateSpinBoxes();
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
    tab = getPDF_CDFWidget(outputs);
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
    tab = getBoxPlotWidget(outputs);
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
    tab = getScatterPlotsWidget(outputs);
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


QWidget* MonteCarloResultWindow::getMinMaxTableWidget()
{
  QStringList stochInputNames;
  for (int i=0; i<physicalModel_.getStochasticInputNames().getSize(); ++i)
    stochInputNames << QString::fromUtf8(physicalModel_.getStochasticInputNames()[i].c_str());
  int nbInputs = stochInputNames.size();
  int nbOutputs = result_.getOutputSample().getDimension();

  QGroupBox * minMaxGroupBox = new QGroupBox(tr("Minimum and Maximum"));
  QVBoxLayout * minMaxGroupBoxLayout = new QVBoxLayout(minMaxGroupBox);
  QStackedLayout * minMaxGroupBoxStackedLayout = new QStackedLayout;

  for (int indexOutput=0; indexOutput<nbOutputs; ++indexOutput)
  {
    ResizableTableViewWithoutScrollBar * minMaxTableView = new ResizableTableViewWithoutScrollBar;
    minMaxTableView->verticalHeader()->hide();
    CustomStandardItemModel * minMaxTable = new CustomStandardItemModel(nbInputs+1, 4);
    minMaxTableView->setModel(minMaxTable);

    // horizontal header
    minMaxTable->setHorizontalHeaderLabels(QStringList() << tr("") << tr("Variable") << tr("Minimum") << tr("Maximum"));

    // vertical header
    minMaxTable->setNotEditableHeaderItem(0, 0, tr("Output"));

    QString rowTitle = tr("Inputs at\nextremum");
    if (nbInputs == 1)
      rowTitle = tr("Input at\nextremum");
    minMaxTable->setNotEditableHeaderItem(1, 0, rowTitle);
    minMaxTableView->setSpan(1, 0, nbInputs, 1);

    // inputs names
    for (int i=0; i<nbInputs; ++i)
      minMaxTable->setNotEditableItem(i+1, 1, stochInputNames[i]);

    // output name
    minMaxTable->setNotEditableItem(0, 1, outputsComboBoxFirstTab_->currentText());
    // min
    minMaxTable->setNotEditableItem(0, 2, result_.getOutputSample().getMin()[indexOutput]);
    // max
    minMaxTable->setNotEditableItem(0, 3, result_.getOutputSample().getMax()[indexOutput]);

    // Xmin/XMax
    if (result_.getListXMin()[indexOutput].getSize() > 1)
    {
      minMaxTable->setHeaderData(2, Qt::Horizontal, QIcon(":/images/task-attention.png"), Qt::DecorationRole);
      minMaxTable->setHeaderData(2, Qt::Horizontal, tr("Information: The output is minimum at another point."), Qt::ToolTipRole);
    }
    if (result_.getListXMax()[indexOutput].getSize() > 1)
    {
      minMaxTable->setHeaderData(3, Qt::Horizontal, QIcon(":/images/task-attention.png"), Qt::DecorationRole);
      minMaxTable->setHeaderData(3, Qt::Horizontal, tr("Information: The output is maximum at another point."), Qt::ToolTipRole);
    }
    for (UnsignedInteger i=0; i<result_.getInputSample().getDimension(); ++i)
    {
      // XMin
      minMaxTable->setNotEditableItem(i+1, 2, result_.getListXMin()[indexOutput][0][i]);
      // XMax
      minMaxTable->setNotEditableItem(i+1, 3, result_.getListXMax()[indexOutput][0][i]);
    }

    // resize table
    minMaxTableView->resizeToContents();

    minMaxGroupBoxStackedLayout->addWidget(minMaxTableView);
  }
  minMaxGroupBoxLayout->addLayout(minMaxGroupBoxStackedLayout);
  connect(outputsComboBoxFirstTab_, SIGNAL(currentIndexChanged(int)), minMaxGroupBoxStackedLayout, SLOT(setCurrentIndex(int)));

  return minMaxGroupBox;
}


QWidget* MonteCarloResultWindow::getMomentsEstimatesTableWidget()
{
  int nbOutputs = result_.getOutputSample().getDimension();

  QGroupBox * momentsGroupBox = new QGroupBox(tr("Moments estimate"));
  QVBoxLayout * momentsGroupBoxLayout = new QVBoxLayout(momentsGroupBox);
  QStackedLayout * momentsGroupBoxStackedLayout = new QStackedLayout;

  int nbColumns = 2;
  if (isConfidenceIntervalRequired_)
    nbColumns = 4;

  for (int indexOutput=0; indexOutput<nbOutputs; ++indexOutput)
  {
    ResizableTableViewWithoutScrollBar * momentsEstimationsTableView = new ResizableTableViewWithoutScrollBar;
    momentsEstimationsTableView->horizontalHeader()->hide();
    momentsEstimationsTableView->verticalHeader()->hide();
    CustomStandardItemModel * momentsEstimationsTable = new CustomStandardItemModel(8, nbColumns);
    momentsEstimationsTableView->setModel(momentsEstimationsTable);

    // vertical header
    momentsEstimationsTable->setNotEditableHeaderItem(0, 0, tr("Estimate"));
    momentsEstimationsTableView->setSpan(0, 0, 2, 1);
    momentsEstimationsTable->setNotEditableHeaderItem(2, 0, tr("Mean"));
    momentsEstimationsTable->setNotEditableHeaderItem(3, 0, tr("Standard deviation"));
    momentsEstimationsTable->setNotEditableHeaderItem(4, 0, tr("Skewness"));
    momentsEstimationsTable->setNotEditableHeaderItem(5, 0, tr("Kurtosis"));
    momentsEstimationsTable->setNotEditableHeaderItem(6, 0, tr("First quartile"));
    momentsEstimationsTable->setNotEditableHeaderItem(7, 0, tr("Third quartile"));

    // horizontal header
    momentsEstimationsTable->setNotEditableHeaderItem(0, 1, tr("Value"));
    momentsEstimationsTableView->setSpan(0, 1, 2, 1);
    if (isConfidenceIntervalRequired_)
    {
      momentsEstimationsTable->setNotEditableHeaderItem(1, 2, tr("Lower bound"));
      momentsEstimationsTable->setNotEditableHeaderItem(1, 3, tr("Upper bound"));
    }
    // Mean
    momentsEstimationsTable->setNotEditableItem(2, 1, result_.getMean()[indexOutput]);

    if (isConfidenceIntervalRequired_)
    {
      const double meanCILowerBound = result_.getMeanConfidenceInterval(levelConfidenceInterval_).getLowerBound()[indexOutput];
      momentsEstimationsTable->setNotEditableItem(2, 2, meanCILowerBound);
      const double meanCIUpperBound = result_.getMeanConfidenceInterval(levelConfidenceInterval_).getUpperBound()[indexOutput];
      momentsEstimationsTable->setNotEditableItem(2, 3, meanCIUpperBound);
    }
    // Standard Deviation
    momentsEstimationsTable->setNotEditableItem(3, 1, result_.getStandardDeviation()[indexOutput]);

    if (isConfidenceIntervalRequired_)
    {
      const double stdCILowerBound = result_.getStdConfidenceInterval(levelConfidenceInterval_).getLowerBound()[indexOutput];
      momentsEstimationsTable->setNotEditableItem(3, 2, stdCILowerBound);
      const double stdCIUpperBound = result_.getStdConfidenceInterval(levelConfidenceInterval_).getUpperBound()[indexOutput];
      momentsEstimationsTable->setNotEditableItem(3, 3, stdCIUpperBound);
    }
    // Skewness
    momentsEstimationsTable->setNotEditableItem(4, 1, result_.getSkewness()[indexOutput]);
    // Kurtosis
    momentsEstimationsTable->setNotEditableItem(5, 1, result_.getKurtosis()[indexOutput]);
    // First quartile
    momentsEstimationsTable->setNotEditableItem(6, 1, result_.getFirstQuartile()[indexOutput]);
    // Third quartile
    momentsEstimationsTable->setNotEditableItem(7, 1, result_.getThirdQuartile()[indexOutput]);

    // resize table
    int titleWidth = 0;
    if (isConfidenceIntervalRequired_)
    {
      momentsEstimationsTable->setNotEditableHeaderItem(0, 2, tr("Confidence interval at ") + QString::number(levelConfidenceInterval_*100) + "%");
      momentsEstimationsTableView->resizeColumnsToContents();
      titleWidth = momentsEstimationsTableView->horizontalHeader()->sectionSize(2);

      // first: clear item at (0,2) because the text is to wide:
      // resizeColumnsToContents takes into account the text of item at (0,2)
      // to resize the column 2, even if there is a setSpan(0, 2, 1, 2)
      momentsEstimationsTable->setItem(0, 2, new QStandardItem);
    }

    momentsEstimationsTableView->resizeToContents();

    if (isConfidenceIntervalRequired_)
    {
      momentsEstimationsTable->setNotEditableHeaderItem(0, 2, tr("Confidence interval at ") + QString::number(levelConfidenceInterval_*100) + "%");
      momentsEstimationsTableView->setSpan(0, 2, 1, 2);
      const int subTitlesWidth = momentsEstimationsTableView->horizontalHeader()->sectionSize(2) + momentsEstimationsTableView->horizontalHeader()->sectionSize(3);
      const int widthCorrection = titleWidth - subTitlesWidth;
      if (widthCorrection > 0)
      {
        // correct the table width
        momentsEstimationsTableView->horizontalHeader()->resizeSection(3, momentsEstimationsTableView->horizontalHeader()->sectionSize(3) + widthCorrection);
        momentsEstimationsTableView->setMinimumWidth(momentsEstimationsTableView->minimumWidth() + widthCorrection);
      }
    }
    momentsGroupBoxStackedLayout->addWidget(momentsEstimationsTableView);
  }
  momentsGroupBoxLayout->addLayout(momentsGroupBoxStackedLayout);
  connect(outputsComboBoxFirstTab_, SIGNAL(currentIndexChanged(int)), momentsGroupBoxStackedLayout, SLOT(setCurrentIndex(int))); 

  return momentsGroupBox;
}


QWidget* MonteCarloResultWindow::getPDF_CDFWidget(const OutputCollection & outputs)
{
  QStringList outputNames;
  for (int i=0; i<outputs.getSize(); ++i)
    outputNames << QString::fromUtf8(outputs[i].getName().c_str());

  QWidget * tab = new QWidget;
  QStackedLayout * plotLayout = new QStackedLayout(tab);

  QVector<PlotWidget*> listPlotWidgets;

  for (int i=0; i<outputs.getSize(); ++i)
  {
    PlotWidget * plot = new PlotWidget;

    // PDF
    plot->plotHistogram(result_.getOutputSample().getMarginal(i));
    plot->plotPDFCurve(result_.getFittedDistribution()[i]);
    plot->setTitle(tr("PDF: ") + QString::fromUtf8(outputs[i].getName().c_str()));
    if (outputs[i].getDescription().size())
      plot->setAxisTitle(QwtPlot::xBottom, QString::fromUtf8(outputs[i].getDescription().c_str()));
    else
      plot->setAxisTitle(QwtPlot::xBottom, QString::fromUtf8(outputs[i].getName().c_str()));

    plotLayout->addWidget(plot);
    listPlotWidgets.append(plot);

    // CDF
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

  return tab;
}


QWidget* MonteCarloResultWindow::getBoxPlotWidget(const OutputCollection & outputs)
{
  QStringList outputNames;
  for (int i=0; i<outputs.getSize(); ++i)
    outputNames << QString::fromUtf8(outputs[i].getName().c_str());

  QWidget * tab = new QWidget;
  QStackedLayout * boxPlotLayout = new QStackedLayout(tab);

  QVector<PlotWidget*> listBoxPlotWidgets;

  for (int i=0; i<outputs.getSize(); ++i)
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

  return tab;
}


QWidget* MonteCarloResultWindow::getScatterPlotsWidget(const OutputCollection & outputs)
{
  QStringList stochInputNames;
  for (int i=0; i<physicalModel_.getStochasticInputNames().getSize(); ++i)
    stochInputNames << QString::fromUtf8(physicalModel_.getStochasticInputNames()[i].c_str());
  QStringList outputNames;
  for (int i=0; i<outputs.getSize(); ++i)
    outputNames << QString::fromUtf8(outputs[i].getName().c_str());

  QWidget * tab = new QWidget;
  QStackedLayout * scatterPlotLayout = new QStackedLayout(tab);

  QVector<PlotWidget*> listScatterPlotWidgets;

  for (int j=0; j<stochInputNames.size(); ++j)
  {
    for (int i=0; i<outputs.getSize(); ++i)
    {
      PlotWidget * plot = new PlotWidget;
      plot->plotScatter(result_.getInputSample().getMarginal(j), result_.getOutputSample().getMarginal(i));
      plot->setTitle(tr("Scatter plot: ") + QString::fromUtf8(outputs[i].getName().c_str()) + tr(" vs ") + stochInputNames[j]);
      String inputDescription = physicalModel_.getInputByName(stochInputNames[j].toStdString()).getDescription();
      if (!inputDescription.empty())
        plot->setAxisTitle(QwtPlot::xBottom, QString::fromUtf8(inputDescription.c_str()));
      else
        plot->setAxisTitle(QwtPlot::xBottom, stochInputNames[j]);
      if (outputs[i].getDescription().size())
        plot->setAxisTitle(QwtPlot::yLeft, QString::fromUtf8(outputs[i].getDescription().c_str()));
      else
        plot->setAxisTitle(QwtPlot::yLeft, QString::fromUtf8(outputs[i].getName().c_str()));
      scatterPlotLayout->addWidget(plot);
      listScatterPlotWidgets.append(plot);
    }
    for (int i=0; i<stochInputNames.size(); ++i)
    {
      PlotWidget * plot = new PlotWidget;
      plot->plotScatter(result_.getInputSample().getMarginal(j), result_.getInputSample().getMarginal(i));
      plot->setTitle(tr("Scatter plot: ") + stochInputNames[i] + tr(" vs ") + stochInputNames[j]);
      String inputDescription = physicalModel_.getInputByName(stochInputNames[j].toStdString()).getDescription();
      if (!inputDescription.empty())
        plot->setAxisTitle(QwtPlot::xBottom, QString::fromUtf8(inputDescription.c_str()));
      else
        plot->setAxisTitle(QwtPlot::xBottom, stochInputNames[j]);
      inputDescription = physicalModel_.getInputByName(stochInputNames[i].toStdString()).getDescription();
      if (!inputDescription.empty())
        plot->setAxisTitle(QwtPlot::yLeft, QString::fromUtf8(inputDescription.c_str()));
      else
        plot->setAxisTitle(QwtPlot::yLeft, stochInputNames[i]);
      scatterPlotLayout->addWidget(plot);
      listScatterPlotWidgets.append(plot);
    }
  }
  for (int j=0; j<outputs.getSize(); ++j)
  {
    for (int i=0; i<outputs.getSize(); ++i)
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
    for (int i=0; i<stochInputNames.size(); ++i)
    {
      PlotWidget * plot = new PlotWidget;
      plot->plotScatter(result_.getOutputSample().getMarginal(j), result_.getInputSample().getMarginal(i));
      plot->setTitle(tr("Scatter plot: ") + stochInputNames[i] + tr(" vs ") + QString::fromUtf8(outputs[j].getName().c_str()));
      if (outputs[j].getDescription().size())
        plot->setAxisTitle(QwtPlot::xBottom, QString::fromUtf8(outputs[j].getDescription().c_str()));
      else
        plot->setAxisTitle(QwtPlot::xBottom, QString::fromUtf8(outputs[j].getName().c_str()));
      String inputDescription = physicalModel_.getInputByName(stochInputNames[i].toStdString()).getDescription();
      if (!inputDescription.empty())
        plot->setAxisTitle(QwtPlot::yLeft, QString::fromUtf8(inputDescription.c_str()));
      else
        plot->setAxisTitle(QwtPlot::yLeft, stochInputNames[i]);

      scatterPlotLayout->addWidget(plot);
      listScatterPlotWidgets.append(plot);
    }
  }

  scatterPlotsConfigurationWidget_ = new GraphConfigurationWidget(listScatterPlotWidgets, stochInputNames, outputNames, GraphConfigurationWidget::Scatter);
  connect(scatterPlotsConfigurationWidget_, SIGNAL(currentPlotChanged(int)), scatterPlotLayout, SLOT(setCurrentIndex(int)));

  return tab;
}


void MonteCarloResultWindow::updateSpinBoxes(int indexOutput)
{
  const double min = result_.getOutputSample().getMin()[indexOutput];
  const double max = result_.getOutputSample().getMax()[indexOutput];
  quantileSpinBox_->setMinimum(min);
  quantileSpinBox_->setMaximum(max);
  quantileSpinBox_->setSingleStep((max-min)/100);
  probaSpinBox_->setValue(0.5);
  probaValueChanged(0.5);
}


void MonteCarloResultWindow::probaValueChanged(double proba)
{
  SignalBlocker blocker(quantileSpinBox_);
  quantileSpinBox_->setValue(result_.getOutputSample().getMarginal(outputsComboBoxFirstTab_->currentIndex()).computeQuantile(proba)[0]);
}


void MonteCarloResultWindow::quantileValueChanged(double quantile)
{
  SignalBlocker blocker(probaSpinBox_);
  double cdf = 0.0;
  double p = 1.0 / double(result_.getOutputSample().getSize());

  for (UnsignedInteger j=0; j<result_.getOutputSample().getSize(); ++j)
    if (result_.getOutputSample()[j][outputsComboBoxFirstTab_->currentIndex()] < quantile)
      cdf += p;

  probaSpinBox_->setValue(cdf);
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
  else if (newState == 0 || newState == 1 || newState == 9)
    showHideGraphConfigurationWidget(-1);
}
}
