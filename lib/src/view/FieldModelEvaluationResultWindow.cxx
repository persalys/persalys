//                                               -*- C++ -*-
/**
 *  @brief QMdiSubWindow for the results of an evaluation of the model
 *
 *  Copyright 2015-2020 EDF-Phimeca
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
#include "persalys/FieldModelEvaluationResultWindow.hxx"

#include "persalys/FieldModelEvaluation.hxx"
#include "persalys/FieldMonteCarloAnalysis.hxx"

#include "persalys/SampleTableModel.hxx"
#include "persalys/StudyTreeViewModel.hxx"

#include "persalys/ExportableTableView.hxx"
#include "persalys/WidgetBoundToDockWidget.hxx"
#include "persalys/QtTools.hxx"
#include "persalys/ContourPlot.hxx"
#include "persalys/ParametersTableView.hxx"
#include "persalys/ResizableStackedWidget.hxx"
#include "persalys/GraphConfigurationWidget.hxx"
#include "persalys/PlotMatrixConfigurationWidget.hxx"

#ifdef PERSALYS_HAVE_PARAVIEW
#include "persalys/PVServerManagerInterface.hxx"
#include "persalys/PVServerManagerSingleton.hxx"
#include "persalys/PVSpreadSheetViewWidget.hxx"
#include "persalys/PVBagChartViewWidget.hxx"
#include "persalys/PVXYChartViewWidget.hxx"
#include "persalys/PVXYChartSettingWidget.hxx"
#include "persalys/PVMatrixPlotViewWidget.hxx"
#include "persalys/PVPlotSettingWidget.hxx"

#include <pqLinksModel.h>
#include <pqApplicationCore.h>
#endif

#include <openturns/KernelSmoothing.hxx>

#include <qwt_legend.h>

#include <QHBoxLayout>
#include <QGroupBox>
#include <QHeaderView>
#include <QSplitter>
#include <QBrush>
#include <QLabel>
#include <QPushButton>

using namespace OT;

namespace PERSALYS
{

FieldModelEvaluationResultWindow::FieldModelEvaluationResultWindow(AnalysisItem * item, QWidget * parent)
  : ResultWindow(item, parent)
  , inputSample_(dynamic_cast<FieldModelEvaluation*>(item->getAnalysis().getImplementation().get())->getResult().getDesignOfExperiment().getInputSample())
  , result_(dynamic_cast<FieldModelEvaluation*>(item->getAnalysis().getImplementation().get())->getProcessSample())
{
  // parameters widget
  setParameters(item->getAnalysis(), tr("Model evaluation parameters"));

  buildInterface();
}


void FieldModelEvaluationResultWindow::buildInterface()
{
  QVBoxLayout * widgetLayout = new QVBoxLayout(this);

  widgetLayout->addWidget(new TitleLabel(tr("Model evaluation"), "user_manual/graphical_interface/field_analysis/user_manual_field_analysis.html#fieldmodelevalresult"));

  // main splitter
  FieldModelEvaluationResultWidget * mainWidget = new FieldModelEvaluationResultWidget(inputSample_, result_, this);

  // tab : PARAMETERS --------------------------------
  if (parametersWidget_)
    mainWidget->getTabWidget()->addTab(parametersWidget_, tr("Parameters"));

  // tab : model description --------------------------------
  if (modelDescriptionWidget_)
    mainWidget->getTabWidget()->addTab(modelDescriptionWidget_, tr("Model"));

  widgetLayout->addWidget(mainWidget, 1);
}


/////////////////
FieldCentralTendencyResultWindow::FieldCentralTendencyResultWindow(AnalysisItem * item, QWidget * parent)
  : ResultWindow(item, parent)
  , inputSample_(dynamic_cast<FieldMonteCarloAnalysis*>(item->getAnalysis().getImplementation().get())->getResult().getDesignOfExperiment().getInputSample())
  , result_(dynamic_cast<FieldMonteCarloAnalysis*>(item->getAnalysis().getImplementation().get())->getResult())
  , errorMessage_(item->getAnalysis().getWarningMessage().c_str())
  , mainWidget_(0)
{
  // parameters widget
  setParameters(item->getAnalysis(), tr("Central tendency parameters"));

  buildInterface();
}


void FieldCentralTendencyResultWindow::buildInterface()
{
  QVBoxLayout * widgetLayout = new QVBoxLayout(this);

  widgetLayout->addWidget(new TitleLabel(tr("Monte Carlo method"), "user_manual/graphical_interface/field_analysis/user_manual_field_analysis.html#fieldmontecarloresult"));

  // main splitter
  mainWidget_ = new FieldModelEvaluationResultWidget(inputSample_,
                                                    result_.getProcessSample(),
                                                    result_.getMeanSample(),
                                                    result_.getLowerQuantileSample(),
                                                    result_.getUpperQuantileSample(),
                                                    this);

  // display Karhunen-Loeve Result
  addDecompositionTab();
  // display correlation
  addCorrelationTab();

  // tab : Error --------------------------------------------
  if (!errorMessage_.isEmpty())
  {
    QWidget * indicesWidget = new QWidget;
    QVBoxLayout * indicesWidgetLayout = new QVBoxLayout(indicesWidget);
    QLabel * errorLabel = new QLabel(tr("An error has occurred during the execution of the analysis") + "\n\n" + errorMessage_);
    indicesWidgetLayout->addWidget(errorLabel);
    indicesWidgetLayout->addStretch();
    mainWidget_->getTabWidget()->addTab(indicesWidget, tr("Error"));
  }

  // tab : PARAMETERS --------------------------------
  if (parametersWidget_)
    mainWidget_->getTabWidget()->addTab(parametersWidget_, tr("Parameters"));

  // tab : model description --------------------------------
  if (modelDescriptionWidget_)
    mainWidget_->getTabWidget()->addTab(modelDescriptionWidget_, tr("Model"));

  widgetLayout->addWidget(mainWidget_, 1);
}


void FieldCentralTendencyResultWindow::addDecompositionTab()
{
  bool canUseParaview = false;
#ifdef PERSALYS_HAVE_PARAVIEW
  if (SubWindow::SupportsOpenGL_3_2())
    canUseParaview = true;
#endif

  // display Karhunen-Loeve Result
  const UnsignedInteger nbOutput = result_.getProcessSample().getDimension();
  if (result_.getKarhunenLoeveResults().getSize() != nbOutput)
    return;
  const QStringList outNames(QtOT::DescriptionToStringList(result_.getProcessSample().getField(0).getValues().getDescription()));
  const QStringList meshParamName = QStringList() << QString::fromUtf8(result_.getProcessSample().getMesh().getDescription()[0].c_str());

  // output tables and graphs
  QTabWidget * klTabWidget = new QTabWidget;

  ResizableStackedWidget * modesStackedWidget = new ResizableStackedWidget;
  klTabWidget->addTab(modesStackedWidget, tr("Modes"));
  connect(mainWidget_->getOutListWidget(), SIGNAL(currentRowChanged(int)), modesStackedWidget, SLOT(setCurrentIndex(int)));

  ResizableStackedWidget * eigStackedWidget = new ResizableStackedWidget;
  klTabWidget->addTab(eigStackedWidget, tr("Eigenvalues"));
  connect(mainWidget_->getOutListWidget(), SIGNAL(currentRowChanged(int)), eigStackedWidget, SLOT(setCurrentIndex(int)));

  QTabWidget * xiTab = 0;
  ResizableStackedWidget * xiPDFStackedWidget = 0;
  ResizableStackedWidget * xiMatrixStackedWidget = 0;
  if (result_.getXiSamples().getSize() == nbOutput)
  {
    xiTab = new QTabWidget;
    klTabWidget->addTab(xiTab, tr("ξi"));

    xiPDFStackedWidget = new ResizableStackedWidget;
    connect(mainWidget_->getOutListWidget(), SIGNAL(currentRowChanged(int)), xiPDFStackedWidget, SLOT(setCurrentIndex(int)));
    xiTab->addTab(xiPDFStackedWidget, tr("PDF"));

    xiMatrixStackedWidget = new ResizableStackedWidget;
    connect(mainWidget_->getOutListWidget(), SIGNAL(currentRowChanged(int)), xiMatrixStackedWidget, SLOT(setCurrentIndex(int)));
    xiTab->addTab(xiMatrixStackedWidget, tr("Plot matrix"));
  }

  // for each output
  for (UnsignedInteger out = 0; out < nbOutput; ++out)
  {
    // get data
    const QStringList currentOutName = QStringList() << outNames[out];
    const UnsignedInteger nbModes = result_.getKarhunenLoeveResults()[out].getEigenValues().getSize();
    ProcessSample scaledModes = result_.getKarhunenLoeveResults()[out].getScaledModesAsProcessSample();

    // compute Cumulative eigen value sum
    Sample cumulEigenSample(nbModes, 2);
    Point sumEigenValues(2, 0);
    sumEigenValues[0] = 1.;
    for (UnsignedInteger i = 0; i < nbModes; ++i)
    {
      cumulEigenSample(i, 0) = i;
      sumEigenValues[1] += result_.getKarhunenLoeveResults()[out].getEigenValues()[i];
      cumulEigenSample(i, 1) = sumEigenValues[1];
    }
    cumulEigenSample /= sumEigenValues;

    // modes widget
    QScrollArea * scrollArea = new QScrollArea;
    scrollArea->setWidgetResizable(true);

    QSplitter * klWidget = new QSplitter;
    scrollArea->setWidget(klWidget);
    eigStackedWidget->addWidget(scrollArea);

    // 1- table
    QWidget * eigenWidget = new QWidget;
    QVBoxLayout * eigenWidgetLayout = new QVBoxLayout(eigenWidget);

    eigenWidgetLayout->addWidget(new QLabel(tr("Number of modes") + " : " + QString::number(nbModes)));

    // table
    CopyableTableView * tableView = new CopyableTableView;
    tableView->verticalHeader()->hide();
    CustomStandardItemModel * tableModel = new CustomStandardItemModel(nbModes, 3, tableView);
    tableModel->setHorizontalHeaderLabels(QStringList() << tr("Mode") << tr("Eigenvalue") << tr("Cumulative\neigenvalue\nsum"));
    tableView->setModel(tableModel);
    for (UnsignedInteger i = 0; i < nbModes; ++i)
    {
      tableModel->setNotEditableItem(i, 0, i);
      tableModel->setNotEditableItem(i, 1, result_.getKarhunenLoeveResults()[out].getEigenValues()[i]);
      tableModel->setNotEditableItem(i, 2, cumulEigenSample(i, 1));
    }
    tableView->resizeToContents();
    eigenWidgetLayout->addWidget(tableView);
    eigenWidgetLayout->addStretch(1);

    klWidget->addWidget(eigenWidget);

    // 2- modes graph
#ifdef PERSALYS_HAVE_PARAVIEW
    if (canUseParaview)
    {
      PVXYChartViewWidget * outPVGraph = new PVXYChartViewWidget(this, PVServerManagerSingleton::Get(), PVXYChartViewWidget::Trajectories);

      Sample sample(scaledModes.getMesh().getVertices());
      QStringList labels;
      for (UnsignedInteger i = 0; i < nbModes; ++i)
      {
        sample.stack(scaledModes.getField(i).getValues().getMarginal(0));
        labels << tr("Mode %1").arg(i);
      }
      sample.setDescription(QtOT::StringListToDescription(meshParamName + labels));
      outPVGraph->PVViewWidget::setData(sample);
      outPVGraph->setChartTitle(tr("Modes"));
      outPVGraph->setAxisTitle(vtkAxis::BOTTOM, meshParamName[0]);
      outPVGraph->setAxisTitle(vtkAxis::LEFT, currentOutName[0]);
      outPVGraph->setXAxisData(meshParamName[0]);
      outPVGraph->setAxisToShow(labels);

      TrajectoriesSettingWidget * scatterSettingWidget = new TrajectoriesSettingWidget(outPVGraph, labels, this);

      modesStackedWidget->addWidget(new WidgetBoundToDockWidget(outPVGraph, scatterSettingWidget, this));
    }
#endif
    if (!canUseParaview)
    {
      PlotWidget * plotWidget = new PlotWidget;
      const Description colors(Drawable().BuildDefaultPalette(nbModes));

      for (UnsignedInteger i = 0; i < nbModes; ++i)
      {
        Sample sample(scaledModes.getMesh().getVertices());
        sample.stack(scaledModes.getField(i).getValues().getMarginal(0));
        plotWidget->plotCurve(sample, QPen(colors[i].c_str()).color(), QwtPlotCurve::Lines, 0, tr("Mode %1").arg(i));
      }
      plotWidget->setTitle(tr("Modes"));
      plotWidget->setAxisTitle(QwtPlot::yLeft, currentOutName[0]);
      plotWidget->setAxisTitle(QwtPlot::xBottom, meshParamName[0]);
      plotWidget->insertLegend(new QwtLegend, QwtPlot::RightLegend);
      // Graph Setting
      SimpleGraphSetting * graphSetting = new SimpleGraphSetting(plotWidget, this);
      modesStackedWidget->addWidget(new WidgetBoundToDockWidget(plotWidget, graphSetting, this));
    }

    // 3- Cumulative eigen value sum graph
#ifdef PERSALYS_HAVE_PARAVIEW
    if (canUseParaview && nbModes > 1)
    {
      PVXYChartViewWidget * eigenPVGraph = new PVXYChartViewWidget(this, PVServerManagerSingleton::Get());
      cumulEigenSample.setDescription(QtOT::StringListToDescription(QStringList() << tr("Mode") << tr("Cumulative sum")));
      eigenPVGraph->setData(cumulEigenSample, Qt::blue);
      eigenPVGraph->setMarkerStyle(2);
      eigenPVGraph->showChart(tr("Mode"), tr("Cumulative sum"));
      eigenPVGraph->setChartTitle(tr("Mode"), tr("Cumulative sum"), tr("Cumulative eigenvalue sum"));
      eigenPVGraph->setAxisRange(vtkAxis::BOTTOM, 0. - 0.1, nbModes - 1 + 0.1);
      const double min = cumulEigenSample(0, 1);
      eigenPVGraph->setAxisRange(vtkAxis::LEFT, min - (1 - min) * 0.04, 1 + (1 - min) * 0.02);

      XYChartSettingWidget * cumulSettingWidget = new XYChartSettingWidget(eigenPVGraph, this);

      klWidget->addWidget(new WidgetBoundToDockWidget(eigenPVGraph, cumulSettingWidget, this));
      klWidget->setCollapsible(0, false);
      klWidget->setStretchFactor(1, 5);
    }
#endif

    // xi variables distributions
    if (result_.getXiSamples().getSize() == nbOutput)
    {
      const Sample xi_sample(result_.getXiSamples()[out]);

      // plot matrix tab --------------------------------
      ResizableStackedWidget * stackedWidget = new ResizableStackedWidget;
      QVector<PlotWidget*> listPlotWidgets;

      // PDF/histogram
      for (UnsignedInteger i = 0; i < nbModes; ++i)
      {

        // PDF
        PlotWidget * pdfPlot = new PlotWidget(tr("distributionPDF"));
        pdfPlot->plotHistogram(xi_sample.getMarginal(i));
        KernelSmoothing gaussianKernel;
        Distribution fittedDistribution(gaussianKernel.build(xi_sample.getMarginal(i)));
        pdfPlot->plotCurve(fittedDistribution.drawPDF().getDrawable(0).getData());
        pdfPlot->setTitle(tr("PDF:") + " " + QString("ξ%1").arg(i));
        pdfPlot->setAxisTitle(QwtPlot::xBottom, QString("ξ%1").arg(i));
        pdfPlot->setAxisTitle(QwtPlot::yLeft, tr("Density"));

        stackedWidget->addWidget(pdfPlot);

        // CDF
        PlotWidget * cdfPlot = new PlotWidget(tr("distributionCDF"));
        cdfPlot->plotHistogram(xi_sample.getMarginal(i), 1);
        cdfPlot->plotCurve(fittedDistribution.drawCDF().getDrawable(0).getData());
        cdfPlot->setTitle(tr("CDF:") + " " + QString("ξ%1").arg(i));
        cdfPlot->setAxisTitle(QwtPlot::xBottom, QString("ξ%1").arg(i));
        cdfPlot->setAxisTitle(QwtPlot::yLeft, tr("CDF"));

        stackedWidget->addWidget(cdfPlot);

        // Graph Setting
        listPlotWidgets.append(pdfPlot);
        listPlotWidgets.append(cdfPlot);
      }
      PDFGraphSetting * graphSetting = new PDFGraphSetting(listPlotWidgets,
          QtOT::DescriptionToStringList(Description::BuildDefault(nbModes, "ξ")), PDFGraphSetting::Ksi, this);
      connect(graphSetting, SIGNAL(currentPlotChanged(int)), stackedWidget, SLOT(setCurrentIndex(int)));

      xiPDFStackedWidget->addWidget(new WidgetBoundToDockWidget(stackedWidget, graphSetting, this));

      // Plot matrix
#ifdef PERSALYS_HAVE_PARAVIEW
      if (canUseParaview)
      {
        PVMatrixPlotViewWidget * pvmatrixWidget = new PVMatrixPlotViewWidget(this, PVServerManagerSingleton::Get());
        pvmatrixWidget->setData(xi_sample);
        // the variables are automatically sorted : use setAxisToShow with the order of the sample
        pvmatrixWidget->setAxisToShow(xi_sample.getDescription());

        // setting widget
        PVPlotSettingWidget * matrixSettingWidget = new PVPlotSettingWidget(pvmatrixWidget, this);

        xiMatrixStackedWidget->addWidget(new WidgetBoundToDockWidget(pvmatrixWidget, matrixSettingWidget, this));
      }
#endif
      if (!canUseParaview)
      {
        PlotMatrixWidget * matrixWidget = new PlotMatrixWidget(xi_sample, xi_sample);
        PlotMatrixConfigurationWidget * plotMatrixSettingWidget = new PlotMatrixConfigurationWidget(matrixWidget, this);

        xiMatrixStackedWidget->addWidget(new WidgetBoundToDockWidget(matrixWidget, plotMatrixSettingWidget, this));
      }
    }
  }

  mainWidget_->getTabWidget()->addTab(klTabWidget, tr("Decomposition"));
}


void FieldCentralTendencyResultWindow::addCorrelationTab()
{
  const UnsignedInteger nbOutput = result_.getProcessSample().getDimension();
  if (result_.getCorrelationFunction().getSize() != nbOutput)
    return;

  const QStringList outNames(QtOT::DescriptionToStringList(result_.getProcessSample().getField(0).getValues().getDescription()));
  const QString meshParamName = QString::fromUtf8(result_.getProcessSample().getMesh().getDescription()[0].c_str());
  const Sample vertices(result_.getProcessSample().getMesh().getVertices());
  const Scalar minValueVertices = vertices.getMin()[0];
  const Scalar maxValueVertices = vertices.getMax()[0];

  // output tables and graphs
  QScrollArea * scrollArea = new QScrollArea;
  scrollArea->setWidgetResizable(true);
  ResizableStackedWidget * outStackedWidget = new ResizableStackedWidget;

  // for each output
  for (UnsignedInteger out = 0; out < nbOutput; ++out)
  {
    Graph graph(result_.getCorrelationFunction()[out].draw(Point(2, minValueVertices), Point(2,  maxValueVertices)));
    ContourPlot * corrPlot = new ContourPlot(graph.getDrawables());
    corrPlot->setTitle(tr("Empirical correlation") + " " + outNames[out]);
    corrPlot->setAxisTitle(QwtPlot::xBottom, meshParamName);
    corrPlot->setAxisTitle(QwtPlot::yLeft, meshParamName + "'");

    SimpleGraphSetting * graphSetting = new SimpleGraphSetting(corrPlot, this);
    outStackedWidget->addWidget(new WidgetBoundToDockWidget(corrPlot, graphSetting, this));
  }
  connect(mainWidget_->getOutListWidget(), SIGNAL(currentRowChanged(int)), outStackedWidget, SLOT(setCurrentIndex(int)));

  scrollArea->setWidget(outStackedWidget);
  mainWidget_->getTabWidget()->addTab(scrollArea, tr("Correlation"));
}

/////////////////


FieldModelEvaluationResultWidget::FieldModelEvaluationResultWidget(const Sample& inputSample, const ProcessSample& ps, QWidget *parent)
  : QWidget(parent)
  , outListWidget_(0)
  , tabWidget_(0)
  , inputSample_(inputSample)
  , processSample_(ps)
  , meanSample_()
  , lowerQuantileSample_()
  , upperQuantileSample_()
{
  buildInterface();
}


FieldModelEvaluationResultWidget::FieldModelEvaluationResultWidget(const Sample& inputSample,
                                                                   const ProcessSample& ps,
                                                                   const Sample& meanS,
                                                                   const Sample& qInfS,
                                                                   const Sample& qSupS,
                                                                   QWidget *parent)
  : QWidget(parent)
  , outListWidget_(0)
  , tabWidget_(0)
  , inputSample_(inputSample)
  , processSample_(ps)
  , meanSample_(meanS)
  , lowerQuantileSample_(qInfS)
  , upperQuantileSample_(qSupS)
{
  buildInterface();
}


void FieldModelEvaluationResultWidget::buildInterface()
{
  QVBoxLayout * mainLayout = new QVBoxLayout(this);
  mainLayout->setContentsMargins(0, 0, 0, 0);

  QSplitter * mainSplitter = new QSplitter(Qt::Horizontal, this);

  // - list outputs
  QGroupBox * outGroupBox = new QGroupBox(tr("Outputs"));
  QVBoxLayout * groupBoxLayout = new QVBoxLayout(outGroupBox);

  outListWidget_ = new VariablesListWidget;
  const QStringList outNames(QtOT::DescriptionToStringList(processSample_.getField(0).getValues().getDescription()));
  outListWidget_->addItems(outNames);
  outListWidget_->setCurrentRow(0);
  groupBoxLayout->addWidget(outListWidget_);

  mainSplitter->addWidget(outGroupBox);

  tabWidget_ = new QTabWidget;
  bool canUseParaview = false;
#ifdef PERSALYS_HAVE_PARAVIEW
  if (SubWindow::SupportsOpenGL_3_2())
  {
    addParaviewWidgetsTabs();
    canUseParaview = true;
  }
#endif
  if (!canUseParaview)
    addWidgetsTabs();

  mainSplitter->addWidget(tabWidget_);
  mainSplitter->setStretchFactor(0, 2);
  mainSplitter->setStretchFactor(1, 10);
  mainLayout->addWidget(mainSplitter);
}


VariablesListWidget * FieldModelEvaluationResultWidget::getOutListWidget()
{
  return outListWidget_;
}


QTabWidget * FieldModelEvaluationResultWidget::getTabWidget()
{
  return tabWidget_;
}


void FieldModelEvaluationResultWidget::addWidgetsTabs()
{
  // get model info
  const UnsignedInteger nbOutput = processSample_.getDimension();
  const UnsignedInteger nbInputPt = inputSample_.getSize();
  const UnsignedInteger nbNodes = processSample_.getMesh().getVerticesNumber();
  const QStringList outNames(QtOT::DescriptionToStringList(processSample_.getField(0).getValues().getDescription()));
  const QString meshParamName = QString::fromUtf8(processSample_.getMesh().getDescription()[0].c_str());
  const Description meshNodesNames(Description::BuildDefault(nbNodes, processSample_.getMesh().getDescription()[0]));
  const Description colors(Drawable().BuildDefaultPalette(nbInputPt));

  // first tab --------------------------------
  QWidget * tab = new QWidget;
  QHBoxLayout * tabLayout = new QHBoxLayout(tab);

  // outputs tab
  QTabWidget * outTabWidget = new QTabWidget;

  for (UnsignedInteger out = 0; out < nbOutput; ++out)
  {
    // trajectories tab --------------------------------
    PlotWidget * plotWidget = new PlotWidget;
    for (UnsignedInteger in = 0; in < nbInputPt; ++in)
    {
      Sample sample(processSample_.getMesh().getVertices());
      sample.stack(processSample_.getField(in).getValues().getMarginal(out));
      plotWidget->plotCurve(sample, QPen(colors[in].c_str()).color(), QwtPlotCurve::Lines, 0, tr("Input %1").arg(in));
    }
    plotWidget->setTitle(nbInputPt == 1 ? tr("Trajectory") : tr("Trajectories"));
    plotWidget->setAxisTitle(QwtPlot::yLeft, outNames[out]);
    plotWidget->setAxisTitle(QwtPlot::xBottom, meshParamName);
    plotWidget->insertLegend(new QwtLegend, QwtPlot::RightLegend);
    // Graph Setting
    SimpleGraphSetting * graphSetting = new SimpleGraphSetting(plotWidget, this);

    outTabWidget->addTab(new WidgetBoundToDockWidget(plotWidget, graphSetting, this), nbInputPt == 1 ? tr("Trajectory") : tr("Trajectories"));

    Sample fieldSample(nbInputPt, nbNodes);
    fieldSample.setDescription(meshNodesNames);

    for (UnsignedInteger in = 0; in < nbInputPt; ++in)
    {
      fieldSample[in] = processSample_.getField(in).getValues().getMarginal(out).asPoint();
    }

    // table tab --------------------------------
    QWidget * outTab = new QWidget;
    QHBoxLayout * outTabLayout = new QHBoxLayout(outTab);
    ExportableTableView * tableView = new ExportableTableView;
    SampleTableModel * tableModel = new SampleTableModel(fieldSample, tableView);
    tableView->setModel(tableModel);
    outTabLayout->addWidget(tableView);
    outTabWidget->addTab(outTab, tr("Table"));

    // quantiles tab ----------------
    if (meanSample_.getSize())
    {
      plotWidget = new PlotWidget;
      // mean
      Sample sample(processSample_.getMesh().getVertices());
      sample.stack(meanSample_.getMarginal(out));
      plotWidget->plotCurve(sample, QPen(Qt::black), QwtPlotCurve::Lines, 0, tr("Mean"));
      // quantile sup
      sample = processSample_.getMesh().getVertices();
      sample.stack(upperQuantileSample_.getMarginal(out));
      plotWidget->plotCurve(sample, QPen(Qt::blue), QwtPlotCurve::Lines, 0, tr("Quantile 95%"));
      // quantile inf
      sample = processSample_.getMesh().getVertices();
      sample.stack(lowerQuantileSample_.getMarginal(out));
      plotWidget->plotCurve(sample, QPen(Qt::red), QwtPlotCurve::Lines, 0, tr("Quantile 5%"));

      plotWidget->setTitle(tr("Quantiles"));
      plotWidget->setAxisTitle(QwtPlot::yLeft, outNames[out]);
      plotWidget->setAxisTitle(QwtPlot::xBottom, meshParamName);
      plotWidget->insertLegend(new QwtLegend, QwtPlot::RightLegend);

      // Graph Setting
      graphSetting = new SimpleGraphSetting(plotWidget, this);

      outTabWidget->addTab(new WidgetBoundToDockWidget(plotWidget, graphSetting, this), tr("Mean trajectory"));
    }
  }

  tabWidget_->addTab(outTabWidget, tr("Result"));

  // input tab
  tab = new QWidget;
  tabLayout = new QHBoxLayout(tab);
  // -- inputs table
  ExportableTableView * tableView = new ExportableTableView;
  SampleTableModel * tableModel = new SampleTableModel(inputSample_, tableView);
  tableView->setModel(tableModel);
  tabLayout->addWidget(tableView);

  tabWidget_->addTab(tab, tr("Input"));
}


#ifdef PERSALYS_HAVE_PARAVIEW
void FieldModelEvaluationResultWidget::addParaviewWidgetsTabs()
{
  // get model info
  const UnsignedInteger nbOutput = processSample_.getDimension();
  const UnsignedInteger nbInputPt = inputSample_.getSize();
  const UnsignedInteger nbNodes = processSample_.getMesh().getVerticesNumber();
  const QStringList outNames(QtOT::DescriptionToStringList(processSample_.getField(0).getValues().getDescription()));
  const QStringList meshParamName = QStringList() << QString::fromUtf8(processSample_.getMesh().getDescription()[0].c_str());
  const Description meshNodesNames(Description::BuildDefault(nbNodes, processSample_.getMesh().getDescription()[0]));
  const Description colors(Drawable().BuildDefaultPalette(nbInputPt));

  // input table tab
  QTabWidget * inTabWidget = new QTabWidget;

  // - with paraview the table is always shown in order to use the selection behavior
  PVSpreadSheetViewWidget * inPVTable = new PVSpreadSheetViewWidget(this, PVServerManagerSingleton::Get());
  inTabWidget->addTab(PVSpreadSheetViewWidget::GetSpreadSheetViewWidget(inPVTable, inputSample_), tr("Table"));

  pqLinksModel * linksModel = pqApplicationCore::instance()->getLinksModel();

  PVMatrixPlotViewWidget * pvmatrixWidget = 0;
  if (nbInputPt > 1)
  {
    const Sample sampleRank(inputSample_.rank() / inputSample_.getSize());
    // Plot matrix
    pvmatrixWidget = new PVMatrixPlotViewWidget(this, PVServerManagerSingleton::Get());
    pvmatrixWidget->setData(sampleRank);
    // the variables are automatically sorted : use setAxisToShow with the order of the sample
    pvmatrixWidget->setAxisToShow(inputSample_.getDescription());

    // setting widget
    MultiPlotSettingWidget * matrixSettingWidget = new MultiPlotSettingWidget(pvmatrixWidget, inputSample_, sampleRank, this);

    inTabWidget->addTab(new WidgetBoundToDockWidget(pvmatrixWidget, matrixSettingWidget, this), tr("Plot matrix"));

    String aStr = (OSS() << inPVTable->getProxy() << pvmatrixWidget->getProxy()).str();
    linksModel->addSelectionLink(aStr.c_str(), inPVTable->getProxy(), pvmatrixWidget->getProxy());
  }

  // output tables and graphs
  QTabWidget * outTabWidget = new QTabWidget;

  ResizableStackedWidget * trajStackedWidget = new ResizableStackedWidget;
  outTabWidget->addTab(trajStackedWidget, nbInputPt == 1 ? tr("Trajectory") : tr("Trajectories"));
  connect(outListWidget_, SIGNAL(currentRowChanged(int)), trajStackedWidget, SLOT(setCurrentIndex(int)));

  // mean graph tab
  if (meanSample_.getSize())
  {
    QScrollArea * scrollArea = new QScrollArea;
    scrollArea->setWidgetResizable(true);
    ResizableStackedWidget * meanStackedWidget = new ResizableStackedWidget;
    scrollArea->setWidget(meanStackedWidget);
    outTabWidget->addTab(scrollArea, tr("Mean trajectory"));
    connect(outListWidget_, SIGNAL(currentRowChanged(int)), meanStackedWidget, SLOT(setCurrentIndex(int)));

    // for each output
    for (UnsignedInteger out = 0; out < nbOutput; ++out)
    {
      const QStringList currentOutName = QStringList() << outNames[out];

      PVXYChartViewWidget * outPVGraph = new PVXYChartViewWidget(this, PVServerManagerSingleton::Get(), PVXYChartViewWidget::Trajectories);
      // mean
      Sample sample(processSample_.getMesh().getVertices());
      sample.stack(meanSample_.getMarginal(out));
      // quantile sup
      sample.stack(upperQuantileSample_.getMarginal(out));
      // quantile inf
      sample.stack(lowerQuantileSample_.getMarginal(out));
      const QStringList labels = QStringList() << tr("Mean") << tr("Quantile 95%") << tr("Quantile 5%");
      sample.setDescription(QtOT::StringListToDescription(meshParamName + labels));

      outPVGraph->PVViewWidget::setData(sample);
      outPVGraph->setChartTitle(tr("Quantiles"));
      outPVGraph->setAxisTitle(vtkAxis::BOTTOM, meshParamName[0]);
      outPVGraph->setAxisTitle(vtkAxis::LEFT, currentOutName[0]);
      outPVGraph->setXAxisData(meshParamName[0]);
      outPVGraph->setAxisToShow(labels);

      TrajectoriesSettingWidget * scatterSettingWidget = new TrajectoriesSettingWidget(outPVGraph, labels, this);

      meanStackedWidget->addWidget(new WidgetBoundToDockWidget(outPVGraph, scatterSettingWidget, this));
    }
  }

  ResizableStackedWidget * bagChartStackedWidget = 0;
  ResizableStackedWidget * functionalBagChartStackedWidget = 0;
  if (nbInputPt > 1)
  {
    functionalBagChartStackedWidget = new ResizableStackedWidget;
    outTabWidget->addTab(functionalBagChartStackedWidget, tr("Functional bag chart"));
    connect(outListWidget_, SIGNAL(currentRowChanged(int)), functionalBagChartStackedWidget, SLOT(setCurrentIndex(int)));

    bagChartStackedWidget = new ResizableStackedWidget;
    outTabWidget->addTab(bagChartStackedWidget, tr("Bag chart"));
    connect(outListWidget_, SIGNAL(currentRowChanged(int)), bagChartStackedWidget, SLOT(setCurrentIndex(int)));
  }

  ResizableStackedWidget * tableStackedWidget = new ResizableStackedWidget;
  outTabWidget->addTab(tableStackedWidget, tr("Table"));
  connect(outListWidget_, SIGNAL(currentRowChanged(int)), tableStackedWidget, SLOT(setCurrentIndex(int)));

  // for each output
  QList<PVXYChartViewWidget*> listCharts;
  for (UnsignedInteger out = 0; out < nbOutput; ++out)
  {
    const QStringList currentOutName(outNames[out]);

    // - table tab
    Sample fieldSample(nbInputPt, nbNodes);
    fieldSample.setDescription(meshNodesNames);
    for (UnsignedInteger in = 0; in < nbInputPt; ++in)
      fieldSample[in] = processSample_.getField(in).getValues().getMarginal(out).asPoint();

    // with paraview the table is always shown in order to use the selection behavior
    PVSpreadSheetViewWidget * outPVTable = new PVSpreadSheetViewWidget(this, PVServerManagerSingleton::Get());
    tableStackedWidget->addWidget(PVSpreadSheetViewWidget::GetSpreadSheetViewWidget(outPVTable, fieldSample));

    // There are selection behavior errors if windows use the same links names: a link name must be unique.
    // The pointers are unique, so we use them to create an unique name...find a better and easier way.
    String aStr = (OSS() << inPVTable->getProxy() << outPVTable->getProxy()).str();
    linksModel->addSelectionLink(aStr.c_str(), inPVTable->getProxy(), outPVTable->getProxy());

    // get fieldSample transpose
    Sample fieldSamplet(nbNodes, nbInputPt);
    fieldSamplet.setDescription(Description::BuildDefault(nbInputPt, "Input"));

    for (UnsignedInteger i = 0; i < nbNodes; ++i)
    {
      for (UnsignedInteger j = 0; j < nbInputPt; ++j)
        fieldSamplet(i, j) = fieldSample(j, i);
    }

    // - graph tab
    PVXYChartViewWidget * outPVGraph = new PVXYChartViewWidget(this, PVServerManagerSingleton::Get(), PVXYChartViewWidget::Trajectories);
    listCharts.append(outPVGraph);

    Sample sample(fieldSamplet);
    sample.stack(processSample_.getMesh().getVertices());

    outPVGraph->PVViewWidget::setData(sample);
    outPVGraph->setChartTitle(nbInputPt == 1 ? tr("Trajectory") : tr("Trajectories"));
    outPVGraph->setAxisTitle(vtkAxis::BOTTOM, meshParamName[0]);
    outPVGraph->setAxisTitle(vtkAxis::LEFT, currentOutName[0]);
    outPVGraph->setXAxisData(meshParamName[0]);
    outPVGraph->setAxisToShow(fieldSamplet.getDescription());

    TrajectoriesSettingWidget * scatterSettingWidget = new TrajectoriesSettingWidget(outPVGraph,
                                                                                     QtOT::DescriptionToStringList(fieldSamplet.getDescription()),
                                                                                     this);

    trajStackedWidget->addWidget(new WidgetBoundToDockWidget(outPVGraph, scatterSettingWidget, this));

    aStr = (OSS() << outPVTable->getProxy() << outPVGraph->getProxy()).str();
    linksModel->addSelectionLink(aStr.c_str(), outPVTable->getProxy(), outPVGraph->getProxy());

    // - functional/bag chart
    if (nbInputPt > 1)
    {

      // functional bag chart
      PVBagChartViewWidget * fBagChartWidget = new PVBagChartViewWidget(this, PVServerManagerSingleton::Get());
      fBagChartWidget->PVViewWidget::setData(sample);
      fBagChartWidget->setXAxisData(meshParamName[0]);
      fBagChartWidget->setChartTitle(tr("Quantiles"));
      fBagChartWidget->setAxisTitle(vtkAxis::BOTTOM, meshParamName[0]);
      fBagChartWidget->setAxisTitle(vtkAxis::LEFT, currentOutName[0]);

      BagChartSettingWidget * settingWidget = new BagChartSettingWidget(fBagChartWidget, this);

      functionalBagChartStackedWidget->addWidget(new WidgetBoundToDockWidget(fBagChartWidget, settingWidget, this));

      // bag chart
      PVBagChartViewWidget * bagChartWidget = new PVBagChartViewWidget(this, PVServerManagerSingleton::Get(), fBagChartWidget->getFilterSource());
      //bagChartWidget->PVViewWidget::setData(fieldSamplet);
      bagChartWidget->setAxisTitle(vtkAxis::BOTTOM, "PC1");
      bagChartWidget->setAxisTitle(vtkAxis::LEFT, "PC2");

      settingWidget = new BagChartSettingWidget(bagChartWidget, this);

      bagChartStackedWidget->addWidget(new WidgetBoundToDockWidget(bagChartWidget, settingWidget, this));

      String aStr = (OSS() << inPVTable->getProxy() << bagChartWidget->getProxy()).str();
      linksModel->addSelectionLink(aStr.c_str(), fBagChartWidget->getProxy(), outPVGraph->getProxy());
    }
  }

  tabWidget_->addTab(outTabWidget, tr("Result"));
  tabWidget_->addTab(inTabWidget, tr("Input"));
}
#endif
}
