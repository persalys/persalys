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
#include "otgui/DesignOfExperimentAnalysis.hxx"
#include "otgui/ExportableTableView.hxx"
#include "otgui/SampleTableModel.hxx"
#include "otgui/WidgetBoundToDockWidget.hxx"
#include "otgui/GraphConfigurationWidget.hxx"
#include "otgui/PlotMatrixConfigurationWidget.hxx"
#include "otgui/QtTools.hxx"

#ifdef OTGUI_HAVE_PARAVIEW
#include "otgui/PVServerManagerInterface.hxx"
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
  : ResultWindow(item, parent)
  , designOfExperiment_()
  , variablesGroupBox_(0)
  , variablesListWidget_(0)
  , tabWidget_(0)
{
  DesignOfExperimentAnalysis * analysis_ptr = dynamic_cast<DesignOfExperimentAnalysis*>(item->getAnalysis().getImplementation().get());
  if (!analysis_ptr)
    throw InvalidArgumentException (HERE) << "The analysis must be a DesignOfExperimentAnalysis";

  designOfExperiment_ = analysis_ptr->getDesignOfExperiment();

  // parameters widget
  setParameters(item->getAnalysis(), tr("Design of experiment evaluation parameters"));

  buildInterface();
}


void DesignOfExperimentWindow::buildInterface()
{
  setWindowTitle(tr("Design of experiment"));

  // main splitter
  QSplitter * mainWidget = new QSplitter(Qt::Horizontal);

  // - list outputs
  variablesGroupBox_ = new QGroupBox(tr("Variables"));
  QVBoxLayout * outputsLayoutGroupBox = new QVBoxLayout(variablesGroupBox_);

  variablesListWidget_ = new OTguiListWidget;
  variablesListWidget_->addItems(QtOT::DescriptionToStringList(designOfExperiment_.getOutputSample().getDescription()));
  outputsLayoutGroupBox->addWidget(variablesListWidget_);

  mainWidget->addWidget(variablesGroupBox_);
  mainWidget->setStretchFactor(0, 1);

  // - tabWidget
  tabWidget_ = new QTabWidget;

  // -- min/max tab --------------------------------
  addMinMaxTab();
  variablesListWidget_->setCurrentRow(0);

  // -- other tabs
#ifdef OTGUI_HAVE_PARAVIEW
  addParaviewWidgetsTabs();
#else
  addTablesTab();
  addPlotMatrixTab();
  addScatterPlotsTab();
#endif
  if (parametersWidget_)
    tabWidget_->addTab(parametersWidget_, tr("Parameters"));

  connect(tabWidget_, SIGNAL(currentChanged(int)), this, SLOT(updateVariablesListVisibility(int)));

  mainWidget->addWidget(tabWidget_);
  mainWidget->setStretchFactor(1, 10);

  setWidget(mainWidget);
}


void DesignOfExperimentWindow::addMinMaxTab()
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
  MinMaxTableGroupBox * minMaxTableGroupBox = new MinMaxTableGroupBox(*dynamic_cast<DataSample*>(designOfExperiment_.getImplementation().get()));
  tabLayout->addWidget(minMaxTableGroupBox);
  connect(variablesListWidget_, SIGNAL(currentRowChanged(int)), minMaxTableGroupBox, SLOT(setCurrentIndexStackedWidget(int)));

  scrollArea->setWidget(tab);
  tabWidget_->addTab(scrollArea, tr("Min/Max"));
}


