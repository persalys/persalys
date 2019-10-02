//                                               -*- C++ -*-
/**
 *  @brief QMdiSubWindow for the analysis of data
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
#include "persalys/DataAnalysisWindow.hxx"

#include "persalys/QtTools.hxx"
#include "persalys/DesignOfExperimentWindow.hxx"
#include "persalys/MinMaxTableGroupBox.hxx"
#include "persalys/MomentsEstimatesTableGroupBox.hxx"
#include "persalys/ParametersTableView.hxx"
#include "persalys/ExportableTableView.hxx"
#include "persalys/SampleTableModel.hxx"
#include "persalys/WidgetBoundToDockWidget.hxx"
#include "persalys/GraphConfigurationWidget.hxx"
#include "persalys/PlotMatrixConfigurationWidget.hxx"
#include "persalys/TemporaryLabel.hxx"
#include "persalys/BoxPlot.hxx"

#ifdef PERSALYS_HAVE_PARAVIEW
#include "persalys/PVServerManagerInterface.hxx"
#include "persalys/PVServerManagerSingleton.hxx"
#include "persalys/PVParCooViewWidget.hxx"
#include "persalys/PVMatrixPlotViewWidget.hxx"
#include "persalys/PVPlotSettingWidget.hxx"
#include "persalys/PVXYChartViewWidget.hxx"
#include "persalys/PVXYChartSettingWidget.hxx"

#include <pqLinksModel.h>
#include <pqApplicationCore.h>
#endif

#include <openturns/Normal.hxx>

#include <QVBoxLayout>
#include <QScrollArea>
#include <QHeaderView>
#include <QSplitter>
#include <QSortFilterProxyModel>
#include <QPushButton>

using namespace OT;

namespace PERSALYS
{

DataAnalysisWindow::DataAnalysisWindow(Item * item, QWidget * parent)
  : ResultWindow(item, parent)
  , designOfExperiment_()
  , result_()
  , hasMaximumCV_(false)
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
{
}


void DataAnalysisWindow::initializeVariablesNames()
{
  PhysicalModel model(designOfExperiment_.getPhysicalModel());

  // inputs
  if (designOfExperiment_.getInputSample().getSize())
  {
    inputNames_ = QtOT::DescriptionToStringList(designOfExperiment_.getInputSample().getDescription());
    inAxisTitles_ = QtOT::GetVariableAxisLabels(model, designOfExperiment_.getInputSample().getDescription());
  }
  // outputs
  if (designOfExperiment_.getOutputSample().getSize())
  {
    outputNames_ = QtOT::DescriptionToStringList(designOfExperiment_.getOutputSample().getDescription());
    outAxisTitles_ = QtOT::GetVariableAxisLabels(model, designOfExperiment_.getOutputSample().getDescription());
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
    // tab: correlation
    addDependenceTab();
  }
  bool canUseParaview = false;
#ifdef PERSALYS_HAVE_PARAVIEW
  if (SubWindow::SupportsOpenGL_3_2())
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
  QGridLayout * tabLayout = new QGridLayout(tab);

  // -- results --
  int row = 0;

  // Stopping criteria
  const QString groupBoxTitle = (analysisStopCriteriaMessage_.isEmpty()) ? tr("") : tr("Stopping criteria");
  QGroupBox * parametersGroupBox = new QGroupBox(groupBoxTitle);
  QVBoxLayout * parametersGroupBoxLayout = new QVBoxLayout(parametersGroupBox);

  // - if algo with a Stopping criteria
  if (!analysisStopCriteriaMessage_.isEmpty())
  {
    QLabel * stopCriteriaLabel = new QLabel(analysisStopCriteriaMessage_);
    parametersGroupBoxLayout->addWidget(stopCriteriaLabel);
  }
  // - if algo with an error message
  if (!analysisErrorMessage_.isEmpty())
  {
    TemporaryLabel * analysisErrorMessageLabel = new TemporaryLabel;
    analysisErrorMessageLabel->setErrorMessage(analysisErrorMessage_);
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
  // - coef of variation
  if (hasMaximumCV_)
  {
    namesList << tr("Sample mean CV");
    const UnsignedInteger nbInputs =  designOfExperiment_.getInputSample().getDimension();
    const Scalar sqrtSampleSize = sqrt(designOfExperiment_.getInputSample().getSize());
    Scalar maxCoefOfVariation = 0.;
    for (UnsignedInteger i = nbInputs; i < result_.getCoefficientOfVariation().getSize(); ++i)
      if (result_.getCoefficientOfVariation()[i].getSize() == 1)
        maxCoefOfVariation = std::max(result_.getCoefficientOfVariation()[i][0] / sqrtSampleSize, maxCoefOfVariation);
    valuesList << QString::number(maxCoefOfVariation);
  }

  ParametersTableView * table = new ParametersTableView(namesList, valuesList, true, true);
  parametersGroupBoxLayout->addWidget(table);
  tabLayout->addWidget(parametersGroupBox, row, 0);

  // moments estimation
  if (result_.getMean().getSize())
  {
    // we want to display output results before the input results
    // input indices
    Indices inInd(inputNames_.size());
    inInd.fill();
    // output indices
    Indices ind(outputNames_.size());
    ind.fill(inputNames_.size());
    // indices with good order
    ind.add(inInd);

    MomentsEstimatesTableGroupBox * estimatesGroupBox = new MomentsEstimatesTableGroupBox(result_,
        isConfidenceIntervalRequired_,
        levelConfidenceInterval_,
        ind);

    tabLayout->addWidget(estimatesGroupBox, ++row, 0);
    connect(variablesListWidget_, SIGNAL(currentRowChanged(int)), estimatesGroupBox, SLOT(setCurrentIndexStackedWidget(int)));
  }

  // min/max table
  MinMaxTableGroupBox * minMaxTableGroupBox = new MinMaxTableGroupBox(designOfExperiment_, false);
  tabLayout->addWidget(minMaxTableGroupBox, ++row, 0);
  connect(variablesListWidget_, SIGNAL(currentRowChanged(int)), minMaxTableGroupBox, SLOT(setCurrentIndexStackedWidget(int)));

  tabLayout->setRowStretch(++row, 1);
  scrollArea->setWidget(tab);
  tabWidget_->addTab(scrollArea, tr("Summary"));
}


void DataAnalysisWindow::addPDF_CDFTab()
{
  if (!result_.getPDF().getSize() || !result_.getCDF().getSize())
    return;

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
    PDFGraphSetting * graphSetting = new PDFGraphSetting(listPlotWidgets, PDFGraphSetting::Result, this);
    connect(graphSetting, SIGNAL(currentPlotChanged(int)), stackedWidget, SLOT(setCurrentIndex(int)));

    tabStackedWidget->addWidget(new WidgetBoundToDockWidget(stackedWidget, graphSetting, this));
  }

  scrollArea->setWidget(tabStackedWidget);
  tabWidget_->addTab(scrollArea, tr("PDF/CDF"));
}


void DataAnalysisWindow::addBoxPlotTab()
{
  if (!result_.getMedian().getSize() || !result_.getFirstQuartile().getSize() ||
      !result_.getThirdQuartile().getSize() || !result_.getOutliers().getSize())
    return;

  QScrollArea * scrollArea = new QScrollArea;
  scrollArea->setWidgetResizable(true);
  QWidget * mainWidget = new QWidget;
  QVBoxLayout * mainLayout = new QVBoxLayout(mainWidget);

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

  BoxPlot * plot = new BoxPlot(variablesNames);

  for (int i = 0; i < variablesNames.size(); ++i)
    plot->addBoxPlot(result_, ind[i]);

  // Graph Setting
  BoxPlotGraphSetting * graphSetting = new BoxPlotGraphSetting(plot, variablesNames, this);
  mainLayout->addWidget(new WidgetBoundToDockWidget(plot, graphSetting, this));

  scrollArea->setWidget(mainWidget);
  tabWidget_->addTab(scrollArea, tr("Box plots"));
}


void DataAnalysisWindow::addDependenceTab()
{
  const CorrelationMatrix C(designOfExperiment_.getSample().computeSpearmanCorrelation());
  const UnsignedInteger dim = designOfExperiment_.getSample().getDimension();

  // consider only significantly non-zero correlations
  const double alpha = 0.05;
  const double epsilon = Normal().computeQuantile(1 - alpha)[0] / std::sqrt(designOfExperiment_.getSample().getSize() - 1);

  QWidget * mainWidget = new QWidget;
  QVBoxLayout * mainLayout = new QVBoxLayout(mainWidget);

  QGroupBox * gpBox = new QGroupBox(tr("Spearman's matrix estimate"));
  QHBoxLayout * gpBoxLayout = new QHBoxLayout(gpBox);

  // table widget
  ExportableTableView * tableView = new ExportableTableView;
  tableView->setExportableAsImage(true);

  CustomStandardItemModel * tableModel = new CustomStandardItemModel(dim, dim, tableView);

  for (UnsignedInteger i = 0; i < dim; ++i)
  {
    for (UnsignedInteger j = 0; j < dim; ++j)
    {
      QStandardItem * item = new QStandardItem;
      item->setData(C(i, j), Qt::UserRole + 10);
      item->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
      item->setFlags(item->flags() & ~Qt::ItemIsEditable);

      if (i != j)
      {
        item->setText(QString::number(C(i, j), 'g', 3));
        // set Background color
        if (std::abs(C(i, j)) > epsilon)
        {
          if (C(i, j) < -0.7)
            item->setBackground(QBrush("#7caef4"));  //dark blue
          else if (C(i, j) >= -0.7 && C(i, j) < -0.3)
            item->setBackground(QBrush("#b0cef8"));  //blue
          else if (C(i, j) >= -0.3 && C(i, j) < 0.)
            item->setBackground(QBrush("#e4eefc"));  //light blue
          else if (C(i, j) > 0. && C(i, j) <= 0.3)
            item->setBackground(QBrush("#fadec3"));  //light orange
          else if (C(i, j) > 0.3 && C(i, j) <= 0.7)
            item->setBackground(QBrush("#f4b87c"));  //orange
          else if (C(i, j) > 0.7)
            item->setBackground(QBrush("#ee9235"));  //dark orange
        }
      }
      else
      {
        item->setText("1.");
        item->setBackground(Qt::black);
      }
      tableModel->setItem(i, j, item);
    }
  }
  QStringList headers(QtOT::DescriptionToStringList(designOfExperiment_.getSample().getDescription()));
  tableModel->setHorizontalHeaderLabels(headers);
  tableModel->setVerticalHeaderLabels(headers);

  tableView->setModel(tableModel);
  tableView->resizeColumnsToContents();
  gpBoxLayout->addWidget(tableView);

  // color bar
  QStringList labels;
  labels << "ρ > 0.7"
         << "0.3 < ρ ≤ 0.7"
         << "ε < ρ ≤ 0.3"
         << "-ε ≤ ρ ≤ ε"
         << "-0.3 ≤ ρ < -ε"
         << "-0.7 ≤ ρ < -0.3"
         << "ρ < -0.7";
  QStringList colors;
  colors << "#ee9235" << "#f4b87c" << "#fadec3" << "#ffffff" << "#e4eefc" << "#b0cef8" << "#7caef4";

  QTableView * colorTable = new QTableView;
  colorTable->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  colorTable->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  colorTable->setFocusPolicy(Qt::NoFocus);
  colorTable->setSelectionMode(QAbstractItemView::NoSelection);
  colorTable->setShowGrid(false);

  QStandardItemModel * colorTableModel = new QStandardItemModel(labels.size(), 1);
  for (int i = 0; i < labels.size(); ++i)
  {
    QPixmap px(20, 20);
    px.fill(colors[i]);
    colorTableModel->setItem(i, 0, new QStandardItem(px, labels[i]));
  }
  colorTableModel->setHorizontalHeaderLabels(QStringList() << tr("Spearman's coefficient"));
  colorTable->setModel(colorTableModel);

  colorTable->resizeColumnsToContents();
  colorTable->verticalHeader()->resizeSections(QHeaderView::ResizeToContents);
  const int w = colorTable->horizontalHeader()->length();
  const int h = colorTable->verticalHeader()->length() + colorTable->horizontalHeader()->height();
  int x1, y1, x2, y2;
  colorTable->getContentsMargins(&x1, &y1, &x2, &y2);
  colorTable->setFixedSize(w + x1 + x2, h + y1 + y2);
  colorTable->verticalHeader()->hide();

  gpBoxLayout->addWidget(colorTable, 0, Qt::AlignTop);

  mainLayout->addWidget(gpBox);

  tabWidget_->addTab(mainWidget, tr("Dependence"));
}


void DataAnalysisWindow::addPlotMatrixTab()
{
  PlotMatrixWidget * plotMatrixWidget = new PlotMatrixWidget(designOfExperiment_.getSample(), designOfExperiment_.getSample());
  plotMatrixWidget->setInputNames(inputNames_);
  plotMatrixWidget->setOutputNames(outputNames_);

  PlotMatrixConfigurationWidget * plotMatrixSettingWidget = new PlotMatrixConfigurationWidget(plotMatrixWidget, this);

  tabWidget_->addTab(new WidgetBoundToDockWidget(plotMatrixWidget, plotMatrixSettingWidget, this), tr("Plot matrix"));
}


void DataAnalysisWindow::addScatterPlotsTab()
{
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

  ScatterGraphSetting * scatterSettingWidget = new ScatterGraphSetting(listScatterPlotWidgets, inputNames_, outputNames_, this);
  connect(scatterSettingWidget, SIGNAL(currentPlotChanged(int)), stackedWidget, SLOT(setCurrentIndex(int)));

  tabWidget_->addTab(new WidgetBoundToDockWidget(stackedWidget, scatterSettingWidget, this), tr("Scatter plot"));
}


void DataAnalysisWindow::addTableTab()
{
  // if no failed points and no not evaluated points:
  if (!failedInputSample_.getSize() && !notEvaluatedInputSample_.getSize())
  {
    // Well evaluated points table
    tabWidget_->addTab(ExportableTableView::GetSampleTableViewWidget(getItem(), designOfExperiment_.getSample()), tr("Table"));
  }
  else
  {
    QTabWidget * tablesTabWidget = new QTabWidget;

    // tab with well evaluated points
    tablesTabWidget->addTab(ExportableTableView::GetSampleTableViewWidget(getItem(), designOfExperiment_.getSample()), tr("Points"));

    // tab with failed points
    if (failedInputSample_.getSize())
    {
      tablesTabWidget->addTab(ExportableTableView::GetSampleTableViewWidget(getItem(), failedInputSample_), tr("Failed blocks"));
    }
    // tab with not evaluated points
    if (notEvaluatedInputSample_.getSize())
    {
      tablesTabWidget->addTab(ExportableTableView::GetSampleTableViewWidget(getItem(), notEvaluatedInputSample_), tr("Non-evaluated points"));
    }

    tabWidget_->addTab(tablesTabWidget, tr("Table"));
  }
}


#ifdef PERSALYS_HAVE_PARAVIEW
void DataAnalysisWindow::addParaviewWidgetsTabs()
{
  // get data info
  const UnsignedInteger failedInSampleSize = failedInputSample_.getSize();
  const UnsignedInteger notEvalInSampleSize = notEvaluatedInputSample_.getSize();
  const UnsignedInteger inSampleSize = designOfExperiment_.getInputSample().getSize();
  const UnsignedInteger inSampleDim = designOfExperiment_.getInputSample().getDimension();

  // table tab
  // with paraview the table is always shown in order to use the selection behavior
  PVSpreadSheetViewWidget * pvSpreadSheetWidget = new PVSpreadSheetViewWidget(this, PVServerManagerSingleton::Get());

  // if no failed points and no not evaluated points:
  if (!failedInSampleSize && !notEvalInSampleSize)
  {
    tabWidget_->addTab(PVSpreadSheetViewWidget::GetSpreadSheetViewWidget(pvSpreadSheetWidget, designOfExperiment_.getSample(), getItem()), tr("Table"));
  }
  else
  {
    // Table tab ------------------------------------------
    QTabWidget * tablesTabWidget = new QTabWidget;

    tablesTabWidget->addTab(PVSpreadSheetViewWidget::GetSpreadSheetViewWidget(pvSpreadSheetWidget, designOfExperiment_.getSample(), getItem()), tr("Table"));

    // -- failed points tab
    if (failedInSampleSize)
    {
      PVSpreadSheetViewWidget * failedPointsTable = new PVSpreadSheetViewWidget(this, PVServerManagerSingleton::Get());
      tablesTabWidget->addTab(PVSpreadSheetViewWidget::GetSpreadSheetViewWidget(failedPointsTable, failedInputSample_, getItem()), tr("Failed points"));
    }
    // -- not evaluated points tab
    if (notEvalInSampleSize)
    {
      PVSpreadSheetViewWidget * notEvaluatedPointsTable = new PVSpreadSheetViewWidget(this, PVServerManagerSingleton::Get());
      tablesTabWidget->addTab(PVSpreadSheetViewWidget::GetSpreadSheetViewWidget(notEvaluatedPointsTable, notEvaluatedInputSample_, getItem()), tr("Non-evaluated points"));
    }

    // -- Cobweb plot tab
    if (failedInSampleSize)
    {
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

      // cobweb setting widget
      MultiPlotSettingWidget * cobwebSettingWidget = new MultiPlotSettingWidget(cobwebWidget, succeedAndFailedInS, succeedAndFailedInSRank, this);

      tablesTabWidget->addTab(new WidgetBoundToDockWidget(cobwebWidget, cobwebSettingWidget, this), tr("Cobweb plot"));
    }

    // -- scatter plots tab
    const bool canBuildScatterPlot = (inSampleSize > 1 &&
                                      (failedInSampleSize == 0 || failedInSampleSize > 1) &&
                                      (notEvalInSampleSize == 0 || notEvalInSampleSize > 1));
    Collection<Sample> samples;
    // if one representation has only one point => paraview error => segfault
    if (canBuildScatterPlot)
    {
      // input sample
      samples.add(designOfExperiment_.getInputSample());
      PVXYChartViewWidget * sampleScatterPlotWidget = new PVXYChartViewWidget(this, PVServerManagerSingleton::Get());
      sampleScatterPlotWidget->setData(designOfExperiment_.getInputSample(), Qt::green);
      std::list<QString> inSLabelsList(inSampleDim, tr("Evaluated points"));
      sampleScatterPlotWidget->setRepresentationLabels(QList<QString>::fromStdList(inSLabelsList), 0);
      // failed input sample
      if (failedInSampleSize)
      {
        samples.add(failedInputSample_);
        sampleScatterPlotWidget->setData(failedInputSample_, Qt::red);
        std::list<QString> failedInSLabelsList(inSampleDim, tr("Failed points"));
        sampleScatterPlotWidget->setRepresentationLabels(QList<QString>::fromStdList(failedInSLabelsList), 1);
      }
      // not evaluated points
      if (notEvalInSampleSize)
      {
        samples.add(notEvaluatedInputSample_);
        sampleScatterPlotWidget->setData(notEvaluatedInputSample_, Qt::blue);
        std::list<QString> notEvaluatedInSLabelsList(inSampleDim, tr("Non-evaluated points"));
        sampleScatterPlotWidget->setRepresentationLabels(QList<QString>::fromStdList(notEvaluatedInSLabelsList), failedInSampleSize > 0 ? 2 : 1);
      }
      sampleScatterPlotWidget->setAxisTitles(inputNames_, inAxisTitles_);

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

      Collection<Sample> rankSamples;
      rankSamples.add(Sample(allInputsSampleRank, 0, inSampleSize));
      if (failedInSampleSize)
        rankSamples.add(Sample(allInputsSampleRank, inSampleSize, inSampleSize + failedInSampleSize));
      if (notEvalInSampleSize)
        rankSamples.add(Sample(allInputsSampleRank, inSampleSize + failedInSampleSize, allInputsSampleRank.getSize()));

      // scatter plots setting widget
      ScatterSettingWidget * inSampleSettingWidget = new ScatterSettingWidget(sampleScatterPlotWidget, samples, rankSamples, this);

      tablesTabWidget->addTab(new WidgetBoundToDockWidget(sampleScatterPlotWidget, inSampleSettingWidget, this), tr("Scatter plot"));
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
  PVParCooViewWidget * cobwebWidget = new PVParCooViewWidget(this, PVServerManagerSingleton::Get());
  const Sample sampleRank(designOfExperiment_.getSample().rank() / designOfExperiment_.getSample().getSize());
  cobwebWidget->setData(sampleRank);
  // the variables are automatically sorted : use setAxisToShow with the order of the sample
  cobwebWidget->setAxisToShow(designOfExperiment_.getSample().getDescription());

  // setting widget
  MultiPlotSettingWidget * cobwebSettingWidget = new MultiPlotSettingWidget(cobwebWidget, designOfExperiment_.getSample(), sampleRank, this);

  tabWidget_->addTab(new WidgetBoundToDockWidget(cobwebWidget, cobwebSettingWidget, this), tr("Cobweb plot"));

  // 2- plot matrix tab --------------------------------
  PVMatrixPlotViewWidget * pvmatrixWidget = new PVMatrixPlotViewWidget(this, PVServerManagerSingleton::Get());
  pvmatrixWidget->setData(sampleRank);
  // the variables are automatically sorted : use setAxisToShow with the order of the sample
  pvmatrixWidget->setAxisToShow(designOfExperiment_.getSample().getDescription());

  // setting widget
  MultiPlotSettingWidget * matrixSettingWidget = new MultiPlotSettingWidget(pvmatrixWidget, designOfExperiment_.getSample(), sampleRank, this);

  tabWidget_->addTab(new WidgetBoundToDockWidget(pvmatrixWidget, matrixSettingWidget, this), tr("Plot matrix"));

  // 3- scatter plots tab --------------------------------
  // sample
  PVXYChartViewWidget * sampleScatterPlotWidget = new PVXYChartViewWidget(this, PVServerManagerSingleton::Get());
  sampleScatterPlotWidget->PVViewWidget::setData(designOfExperiment_.getSample());
  if ((inputNames_ + outputNames_) != (inAxisTitles_ + outAxisTitles_))
    sampleScatterPlotWidget->setAxisTitles(inputNames_ + outputNames_, inAxisTitles_ + outAxisTitles_);

  ScatterSettingWidget * scatterSettingWidget = new ScatterSettingWidget(sampleScatterPlotWidget,
      designOfExperiment_.getSample(),
      sampleRank,
      inputNames_,
      outputNames_,
      this);

  tabWidget_->addTab(new WidgetBoundToDockWidget(sampleScatterPlotWidget, scatterSettingWidget, this), tr("Scatter plot"));

  // 4- links model --------------------------------
  pqLinksModel * linksModel = pqApplicationCore::instance()->getLinksModel();

  // There are selection behavior errors if windows use the same links names: a link name must be unique.
  // The pointers are uniques, so we use them to create an unique name...find a better and easier way.
  String aStr = (OSS() << pvSpreadSheet->getProxy() << pvmatrixWidget->getProxy()).str();
  linksModel->addSelectionLink(aStr.c_str(), pvSpreadSheet->getProxy(), pvmatrixWidget->getProxy());
  aStr = (OSS() << cobwebWidget->getProxy() << pvSpreadSheet->getProxy()).str();
  linksModel->addSelectionLink(aStr.c_str(), cobwebWidget->getProxy(), pvSpreadSheet->getProxy());
  aStr = (OSS() << sampleScatterPlotWidget->getProxy() << pvSpreadSheet->getProxy()).str();
  linksModel->addSelectionLink(aStr.c_str(), sampleScatterPlotWidget->getProxy(), pvSpreadSheet->getProxy());
}
#endif


void DataAnalysisWindow::updateVariablesListVisibility(int indexTab)
{
  variablesGroupBox_->setVisible(indexTab == 0 || indexTab == 1);
}
}
