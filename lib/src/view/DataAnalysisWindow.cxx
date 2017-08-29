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
#include "otgui/ParametersTableView.hxx"
#include "otgui/ExportableTableView.hxx"
#include "otgui/SampleTableModel.hxx"
#include "otgui/WidgetBoundToDockWidget.hxx"
#include "otgui/GraphConfigurationWidget.hxx"
#include "otgui/PlotMatrixConfigurationWidget.hxx"

#ifdef OTGUI_HAVE_PARAVIEW
#include "otgui/PVServerManagerInterface.hxx"
#include "otgui/PVServerManagerSingleton.hxx"
#include "otgui/PVSpreadSheetViewWidget.hxx"
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

using namespace OT;

namespace OTGUI {

DataAnalysisWindow::DataAnalysisWindow(AnalysisItem * item, QWidget * parent)
  : ResultWindow(item, parent)
  , designOfExperiment_()
  , result_()
  , failedInputSample_()
  , resultsSampleIsValid_(true)
  , sampleSizeTitle_(tr("Sample size"))
  , inputNames_(QStringList())
  , inAxisTitles_(QStringList())
  , outputNames_(QStringList())
  , outAxisTitles_(QStringList())
  , isConfidenceIntervalRequired_(false)
  , levelConfidenceInterval_(0.)
  , showTable_(false)
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

