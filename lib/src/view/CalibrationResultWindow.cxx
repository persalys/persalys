//                                               -*- C++ -*-
/**
 *  @brief QMdiSubWindow for the results of an evaluation of the model
 *
 *  Copyright 2015-2026 EDF-Phimeca
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
#include <openturns/NormalityTest.hxx>
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
  const auto * analysis = dynamic_cast<const CalibrationAnalysis*>(item->getAnalysis().getImplementation().get());
  if (!analysis)
    throw InvalidArgumentException(HERE) << "CalibrationResultWindow: the analysis is not a CalibrationAnalysis";

  result_       = analysis->getResult();
  observations_ = analysis->getObservations();

  // parameters widget
  ResultWindow::setParameters(item->getAnalysis(), tr("Calibration parameters"));

  // build window
  auto * widgetLayout = new QVBoxLayout(this);

  // title
  widgetLayout->addWidget(new TitleLabel(tr("Calibration"), "user_manual/graphical_interface/deterministic_analysis/user_manual_deterministic_analysis.html#calibrationresult"));

  // main splitter
  auto * mainWidget = new QSplitter(Qt::Horizontal);

  // - list outputs
  auto * outputsGroupBox = new QGroupBox(tr("Output"));
  auto * outputsLayoutGroupBox = new QVBoxLayout(outputsGroupBox);

  auto * outputsListWidget = new VariablesListWidget;
  outputsListWidget->addItems(QtOT::DescriptionToStringList(observations_.getOutputSample().getDescription()));
  outputsListWidget->setCurrentRow(0);
  outputsLayoutGroupBox->addWidget(outputsListWidget);

  mainWidget->addWidget(outputsGroupBox);
  mainWidget->setStretchFactor(0, 1);

  auto * tabWidget = new QTabWidget;

  // tab: Theta --------------------------------------------
  tabWidget->addTab(getThetaTabWidget(analysis), tr("θ"));

  // tab: Prediction ---------------------------------------
#ifdef PERSALYS_HAVE_PARAVIEW
  if (SubWindow::SupportsOpenGL_3_2())
  {
    auto * predictionStackedWidget = new ResizableStackedWidget;
    const UnsignedInteger nbOutput = observations_.getOutputSample().getDimension();

    for (UnsignedInteger out = 0; out < nbOutput; ++out)
      predictionStackedWidget->addWidget(getPredictionTabWidget(out));
    
    connect(outputsListWidget, SIGNAL(currentRowChanged(int)), predictionStackedWidget, SLOT(setCurrentIndex(int)));

    tabWidget->addTab(predictionStackedWidget, tr("Prediction"));
  }
#endif

  // tab : Parameters --------------------------------------
  if (parametersWidget_)
    tabWidget->addTab(parametersWidget_, tr("Parameters"));

  // tab : model ---------- --------------------------------
  if (modelDescriptionWidget_)
    tabWidget->addTab(modelDescriptionWidget_, tr("Model"));

  mainWidget->addWidget(tabWidget);
  mainWidget->setStretchFactor(1, 10);

  // hide output column when not on Prediction tab (output selection is only used in Prediction subtabs)
  outputsGroupBox->setVisible(false);
  connect(tabWidget, &QTabWidget::currentChanged, this, [outputsGroupBox, tabWidget](int index) {
    outputsGroupBox->setVisible(tabWidget->tabText(index) == tr("Prediction"));
  });

  widgetLayout->addWidget(mainWidget, 1);
}

QTabWidget * CalibrationResultWindow::getThetaTabWidget(const CalibrationAnalysis * analysis)
{
  auto * thetaTabWidget = new QTabWidget;

  // tab: Optimal -----------------------------------------
  auto * optimalTab       = new QWidget;
  auto * optimalTabLayout = new QVBoxLayout(optimalTab);

  auto * optimalTabScrollArea = new QScrollArea;
  optimalTabScrollArea->setWidgetResizable(true);

  // optimal value
  auto * thetaGroupBox        = new QGroupBox(tr("Optimal θ"));
  auto * thetaGroupBoxLayout  = new QVBoxLayout(thetaGroupBox);

  // table view
  auto * thetaTableView = new CopyableTableView;
  thetaTableView->verticalHeader()->hide();
  
  // table model
  const int nbInputs = (int) result_.getCalibrationResult().getParameterMAP().getSize();
  auto * thetaTableModel = new CustomStandardItemModel(nbInputs, 3, thetaTableView);
  thetaTableView->setModel(thetaTableModel);

  // horizontal header
  const QString level(QString::number(analysis->getConfidenceIntervalLength() * 100));
  thetaTableModel->setHorizontalHeaderLabels(QStringList() << tr("Input") << tr("Value") << tr("Confidence interval at") + " " + level + "%");

  // theta and confidence interval
  for (int i = 0; i < nbInputs; ++i)
  {
    thetaTableModel->setNotEditableItem(i, 0, analysis->getCalibratedInputs()[i].c_str());
    thetaTableModel->setNotEditableItem(i, 1, result_.getCalibrationResult().getParameterMAP()[i]);
    if ((int) result_.getConfidenceInterval().getDimension() == nbInputs)
      thetaTableModel->setNotEditableItem(i, 2, result_.getConfidenceInterval().getMarginal(i).__str__().c_str());
    else
      thetaTableModel->setNotEditableItem(i, 2, tr("Not available"));
  }

  // resize table
  thetaTableView->resizeToContents();

  thetaGroupBoxLayout->addWidget(thetaTableView);
  optimalTabLayout->addWidget(thetaGroupBox, 0, Qt::AlignTop);

  // residual results table
  const bool isBayesian = result_.getCalibrationResult().isBayesian();
  const UnsignedInteger nbOutput = observations_.getOutputSample().getDimension();
  const int nbResidualCols = isBayesian ? 8 : 4;

  auto * residualsGroupBox = new QGroupBox(tr("Residual results"));
  auto * residualsGroupBoxLayout = new QVBoxLayout(residualsGroupBox);

  auto * residualsTableView = new CopyableTableView;
  residualsTableView->verticalHeader()->hide();

  auto * residualsTableModel = new CustomStandardItemModel(nbOutput, nbResidualCols, residualsTableView);
  residualsTableView->setModel(residualsTableModel);

  if (isBayesian)
    residualsTableModel->setHorizontalHeaderLabels(QStringList() << tr("Output")
      << tr("σ(observations)") << tr("σ(residuals initial)") << tr("σ(residuals calibrated)")
      << tr("CvM normality (calibrated)") << tr("CvM p-value")
      << tr("RMSE initial") << tr("RMSE calibrated"));
  else
    residualsTableModel->setHorizontalHeaderLabels(QStringList() << tr("Output")
      << tr("σ(observations)") << tr("σ(residuals)")
      << tr("RMSE"));

  for (UnsignedInteger out = 0; out < nbOutput; ++out)
  {
    const Sample outputObs(observations_.getOutputSample().getMarginal(out));
    const Sample priorPred(result_.getCalibrationResult().getOutputAtPriorMean().getMarginal(out));
    const Sample priorResiduals(outputObs - priorPred);

    const double obsStd = result_.getCalibrationResult().getObservationsError().getStandardDeviation()[out];
    const double priorResidualStd = priorResiduals.computeStandardDeviation()[0];
    const double priorRMSE = std::sqrt(priorResiduals.computeRawMoment(2)[0]);

    residualsTableModel->setNotEditableItem(out, 0, observations_.getOutputSample().getDescription()[out].c_str());
    residualsTableModel->setNotEditableItem(out, 1, obsStd);

    if (isBayesian)
    {
      const Sample postPred(result_.getCalibrationResult().getOutputAtPosteriorMean().getMarginal(out));
      const Sample postResiduals(outputObs - postPred);
      const double postResidualStd = postResiduals.computeStandardDeviation()[0];
      const TestResult postCvM(NormalityTest::CramerVonMisesNormal(postResiduals, 0.05));
      const double postRMSE = std::sqrt(postResiduals.computeRawMoment(2)[0]);

      residualsTableModel->setNotEditableItem(out, 2, priorResidualStd);
      residualsTableModel->setNotEditableItem(out, 3, postResidualStd);
      residualsTableModel->setNotEditableItem(out, 4, postCvM.getBinaryQualityMeasure() ? tr("True") : tr("False"));
      residualsTableModel->setNotEditableItem(out, 5, postCvM.getPValue());
      residualsTableModel->setNotEditableItem(out, 6, priorRMSE);
      residualsTableModel->setNotEditableItem(out, 7, postRMSE);
    }
    else
    {
      residualsTableModel->setNotEditableItem(out, 2, priorResidualStd);
      residualsTableModel->setNotEditableItem(out, 3, priorRMSE);
    }
  }

  residualsTableView->resizeToContents();
  residualsGroupBoxLayout->addWidget(residualsTableView);
  optimalTabLayout->addWidget(residualsGroupBox, 0, Qt::AlignTop);

  optimalTabLayout->addStretch();
  optimalTabScrollArea->setWidget(optimalTab);

  thetaTabWidget->addTab(optimalTabScrollArea, tr("Optimal"));

  // tab: PDF -----------------------------------------
#ifdef PERSALYS_HAVE_PARAVIEW
  if (SubWindow::SupportsOpenGL_3_2())
  {
    // variable names
    const Description calibratedInputNames(analysis->getCalibratedInputs());
    PhysicalModel model(observations_.getPhysicalModel());
    QStringList calibratedInAxisTitles_(QtOT::GetVariableAxisLabels(model, calibratedInputNames));

    const GridLayout parameterDistributionsGraph(result_.getCalibrationResult().drawParameterDistributions());

    auto * pvWidget = new PVXYChartViewWidget(this, PVServerManagerSingleton::Get(), PVXYChartViewWidget::Trajectories);
    Sample pdfSamplePrior;      // can not initialize size to DefaultPointNumber because of possible discrete distribution
    Sample pdfSamplePosterior;  // can not initialize size to DefaultPointNumber because of possible discrete distribution
    Description sampleDescription;
    QStringList pdfNames;

    for (int i = 0; i < nbInputs; ++i)
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
      for (int i = 0; i < nbInputs; ++i)
        pdfSamplePrior[1][2*i+1] = 0.1 * pdfSamplePosterior.getMarginal(2*i+1).getMax()[0];
    }

    pvWidget->setData(pdfSamplePrior, QColor("#D55E00"));     // vermillion (Okabe-Ito)
    pvWidget->setData(pdfSamplePosterior, QColor("#009E73")); // bluish-green (Okabe-Ito)
    if (result_.getCalibrationResult().isBayesian())
    {
      pvWidget->setRepresentationLabels(QVector<QString>(nbInputs * 2, tr("Prior")).toList(), 0);
      pvWidget->setRepresentationLabels(QVector<QString>(nbInputs * 2, tr("Posterior")).toList(), 1);
    }
    else
    {
      pvWidget->setRepresentationLabels(QVector<QString>(nbInputs * 2, tr("Initial")).toList(), 0);
      pvWidget->setRepresentationLabels(QVector<QString>(nbInputs * 2, tr("Calibrated")).toList(), 1);
    }
    for (int i = 0; i < nbInputs; ++i)
    {
      const QString varX(calibratedInputNames[i].c_str());
      const QString varY((calibratedInputNames[i] + "pdf").c_str());
      pvWidget->setChartTitle(tr("PDF:") + " " + calibratedInputNames[i].c_str(), varX, varY);
      pvWidget->setAxisTitle(vtkAxis::BOTTOM, calibratedInAxisTitles_[i], varX, varY);
      pvWidget->setAxisTitle(vtkAxis::LEFT, tr("Density"), varX, varY);
    }
    pvWidget->setXAxisData(calibratedInputNames[0].c_str());
    pvWidget->setAxisToShow(Description(1, calibratedInputNames[0] + "pdf"));

    auto * settingWidget = new MultiPDFSettingWidget(pvWidget, QtOT::DescriptionToStringList(calibratedInputNames), pdfNames);

    thetaTabWidget->addTab(new WidgetBoundToDockWidget(pvWidget, settingWidget, this), tr("PDF"));
  }
#endif

  return thetaTabWidget;
}

QTabWidget * CalibrationResultWindow::getPredictionTabWidget(const UnsignedInteger i)
{
  auto * predTabWidget = new QTabWidget;
  (void)i;
#ifdef PERSALYS_HAVE_PARAVIEW

  // Okabe-Ito colorblind-safe palette: blue / vermillion / bluish-green
  QMap<QString, QColor> Colors_ = {
    {QObject::tr("Data"),         QColor("#0072B2")},  // dark blue
    {QObject::tr("Prior"),        QColor("#D55E00")},  // vermillion
    {QObject::tr("Posterior"),    QColor("#009E73")},  // bluish-green
    {QObject::tr("Observations"), QColor("#0072B2")},  // dark blue
    {QObject::tr("Initial"),      QColor("#D55E00")},  // vermillion
    {QObject::tr("Calibrated"),   QColor("#009E73")}   // bluish-green
  };

  // get descriptions
  const Description obsInDescription(observations_.getInputSample().getDescription());
  const QStringList obsInNames(QtOT::DescriptionToStringList(obsInDescription));
  const QStringList obsInLabels(QtOT::GetVariableAxisLabels(observations_.getPhysicalModel(), obsInDescription));

  QStringList labels(QStringList() << tr("Observations") << tr("Initial"));

  // output samples
  const Sample outputObs(observations_.getOutputSample().getMarginal(i));

  Sample outputSamples(outputObs);
  outputSamples.stack(result_.getCalibrationResult().getOutputAtPriorMean().getMarginal(i));
  if (result_.getCalibrationResult().getOutputAtPosteriorMean().getSize())
  {
    outputSamples.stack(result_.getCalibrationResult().getOutputAtPosteriorMean().getMarginal(i));
    labels << tr("Calibrated");
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
  sampleDescription.add(tr("Initial residuals").toStdString());
  if (result_.getCalibrationResult().getOutputAtPosteriorMean().getSize())
  {
    sample.stack(outputObs - result_.getCalibrationResult().getOutputAtPosteriorMean().getMarginal(i));
    sampleDescription.add(tr("Calibrated residuals").toStdString());
  }
  sample.setDescription(sampleDescription);

  // create charts
  pqLinksModel * linksModel = pqApplicationCore::instance()->getLinksModel();

  // - table
  auto * pvSpreadSheetWidget = new PVSpreadSheetViewWidget(this, PVServerManagerSingleton::Get());
  predTabWidget->addTab(PVSpreadSheetViewWidget::GetSpreadSheetViewWidget(pvSpreadSheetWidget, sample, getItem()), tr("Table"));

  // - predictions vs observations
  auto * pvWidget = new PVXYChartViewWidget(this, PVServerManagerSingleton::Get(), PVXYChartViewWidget::TrajectoriesPoints);

  pvWidget->PVViewWidget::setData(outputSamples);
  pvWidget->setSerieColors(Colors_);
//   pvWidget->setSerieLineStyles(QMap<QString, int>{{tr("Data"), 1}}); // if line : pb with log scale (points order)
  pvWidget->setXAxisData(tr("Observations"));
  pvWidget->setAxisToShow(outDescription);
  pvWidget->setChartTitle(tr("Predictions vs observations"));
  pvWidget->setAxisTitle(vtkAxis::BOTTOM, tr("Observations"));
  pvWidget->setAxisTitle(vtkAxis::LEFT, tr("Predictions"));
  pvWidget->getLegend()->SetHorizontalAlignment(vtkChartLegend::LEFT);

  auto * chartSetting = new TrajectoriesSettingWidget(pvWidget, labels, this);

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
  if (result_.getCalibrationResult().isBayesian())
    pvWidget->setRepresentationLabels(QStringList() << tr("Normal, hypothesis")
                                      << tr("Normal, hypothesis"));
  else
    pvWidget->setRepresentationLabels(QStringList() << tr("Normal, estimated")
                                      << tr("Normal, estimated"));

  if (result_.getPriorResidualsPDF()[i].getSize())
  {
    pvWidget->setData(result_.getPriorResidualsPDF()[i], Colors_[tr("Prior")]);
    pvWidget->setRepresentationLabels(QStringList() << tr("Initial") << tr("Initial"), 1);
  }
  if (result_.getPosteriorResidualsPDF()[i].getSize())
  {
    pvWidget->setData(result_.getPosteriorResidualsPDF()[i], Colors_[tr("Posterior")]);
    pvWidget->setRepresentationLabels(QStringList() << tr("Calibrated") << tr("Calibrated"), 2);
  }

  pvWidget->setXAxisData("v0");
  pvWidget->setAxisToShow(QStringList() << "v1");
  pvWidget->setChartTitle(tr("Residuals analysis"));
  pvWidget->setAxisTitle(vtkAxis::LEFT, tr("Density"));
  pvWidget->setAxisTitle(vtkAxis::BOTTOM, tr("Residuals"));

  chartSetting = new TrajectoriesSettingWidget(pvWidget, labels, this);

  auto * residualsTab   = new QWidget;
  auto *residualsLayout = new QVBoxLayout(residualsTab);

  auto *graphContainer  = new QWidget;
  auto *graphLayout     = new QVBoxLayout(graphContainer);
  graphLayout->setContentsMargins(0, 0, 0, 0);
  graphLayout->addWidget(new WidgetBoundToDockWidget(pvWidget, chartSetting, this));
  graphContainer->setLayout(graphLayout);
  residualsLayout->addWidget(graphContainer, 1);

  const double std = result_.getCalibrationResult().getObservationsError().getStandardDeviation()[i];
  QString stdLabel = tr("Residuals standard deviation (assuming gaussian distribution): σ = %1").arg(std);
  auto * stdLabelWidget = new QLabel(stdLabel);
  residualsLayout->addWidget(stdLabelWidget);

  predTabWidget->addTab(residualsTab, tr("Residuals"));

  // - residuals QQ-plot

  Graph qqPlotGraph(result_.getCalibrationResult().drawResidualsNormalPlot().getGraph(0, i));
  auto * qqPlot = new PlotWidget(tr("qqPlot"));
  auto * qqPlotSettingWidget = new SimpleGraphSetting(qqPlot, this);
  qqPlotSettingWidget->hide();
  qqPlot->plotCurve(qqPlotGraph.getDrawable(1).getData(), QPen(QColor("#0072B2"), 5), QwtPlotCurve::Dots);
  qqPlot->plotCurve(qqPlotGraph.getDrawable(0).getData(), QPen(QColor("#D55E00"), 1, Qt::DashDotLine));

  qqPlot->setAxisTitle(QwtPlot::xBottom, tr("Residuals"));
  qqPlot->setAxisTitle(QwtPlot::yLeft, tr("Standard normal quantiles"));

  predTabWidget->addTab(new WidgetBoundToDockWidget(qqPlot, qqPlotSettingWidget, this), tr("Residuals QQ-plot"));

#endif

  return predTabWidget;
}

}
