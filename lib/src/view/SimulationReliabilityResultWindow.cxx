//                                               -*- C++ -*-
/**
 *  @brief QMdiSubWindow for the results of the reliability analysis using simulation method
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
#include "otgui/SimulationReliabilityResultWindow.hxx"

#include "otgui/MonteCarloReliabilityAnalysis.hxx"
#include "otgui/ImportanceSamplingAnalysis.hxx"
#include "otgui/FORMImportanceSamplingAnalysis.hxx"
#include "otgui/ParametersTableView.hxx"
#include "otgui/ApproximationResultTabWidget.hxx"
#include "otgui/ResizableStackedWidget.hxx"
#include "otgui/GraphConfigurationWidget.hxx"
#include "otgui/WidgetBoundToDockWidget.hxx"
#include "otgui/TranslationManager.hxx"

#include <qwt_legend.h>
#include <qwt_scale_engine.h>

#include <QVBoxLayout>
#include <QScrollArea>
#include <QSplitter>
#include <QGroupBox>

using namespace OT;

namespace OTGUI
{

SimulationReliabilityResultWindow::SimulationReliabilityResultWindow(AnalysisItem * item, QWidget * parent)
  : ResultWindow(item, parent)
  , result_(dynamic_cast<SimulationReliabilityAnalysis*>(item->getAnalysis().getImplementation().get())->getResult())
  , titleLabel_(0)
  , formTabWidget_(0)
{
  // title
  const QString methodName = TranslationManager::GetTranslatedParameterName(item->getAnalysis().getImplementation()->getParameters()[0].second);
  titleLabel_ = new TitleLabel(methodName);

  // FORM result widget
  if (dynamic_cast<const FORMImportanceSamplingAnalysis*>(item->getAnalysis().getImplementation().get()))
  {
    FORMImportanceSamplingAnalysis analysis = *dynamic_cast<const FORMImportanceSamplingAnalysis*>(item->getAnalysis().getImplementation().get());
    formTabWidget_ = new ApproximationResultTabWidget(analysis.getFORMResult(), analysis, this);
    titleLabel_->setDocLink("user_manual/graphical_interface/probabilistic_analysis/user_manual_probabilistic_analysis.html#formisresult");
  }
  else
  {
    titleLabel_->setDocLink("user_manual/graphical_interface/probabilistic_analysis/user_manual_probabilistic_analysis.html#reliamontecarloresult");
  }

  // parameters widget
  setParameters(item->getAnalysis(), tr("Threshold exceedance parameters"));

  buildInterface();
}


void SimulationReliabilityResultWindow::buildInterface()
{
  QVBoxLayout * widgetLayout = new QVBoxLayout(this);

  widgetLayout->addWidget(titleLabel_);

  // get output info
  QString outputName(QString::fromUtf8(result_.getSimulationResult().getEvent().getDescription()[0].c_str()));

  // main splitter
  QSplitter * mainWidget = new QSplitter(Qt::Horizontal);

  // - list outputs
  QGroupBox * outputsGroupBox = new QGroupBox(tr("Output"));
  QVBoxLayout * outputsLayoutGroupBox = new QVBoxLayout(outputsGroupBox);

  VariablesListWidget * outputsListWidget = new VariablesListWidget;
  outputsListWidget->addItems(QStringList() << outputName);
  outputsListWidget->setCurrentRow(0);
  outputsLayoutGroupBox->addWidget(outputsListWidget);

  mainWidget->addWidget(outputsGroupBox);
  mainWidget->setStretchFactor(0, 1);

  // tab widget
  QTabWidget * tabWidget = new QTabWidget;

  // first tab : summary --------------------------------
  tabWidget->addTab(getSummaryTab(), tr("Summary"));

  // second tab : output histogram --------------------------------
  tabWidget->addTab(getHistogramTab(), tr("Histogram"));

  // third tab : convergence --------------------------------
  tabWidget->addTab(getConvergenceTab(), tr("Convergence graph"));

  // fourth tab : FORM result --------------------------------
  if (formTabWidget_)
    tabWidget->addTab(formTabWidget_, tr("FORM results"));

  // fifth tab : parameters --------------------------------
  if (parametersWidget_)
    tabWidget->addTab(parametersWidget_, tr("Parameters"));

  // tab : model description --------------------------------
  if (modelDescriptionWidget_)
    tabWidget->addTab(modelDescriptionWidget_, tr("Model"));

  //
  mainWidget->addWidget(tabWidget);
  mainWidget->setStretchFactor(1, 10);

  widgetLayout->addWidget(mainWidget, 1);
}


QWidget* SimulationReliabilityResultWindow::getSummaryTab()
{
  QWidget * tab = new QWidget;
  QVBoxLayout * tabLayout = new QVBoxLayout(tab);

  QScrollArea * scrollArea = new QScrollArea;
  scrollArea->setWidgetResizable(true);

  // Stopping criteria
  QGroupBox * parametersGroupBox = new QGroupBox(tr("Stopping criteria"));
  QVBoxLayout * parametersGroupBoxLayout = new QVBoxLayout(parametersGroupBox);

  QStringList namesList;
  // elapsed time
  if (result_.getElapsedTime() > 0.)
    namesList << tr("Elapsed time");
  // sample size
  namesList << tr("Number of calls");

  QStringList valuesList;
  if (result_.getElapsedTime() > 0.)
    valuesList << QString::number(result_.getElapsedTime()) + " s";
  valuesList << QString::number(result_.getSimulationResult().getOuterSampling() * result_.getSimulationResult().getBlockSize());

  ParametersTableView * parametersTable = new ParametersTableView(namesList, valuesList, true, true);
  parametersGroupBoxLayout->addWidget(parametersTable);
  tabLayout->addWidget(parametersGroupBox, 0, Qt::AlignTop);

  // probability estimate table
  QGroupBox * groupBox = new QGroupBox(tr("Failure probability estimate"));
  QVBoxLayout * groupBoxLayout = new QVBoxLayout(groupBox);

  CopyableTableView * resultsTable = new CopyableTableView;
  resultsTable->horizontalHeader()->hide();
  resultsTable->verticalHeader()->hide();
  CustomStandardItemModel * resultsTableModel = new CustomStandardItemModel(4, 4, resultsTable);
  resultsTable->setModel(resultsTableModel);

  // horizontal header
  resultsTableModel->setNotEditableHeaderItem(0, 0, tr("Estimate"));
  resultsTable->setSpan(0, 0, 2, 1);

  resultsTableModel->setNotEditableHeaderItem(0, 1, tr("Value"));
  resultsTable->setSpan(0, 1, 2, 1);

  // Failure probability
  const Scalar pfEstimate = result_.getSimulationResult().getProbabilityEstimate();

  resultsTableModel->setNotEditableHeaderItem(2, 0, tr("Failure probability"));
  resultsTableModel->setNotEditableItem(2, 1, pfEstimate);

  // - lower bound
  resultsTableModel->setNotEditableHeaderItem(1, 2, tr("Lower bound"));
  const double pfCILowerBound = std::max(0.0, pfEstimate - 0.5 * result_.getSimulationResult().getConfidenceLength());
  resultsTableModel->setNotEditableItem(2, 2, pfCILowerBound);

  // - upper bound
  resultsTableModel->setNotEditableHeaderItem(1, 3, tr("Upper bound"));
  const double pfCIUpperBound = std::min(1.0, pfEstimate + 0.5 * result_.getSimulationResult().getConfidenceLength());
  resultsTableModel->setNotEditableItem(2, 3, pfCIUpperBound);

  // Coefficient of variation
  resultsTableModel->setNotEditableHeaderItem(3, 0, tr("Coefficient of variation"));
  resultsTableModel->setNotEditableItem(3, 1, result_.getSimulationResult().getCoefficientOfVariation());

  // resize to contents
  resultsTable->resizeToContents();

  // Confidence interval: do it after resizeToContents
  resultsTableModel->setNotEditableHeaderItem(0, 2, tr("Confidence interval at 95%"));
  resultsTable->setSpan(0, 2, 1, 2);

  groupBoxLayout->addWidget(resultsTable);

  tabLayout->addWidget(groupBox, 0, Qt::AlignTop);
  tabLayout->addStretch();

  scrollArea->setWidget(tab);

  return scrollArea;
}


QWidget* SimulationReliabilityResultWindow::getHistogramTab()
{
  QScrollArea * scrollArea = new QScrollArea;
  scrollArea->setWidgetResizable(true);
  WidgetBoundToDockWidget * plotWidget = new WidgetBoundToDockWidget(this);
  QVBoxLayout * plotWidgetLayout = new QVBoxLayout(plotWidget);

  // get output info
  QString outputName(QString::fromUtf8(result_.getSimulationResult().getEvent().getDescription()[0].c_str()));

  // plot histogram
  PlotWidget * plot = new PlotWidget(tr("histogram"));
  plot->plotHistogram(result_.getDesignOfExperiment().getOutputSample(), 2, 0, tr("%1 distribution").arg(outputName));

  // plot threshold
  Sample threshold(2, 2);
  threshold(0, 0) = result_.getSimulationResult().getEvent().getThreshold();
  threshold(1, 0) = plot->axisInterval(QwtPlot::yLeft).minValue();
  threshold(1, 0) = result_.getSimulationResult().getEvent().getThreshold();
  threshold(1, 1) = plot->axisInterval(QwtPlot::yLeft).maxValue();
  plot->plotCurve(threshold, QPen(Qt::red), QwtPlotCurve::Lines, 0, tr("Threshold"));

  plot->setAxisTitle(QwtPlot::xBottom, tr("Values"));
  plot->setAxisTitle(QwtPlot::yLeft, tr("Number of simulations"));
  plot->insertLegend(new QwtLegend, QwtPlot::BottomLegend);
  plot->setTitle(tr("%1 output distribution").arg(outputName));

  plotWidgetLayout->addWidget(plot);

  GraphConfigurationWidget * histogramSettingWidget = new GraphConfigurationWidget(plot,
      QStringList(),
      QStringList(),
      GraphConfigurationWidget::NoType,
      this);
  plotWidget->setDockWidget(histogramSettingWidget);
  scrollArea->setWidget(plotWidget);

  return scrollArea;
}


QWidget* SimulationReliabilityResultWindow::getConvergenceTab()
{
  QScrollArea * scrollArea = new QScrollArea;
  scrollArea->setWidgetResizable(true);
  WidgetBoundToDockWidget * plotWidget = new WidgetBoundToDockWidget(this);
  QVBoxLayout * plotWidgetLayout = new QVBoxLayout(plotWidget);

  // do not use a simple QWidget here otherwise it is not possible to resize the window
  ResizableStackedWidget * stackedWidget = new ResizableStackedWidget;

  PlotWidget * plot = new PlotWidget(tr("convergence"));
  // plot pf convergence
  plot->plotCurve(result_.getConvergenceSample(), QPen(Qt::red), QwtPlotCurve::Lines, 0, tr("Probability estimate"));
  // plot lower bound
  if (result_.getConvergenceSampleLowerBound().getSize())
    plot->plotCurve(result_.getConvergenceSampleLowerBound(), QPen(Qt::green), QwtPlotCurve::Lines, 0, tr("Lower bound"));
  // plot upper bound
  if (result_.getConvergenceSampleUpperBound().getSize())
    plot->plotCurve(result_.getConvergenceSampleUpperBound(), QPen(Qt::green), QwtPlotCurve::Lines, 0, tr("Upper bound"));

  plot->setTitle(tr("Probability estimate convergence graph at level 0.95"));
  plot->setAxisTitle(QwtPlot::yLeft, tr("Estimate"));
  plot->setAxisTitle(QwtPlot::xBottom, tr("Outer iteration"));
  plot->insertLegend(new QwtLegend, QwtPlot::BottomLegend);
#if (QWT_VERSION >= 0x060100)
  QwtLogScaleEngine * scaleEngin = new QwtLogScaleEngine();
#else
  QwtScaleEngine * scaleEngin = new QwtLog10ScaleEngine();
#endif
  plot->setAxisScaleEngine(QwtPlot::xBottom, scaleEngin);

  stackedWidget->addWidget(plot);

  GraphConfigurationWidget * convergenceGraphSettingWidget = new GraphConfigurationWidget(plot,
      QStringList(),
      QStringList(),
      GraphConfigurationWidget::NoType,
      this);
  plotWidget->setDockWidget(convergenceGraphSettingWidget);
  plotWidgetLayout->addWidget(stackedWidget);
  scrollArea->setWidget(plotWidget);

  return scrollArea;
}
}
