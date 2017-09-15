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

namespace OTGUI {

DesignOfExperimentWindow::DesignOfExperimentWindow(AnalysisItem* item, QWidget * parent)
  : DataAnalysisWindow(item, parent)
  , notEvaluatedInputSample_()
{
  initialize(item);

  setWindowTitle(tr("Design of experiment"));

  buildInterface();
}


void DesignOfExperimentWindow::initialize(AnalysisItem* item)
{
  DesignOfExperimentEvaluation analysis(*dynamic_cast<DesignOfExperimentEvaluation*>(item->getAnalysis().getImplementation().get()));

  designOfExperiment_ = analysis.getDesignOfExperiment();
  failedInputSample_ = analysis.getFailedInputSample();
  notEvaluatedInputSample_ = analysis.getNotEvaluatedInputSample();

  // parameters widget
  setParameters(item->getAnalysis(), tr("Design of experiment evaluation parameters"));
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
  if (!failedInputSample_.getSize() && !notEvaluatedInputSample_.getSize())
  {
    DataAnalysisWindow::addParaviewWidgetsTabs();
    return;
  }

  // tables tab ------------------------------------------
  QTabWidget * tablesTabWidget = new QTabWidget;

  // -- doe tab
  PVSpreadSheetViewWidget * spreadSheetWidget = new PVSpreadSheetViewWidget(this, PVServerManagerSingleton::Get());
  spreadSheetWidget->setData(designOfExperiment_.getSample());

  tablesTabWidget->addTab(spreadSheetWidget, tr("DOE"));

  PVXYChartViewWidget * sampleScatterPlotWidget = 0;
  PVXYChartViewWidget * sampleRankScatterPlotWidget = 0;

  const UnsignedInteger failedInSampleSize = failedInputSample_.getSize();
  const UnsignedInteger notEvalInSampleSize = notEvaluatedInputSample_.getSize();

  if (failedInSampleSize || notEvalInSampleSize)
  {
    // allInputsSample : input sample + failed input sample + not evaluated input sample
    Sample allInputsSample(designOfExperiment_.getInputSample());
    const UnsignedInteger inSampleSize = designOfExperiment_.getInputSample().getSize();

    if (failedInSampleSize)
      allInputsSample.add(failedInputSample_);
    if (notEvalInSampleSize)
      allInputsSample.add(notEvaluatedInputSample_);

    // allInputsSample rank
    const Sample allInputsSampleRank(allInputsSample.rank() / allInputsSample.getSize());

    // --- data for scatter plots tab
    // sample scatter plot
    sampleScatterPlotWidget = new PVXYChartViewWidget(this, PVServerManagerSingleton::Get());
    sampleScatterPlotWidget->setData(designOfExperiment_.getInputSample(), Qt::green);

    // sample rank scatter plot
    const Sample inputsRank(allInputsSampleRank, 0, inSampleSize);
    sampleRankScatterPlotWidget = new PVXYChartViewWidget(this, PVServerManagerSingleton::Get());
    sampleRankScatterPlotWidget->setData(inputsRank, Qt::green);

    // labels for the legend
    std::list<QString> inSLabelsList(designOfExperiment_.getInputSample().getDimension(), tr("Succeed points"));
    sampleScatterPlotWidget->setRepresentationLabels(QList<QString>::fromStdList(inSLabelsList), 0);
    sampleRankScatterPlotWidget->setRepresentationLabels(QList<QString>::fromStdList(inSLabelsList), 0);

    // -- failed points tab
    if (failedInSampleSize)
    {
      // --- table tab
      PVSpreadSheetViewWidget * failedPointsTable = new PVSpreadSheetViewWidget(this, PVServerManagerSingleton::Get());
      failedPointsTable->setData(failedInputSample_);

      tablesTabWidget->addTab(failedPointsTable, tr("Failed points"));

      // --- Cobweb tab
      WidgetBoundToDockWidget * cobwebTabWidget = new WidgetBoundToDockWidget(this);
      QVBoxLayout * cobwebTabWidgetLayout = new QVBoxLayout(cobwebTabWidget);

      PVParCooViewWidget * cobwebWidget = new PVParCooViewWidget(this, PVServerManagerSingleton::Get());
      Sample succeedInS(designOfExperiment_.getInputSample());
      succeedInS.stack(Sample(inSampleSize, Point(1, 1.0)));

      Sample failedInS(failedInputSample_);
      failedInS.stack(Sample(failedInSampleSize, 1));

      Sample succeedAndFailedInS(succeedInS);
      succeedAndFailedInS.add(failedInS);
      Description desc(designOfExperiment_.getInputSample().getDescription());
      desc.add(tr("Status").toUtf8().constData());
      succeedAndFailedInS.setDescription(desc);
      cobwebWidget->setData(succeedAndFailedInS);
      cobwebTabWidgetLayout->addWidget(cobwebWidget);

      PVPlotSettingWidget * cobwebSettingWidget = new PVPlotSettingWidget(cobwebWidget, this);
      cobwebTabWidget->setDockWidget(cobwebSettingWidget);

      tablesTabWidget->addTab(cobwebTabWidget, tr("Cobweb plot"));

      // --- data for scatter plots tab
      // sample scatter plot
      sampleScatterPlotWidget->setData(failedInputSample_, Qt::red);

      // sample rank scatter plot
      const Sample failedInputsRank(allInputsSampleRank, inSampleSize, inSampleSize + failedInSampleSize);
      sampleRankScatterPlotWidget->setData(failedInputsRank, Qt::red);

      // labels for the legend
      std::list<QString> failedInSLabelsList(designOfExperiment_.getInputSample().getDimension(), tr("Failed points"));
      sampleScatterPlotWidget->setRepresentationLabels(QList<QString>::fromStdList(failedInSLabelsList), 1);
      sampleRankScatterPlotWidget->setRepresentationLabels(QList<QString>::fromStdList(failedInSLabelsList), 1);
    }
    // -- not evaluated points tab
    if (notEvalInSampleSize)
    {
      // --- table tab
      PVSpreadSheetViewWidget * notEvaluatedPointsTable = new PVSpreadSheetViewWidget(this, PVServerManagerSingleton::Get());
      notEvaluatedPointsTable->setData(notEvaluatedInputSample_);

      tablesTabWidget->addTab(notEvaluatedPointsTable, tr("Not evaluated points"));

      // --- data for scatter plots tab
      // sample scatter plot
      sampleScatterPlotWidget->setData(notEvaluatedInputSample_, Qt::blue);
      // sample rank scatter plot
      const Sample notEvaluatedInputsRank(allInputsSampleRank, inSampleSize + failedInSampleSize, allInputsSampleRank.getSize());
      sampleRankScatterPlotWidget->setData(notEvaluatedInputsRank, Qt::blue);
      // labels for the legend
      std::list<QString> notEvaluatedInSLabelsList(designOfExperiment_.getInputSample().getDimension(), tr("Not evaluated points"));
      sampleScatterPlotWidget->setRepresentationLabels(QList<QString>::fromStdList(notEvaluatedInSLabelsList), failedInSampleSize > 0 ? 2 : 1);
      sampleRankScatterPlotWidget->setRepresentationLabels(QList<QString>::fromStdList(notEvaluatedInSLabelsList), failedInSampleSize > 0 ? 2 : 1);
    }
  }

  // scatter plots tab
  if (sampleScatterPlotWidget)
  {
    // add in tab widget
    WidgetBoundToDockWidget * scatterTabWidget = new WidgetBoundToDockWidget(this);
    QVBoxLayout * scatterTabWidgetLayout = new QVBoxLayout(scatterTabWidget);
    sampleScatterPlotWidget->setAxisTitles(inputNames_, inAxisTitles_);
    if (inputNames_ != inAxisTitles_)
      sampleRankScatterPlotWidget->setAxisTitles(inputNames_, inAxisTitles_);
    scatterTabWidgetLayout->addWidget(sampleScatterPlotWidget);
    scatterTabWidgetLayout->addWidget(sampleRankScatterPlotWidget);
    tablesTabWidget->addTab(scatterTabWidget, tr("Scatter plots"));

    // setting widget
    PVXYChartSettingWidget * inSampleSettingWidget = new PVXYChartSettingWidget(sampleScatterPlotWidget,
                                                                                sampleRankScatterPlotWidget,
                                                                                inputNames_,
                                                                                QStringList(),
                                                                                PVXYChartSettingWidget::Scatter,
                                                                                this);
    scatterTabWidget->setDockWidget(inSampleSettingWidget);
  }

  tabWidget_->addTab(tablesTabWidget, tr("Table"));

  // cobweb tab ------------------------------------------
  WidgetBoundToDockWidget * cobwebTabWidget = new WidgetBoundToDockWidget(this);
  QVBoxLayout * cobwebTabWidgetLayout = new QVBoxLayout(cobwebTabWidget);

  PVParCooViewWidget * cobwebWidget = new PVParCooViewWidget(this, PVServerManagerSingleton::Get());
  cobwebWidget->setData(designOfExperiment_.getSample());
  cobwebTabWidgetLayout->addWidget(cobwebWidget);

  PVPlotSettingWidget * cobwebSettingWidget = new PVPlotSettingWidget(cobwebWidget, this);
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

  // sample rank
  const Sample sampleRank(designOfExperiment_.getSample().rank() / designOfExperiment_.getSample().getSize());
  PVXYChartViewWidget * rankPvXYChartWidget = new PVXYChartViewWidget(this, PVServerManagerSingleton::Get());
  rankPvXYChartWidget->PVViewWidget::setData(sampleRank);
  if ((inputNames_ + outputNames_) != (inAxisTitles_ + outAxisTitles_))
    rankPvXYChartWidget->setAxisTitles(inputNames_ + outputNames_, inAxisTitles_ + outAxisTitles_);
  xyScatterTabWidgetLayout->addWidget(rankPvXYChartWidget);

  tabWidget_->addTab(xyScatterTabWidget, tr("Scatter plots"));

  // setting widget
  PVXYChartSettingWidget * inOutSampleSettingWidget = new PVXYChartSettingWidget(pvXYChartWidget,
                                                                                 rankPvXYChartWidget,
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
  aStr = (OSS() << rankPvXYChartWidget->getProxy() << spreadSheetWidget->getProxy()).str();
  linksModel->addSelectionLink(aStr.c_str(), rankPvXYChartWidget->getProxy(), spreadSheetWidget->getProxy());
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
