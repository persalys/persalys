//                                               -*- C++ -*-
/**
 *  @brief QMdiSubWindow to display the designs of experiments
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
#include "otgui/DesignOfExperimentWindow.hxx"

#include "otgui/MinMaxTableGroupBox.hxx"
#include "otgui/ParametersTableView.hxx"
#include "otgui/DesignOfExperimentEvaluation.hxx"
#include "otgui/ExportableTableView.hxx"
#include "otgui/SampleTableModel.hxx"
#include "otgui/WidgetBoundToDockWidget.hxx"
#include "otgui/QtTools.hxx"

#ifdef OTGUI_HAVE_PARAVIEW
#include "otgui/PVServerManagerInterface.hxx"
#include "otgui/PVServerManagerSingleton.hxx"
#include "otgui/PVSpreadSheetViewWidget.hxx"
#include "otgui/PVParCooViewWidget.hxx"
#include "otgui/PVMatrixPlotViewWidget.hxx"
#include "otgui/PVXYChartViewWidget.hxx"
#include "otgui/PVXYChartSettingWidget.hxx"
#include "otgui/PVPlotSettingWidget.hxx"

#include <pqLinksModel.h>
#include <pqApplicationCore.h>
#endif

#include <QVBoxLayout>
#include <QScrollArea>
#include <QSplitter>
#include <QSortFilterProxyModel>

using namespace OT;

namespace OTGUI
{

DesignOfExperimentWindow::DesignOfExperimentWindow(AnalysisItem* item, QWidget * parent)
  : DataAnalysisWindow(item, parent)
  , notEvaluatedInputSample_()
{
  initialize(item);

  setWindowTitle(tr("Design of experiments"));

  buildInterface();
}


void DesignOfExperimentWindow::initialize(AnalysisItem* item)
{
  DesignOfExperimentEvaluation analysis(*dynamic_cast<DesignOfExperimentEvaluation*>(item->getAnalysis().getImplementation().get()));

  designOfExperiment_ = analysis.getDesignOfExperiment();
  failedInputSample_ = analysis.getFailedInputSample();
  notEvaluatedInputSample_ = analysis.getNotEvaluatedInputSample();

  // parameters widget
  setParameters(item->getAnalysis(), tr("Design of experiments evaluation parameters"));
}


void DesignOfExperimentWindow::fillListWidget()
{
  variablesGroupBox_->setTitle(tr("Outputs"));
  variablesListWidget_->addItems(QtOT::DescriptionToStringList(designOfExperiment_.getOutputSample().getDescription()));
  variablesListWidget_->setCurrentRow(0);
}


void DesignOfExperimentWindow::fillTabWidget()
{
  // -- min/max tab --------------------------------
  addSummaryTab();

  // -- other tabs
#ifdef OTGUI_HAVE_PARAVIEW
  addParaviewWidgetsTabs();
#else
  addTableTab();
  addPlotMatrixTab();
  addScatterPlotsTab();
#endif
  if (parametersWidget_)
    tabWidget_->addTab(parametersWidget_, tr("Parameters"));

  connect(tabWidget_, SIGNAL(currentChanged(int)), this, SLOT(updateVariablesListVisibility(int)));
}


void DesignOfExperimentWindow::addSummaryTab()
{
  QScrollArea * scrollArea = new QScrollArea;
  scrollArea->setWidgetResizable(true);

  QWidget * tab = new QWidget;
  QVBoxLayout * tabLayout = new QVBoxLayout(tab);
  tabLayout->setSizeConstraint(QLayout::SetFixedSize);

  // number of simulations
  QGroupBox * aGroupBox = new QGroupBox;
  QVBoxLayout * aGroupBoxLayout = new QVBoxLayout(aGroupBox);

  const UnsignedInteger sampleSize = designOfExperiment_.getOutputSample().getSize();
  ParametersTableView * table = new ParametersTableView(QStringList() << tr("Sample size"),
      QStringList() << QString::number(sampleSize),
      true,
      true);
  aGroupBoxLayout->addWidget(table);
  tabLayout->addWidget(aGroupBox);

  // min/max table
  MinMaxTableGroupBox * minMaxTableGroupBox = new MinMaxTableGroupBox(designOfExperiment_);
  tabLayout->addWidget(minMaxTableGroupBox);
  connect(variablesListWidget_, SIGNAL(currentRowChanged(int)), minMaxTableGroupBox, SLOT(setCurrentIndexStackedWidget(int)));

  scrollArea->setWidget(tab);
  tabWidget_->addTab(scrollArea, tr("Min/Max"));
}


#ifdef OTGUI_HAVE_PARAVIEW
void DesignOfExperimentWindow::addParaviewWidgetsTabs()
{
  // get data info
  const UnsignedInteger failedInSampleSize = failedInputSample_.getSize();
  const UnsignedInteger notEvalInSampleSize = notEvaluatedInputSample_.getSize();
  const UnsignedInteger inSampleSize = designOfExperiment_.getInputSample().getSize();
  const UnsignedInteger inSampleDim = designOfExperiment_.getInputSample().getDimension();

  // if no failed points and no not evaluated points:
  // use the DataAnalysisWindow tabs
  if (!failedInSampleSize && !notEvalInSampleSize)
  {
    DataAnalysisWindow::addParaviewWidgetsTabs();
    return;
  }

  // Table tab ------------------------------------------
  QTabWidget * tablesTabWidget = new QTabWidget;

  // -- doe tab
  PVSpreadSheetViewWidget * spreadSheetWidget = new PVSpreadSheetViewWidget(this, PVServerManagerSingleton::Get());
  spreadSheetWidget->setData(designOfExperiment_.getSample());

  tablesTabWidget->addTab(spreadSheetWidget, tr("DOE"));

  // -- failed points tab
  if (failedInSampleSize)
  {
    PVSpreadSheetViewWidget * failedPointsTable = new PVSpreadSheetViewWidget(this, PVServerManagerSingleton::Get());
    failedPointsTable->setData(failedInputSample_);

    tablesTabWidget->addTab(failedPointsTable, tr("Failed points"));
  }
  // -- not evaluated points tab
  if (notEvalInSampleSize)
  {
    PVSpreadSheetViewWidget * notEvaluatedPointsTable = new PVSpreadSheetViewWidget(this, PVServerManagerSingleton::Get());
    notEvaluatedPointsTable->setData(notEvaluatedInputSample_);

    tablesTabWidget->addTab(notEvaluatedPointsTable, tr("Not evaluated points"));
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
    desc.add(tr("Status").toUtf8().constData());
    succeedAndFailedInS.setDescription(desc);

    cobwebWidget->setData(succeedAndFailedInS);
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

  // if only one point : do not need the following graphs
  if (designOfExperiment_.getSample().getSize() < 2)
    return;

  // cobweb tab ------------------------------------------
  WidgetBoundToDockWidget * cobwebTabWidget = new WidgetBoundToDockWidget(this);
  QVBoxLayout * cobwebTabWidgetLayout = new QVBoxLayout(cobwebTabWidget);

  PVParCooViewWidget * cobwebWidget = new PVParCooViewWidget(this, PVServerManagerSingleton::Get());
  cobwebWidget->setData(designOfExperiment_.getSample());
  cobwebWidget->setAxisToShow(designOfExperiment_.getSample().getDescription());
  cobwebTabWidgetLayout->addWidget(cobwebWidget);

  const Sample sampleRank(designOfExperiment_.getSample().rank() / designOfExperiment_.getSample().getSize());
  PVPlotSettingWidget * cobwebSettingWidget = new PVPlotSettingWidget(cobwebWidget,
      designOfExperiment_.getSample(),
      sampleRank,
      this);
  cobwebTabWidget->setDockWidget(cobwebSettingWidget);

  tabWidget_->addTab(cobwebTabWidget, tr("Cobweb plot"));

  // plot matrix tab -------------------------------------
  WidgetBoundToDockWidget * matrixTabWidget = new WidgetBoundToDockWidget(this);
  QVBoxLayout * matrixTabWidgetLayout = new QVBoxLayout(matrixTabWidget);

  PVMatrixPlotViewWidget * pvmatrixWidget = new PVMatrixPlotViewWidget(this, PVServerManagerSingleton::Get());
  pvmatrixWidget->setData(designOfExperiment_.getSample());
  pvmatrixWidget->setAxisToShow(designOfExperiment_.getSample().getDescription());
  matrixTabWidgetLayout->addWidget(pvmatrixWidget);

  tabWidget_->addTab(matrixTabWidget, tr("Plot matrix"));

  // setting widget
  PVPlotSettingWidget * matrixSettingWidget = new PVPlotSettingWidget(pvmatrixWidget, this);
  matrixTabWidget->setDockWidget(matrixSettingWidget);

  // X-Y scatter plots tab --------------------------------

  // build widgets
  WidgetBoundToDockWidget * xyScatterTabWidget = new WidgetBoundToDockWidget(this);
  QVBoxLayout * xyScatterTabWidgetLayout = new QVBoxLayout(xyScatterTabWidget);

  // sample
  PVXYChartViewWidget * pvXYChartWidget = new PVXYChartViewWidget(this, PVServerManagerSingleton::Get());
  pvXYChartWidget->PVViewWidget::setData(designOfExperiment_.getSample());
  if ((inputNames_ + outputNames_) != (inAxisTitles_ + outAxisTitles_))
    pvXYChartWidget->setAxisTitles(inputNames_ + outputNames_, inAxisTitles_ + outAxisTitles_);
  xyScatterTabWidgetLayout->addWidget(pvXYChartWidget);

  tabWidget_->addTab(xyScatterTabWidget, tr("Scatter plots"));

  // setting widget
  PVXYChartSettingWidget * inOutSampleSettingWidget = new PVXYChartSettingWidget(pvXYChartWidget,
      designOfExperiment_.getSample(),
      sampleRank,
      inputNames_,
      outputNames_,
      PVXYChartSettingWidget::Scatter,
      this);
  xyScatterTabWidget->setDockWidget(inOutSampleSettingWidget);

  // links model
  pqLinksModel * linksModel = pqApplicationCore::instance()->getLinksModel();

  // There are selection behavior errors if windows use the same links names: a link name must be unique.
  // The pointers are uniques, so we use them to create an unique name...find a better and easier way.
  String aStr = (OSS() << cobwebWidget->getProxy() << pvmatrixWidget->getProxy()).str();
  linksModel->addSelectionLink(aStr.c_str(), cobwebWidget->getProxy(), pvmatrixWidget->getProxy());
  aStr = (OSS() << spreadSheetWidget->getProxy() << pvmatrixWidget->getProxy()).str();
  linksModel->addSelectionLink(aStr.c_str(), spreadSheetWidget->getProxy(), pvmatrixWidget->getProxy());
  aStr = (OSS() << cobwebWidget->getProxy() << spreadSheetWidget->getProxy()).str();
  linksModel->addSelectionLink(aStr.c_str(), cobwebWidget->getProxy(), spreadSheetWidget->getProxy());
  aStr = (OSS() << pvXYChartWidget->getProxy() << spreadSheetWidget->getProxy()).str();
  linksModel->addSelectionLink(aStr.c_str(), pvXYChartWidget->getProxy(), spreadSheetWidget->getProxy());
}
#endif


void DesignOfExperimentWindow::addTableTab()
{
  if (!failedInputSample_.getSize() && !notEvaluatedInputSample_.getSize())
  {
    DataAnalysisWindow::addTableTab();
    return;
  }

  QTabWidget * tablesTabWidget = new QTabWidget;

  // tab with well evaluated points
  ExportableTableView * tableView = new ExportableTableView;
  tableView->setSortingEnabled(true);

  SampleTableModel * tableModel = new SampleTableModel(designOfExperiment_.getSample(), tableView);
  QSortFilterProxyModel * proxyModel = new QSortFilterProxyModel(tableView);
  proxyModel->setSourceModel(tableModel);
  tableView->setModel(proxyModel);
  tableView->sortByColumn(0, Qt::AscendingOrder);

  tablesTabWidget->addTab(tableView, tr("DOE"));

  // tab with failed points
  if (failedInputSample_.getSize())
  {
    tableView = new ExportableTableView;
    tableView->setSortingEnabled(true);
    tableModel = new SampleTableModel(failedInputSample_, tableView);
    proxyModel = new QSortFilterProxyModel(tableView);
    proxyModel->setSourceModel(tableModel);
    tableView->setModel(proxyModel);
    tableView->sortByColumn(0, Qt::AscendingOrder);

    tablesTabWidget->addTab(tableView, tr("Failed points"));
  }
  // tab with not evaluated points
  if (notEvaluatedInputSample_.getSize())
  {
    tableView = new ExportableTableView;
    tableView->setSortingEnabled(true);
    tableModel = new SampleTableModel(notEvaluatedInputSample_, tableView);
    proxyModel = new QSortFilterProxyModel(tableView);
    proxyModel->setSourceModel(tableModel);
    tableView->setModel(proxyModel);
    tableView->sortByColumn(0, Qt::AscendingOrder);

    tablesTabWidget->addTab(tableView, tr("Not evaluated points"));
  }

  tabWidget_->addTab(tablesTabWidget, tr("Table"));
}


void DesignOfExperimentWindow::updateVariablesListVisibility(int indexTab)
{
  // if indextab == summary : variablesGroupBox_ is visible
  variablesGroupBox_->setVisible(indexTab == 0);
}
}