#ifdef OTGUI_HAVE_PARAVIEW
void DesignOfExperimentWindow::addParaviewWidgetsTabs()
{
  // input names/descriptions
  QStringList inputNames;
  QStringList inAxisTitles;
  // output names/descriptions
  QStringList outputNames;
  QStringList outAxisTitles;

  // get labels
  getScatterPlotLabels(designOfExperiment_.getInputSample(), inputNames, inAxisTitles, outputNames, outAxisTitles);

  // tables tab ------------------------------------------
  QTabWidget * tablesTabWidget = new QTabWidget;

  // -- doe tab
  PVSpreadSheetViewWidget * spreadSheetWidget = new PVSpreadSheetViewWidget(this, new PVServerManagerInterface);
  spreadSheetWidget->setData(designOfExperiment_.getSample());

  tablesTabWidget->addTab(spreadSheetWidget, tr("DOE"));

  PVXYChartViewWidget * sampleScatterPlotWidget = 0;
  PVXYChartViewWidget * sampleRankScatterPlotWidget = 0;

  const int failedInSampleSize = designOfExperiment_.getFailedInputSample().getSize();
  const int notEvalInSampleSize = designOfExperiment_.getNotEvaluatedInputSample().getSize();

  if (failedInSampleSize || notEvalInSampleSize)
  {
    // allInputsSample : input sample + failed input sample + not evaluated input sample
    Sample allInputsSample(designOfExperiment_.getInputSample());
    const UnsignedInteger inSampleSize = designOfExperiment_.getInputSample().getSize();

    if (failedInSampleSize)
      allInputsSample.add(designOfExperiment_.getFailedInputSample());
    if (notEvalInSampleSize)
      allInputsSample.add(designOfExperiment_.getNotEvaluatedInputSample());

    // allInputsSample rank
    const Sample allInputsSampleRank(allInputsSample.rank() / allInputsSample.getSize());

    // --- data for scatter plots tab
    // sample scatter plot
    sampleScatterPlotWidget = new PVXYChartViewWidget(this, new PVServerManagerInterface);
    sampleScatterPlotWidget->setData(designOfExperiment_.getInputSample(), Qt::green);

    // sample rank scatter plot
    const Sample inputsRank(allInputsSampleRank, 0, inSampleSize);
    sampleRankScatterPlotWidget = new PVXYChartViewWidget(this, new PVServerManagerInterface);
    sampleRankScatterPlotWidget->setData(inputsRank, Qt::green);

    // labels for the legend
    std::list<QString> inSLabelsList(designOfExperiment_.getInputSample().getDimension(), tr("Succeed points"));
    sampleScatterPlotWidget->setRepresentationLabels(QList<QString>::fromStdList(inSLabelsList), 0);
    sampleRankScatterPlotWidget->setRepresentationLabels(QList<QString>::fromStdList(inSLabelsList), 0);

    // -- failed points tab
    if (failedInSampleSize)
    {
      // --- table tab
      PVSpreadSheetViewWidget * failedPointsTable = new PVSpreadSheetViewWidget(this, new PVServerManagerInterface);
      failedPointsTable->setData(designOfExperiment_.getFailedInputSample());
      tablesTabWidget->addTab(failedPointsTable, tr("Failed points"));

      // --- data for scatter plots tab
      // sample scatter plot
      sampleScatterPlotWidget->setData(designOfExperiment_.getFailedInputSample(), Qt::red);

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
      PVSpreadSheetViewWidget * notEvaluatedPointsTable = new PVSpreadSheetViewWidget(this, new PVServerManagerInterface);
      notEvaluatedPointsTable->setData(designOfExperiment_.getNotEvaluatedInputSample());
      tablesTabWidget->addTab(notEvaluatedPointsTable, tr("Not evaluated points"));

      // --- data for scatter plots tab
      // sample scatter plot
      sampleScatterPlotWidget->setData(designOfExperiment_.getNotEvaluatedInputSample(), Qt::blue);
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
    sampleScatterPlotWidget->setAxisTitles(inputNames, inAxisTitles);
    if (inputNames != inAxisTitles)
      sampleRankScatterPlotWidget->setAxisTitles(inputNames, inAxisTitles);
    scatterTabWidgetLayout->addWidget(sampleScatterPlotWidget);
    scatterTabWidgetLayout->addWidget(sampleRankScatterPlotWidget);
    tablesTabWidget->addTab(scatterTabWidget, tr("Scatter plots"));

    // setting widget
    PVXYChartSettingWidget * inSampleSettingWidget = new PVXYChartSettingWidget(sampleScatterPlotWidget,
                                                                                sampleRankScatterPlotWidget,
                                                                                inputNames,
                                                                                QStringList(),
                                                                                PVXYChartSettingWidget::Scatter,
                                                                                this);
    scatterTabWidget->setDockWidget(inSampleSettingWidget);
  }

  tabWidget_->addTab(tablesTabWidget, tr("Table"));

  // cobweb tab ------------------------------------------
  WidgetBoundToDockWidget * cobwebTabWidget = new WidgetBoundToDockWidget(this);
  QVBoxLayout * cobwebTabWidgetLayout = new QVBoxLayout(cobwebTabWidget);

  PVParCooViewWidget * cobwebWidget = new PVParCooViewWidget(this, new PVServerManagerInterface);
  cobwebWidget->setData(designOfExperiment_.getSample());
  cobwebTabWidgetLayout->addWidget(cobwebWidget);

  PVPlotSettingWidget * cobwebSettingWidget = new PVPlotSettingWidget(cobwebWidget, this);
  cobwebTabWidget->setDockWidget(cobwebSettingWidget);

  tabWidget_->addTab(cobwebTabWidget, tr("Cobweb plot"));

  // plot matrix tab -------------------------------------
  WidgetBoundToDockWidget * matrixTabWidget = new WidgetBoundToDockWidget(this);
  QVBoxLayout * matrixTabWidgetLayout = new QVBoxLayout(matrixTabWidget);

  PVMatrixPlotViewWidget * pvmatrixWidget = new PVMatrixPlotViewWidget(this, new PVServerManagerInterface);
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
  PVXYChartViewWidget * pvXYChartWidget = new PVXYChartViewWidget(this, new PVServerManagerInterface);
  pvXYChartWidget->PVViewWidget::setData(designOfExperiment_.getSample());
  if ((inputNames + outputNames) != (inAxisTitles + outAxisTitles))
    pvXYChartWidget->setAxisTitles(inputNames + outputNames, inAxisTitles + outAxisTitles);
  xyScatterTabWidgetLayout->addWidget(pvXYChartWidget);

  // sample rank
  const Sample sampleRank(designOfExperiment_.getSample().rank() / designOfExperiment_.getSample().getSize());
  PVXYChartViewWidget * rankPvXYChartWidget = new PVXYChartViewWidget(this, new PVServerManagerInterface);
  rankPvXYChartWidget->PVViewWidget::setData(sampleRank);
  if ((inputNames + outputNames) != (inAxisTitles + outAxisTitles))
    rankPvXYChartWidget->setAxisTitles(inputNames + outputNames, inAxisTitles + outAxisTitles);
  xyScatterTabWidgetLayout->addWidget(rankPvXYChartWidget);

  tabWidget_->addTab(xyScatterTabWidget, tr("Scatter plots"));

  // setting widget
  PVXYChartSettingWidget * inOutSampleSettingWidget = new PVXYChartSettingWidget(pvXYChartWidget,
                                                                                 rankPvXYChartWidget,
                                                                                 inputNames,
                                                                                 outputNames,
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


void DesignOfExperimentWindow::addTablesTab()
{
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
  if (designOfExperiment_.getFailedInputSample().getSize())
  {
    tableView = new ExportableTableView;
    tableView->setSortingEnabled(true);
    tableModel = new SampleTableModel(designOfExperiment_.getFailedInputSample(), tableView);
    proxyModel = new QSortFilterProxyModel(tableView);
    proxyModel->setSourceModel(tableModel);
    tableView->setModel(proxyModel);
    tableView->sortByColumn(0, Qt::AscendingOrder);

    tablesTabWidget->addTab(tableView, tr("Failed points"));
  }
  // tab with not evaluated points
  if (designOfExperiment_.getNotEvaluatedInputSample().getSize())
  {
    tableView = new ExportableTableView;
    tableView->setSortingEnabled(true);
    tableModel = new SampleTableModel(designOfExperiment_.getNotEvaluatedInputSample(), tableView);
    proxyModel = new QSortFilterProxyModel(tableView);
    proxyModel->setSourceModel(tableModel);
    tableView->setModel(proxyModel);
    tableView->sortByColumn(0, Qt::AscendingOrder);

    tablesTabWidget->addTab(tableView, tr("Not evaluated points"));
  }

  tabWidget_->addTab(tablesTabWidget, tr("Table"));
}


void DesignOfExperimentWindow::addPlotMatrixTab()
{
  WidgetBoundToDockWidget * matrixTabWidget = new WidgetBoundToDockWidget(this);
  QVBoxLayout * matrixTabWidgetLayout = new QVBoxLayout(matrixTabWidget);

  // - plot matrix X-X --------------------------------
  PlotMatrixWidget * plotMatrix = new PlotMatrixWidget(designOfExperiment_.getSample(), designOfExperiment_.getSample());
  plotMatrix->setInputNames(QtOT::DescriptionToStringList(designOfExperiment_.getInputSample().getDescription()));
  plotMatrix->setOutputNames(QtOT::DescriptionToStringList(designOfExperiment_.getOutputSample().getDescription()));

  PlotMatrixConfigurationWidget * plotMatrixSettingWidget = new PlotMatrixConfigurationWidget(plotMatrix, this);
  matrixTabWidget->setDockWidget(plotMatrixSettingWidget);

  matrixTabWidgetLayout->addWidget(plotMatrix);

  tabWidget_->addTab(matrixTabWidget, tr("Plot matrix"));
}


void DesignOfExperimentWindow::getScatterPlotLabels(const Sample& inS,
                                                    QStringList& inNames,
                                                    QStringList& inAxisNames,
                                                    QStringList& outNames,
                                                    QStringList& outAxisNames)
{
  // 1 - input sample
  // input names/descriptions
  for (UnsignedInteger i = 0; i < inS.getDimension(); ++i)
  {
    const String inputName = inS.getDescription()[i];
    inNames << QString::fromUtf8(inputName.c_str());

    QString inputDescription;
    try
    {
      inputDescription = QString::fromUtf8(designOfExperiment_.getPhysicalModel().getInputByName(inputName).getDescription().c_str());
    }
    catch (std::exception) // maybe the variable does not exist anymore
    {
      //do nothing
    }
    if (!inputDescription.isEmpty())
      inAxisNames << inputDescription;
    else
      inAxisNames << inNames.last();
  }

  // 2 - output sample
  const Sample outS(designOfExperiment_.getOutputSample());
  // output names/descriptions
  for (UnsignedInteger i = 0; i < outS.getDimension(); ++i)
  {
    const String outputName = outS.getDescription()[i];
    outNames << QString::fromUtf8(outputName.c_str());
    QString outputDescription;
    try
    {
      outputDescription = QString::fromUtf8(designOfExperiment_.getPhysicalModel().getOutputByName(outputName).getDescription().c_str());
    }
    catch (std::exception) // maybe the variable does not exist anymore
    {
      //do nothing
    }
    if (!outputDescription.isEmpty())
      outAxisNames << outputDescription;
    else
      outAxisNames << outNames.last();
  }
}


void DesignOfExperimentWindow::addScatterPlotsTab()
{
  // 1 - get variable inputs indices
  const UnsignedInteger nbInputs = designOfExperiment_.getVariableInputNames().getSize();
  Indices ind(nbInputs);
  for (UnsignedInteger i = 0; i < nbInputs; ++i)
  {
    for (UnsignedInteger j = 0; j < designOfExperiment_.getInputSample().getDimension(); ++j)
    {
      if (designOfExperiment_.getVariableInputNames()[i] == designOfExperiment_.getInputSample().getDescription()[j])
      {
        ind[i] = j;
        break;
      }
    }
  }

  // 2 - input sample
  const Sample inS(designOfExperiment_.getInputSample().getMarginal(ind));
  // input names/descriptions
  QStringList inputNames;
  QStringList inAxisTitles;
 
  // 3 - output sample
  const Sample outS(designOfExperiment_.getOutputSample());
  // output names/descriptions
  QStringList outputNames;
  QStringList outAxisTitles;

  // get labels
  getScatterPlotLabels(inS, inputNames, inAxisTitles, outputNames, outAxisTitles);

  // 4 - scatter plots
  WidgetBoundToDockWidget * scatterWidget = new WidgetBoundToDockWidget;
  QVBoxLayout * scatterWidgetLayout = new QVBoxLayout(scatterWidget);

  ResizableStackedWidget * stackedWidget = new ResizableStackedWidget;
  QVector<PlotWidget*> listScatterPlotWidgets = GetListScatterPlots(inS,
                                                                    outS,
                                                                    designOfExperiment_.getFailedInputSample(),
                                                                    inputNames,
                                                                    inAxisTitles,
                                                                    outputNames,
                                                                    outAxisTitles);
  for (int i = 0; i < listScatterPlotWidgets.size(); ++i)
    stackedWidget->addWidget(listScatterPlotWidgets[i]);

  GraphConfigurationWidget * scatterPlotsSettingWidget = new GraphConfigurationWidget(listScatterPlotWidgets,
                                                                                      inputNames,
                                                                                      outputNames,
                                                                                      GraphConfigurationWidget::Scatter,
                                                                                      this);
  connect(scatterPlotsSettingWidget, SIGNAL(currentPlotChanged(int)), stackedWidget, SLOT(setCurrentIndex(int)));
  scatterWidget->setDockWidget(scatterPlotsSettingWidget);
  scatterWidgetLayout->addWidget(stackedWidget);

  tabWidget_->addTab(scatterWidget, tr("Scatter plots"));
}


QVector<PlotWidget*> DesignOfExperimentWindow::GetListScatterPlots(const Sample& inS,
                                                                   const Sample& outS,
                                                                   const Sample& notValidInS,
                                                                   const QStringList inNames,
                                                                   const QStringList inAxisNames,
                                                                   const QStringList outNames,
                                                                   const QStringList outAxisNames
                                                                  )
{
  QVector<PlotWidget*> listScatterPlotWidgets;

  const UnsignedInteger nbInputs = inS.getSize() ? inS.getDimension() : 0;
  const UnsignedInteger nbOutputs = outS.getSize() ? outS.getDimension() : 0;
  const QPen pen(Qt::blue, 4);
  const QPen notValidPen(Qt::red, 4);

  // in rank
  Sample inSrank;
  if (nbInputs)
    inSrank = inS.rank() / inS.getSize();
  Sample notValidInSrank;
  if (notValidInS.getSize())
    notValidInSrank = notValidInS.rank() / notValidInS.getSize();
  // out rank
  Sample outSrank;
  if (nbOutputs)
    outSrank = outS.rank() / outS.getSize();

  for (UnsignedInteger j = 0; j < nbInputs; ++j)
  {
    for (UnsignedInteger i = 0; i < nbOutputs; ++i)
    {
      PlotWidget * plot = new PlotWidget(tr("scatterplot"));
      plot->plotScatter(inS.getMarginal(j), outS.getMarginal(i), pen, inAxisNames[j], outAxisNames[i]);
      plot->setTitle(tr("Scatter plot:") + " " + outNames[i] + " " + tr("vs") + " " + inNames[j]);
      listScatterPlotWidgets.append(plot);

      // ranks
      plot = new PlotWidget(tr("scatterplot"));
      plot->plotScatter(inSrank.getMarginal(j), outSrank.getMarginal(i), pen, inAxisNames[j], outAxisNames[i]);
      plot->setTitle(tr("Scatter plot:") + " " + outNames[i] + " " + tr("vs") + " " + inNames[j]);
      listScatterPlotWidgets.append(plot);
    }
    for (UnsignedInteger i = 0; i < nbInputs; ++i)
    {
      if (i != j)
      {
        PlotWidget * plot = new PlotWidget(tr("scatterplot"));
        plot->plotScatter(inS.getMarginal(j), inS.getMarginal(i), pen, inAxisNames[j], inAxisNames[i]);
        if (notValidInS.getSize())
          plot->plotScatter(notValidInS.getMarginal(j), notValidInS.getMarginal(i), notValidPen, inAxisNames[j], inAxisNames[i]);
        plot->setTitle(tr("Scatter plot:") + " " + inNames[i] + " " + tr("vs") + " " + inNames[j]);
        listScatterPlotWidgets.append(plot);

        // ranks
        plot = new PlotWidget(tr("scatterplot"));
        plot->plotScatter(inSrank.getMarginal(j), inSrank.getMarginal(i), pen, inAxisNames[j], inAxisNames[i]);
        if (notValidInS.getSize())
          plot->plotScatter(notValidInSrank.getMarginal(j), notValidInSrank.getMarginal(i), notValidPen, inAxisNames[j], inAxisNames[i]);
        plot->setTitle(tr("Scatter plot:") + " " + inNames[i] + " " + tr("vs") + " " + inNames[j]);
        listScatterPlotWidgets.append(plot);
      }
    }
  }
  for (UnsignedInteger j = 0; j < nbOutputs; ++j)
  {
    for (UnsignedInteger i = 0; i < nbOutputs; ++i)
    {
      if (i != j)
      {
        PlotWidget * plot = new PlotWidget(tr("scatterplot"));
        plot->plotScatter(outS.getMarginal(j), outS.getMarginal(i), pen, outAxisNames[j], outAxisNames[i]);
        plot->setTitle(tr("Scatter plot:") + " " + outNames[i] + " " + tr("vs") + " " + outNames[j]);
        listScatterPlotWidgets.append(plot);

        // ranks
        plot = new PlotWidget(tr("scatterplot"));
        plot->plotScatter(outSrank.getMarginal(j), outSrank.getMarginal(i), pen, outAxisNames[j], outAxisNames[i]);
        plot->setTitle(tr("Scatter plot:") + " " + outNames[i] + " " + tr("vs") + " " + outNames[j]);
        listScatterPlotWidgets.append(plot);
      }
    }
    for (UnsignedInteger i = 0; i < nbInputs; ++i)
    {
      PlotWidget * plot = new PlotWidget(tr("scatterplot"));
      plot->plotScatter(outS.getMarginal(j), inS.getMarginal(i), pen, outAxisNames[j], inAxisNames[i]);
      plot->setTitle(tr("Scatter plot:") + " " + inNames[i] + " " + tr("vs") + " " + outNames[j]);
      listScatterPlotWidgets.append(plot);

      // ranks
      plot = new PlotWidget(tr("scatterplot"));
      plot->plotScatter(outSrank.getMarginal(j), inSrank.getMarginal(i), pen, outAxisNames[j], inAxisNames[i]);
      plot->setTitle(tr("Scatter plot:") + " " + inNames[i] + " " + tr("vs") + " " + outNames[j]);
      listScatterPlotWidgets.append(plot);
    }
  }
  return listScatterPlotWidgets;
}


void DesignOfExperimentWindow::updateVariablesListVisibility(int indexTab)
{
  // if indextab == summary : variablesGroupBox_ is visible
  variablesGroupBox_->setVisible(indexTab == 0);
}
}