  // outputs
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


void DataAnalysisWindow::buildInterface()
{
  // get output info
  initializeVariablesNames();
  QStringList variablesNames = outputNames_ + inputNames_;

  // main splitter
  QSplitter * mainWidget = new QSplitter(Qt::Horizontal);

  // - list outputs
  variablesGroupBox_ = new QGroupBox(tr("Variable(s)", "", variablesNames.size()));
  QVBoxLayout * groupBoxLayout = new QVBoxLayout(variablesGroupBox_);

  variablesListWidget_ = new OTguiListWidget;
  fillListWidget();
  groupBoxLayout->addWidget(variablesListWidget_);

  mainWidget->addWidget(variablesGroupBox_);
  mainWidget->setStretchFactor(0, 1);

  tabWidget_ = new QTabWidget;
  fillTabWidget();

  mainWidget->addWidget(tabWidget_);
  mainWidget->setStretchFactor(1, 10);

  setWidget(mainWidget);
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
  // tab: PDF/CDF
  addPDF_CDFTab();
  // tab: box plots
  addBoxPlotTab();
#ifdef OTGUI_HAVE_PARAVIEW
  addParaviewWidgetsTabs();
#else
  // tab: Table --------------------------------
  addTableTab();
  // tab: plot matrix
  addPlotMatrixTab();
  // tab: scatter plots
  addScatterPlotsTab();
#endif

  // tab: Parameters --------------------------------
  if (parametersWidget_)
    tabWidget_->addTab(parametersWidget_, tr("Parameters"));

  //
  connect(tabWidget_, SIGNAL(currentChanged(int)), this, SLOT(updateVariablesListVisibility(int)));
}


void DataAnalysisWindow::addSummaryTab()
{
  QWidget * tab = new QWidget;
  QVBoxLayout * tabLayout = new QVBoxLayout(tab);

  QScrollArea * scrollArea = new QScrollArea;
  scrollArea->setWidgetResizable(true);
  tabLayout->setSizeConstraint(QLayout::SetFixedSize);

  // -- results --
  QVBoxLayout * vbox = new QVBoxLayout;

  // stop criteria
  QString groupBoxTitle = (result_.getElapsedTime() > 0.) ? tr("Stop criteria") : tr("");
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
  valuesList << QString::number(designOfExperiment_.getSample().getSize());

  ParametersTableView * table = new ParametersTableView(namesList, valuesList, true, true);
  parametersGroupBoxLayout->addWidget(table);
  parametersGroupBoxLayout->addStretch();
  vbox->addWidget(parametersGroupBox);

  // min/max table
  MinMaxTableGroupBox * minMaxTableGroupBox = new MinMaxTableGroupBox(designOfExperiment_, false);
  vbox->addWidget(minMaxTableGroupBox);
  connect(variablesListWidget_, SIGNAL(currentRowChanged(int)), minMaxTableGroupBox, SLOT(setCurrentIndexStackedWidget(int)));

  // moments estimation
  try
  {
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


void DataAnalysisWindow::addPDF_CDFTab()
{
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

  tabWidget_->addTab(tabStackedWidget, tr("PDF/CDF"));
}


void DataAnalysisWindow::addBoxPlotTab()
{
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
    QVector<PlotWidget*> listBoxPlotWidgets;
    listBoxPlotWidgets.append(plot);
    GraphConfigurationWidget * graphSetting = new GraphConfigurationWidget(listBoxPlotWidgets,
                                                                           QStringList(),
                                                                           QStringList(),
                                                                           GraphConfigurationWidget::NoType,
                                                                           this);
    boxPlotWidget->setDockWidget(graphSetting);

    tabStackedWidget->addWidget(boxPlotWidget);
  }

  tabWidget_->addTab(tabStackedWidget, tr("Box plots"));
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
  QWidget * tab = new QWidget;
  QVBoxLayout * tabLayout = new QVBoxLayout(tab);

  ExportableTableView * tabResultView = new ExportableTableView;
  tabResultView->setSortingEnabled(true);

  SampleTableModel * tabResultModel = new SampleTableModel(designOfExperiment_.getSample(), tabResultView);
  QSortFilterProxyModel * proxyModel = new QSortFilterProxyModel(tabResultView);
  proxyModel->setSourceModel(tabResultModel);

  tabResultView->setModel(proxyModel);
  tabResultView->sortByColumn(0, Qt::AscendingOrder);

  tabLayout->addWidget(tabResultView);

  tabWidget_->addTab(tab, tr("Table"));
}


#ifdef OTGUI_HAVE_PARAVIEW
void DataAnalysisWindow::addParaviewWidgetsTabs()
{
  // table tab
  // with paraview the table is always shown in order to use the selection behavior
  PVSpreadSheetViewWidget * pvSpreadSheetWidget = new PVSpreadSheetViewWidget(this, PVServerManagerSingleton::Get());
  pvSpreadSheetWidget->setData(designOfExperiment_.getSample());

  tabWidget_->addTab(pvSpreadSheetWidget, tr("Table"));

  // if only one variable : do not need the following graphs
  if (designOfExperiment_.getSample().getDimension() < 2)
    return;

  // cobweb tab --------------------------------
  WidgetBoundToDockWidget * cobwebTabWidget = new WidgetBoundToDockWidget(this);
  QVBoxLayout * cobwebTabWidgetLayout = new QVBoxLayout(cobwebTabWidget);

  PVParCooViewWidget * cobwebWidget = new PVParCooViewWidget(this, PVServerManagerSingleton::Get());
  cobwebWidget->setData(designOfExperiment_.getSample());
  cobwebTabWidgetLayout->addWidget(cobwebWidget);

  PVPlotSettingWidget * cobwebSettingWidget = new PVPlotSettingWidget(cobwebWidget, this);
  cobwebTabWidget->setDockWidget(cobwebSettingWidget);

  tabWidget_->addTab(cobwebTabWidget, tr("Cobweb plot"));

  // plot matrix tab --------------------------------
  WidgetBoundToDockWidget * matrixTabWidget = new WidgetBoundToDockWidget(this);
  QVBoxLayout * matrixTabWidgetLayout = new QVBoxLayout(matrixTabWidget);

  PVMatrixPlotViewWidget * pvmatrixWidget = new PVMatrixPlotViewWidget(this, PVServerManagerSingleton::Get());
  pvmatrixWidget->setData(designOfExperiment_.getSample());
  // the variables are automatically sorted : use setAxisToShow with the order of the sample
  pvmatrixWidget->setAxisToShow(designOfExperiment_.getSample().getDescription());
  matrixTabWidgetLayout->addWidget(pvmatrixWidget);

  tabWidget_->addTab(matrixTabWidget, tr("Plot matrix"));

  // setting widget
  PVPlotSettingWidget * matrixSettingWidget = new PVPlotSettingWidget(pvmatrixWidget, this);
  matrixTabWidget->setDockWidget(matrixSettingWidget);

  // scatter plots tab --------------------------------
  WidgetBoundToDockWidget * scatterTabWidget = new WidgetBoundToDockWidget(this);
  QVBoxLayout * scatterTabWidgetLayout = new QVBoxLayout(scatterTabWidget);

  // sample
  PVXYChartViewWidget * sampleScatterPlotWidget = new PVXYChartViewWidget(this, PVServerManagerSingleton::Get());
  sampleScatterPlotWidget->PVViewWidget::setData(designOfExperiment_.getSample());
  if ((inputNames_ + outputNames_) != (inAxisTitles_ + outAxisTitles_))
    sampleScatterPlotWidget->setAxisTitles(inputNames_ + outputNames_, inAxisTitles_ + outAxisTitles_);
  scatterTabWidgetLayout->addWidget(sampleScatterPlotWidget);

  // sample rank
  const Sample sampleRank(designOfExperiment_.getSample().rank() / designOfExperiment_.getSample().getSize());
  PVXYChartViewWidget * sampleRankScatterPlotWidget = new PVXYChartViewWidget(this, PVServerManagerSingleton::Get());
  sampleRankScatterPlotWidget->PVViewWidget::setData(sampleRank);
  if ((inputNames_ + outputNames_) != (inAxisTitles_ + outAxisTitles_))
    sampleRankScatterPlotWidget->setAxisTitles(inputNames_ + outputNames_, inAxisTitles_ + outAxisTitles_);
  scatterTabWidgetLayout->addWidget(sampleRankScatterPlotWidget);

  PVXYChartSettingWidget * scatterSettingWidget = new PVXYChartSettingWidget(sampleScatterPlotWidget,
                                                                             sampleRankScatterPlotWidget,
                                                                             inputNames_,
                                                                             outputNames_,
                                                                             PVXYChartSettingWidget::Scatter,
                                                                             this);
  scatterTabWidget->setDockWidget(scatterSettingWidget);

  tabWidget_->addTab(scatterTabWidget, tr("Scatter plots"));

  // links model
  pqLinksModel * linksModel = pqApplicationCore::instance()->getLinksModel();

  // There are selection behavior errors if windows use the same links names: a link name must be unique.
  // The pointers are uniques, so we use them to create an unique name...find a better and easier way.
  String aStr = (OSS() << cobwebWidget->getProxy() << pvmatrixWidget->getProxy()).str();
  linksModel->addSelectionLink(aStr.c_str(), cobwebWidget->getProxy(), pvmatrixWidget->getProxy());
  aStr = (OSS() << pvSpreadSheetWidget->getProxy() << pvmatrixWidget->getProxy()).str();
  linksModel->addSelectionLink(aStr.c_str(), pvSpreadSheetWidget->getProxy(), pvmatrixWidget->getProxy());
  aStr = (OSS() << cobwebWidget->getProxy() << pvSpreadSheetWidget->getProxy()).str();
  linksModel->addSelectionLink(aStr.c_str(), cobwebWidget->getProxy(), pvSpreadSheetWidget->getProxy());
  aStr = (OSS() << sampleScatterPlotWidget->getProxy() << pvSpreadSheetWidget->getProxy()).str();
  linksModel->addSelectionLink(aStr.c_str(), sampleScatterPlotWidget->getProxy(), pvSpreadSheetWidget->getProxy());
  aStr = (OSS() << sampleRankScatterPlotWidget->getProxy() << pvSpreadSheetWidget->getProxy()).str();
  linksModel->addSelectionLink(aStr.c_str(), sampleRankScatterPlotWidget->getProxy(), pvSpreadSheetWidget->getProxy());
}
#endif


void DataAnalysisWindow::updateSpinBoxes(int indexList)
{
  SignalBlocker blocker(quantileSpinBox_);

  // index of the variable in result_
  const UnsignedInteger indexVar = variablesListWidget_->item(indexList)->data(Qt::UserRole).toInt();

  if (result_.getMin().getSize() && result_.getMax().getSize())
  {
    const double min = result_.getMin()[indexVar][0];
    const double max = result_.getMax()[indexVar][0];

    quantileSpinBox_->setMinimum(min);
    quantileSpinBox_->setMaximum(max);
    quantileSpinBox_->setSingleStep((max - min)/100);
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
