//                                               -*- C++ -*-
/**
 *  @brief QMdiSubWindow for the results of an evaluation of the model
 *
 *  Copyright 2015-2024 EDF-Phimeca
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
#include "persalys/CalibrationResultWindow.hxx"

#include "persalys/CopyableTableView.hxx"
#include "persalys/CustomStandardItemModel.hxx"
#include "persalys/WidgetBoundToDockWidget.hxx"
#include "persalys/ResizableStackedWidget.hxx"
#include "persalys/QtTools.hxx"
#include "persalys/PlotWidget.hxx"
#include "persalys/GraphConfigurationWidget.hxx"

#ifdef PERSALYS_HAVE_PARAVIEW
#include "persalys/PVServerManagerInterface.hxx"
#include "persalys/PVServerManagerSingleton.hxx"
#include "persalys/PVXYChartViewWidget.hxx"
#include "persalys/PVXYChartSettingWidget.hxx"
#include "persalys/PVSpreadSheetViewWidget.hxx"

#include <pqLinksModel.h>
#include <pqApplicationCore.h>
#endif

#include <openturns/KernelSmoothing.hxx>
#include <openturns/ResourceMap.hxx>

#include <QVBoxLayout>
#include <QGroupBox>
#include <QScrollArea>
#include <QSplitter>
#include <QHeaderView>

using namespace OT;

namespace PERSALYS
{

CalibrationResultWindow::CalibrationResultWindow(AnalysisItem *item, QWidget *parent)
  : ResultWindow(item, parent)
  , result_()
  , observations_()
{
  const CalibrationAnalysis * analysis = dynamic_cast<const CalibrationAnalysis*>(item->getAnalysis().getImplementation().get());
  if (!analysis)
    throw InvalidArgumentException(HERE) << "CalibrationResultWindow: the analysis is not a CalibrationAnalysis";

  result_ = analysis->getResult();
  observations_ = analysis->getObservations();

  // parameters widget
  setParameters(item->getAnalysis(), tr("Calibration parameters"));

  // get variable names
  const Description calibratedInputNames(analysis->getCalibratedInputs());
  PhysicalModel model(observations_.getPhysicalModel());
  QStringList calibratedInAxisTitles_(QtOT::GetVariableAxisLabels(model, calibratedInputNames));

  // build window

  QVBoxLayout * widgetLayout = new QVBoxLayout(this);

  // title
  widgetLayout->addWidget(new TitleLabel("Calibration", "user_manual/graphical_interface/deterministic_analysis/user_manual_deterministic_analysis.html#calibrationresult"));

  // main splitter
  QSplitter * mainWidget = new QSplitter(Qt::Horizontal);

  // - list outputs
  QGroupBox * outputsGroupBox = new QGroupBox(tr("Output"));
  QVBoxLayout * outputsLayoutGroupBox = new QVBoxLayout(outputsGroupBox);

  VariablesListWidget * outputsListWidget = new VariablesListWidget;
  outputsListWidget->addItems(QtOT::DescriptionToStringList(observations_.getOutputSample().getDescription()));
  outputsListWidget->setCurrentRow(0);
  outputsLayoutGroupBox->addWidget(outputsListWidget);

  mainWidget->addWidget(outputsGroupBox);
  mainWidget->setStretchFactor(0, 1);

  // tab widget
  QTabWidget * tabWidget = new QTabWidget;

  // first tab --------------------------------
  QTabWidget * tabThetaWidget = new QTabWidget;

  QWidget * tab = new QWidget;
  QVBoxLayout * tabLayout = new QVBoxLayout(tab);

  QScrollArea * scrollArea = new QScrollArea;
  scrollArea->setWidgetResizable(true);

  // optimal value
  QGroupBox * thetaGroupBox = new QGroupBox(tr("Optimal θ"));
  QVBoxLayout * thetaGroupBoxLayout = new QVBoxLayout(thetaGroupBox);

  // table view
  CopyableTableView * thetaTableView = new CopyableTableView;
  thetaTableView->verticalHeader()->hide();
  // table model
  const UnsignedInteger nbInputs = result_.getCalibrationResult().getParameterMAP().getSize();
  CustomStandardItemModel * thetaTableModel = new CustomStandardItemModel(nbInputs, 3, thetaTableView);
  thetaTableView->setModel(thetaTableModel);

  // horizontal header
  const QString level(QString::number(analysis->getConfidenceIntervalLength() * 100));
  thetaTableModel->setHorizontalHeaderLabels(QStringList() << tr("Input") << tr("Value") << tr("Confidence interval at") + " " + level + "%");

  // theta and confidence interval
  for (UnsignedInteger i = 0; i < nbInputs; ++i)
  {
    thetaTableModel->setNotEditableItem(i, 0, analysis->getCalibratedInputs()[i].c_str());
    thetaTableModel->setNotEditableItem(i, 1, result_.getCalibrationResult().getParameterMAP()[i]);
    if (result_.getConfidenceInterval().getDimension() == nbInputs)
      thetaTableModel->setNotEditableItem(i, 2, result_.getConfidenceInterval().getMarginal(i).__str__().c_str());
    else
      thetaTableModel->setNotEditableItem(i, 2, tr("Not available"));
  }

  // resize table
  thetaTableView->resizeToContents();

  thetaGroupBoxLayout->addWidget(thetaTableView);
  tabLayout->addWidget(thetaGroupBox, 0, Qt::AlignTop);
  tabLayout->addStretch();
  scrollArea->setWidget(tab);

  const GridLayout parameterDistributionsGraph(result_.getCalibrationResult().drawParameterDistributions());

  tabThetaWidget->addTab(scrollArea, tr("Optimal"));
  tabWidget->addTab(tabThetaWidget, "θ");

#ifdef PERSALYS_HAVE_PARAVIEW
  if (SubWindow::SupportsOpenGL_3_2())
  {
    // - PDF
    PVXYChartViewWidget * pvWidget = new PVXYChartViewWidget(this, PVServerManagerSingleton::Get(), PVXYChartViewWidget::Trajectories);
    Sample pdfSamplePrior; // can not initialize size to DefaultPointNumber because of possible discrete distribution
    Sample pdfSamplePosterior; // can not initialize size to DefaultPointNumber because of possible discrete distribution
    Description sampleDescription;
    QStringList pdfNames;
    for (UnsignedInteger i = 0; i < nbInputs; ++i)
    {
      if (i == 0)
      {
        pdfSamplePrior = parameterDistributionsGraph.getGraph(0, 0).getDrawables()[0].getData();
        pdfSamplePosterior = parameterDistributionsGraph.getGraph(0, 0).getDrawables()[1].getData();
      }
      else
      {
        pdfSamplePrior.stack(parameterDistributionsGraph.getGraph(0, i).getDrawables()[0].getData());
        pdfSamplePosterior.stack(parameterDistributionsGraph.getGraph(0, i).getDrawables()[1].getData());
      }
      sampleDescription.add(calibratedInputNames[i]);
      sampleDescription.add(calibratedInputNames[i] + "pdf");
      pdfNames << (calibratedInputNames[i] + "pdf").c_str();
    }
    pdfSamplePrior.setDescription(sampleDescription);
    pdfSamplePosterior.setDescription(sampleDescription);

    // If only one point, draw vertical line for prior
    if(pdfSamplePrior.getSize() == 1)
    {
      pdfSamplePrior.add(pdfSamplePrior[0]);
      pdfSamplePrior[1][1] = 0.1 * pdfSamplePosterior.getMarginal(1).getMax()[0];
    }

    pvWidget->setData(pdfSamplePrior, Qt::red);
    pvWidget->setData(pdfSamplePosterior, Qt::green);
    pvWidget->setRepresentationLabels(QVector<QString>(nbInputs * 2, tr("Prior")).toList(), 0);
    pvWidget->setRepresentationLabels(QVector<QString>(nbInputs * 2, tr("Posterior")).toList(), 1);
    for (UnsignedInteger i = 0; i < nbInputs; ++i)
    {
      const QString varX(calibratedInputNames[i].c_str());
      const QString varY((calibratedInputNames[i] + "pdf").c_str());
      pvWidget->setChartTitle(tr("PDF:") + " " + calibratedInputNames[i].c_str(), varX, varY);
      pvWidget->setAxisTitle(vtkAxis::BOTTOM, calibratedInAxisTitles_[i], varX, varY);
      pvWidget->setAxisTitle(vtkAxis::LEFT, tr("Density"), varX, varY);
    }
    pvWidget->setXAxisData(calibratedInputNames[0].c_str());
    pvWidget->setAxisToShow(Description(1, calibratedInputNames[0] + "pdf"));

    MultiPDFSettingWidget * settingWidget = new MultiPDFSettingWidget(pvWidget, QtOT::DescriptionToStringList(calibratedInputNames), pdfNames);

    tabThetaWidget->addTab(new WidgetBoundToDockWidget(pvWidget, settingWidget, this), tr("PDF"));

    // tab : Prediction
    ResizableStackedWidget * predictionStackedWidget = new ResizableStackedWidget;
    // for each output
    const UnsignedInteger nbOutput = observations_.getOutputSample().getDimension();
    for (UnsignedInteger out = 0; out < nbOutput; ++out)
    {
      predictionStackedWidget->addWidget(getPredictionTabWidget(out));
    }
    connect(outputsListWidget, SIGNAL(currentRowChanged(int)), predictionStackedWidget, SLOT(setCurrentIndex(int)));
    tabWidget->addTab(predictionStackedWidget, tr("Prediction"));
  }
#endif

  // tab : PARAMETERS --------------------------------
  if (parametersWidget_)
    tabWidget->addTab(parametersWidget_, tr("Parameters"));

  // tab : model description --------------------------------
  if (modelDescriptionWidget_)
    tabWidget->addTab(modelDescriptionWidget_, tr("Model"));

  mainWidget->addWidget(tabWidget);
  mainWidget->setStretchFactor(1, 10);

  widgetLayout->addWidget(mainWidget, 1);
}


QTabWidget * CalibrationResultWindow::getPredictionTabWidget(const UnsignedInteger i)
{
  QTabWidget * predTabWidget = new QTabWidget;
  (void)i;
#ifdef PERSALYS_HAVE_PARAVIEW

  QMap<QString, QColor> Colors_ = {{QObject::tr("Data"), Qt::blue}, {QObject::tr("Prior"), Qt::red}, {QObject::tr("Posterior"), Qt::green}};

  // get descriptions
  const Description obsInDescription(observations_.getInputSample().getDescription());
  const QStringList obsInNames(QtOT::DescriptionToStringList(obsInDescription));
  const QStringList obsInLabels(QtOT::GetVariableAxisLabels(observations_.getPhysicalModel(), obsInDescription));

  QStringList labels(QStringList() << tr("Data") << tr("Prior"));

  // output samples
  const Sample outputObs(observations_.getOutputSample().getMarginal(i));

  Sample outputSamples(outputObs);
  outputSamples.stack(result_.getCalibrationResult().getOutputAtPriorMean().getMarginal(i));
  if (result_.getCalibrationResult().getOutputAtPosteriorMean().getSize())
  {
    outputSamples.stack(result_.getCalibrationResult().getOutputAtPosteriorMean().getMarginal(i));
    labels << tr("Posterior");
  }
  const Description outDescription(QtOT::StringListToDescription(labels));
  outputSamples.setDescription(outDescription);

  // + input sample
  Sample inOutSample(outputSamples);
  inOutSample.stack(observations_.getInputSample());

  Sample sample(inOutSample);
  Description sampleDescription(sample.getDescription());

  // + residual samples
  sample.stack(outputObs - result_.getCalibrationResult().getOutputAtPriorMean().getMarginal(i));
  sampleDescription.add(tr("Prior residuals").toStdString());
  if (result_.getCalibrationResult().getOutputAtPosteriorMean().getSize())
  {
    sample.stack(outputObs - result_.getCalibrationResult().getOutputAtPosteriorMean().getMarginal(i));
    sampleDescription.add(tr("Posterior residuals").toStdString());
  }
  sample.setDescription(sampleDescription);

  // create charts
  pqLinksModel * linksModel = pqApplicationCore::instance()->getLinksModel();

  // - table
  PVSpreadSheetViewWidget * pvSpreadSheetWidget = new PVSpreadSheetViewWidget(this, PVServerManagerSingleton::Get());
  predTabWidget->addTab(PVSpreadSheetViewWidget::GetSpreadSheetViewWidget(pvSpreadSheetWidget, sample, getItem()), tr("Table"));

  // - predictions vs observations
  PVXYChartViewWidget * pvWidget = new PVXYChartViewWidget(this, PVServerManagerSingleton::Get(), PVXYChartViewWidget::TrajectoriesPoints);

  pvWidget->PVViewWidget::setData(outputSamples);
  pvWidget->setSerieColors(Colors_);
//   pvWidget->setSerieLineStyles(QMap<QString, int>{{tr("Data"), 1}}); // if line : pb with log scale (points order)
  pvWidget->setXAxisData(tr("Data"));
  pvWidget->setAxisToShow(outDescription);
  pvWidget->setChartTitle(tr("Predictions vs observations"));
  pvWidget->setAxisTitle(vtkAxis::BOTTOM, tr("Observations"));
  pvWidget->setAxisTitle(vtkAxis::LEFT, tr("Predictions"));
  pvWidget->getLegend()->SetHorizontalAlignment(vtkChartLegend::LEFT);

  TrajectoriesSettingWidget * chartSetting = new TrajectoriesSettingWidget(pvWidget, labels, this);

  // There are selection behavior errors if windows use the same links names: a link name must be unique.
  // The pointers are uniques, so we use them to create an unique name...find a better and easier way.
  String aStr = (OSS() << pvSpreadSheetWidget->getProxy() << pvWidget->getProxy()).str();
  linksModel->addSelectionLink(aStr.c_str(), pvSpreadSheetWidget->getProxy(), pvWidget->getProxy());

  predTabWidget->addTab(new WidgetBoundToDockWidget(pvWidget, chartSetting, this), tr("vs Observations"));

  // - predictions vs Inputs
  pvWidget = new PVXYChartViewWidget(this, PVServerManagerSingleton::Get(), PVXYChartViewWidget::TrajectoriesPoints);

  pvWidget->PVViewWidget::setData(inOutSample);
  pvWidget->setAxisTitles(obsInNames, obsInLabels);
  pvWidget->setChartTitle(tr("Predictions vs inputs"));
  pvWidget->setSerieColors(Colors_);
  pvWidget->setXAxisData(observations_.getInputSample().getDescription()[0].c_str());
  pvWidget->setAxisToShow(outDescription);
  pvWidget->setAxisTitle(vtkAxis::LEFT, tr("Predictions"));
  pvWidget->getLegend()->SetHorizontalAlignment(vtkChartLegend::LEFT);

  chartSetting = new TrajectoriesSettingWidget(pvWidget, obsInNames, labels, this);

  // There are selection behavior errors if windows use the same links names: a link name must be unique.
  // The pointers are uniques, so we use them to create an unique name...find a better and easier way.
  aStr = (OSS() << pvSpreadSheetWidget->getProxy() << pvWidget->getProxy()).str();
  linksModel->addSelectionLink(aStr.c_str(), pvSpreadSheetWidget->getProxy(), pvWidget->getProxy());

  predTabWidget->addTab(new WidgetBoundToDockWidget(pvWidget, chartSetting, this), tr("vs Inputs"));

  // - residuals distribution
  Distribution observationsError(result_.getCalibrationResult().getObservationsError().getMarginal(i));
  pvWidget = new PVXYChartViewWidget(this, PVServerManagerSingleton::Get(), PVXYChartViewWidget::Trajectories);

  Graph residualPDF(observationsError.drawPDF());
  pvWidget->setData(residualPDF.getDrawables()[0].getData(), Colors_[tr("Data")]);
  pvWidget->setRepresentationLabels(QStringList() << tr("Data") << tr("Data"));

  if (result_.getPriorResidualsPDF()[i].getSize())
  {
    pvWidget->setData(result_.getPriorResidualsPDF()[i], Colors_[tr("Prior")]);
    pvWidget->setRepresentationLabels(QStringList() << tr("Prior") << tr("Prior"), 1);
  }
  if (result_.getPosteriorResidualsPDF()[i].getSize())
  {
    pvWidget->setData(result_.getPosteriorResidualsPDF()[i], Colors_[tr("Posterior")]);
    pvWidget->setRepresentationLabels(QStringList() << tr("Posterior") << tr("Posterior"), 2);
  }

  pvWidget->setXAxisData("v0");
  pvWidget->setAxisToShow(QStringList() << "v1");
  pvWidget->setChartTitle(tr("Residuals analysis"));
  pvWidget->setAxisTitle(vtkAxis::LEFT, tr("Density"));
  pvWidget->setAxisTitle(vtkAxis::BOTTOM, tr("Residuals"));

  chartSetting = new TrajectoriesSettingWidget(pvWidget, labels, this);

  predTabWidget->addTab(new WidgetBoundToDockWidget(pvWidget, chartSetting, this), tr("Residuals"));

  // - residuals QQ-plot

  Graph qqPlotGraph(result_.getCalibrationResult().drawResidualsNormalPlot().getGraph(0, i));
  PlotWidget * qqPlot = new PlotWidget(tr("qqPlot"));
  SimpleGraphSetting * qqPlotSettingWidget = new SimpleGraphSetting(qqPlot, this);
  qqPlotSettingWidget->hide();
  qqPlot->plotCurve(qqPlotGraph.getDrawable(1).getData(), QPen(Qt::blue, 5), QwtPlotCurve::Dots);
  qqPlot->plotCurve(qqPlotGraph.getDrawable(0).getData(), QPen(Qt::red, 1, Qt::DashDotLine));

  qqPlot->setAxisTitle(QwtPlot::xBottom, tr("Residuals"));
  qqPlot->setAxisTitle(QwtPlot::yLeft, tr("Standard normal quantiles"));

  predTabWidget->addTab(new WidgetBoundToDockWidget(qqPlot, qqPlotSettingWidget, this), tr("Residuals QQ-plot"));

#endif

  return predTabWidget;
}
}
