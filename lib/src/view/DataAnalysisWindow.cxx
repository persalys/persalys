//                                               -*- C++ -*-
/**
 *  @brief QMdiSubWindow for the analysis of data
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
#include "otgui/DataAnalysisWindow.hxx"

#include "otgui/QtTools.hxx"
#include "otgui/DesignOfExperimentWindow.hxx"
#include "otgui/MinMaxTableGroupBox.hxx"
#include "otgui/MomentsEstimatesTableGroupBox.hxx"
#include "otgui/ParametersTableView.hxx"
#include "otgui/ExportableTableView.hxx"
#include "otgui/SampleTableModel.hxx"
#include "otgui/WidgetBoundToDockWidget.hxx"
#include "otgui/GraphConfigurationWidget.hxx"
#include "otgui/PlotMatrixConfigurationWidget.hxx"

#ifdef OTGUI_HAVE_PARAVIEW
#include "otgui/PVServerManagerInterface.hxx"
#include "otgui/PVServerManagerSingleton.hxx"
#include "otgui/PVParCooViewWidget.hxx"
#include "otgui/PVMatrixPlotViewWidget.hxx"
#include "otgui/PVPlotSettingWidget.hxx"
#include "otgui/PVXYChartViewWidget.hxx"
#include "otgui/PVXYChartSettingWidget.hxx"

#include <pqLinksModel.h>
#include <pqApplicationCore.h>
#endif

#include <QVBoxLayout>
#include <QScrollArea>
#include <QHeaderView>
#include <QSplitter>
#include <QSortFilterProxyModel>
#include <QPushButton>

using namespace OT;

namespace OTGUI
{

DataAnalysisWindow::DataAnalysisWindow(Item * item, QWidget * parent)
  : ResultWindow(item, parent)
  , designOfExperiment_()
  , result_()
  , analysisStopCriteriaMessage_()
  , analysisErrorMessage_()
  , failedInputSample_()
  , notEvaluatedInputSample_()
  , resultsSampleIsValid_(true)
  , sampleSizeTitle_(tr("Sample size"))
  , inputNames_(QStringList())
  , inAxisTitles_(QStringList())
  , outputNames_(QStringList())
  , outAxisTitles_(QStringList())
  , isConfidenceIntervalRequired_(false)
  , levelConfidenceInterval_(0.)
  , showTable_(false)
  , titleLabel_(new TitleLabel)
  , variablesGroupBox_(0)
  , variablesListWidget_(0)
  , tabWidget_(0)
  , probaSpinBox_(0)
  , quantileSpinBox_(0)
{
}


void DataAnalysisWindow::initializeVariablesNames()
{
  PhysicalModel model(designOfExperiment_.getPhysicalModel());

  // inputs
  if (designOfExperiment_.getInputSample().getSize())
  {
    for (UnsignedInteger i = 0; i < designOfExperiment_.getInputSample().getDimension(); ++i)
    {
      const String inputName = designOfExperiment_.getInputSample().getDescription()[i];
      inputNames_ << QString::fromUtf8(inputName.c_str());
      QString inputDescription;
      if (model.hasInputNamed(inputName))
      {
        inputDescription = QString::fromUtf8(model.getInputByName(inputName).getDescription().c_str());
      }
      if (!inputDescription.isEmpty())
        inAxisTitles_ << inputDescription;
      else
        inAxisTitles_ << inputNames_.last();
    }
  }

  // outputs
  if (designOfExperiment_.getOutputSample().getSize())
  {
    for (UnsignedInteger i = 0; i < designOfExperiment_.getOutputSample().getDimension(); ++i)
    {
      const String outputName = designOfExperiment_.getOutputSample().getDescription()[i];
      outputNames_ << QString::fromUtf8(outputName.c_str());
      QString outputDescription;
      if (model.hasOutputNamed(outputName))
      {
        outputDescription = QString::fromUtf8(model.getOutputByName(outputName).getDescription().c_str());
      }
      if (!outputDescription.isEmpty())
        outAxisTitles_ << outputDescription;
      else
        outAxisTitles_ << outputNames_.last();
    }
  }
}


void DataAnalysisWindow::buildInterface()
{
  // get output info
  initializeVariablesNames();
  QStringList variablesNames = outputNames_ + inputNames_;

  QVBoxLayout * widgetLayout = new QVBoxLayout(this);

  widgetLayout->addWidget(titleLabel_);

  // main splitter
  QSplitter * mainWidget = new QSplitter(Qt::Horizontal);

  // - list outputs
  variablesGroupBox_ = new QGroupBox(tr("Variables"));
  QVBoxLayout * groupBoxLayout = new QVBoxLayout(variablesGroupBox_);

  variablesListWidget_ = new VariablesListWidget;
  fillListWidget();
  groupBoxLayout->addWidget(variablesListWidget_);

  mainWidget->addWidget(variablesGroupBox_);
  mainWidget->setStretchFactor(0, 1);

  tabWidget_ = new QTabWidget;
  fillTabWidget();

  mainWidget->addWidget(tabWidget_);
  mainWidget->setStretchFactor(1, 10);

  widgetLayout->addWidget(mainWidget, 1);
}


void DataAnalysisWindow::fillListWidget()
{
  variablesListWidget_->addItems(outputNames_ + inputNames_);
  for (int i = 0; i < outputNames_.size(); ++i)
    variablesListWidget_->item(i)->setData(Qt::UserRole, inputNames_.size() + i);
  for (int i = 0; i < inputNames_.size(); ++i)
    variablesListWidget_->item(outputNames_.size() + i)->setData(Qt::UserRole, i);

  variablesListWidget_->setCurrentRow(0);
  connect(variablesListWidget_, SIGNAL(currentRowChanged(int)), this, SLOT(updateSpinBoxes(int)));
}


void DataAnalysisWindow::fillTabWidget()
{
  // tab: Summary
  addSummaryTab();
  if (designOfExperiment_.getSample().getSize() > 1)
  {
    // tab: PDF/CDF
    addPDF_CDFTab();
    // tab: box plots
    addBoxPlotTab();
  }
  bool canUseParaview = false;
#ifdef OTGUI_HAVE_PARAVIEW
  if (SubWindow::HaveOpenGL32())
  {
    addParaviewWidgetsTabs();
    canUseParaview = true;
  }
#endif
  if (!canUseParaview)
  {
    // tab: Table --------------------------------
    addTableTab();
    if (designOfExperiment_.getSample().getDimension() > 1 && designOfExperiment_.getSample().getSize() > 1)
    {
      // tab: plot matrix
      addPlotMatrixTab();
      // tab: scatter plots
      addScatterPlotsTab();
    }
  }

  // tab: Parameters --------------------------------
  if (parametersWidget_)
    tabWidget_->addTab(parametersWidget_, tr("Parameters"));

  // tab : model description --------------------------------
  if (modelDescriptionWidget_)
    tabWidget_->addTab(modelDescriptionWidget_, tr("Model"));

  //
  connect(tabWidget_, SIGNAL(currentChanged(int)), this, SLOT(updateVariablesListVisibility(int)));
}


void DataAnalysisWindow::addSummaryTab()
{
  QScrollArea * scrollArea = new QScrollArea;
  scrollArea->setWidgetResizable(true);

  QWidget * tab = new QWidget;
  QVBoxLayout * tabLayout = new QVBoxLayout(tab);

  // -- results --

  // stop criteria
  const QString groupBoxTitle = (analysisStopCriteriaMessage_.isEmpty()) ? tr("") : tr("Stop criteria");
  QGroupBox * parametersGroupBox = new QGroupBox(groupBoxTitle);
  QVBoxLayout * parametersGroupBoxLayout = new QVBoxLayout(parametersGroupBox);

  // - if algo with a stop criteria
  if (!analysisStopCriteriaMessage_.isEmpty())
  {
    QLabel * stopCriteriaLabel = new QLabel(analysisStopCriteriaMessage_);
    parametersGroupBoxLayout->addWidget(stopCriteriaLabel);
  }
  // - if algo with a error message
  if (!analysisErrorMessage_.isEmpty())
  {
    QLabel * analysisErrorMessageLabel = new QLabel(QString("<font color=red>%1</font>").arg(analysisErrorMessage_));
    analysisErrorMessageLabel->setWordWrap(true);
    parametersGroupBoxLayout->addWidget(analysisErrorMessageLabel);
  }

  // parameters values
  QStringList namesList;
  QStringList valuesList;
  // - sample size
  namesList << sampleSizeTitle_;
  valuesList << QString::number(designOfExperiment_.getSample().getSize());
  // - elapsed time
  if (result_.getElapsedTime() > 0.)
  {
    namesList << tr("Elapsed time");
    valuesList << QString::number(result_.getElapsedTime()) + " s";
  }

  ParametersTableView * table = new ParametersTableView(namesList, valuesList, true, true);
  parametersGroupBoxLayout->addWidget(table);
  tabLayout->addWidget(parametersGroupBox, 0, Qt::AlignTop);

  // min/max table
  MinMaxTableGroupBox * minMaxTableGroupBox = new MinMaxTableGroupBox(designOfExperiment_, false);
  tabLayout->addWidget(minMaxTableGroupBox, 0, Qt::AlignTop);
  connect(variablesListWidget_, SIGNAL(currentRowChanged(int)), minMaxTableGroupBox, SLOT(setCurrentIndexStackedWidget(int)));

  if (result_.getMean().getSize())
  {
    // moments estimation
    // we want to display output results before the input results
    // input indices
    Indices inInd(inputNames_.size());
    inInd.fill();
    // ouput indices
    Indices ind(outputNames_.size());
    ind.fill(inputNames_.size());
    // indices with good order
    ind.add(inInd);

    MomentsEstimatesTableGroupBox * estimatesGroupBox = new MomentsEstimatesTableGroupBox(result_,
        isConfidenceIntervalRequired_,
        levelConfidenceInterval_,
        ind);
    tabLayout->addWidget(estimatesGroupBox, 0, Qt::AlignTop);
    connect(variablesListWidget_, SIGNAL(currentRowChanged(int)), estimatesGroupBox, SLOT(setCurrentIndexStackedWidget(int)));

    // quantiles
    QGridLayout * quantLayout = new QGridLayout;

    // Probability
    QLabel * label = new QLabel(tr("Probability"));
    label->setStyleSheet("QLabel {font: bold;}");
    quantLayout->addWidget(label, 0, 0);
    probaSpinBox_ = new DoubleSpinBox;
    label->setBuddy(probaSpinBox_);
    probaSpinBox_->setMinimum(0.0);
    probaSpinBox_->setMaximum(1.0);
    probaSpinBox_->setSingleStep(0.01);
    quantLayout->addWidget(probaSpinBox_, 0, 1);
    // Quantile
    label = new QLabel(tr("Empirical quantile"));
    label->setStyleSheet("QLabel {font: bold;}");
    quantLayout->addWidget(label, 1, 0);
    quantileSpinBox_ = new DoubleSpinBox;
    label->setBuddy(quantileSpinBox_);
    quantileSpinBox_->setDecimals(8);
    quantLayout->addWidget(quantileSpinBox_, 1, 1);
    quantLayout->setColumnStretch(1, 1);

    connect(probaSpinBox_, SIGNAL(valueChanged(double)), this, SLOT(probaValueChanged(double)));
    connect(quantileSpinBox_, SIGNAL(valueChanged(double)), this, SLOT(quantileValueChanged(double)));

    tabLayout->addLayout(quantLayout);
    updateSpinBoxes();
  }

  tabLayout->addStretch();

  scrollArea->setWidget(tab);
  tabWidget_->addTab(scrollArea, tr("Summary"));
}


void DataAnalysisWindow::addPDF_CDFTab()
{
  QScrollArea * scrollArea = new QScrollArea;
  scrollArea->setWidgetResizable(true);
  ResizableStackedWidget * tabStackedWidget = new ResizableStackedWidget;
  connect(variablesListWidget_, SIGNAL(currentRowChanged(int)), tabStackedWidget, SLOT(setCurrentIndex(int)));

  const QStringList variablesNames = outputNames_ + inputNames_;
  const QStringList variablesAxisTitles = outAxisTitles_ + inAxisTitles_;

  // we want to display output results before the input results
  // input indices
  Indices inInd(inputNames_.size());
  inInd.fill();
  // ouput indices
  Indices ind(outputNames_.size());
  ind.fill(inputNames_.size());
  // indices with good order
  ind.add(inInd);

  for (int i = 0; i < variablesNames.size(); ++i)
  {
    WidgetBoundToDockWidget * plotWidget = new WidgetBoundToDockWidget(this);
    QVBoxLayout * plotWidgetLayout = new QVBoxLayout(plotWidget);

    ResizableStackedWidget * stackedWidget = new ResizableStackedWidget;

    // PDF
    PlotWidget * pdfPlot = new PlotWidget(tr("distributionPDF"));
    pdfPlot->plotHistogram(designOfExperiment_.getSample().getMarginal(ind[i]));
    if (result_.getPDF()[ind[i]].getSize())
      pdfPlot->plotCurve(result_.getPDF()[ind[i]]);
    pdfPlot->setTitle(tr("PDF:") + " " + variablesNames[i]);
    pdfPlot->setAxisTitle(QwtPlot::xBottom, variablesAxisTitles[i]);
    pdfPlot->setAxisTitle(QwtPlot::yLeft, tr("Density"));

    stackedWidget->addWidget(pdfPlot);

    // CDF
    PlotWidget * cdfPlot = new PlotWidget(tr("distributionCDF"));
    cdfPlot->plotHistogram(designOfExperiment_.getSample().getMarginal(ind[i]), 1);
    if (result_.getCDF()[ind[i]].getSize())
      cdfPlot->plotCurve(result_.getCDF()[ind[i]]);
    cdfPlot->setTitle(tr("CDF:") + " " + variablesNames[i]);
    cdfPlot->setAxisTitle(QwtPlot::xBottom, variablesAxisTitles[i]);
    cdfPlot->setAxisTitle(QwtPlot::yLeft, tr("CDF"));

    stackedWidget->addWidget(cdfPlot);

    // Graph Setting
    QVector<PlotWidget*> listPlotWidgets;
    listPlotWidgets.append(pdfPlot);
    listPlotWidgets.append(cdfPlot);
    GraphConfigurationWidget * graphSetting = new GraphConfigurationWidget(listPlotWidgets,
        QStringList(),
        QStringList(),
        GraphConfigurationWidget::PDFResult,
        this);
    connect(graphSetting, SIGNAL(currentPlotChanged(int)), stackedWidget, SLOT(setCurrentIndex(int)));

    plotWidget->setDockWidget(graphSetting);
    plotWidgetLayout->addWidget(stackedWidget);

    tabStackedWidget->addWidget(plotWidget);
  }

  scrollArea->setWidget(tabStackedWidget);
  tabWidget_->addTab(scrollArea, tr("PDF/CDF"));
}


void DataAnalysisWindow::addBoxPlotTab()
{
  QScrollArea * scrollArea = new QScrollArea;
  scrollArea->setWidgetResizable(true);
  ResizableStackedWidget * tabStackedWidget = new ResizableStackedWidget;
  connect(variablesListWidget_, SIGNAL(currentRowChanged(int)), tabStackedWidget, SLOT(setCurrentIndex(int)));

  const QStringList variablesNames = outputNames_ + inputNames_;
  const QStringList variablesAxisTitles = outAxisTitles_ + inAxisTitles_;

  // we want to display output results before the input results
  // input indices
  Indices inInd(inputNames_.size());
  inInd.fill();
  // ouput indices
  Indices ind(outputNames_.size());
  ind.fill(inputNames_.size());
  // indices with good order
  ind.add(inInd);

  for (int i = 0; i < variablesNames.size(); ++i)
  {
    WidgetBoundToDockWidget * boxPlotWidget = new WidgetBoundToDockWidget(this);
    QVBoxLayout * boxPlotWidgetLayout = new QVBoxLayout(boxPlotWidget);

    PlotWidget * plot = new PlotWidget(tr("boxplot"));

    const double median = result_.getMedian()[ind[i]][0];
    const double Q1 = result_.getFirstQuartile()[ind[i]][0];
    const double Q3 = result_.getThirdQuartile()[ind[i]][0];
    plot->plotBoxPlot(median, Q1, Q3, Q1 - 1.5 * (Q3 - Q1), Q3 + 1.5 * (Q3 - Q1), result_.getOutliers()[ind[i]]);
    plot->setTitle(tr("Box plot:") + " " + variablesNames[i]);
    plot->setAxisTitle(QwtPlot::yLeft, variablesAxisTitles[i]);

    boxPlotWidgetLayout->addWidget(plot);

    // Graph Setting
    GraphConfigurationWidget * graphSetting = new GraphConfigurationWidget(plot,
        QStringList(),
        QStringList(),
        GraphConfigurationWidget::NoType,
        this);
    boxPlotWidget->setDockWidget(graphSetting);

    tabStackedWidget->addWidget(boxPlotWidget);
  }

  scrollArea->setWidget(tabStackedWidget);
  tabWidget_->addTab(scrollArea, tr("Box plots"));
}


void DataAnalysisWindow::addPlotMatrixTab()
{
  WidgetBoundToDockWidget * matrixTabWidget = new WidgetBoundToDockWidget(this);
  QVBoxLayout * matrixTabWidgetLayout = new QVBoxLayout(matrixTabWidget);
  PlotMatrixWidget * plotMatrixWidget = new PlotMatrixWidget(designOfExperiment_.getSample(), designOfExperiment_.getSample());
  plotMatrixWidget->setInputNames(inputNames_);
  plotMatrixWidget->setOutputNames(outputNames_);
  matrixTabWidgetLayout->addWidget(plotMatrixWidget);

  PlotMatrixConfigurationWidget * plotMatrixSettingWidget = new PlotMatrixConfigurationWidget(plotMatrixWidget, this);
  matrixTabWidget->setDockWidget(plotMatrixSettingWidget);

  tabWidget_->addTab(matrixTabWidget, tr("Plot matrix"));
}


void DataAnalysisWindow::addScatterPlotsTab()
{
  WidgetBoundToDockWidget * scatterWidget = new WidgetBoundToDockWidget(this);
  QVBoxLayout * scatterWidgetLayout = new QVBoxLayout(scatterWidget);

  QVector<PlotWidget*> listScatterPlotWidgets = PlotWidget::GetListScatterPlots(designOfExperiment_.getInputSample(),
      designOfExperiment_.getOutputSample(),
      failedInputSample_,
      inputNames_,
      inAxisTitles_,
      outputNames_,
      outAxisTitles_);

  ResizableStackedWidget * stackedWidget = new ResizableStackedWidget;
  for (int i = 0; i < listScatterPlotWidgets.size(); ++i)
    stackedWidget->addWidget(listScatterPlotWidgets[i]);

  GraphConfigurationWidget * scatterSettingWidget = new GraphConfigurationWidget(listScatterPlotWidgets,
      inputNames_,
      outputNames_,
      GraphConfigurationWidget::Scatter,
      this);
  scatterWidget->setDockWidget(scatterSettingWidget);
  connect(scatterSettingWidget, SIGNAL(currentPlotChanged(int)), stackedWidget, SLOT(setCurrentIndex(int)));

  scatterWidgetLayout->addWidget(stackedWidget);
  tabWidget_->addTab(scatterWidget, tr("Scatter plots"));
}


void DataAnalysisWindow::addTableTab()
{
  // Well evaluated points
  ExportableTableView * tableView = new ExportableTableView;
  connect(getItem(), SIGNAL(dataExportRequested()), tableView, SLOT(exportData()));
  tableView->setSortingEnabled(true);

  SampleTableModel * tableModel = new SampleTableModel(designOfExperiment_.getSample(), tableView);
  QSortFilterProxyModel * proxyModel = new QSortFilterProxyModel(tableView);
  proxyModel->setSourceModel(tableModel);
  proxyModel->setSortRole(Qt::UserRole);

  tableView->setModel(proxyModel);
  tableView->sortByColumn(0, Qt::AscendingOrder);

  QWidget * tab = new QWidget;
  QVBoxLayout * tabLayout = new QVBoxLayout(tab);

  QHBoxLayout * hLayout = new QHBoxLayout;
  // sample size
  QLabel * sizeLabel = new QLabel(tr("Size") + " : " + QString::number(designOfExperiment_.getSample().getSize()));
  hLayout->addWidget(sizeLabel);
  hLayout->addStretch();
  // export button
  QPushButton * exportButton = new QPushButton(QIcon(":/images/document-export-table.png"), tr("Export"));
  hLayout->addWidget(exportButton);
  connect(exportButton, SIGNAL(clicked()), tableView, SLOT(exportData()));
  tabLayout->addLayout(hLayout);

  // table
  tabLayout->addWidget(tableView);

  // if no failed points and no not evaluated points:
  if (!failedInputSample_.getSize() && !notEvaluatedInputSample_.getSize())
  {
    tabWidget_->addTab(tab, tr("Table"));
  }
  else
  {
    QTabWidget * tablesTabWidget = new QTabWidget;

    tablesTabWidget->addTab(tab, tr("Points"));

    // tab with failed points
    if (failedInputSample_.getSize())
    {
      tab = new QWidget;
      tabLayout = new QVBoxLayout(tab);

      // sample size
      sizeLabel = new QLabel(tr("Size") + " : " + QString::number(failedInputSample_.getSize()));
      tabLayout->addWidget(sizeLabel);

      tableView = new ExportableTableView;
      tableView->setSortingEnabled(true);
      tableModel = new SampleTableModel(failedInputSample_, tableView);
      proxyModel = new QSortFilterProxyModel(tableView);
      proxyModel->setSourceModel(tableModel);
      proxyModel->setSortRole(Qt::UserRole);
      tableView->setModel(proxyModel);
      tableView->sortByColumn(0, Qt::AscendingOrder);
      tabLayout->addWidget(tableView);

      tablesTabWidget->addTab(tab, tr("Failed blocks"));
    }
    // tab with not evaluated points
    if (notEvaluatedInputSample_.getSize())
    {
      tab = new QWidget;
      tabLayout = new QVBoxLayout(tab);

      // sample size
      sizeLabel = new QLabel(tr("Size") + " : " + QString::number(failedInputSample_.getSize()));
      tabLayout->addWidget(sizeLabel);

      tableView = new ExportableTableView;
      tableView->setSortingEnabled(true);
      tableModel = new SampleTableModel(notEvaluatedInputSample_, tableView);
      proxyModel = new QSortFilterProxyModel(tableView);
      proxyModel->setSourceModel(tableModel);
      proxyModel->setSortRole(Qt::UserRole);
      tableView->setModel(proxyModel);
      tableView->sortByColumn(0, Qt::AscendingOrder);
      tabLayout->addWidget(tableView);

      tablesTabWidget->addTab(tab, tr("Not evaluated points"));
    }

    tabWidget_->addTab(tablesTabWidget, tr("Table"));
  }
}


#ifdef OTGUI_HAVE_PARAVIEW
void DataAnalysisWindow::addParaviewWidgetsTabs()
{
  // get data info
  const UnsignedInteger failedInSampleSize = failedInputSample_.getSize();
  const UnsignedInteger notEvalInSampleSize = notEvaluatedInputSample_.getSize();
  const UnsignedInteger inSampleSize = designOfExperiment_.getInputSample().getSize();
  const UnsignedInteger inSampleDim = designOfExperiment_.getInputSample().getDimension();

  // table tab
  QWidget * tableWidget = new QWidget;
  QVBoxLayout * tableWidgetLayout = new QVBoxLayout(tableWidget);

  // with paraview the table is always shown in order to use the selection behavior
  PVSpreadSheetViewWidget * pvSpreadSheetWidget = new PVSpreadSheetViewWidget(this, PVServerManagerSingleton::Get());
  pvSpreadSheetWidget->setData(designOfExperiment_.getSample());
  connect(getItem(), SIGNAL(dataExportRequested()), pvSpreadSheetWidget, SLOT(exportData()));

  QHBoxLayout * hLayout = new QHBoxLayout;
  // sample size
  QLabel * sizeLabel = new QLabel(tr("Size") + " : " + QString::number(designOfExperiment_.getSample().getSize()));
  hLayout->addWidget(sizeLabel);
  hLayout->addStretch();
  // export button
  QPushButton * exportButton = new QPushButton(QIcon(":/images/document-export-table.png"), tr("Export"));
  hLayout->addWidget(exportButton);
  connect(exportButton, SIGNAL(clicked()), pvSpreadSheetWidget, SLOT(exportData()));
  tableWidgetLayout->addLayout(hLayout);

  tableWidgetLayout->addWidget(pvSpreadSheetWidget);

  // if no failed points and no not evaluated points:
  if (!failedInSampleSize && !notEvalInSampleSize)
  {
    tabWidget_->addTab(tableWidget, tr("Table"));
  }
  else
  {
    // Table tab ------------------------------------------
    QTabWidget * tablesTabWidget = new QTabWidget;

    tablesTabWidget->addTab(tableWidget, tr("Table"));

    // -- failed points tab
    if (failedInSampleSize)
    {
      tableWidget = new QWidget;
      tableWidgetLayout = new QVBoxLayout(tableWidget);

      sizeLabel = new QLabel(tr("Size") + " : " + QString::number(failedInSampleSize));
      tableWidgetLayout->addWidget(sizeLabel);

      PVSpreadSheetViewWidget * failedPointsTable = new PVSpreadSheetViewWidget(this, PVServerManagerSingleton::Get());
      failedPointsTable->setData(failedInputSample_);
      tableWidgetLayout->addWidget(failedPointsTable);

      tablesTabWidget->addTab(tableWidget, tr("Failed points"));
    }
    // -- not evaluated points tab
    if (notEvalInSampleSize)
    {
      tableWidget = new QWidget;
      tableWidgetLayout = new QVBoxLayout(tableWidget);

      sizeLabel = new QLabel(tr("Size") + " : " + QString::number(failedInSampleSize));
      tableWidgetLayout->addWidget(sizeLabel);

      PVSpreadSheetViewWidget * notEvaluatedPointsTable = new PVSpreadSheetViewWidget(this, PVServerManagerSingleton::Get());
      notEvaluatedPointsTable->setData(notEvaluatedInputSample_);
      tableWidgetLayout->addWidget(notEvaluatedPointsTable);

      tablesTabWidget->addTab(tableWidget, tr("Not evaluated points"));
    }

    // -- Cobweb plot tab
    if (failedInSampleSize)
    {
      WidgetBoundToDockWidget * cobwebTabWidget = new WidgetBoundToDockWidget(this);
      QVBoxLayout * cobwebTabWidgetLayout = new QVBoxLayout(cobwebTabWidget);

      PVParCooViewWidget * cobwebWidget = new PVParCooViewWidget(this, PVServerManagerSingleton::Get());

      // input sample + failed input sample
      Sample succeedAndFailedInS(designOfExperiment_.getInputSample());
      succeedAndFailedInS.add(failedInputSample_);

      Sample succeedAndFailedInSRank(succeedAndFailedInS.rank() / succeedAndFailedInS.getSize());

      // add a status column
      // 0. : failed point
      // 1. : succeeded point
      Sample statusColumnSample(inSampleSize, Point(1, 1.0));
      statusColumnSample.add(Sample(failedInSampleSize, 1));
      succeedAndFailedInS.stack(statusColumnSample);
      succeedAndFailedInSRank.stack(statusColumnSample);

      // set columns name
      Description desc(designOfExperiment_.getInputSample().getDescription());
      desc.add(tr("Status\n0: failed; 1: ok").toUtf8().constData());
      succeedAndFailedInS.setDescription(desc);
      succeedAndFailedInSRank.setDescription(desc);

      cobwebWidget->setData(succeedAndFailedInSRank);
      cobwebWidget->setAxisToShow(desc);
      cobwebTabWidgetLayout->addWidget(cobwebWidget);

      // cobweb setting widget
      PVPlotSettingWidget * cobwebSettingWidget = new PVPlotSettingWidget(cobwebWidget, succeedAndFailedInS, succeedAndFailedInSRank, this);
      cobwebTabWidget->setDockWidget(cobwebSettingWidget);

      tablesTabWidget->addTab(cobwebTabWidget, tr("Cobweb plot"));
    }

    // -- scatter plots tab
    const bool canBuildScatterPlot = (inSampleSize > 1 &&
                                      (failedInSampleSize == 0 || failedInSampleSize > 1) &&
                                      (notEvalInSampleSize == 0 || notEvalInSampleSize > 1));
    // if one representation has only one point => paraview error => segfault
    if (canBuildScatterPlot)
    {
      // input sample
      PVXYChartViewWidget * sampleScatterPlotWidget = new PVXYChartViewWidget(this, PVServerManagerSingleton::Get());
      sampleScatterPlotWidget->setData(designOfExperiment_.getInputSample(), Qt::green);
      std::list<QString> inSLabelsList(inSampleDim, tr("Succeed points"));
      sampleScatterPlotWidget->setRepresentationLabels(QList<QString>::fromStdList(inSLabelsList), 0);
      // failed input sample
      if (failedInSampleSize)
      {
        sampleScatterPlotWidget->setData(failedInputSample_, Qt::red);
        std::list<QString> failedInSLabelsList(inSampleDim, tr("Failed points"));
        sampleScatterPlotWidget->setRepresentationLabels(QList<QString>::fromStdList(failedInSLabelsList), 1);
      }
      // not evaluated points
      if (notEvalInSampleSize)
      {
        sampleScatterPlotWidget->setData(notEvaluatedInputSample_, Qt::blue);
        std::list<QString> notEvaluatedInSLabelsList(inSampleDim, tr("Not evaluated points"));
        sampleScatterPlotWidget->setRepresentationLabels(QList<QString>::fromStdList(notEvaluatedInSLabelsList), failedInSampleSize > 0 ? 2 : 1);
      }
      WidgetBoundToDockWidget * scatterTabWidget = new WidgetBoundToDockWidget(this);
      QVBoxLayout * scatterTabWidgetLayout = new QVBoxLayout(scatterTabWidget);
      sampleScatterPlotWidget->setAxisTitles(inputNames_, inAxisTitles_);
      scatterTabWidgetLayout->addWidget(sampleScatterPlotWidget);

      // samples rank
      // allInputsSample : input sample + failed input sample + not evaluated input sample
      Sample allInputsSample(designOfExperiment_.getInputSample());
      if (failedInSampleSize)
        allInputsSample.add(failedInputSample_);
      if (notEvalInSampleSize)
        allInputsSample.add(notEvaluatedInputSample_);

      // allInputsSample rank
      const Sample allInputsSampleRank(allInputsSample.rank() / allInputsSample.getSize());
      const Sample inputsRank(allInputsSampleRank, 0, inSampleSize);
      const Sample failedInputsRank(allInputsSampleRank, inSampleSize, inSampleSize + failedInSampleSize);
      const Sample notEvaluatedInputsRank(allInputsSampleRank, inSampleSize + failedInSampleSize, allInputsSampleRank.getSize());

      // scatter plots setting widget
      PVXYChartSettingWidget * inSampleSettingWidget = new PVXYChartSettingWidget(sampleScatterPlotWidget,
          designOfExperiment_.getInputSample(),
          inputsRank,
          failedInputSample_,
          failedInputsRank,
          notEvaluatedInputSample_,
          notEvaluatedInputsRank,
          PVXYChartSettingWidget::Scatter,
          this);
      scatterTabWidget->setDockWidget(inSampleSettingWidget);

      tablesTabWidget->addTab(scatterTabWidget, tr("Scatter plots"));
    }

    tabWidget_->addTab(tablesTabWidget, tr("Table"));
  }

  // if only one variable or if only one point : do not need the following graphs
  if (designOfExperiment_.getSample().getDimension() > 1 && designOfExperiment_.getSample().getSize() > 1)
    addParaviewPlotWidgetsTabs(pvSpreadSheetWidget);
}


void DataAnalysisWindow::addParaviewPlotWidgetsTabs(PVSpreadSheetViewWidget * pvSpreadSheet)
{
  Q_ASSERT(pvSpreadSheet);

  // 1- cobweb tab --------------------------------
  WidgetBoundToDockWidget * cobwebTabWidget = new WidgetBoundToDockWidget(this);
  QVBoxLayout * cobwebTabWidgetLayout = new QVBoxLayout(cobwebTabWidget);

  PVParCooViewWidget * cobwebWidget = new PVParCooViewWidget(this, PVServerManagerSingleton::Get());
  const Sample sampleRank(designOfExperiment_.getSample().rank() / designOfExperiment_.getSample().getSize());
  cobwebWidget->setData(sampleRank);
  // the variables are automatically sorted : use setAxisToShow with the order of the sample
  cobwebWidget->setAxisToShow(designOfExperiment_.getSample().getDescription());
  cobwebTabWidgetLayout->addWidget(cobwebWidget);

  // setting widget
  PVPlotSettingWidget * cobwebSettingWidget = new PVPlotSettingWidget(cobwebWidget,
      designOfExperiment_.getSample(),
      sampleRank,
      this);
  cobwebTabWidget->setDockWidget(cobwebSettingWidget);

  tabWidget_->addTab(cobwebTabWidget, tr("Cobweb plot"));

  // 2- plot matrix tab --------------------------------
  WidgetBoundToDockWidget * matrixTabWidget = new WidgetBoundToDockWidget(this);
  QVBoxLayout * matrixTabWidgetLayout = new QVBoxLayout(matrixTabWidget);

  PVMatrixPlotViewWidget * pvmatrixWidget = new PVMatrixPlotViewWidget(this, PVServerManagerSingleton::Get());
  pvmatrixWidget->setData(designOfExperiment_.getSample());
  // the variables are automatically sorted : use setAxisToShow with the order of the sample
  pvmatrixWidget->setAxisToShow(designOfExperiment_.getSample().getDescription());
  matrixTabWidgetLayout->addWidget(pvmatrixWidget);

  // setting widget
  PVPlotSettingWidget * matrixSettingWidget = new PVPlotSettingWidget(pvmatrixWidget, this);
  matrixTabWidget->setDockWidget(matrixSettingWidget);

  tabWidget_->addTab(matrixTabWidget, tr("Plot matrix"));

  // 3- scatter plots tab --------------------------------
  WidgetBoundToDockWidget * scatterTabWidget = new WidgetBoundToDockWidget(this);
  QVBoxLayout * scatterTabWidgetLayout = new QVBoxLayout(scatterTabWidget);

  // sample
  PVXYChartViewWidget * sampleScatterPlotWidget = new PVXYChartViewWidget(this, PVServerManagerSingleton::Get());
  sampleScatterPlotWidget->PVViewWidget::setData(designOfExperiment_.getSample());
  if ((inputNames_ + outputNames_) != (inAxisTitles_ + outAxisTitles_))
    sampleScatterPlotWidget->setAxisTitles(inputNames_ + outputNames_, inAxisTitles_ + outAxisTitles_);
  scatterTabWidgetLayout->addWidget(sampleScatterPlotWidget);

  PVXYChartSettingWidget * scatterSettingWidget = new PVXYChartSettingWidget(sampleScatterPlotWidget,
      designOfExperiment_.getSample(),
      sampleRank,
      inputNames_,
      outputNames_,
      PVXYChartSettingWidget::Scatter,
      this);
  scatterTabWidget->setDockWidget(scatterSettingWidget);

  tabWidget_->addTab(scatterTabWidget, tr("Scatter plots"));

  // 4- links model --------------------------------
  pqLinksModel * linksModel = pqApplicationCore::instance()->getLinksModel();

  // There are selection behavior errors if windows use the same links names: a link name must be unique.
  // The pointers are uniques, so we use them to create an unique name...find a better and easier way.
  String aStr = (OSS() << cobwebWidget->getProxy() << pvmatrixWidget->getProxy()).str();
  linksModel->addSelectionLink(aStr.c_str(), cobwebWidget->getProxy(), pvmatrixWidget->getProxy());
  aStr = (OSS() << pvSpreadSheet->getProxy() << pvmatrixWidget->getProxy()).str();
  linksModel->addSelectionLink(aStr.c_str(), pvSpreadSheet->getProxy(), pvmatrixWidget->getProxy());
  aStr = (OSS() << cobwebWidget->getProxy() << pvSpreadSheet->getProxy()).str();
  linksModel->addSelectionLink(aStr.c_str(), cobwebWidget->getProxy(), pvSpreadSheet->getProxy());
  aStr = (OSS() << sampleScatterPlotWidget->getProxy() << pvSpreadSheet->getProxy()).str();
  linksModel->addSelectionLink(aStr.c_str(), sampleScatterPlotWidget->getProxy(), pvSpreadSheet->getProxy());
}
#endif


void DataAnalysisWindow::updateSpinBoxes(int indexList)
{
  SignalBlocker blocker(quantileSpinBox_);

  // index of the variable in result_
  const UnsignedInteger indexVar = variablesListWidget_->item(indexList)->data(Qt::UserRole).toInt();

  // check
  if (indexVar >= result_.getMin().getSize() || indexVar >= result_.getMax().getSize())
    throw InvalidArgumentException(HERE) << "The result dimension does not match the sample dimension";

  if (result_.getMin().getSize() && result_.getMax().getSize())
  {
    const double min = result_.getMin()[indexVar][0];
    const double max = result_.getMax()[indexVar][0];

    quantileSpinBox_->setMinimum(min);
    quantileSpinBox_->setMaximum(max);
    quantileSpinBox_->setSingleStep((max - min) / 100);
  }
  probaSpinBox_->setValue(0.5);
  // if the previous value of probaSpinBox_ was 0.5, the signal valueChanged is not emitted
  probaValueChanged(0.5);
}


void DataAnalysisWindow::probaValueChanged(double proba)
{
  SignalBlocker blocker(quantileSpinBox_);

  // index of the variable in result_
  const UnsignedInteger indexVar = variablesListWidget_->item(variablesListWidget_->currentRow())->data(Qt::UserRole).toInt();
  quantileSpinBox_->setValue(designOfExperiment_.getSample().getMarginal(indexVar).computeQuantile(proba)[0]);
}


void DataAnalysisWindow::quantileValueChanged(double quantile)
{
  SignalBlocker blocker(probaSpinBox_);

  // index of the variable in result_
  const UnsignedInteger indexVar = variablesListWidget_->item(variablesListWidget_->currentRow())->data(Qt::UserRole).toInt();

  double cdf = 0.0;
  const double p = 1.0 / double(designOfExperiment_.getSample().getSize());

  for (UnsignedInteger j = 0; j < designOfExperiment_.getSample().getSize(); ++j)
    if (designOfExperiment_.getSample()[j][indexVar] < quantile)
      cdf += p;

  probaSpinBox_->setValue(cdf);
}


void DataAnalysisWindow::updateVariablesListVisibility(int indexTab)
{
  variablesGroupBox_->setVisible(indexTab == 0 || indexTab == 1 || indexTab == 2);
}
}
