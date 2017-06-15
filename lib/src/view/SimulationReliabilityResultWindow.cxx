//                                               -*- C++ -*-
/**
 *  @brief QMdiSubWindow for the results of the reliability analysis using simulation method
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
#include "otgui/SimulationReliabilityResultWindow.hxx"

#include "otgui/MonteCarloReliabilityAnalysis.hxx"
#include "otgui/ImportanceSamplingAnalysis.hxx"
#include "otgui/FORMImportanceSamplingAnalysis.hxx"
#include "otgui/ParametersTableView.hxx"
#include "otgui/ApproximationResultTabWidget.hxx"
#include "otgui/ResizableStackedWidget.hxx"

#include <qwt_legend.h>
#include <qwt_scale_engine.h>

#include <QScrollArea>
#include <QSplitter>
#include <QListWidget>
#include <QGroupBox>

using namespace OT;

namespace OTGUI {

SimulationReliabilityResultWindow::SimulationReliabilityResultWindow(AnalysisItem * item)
  : ResultWindow(item)
  , result_(dynamic_cast<SimulationReliabilityAnalysis*>(&*item->getAnalysis().getImplementation())->getResult())
  , tabWidget_(0)
  , formTabWidget_(0)
  , histogramConfigurationWidget_(0)
  , convergenceGraphConfigurationWidget_(0)
{
  // FORM result widget
  if (dynamic_cast<const FORMImportanceSamplingAnalysis*>(&*item->getAnalysis().getImplementation()))
  {
    FORMImportanceSamplingAnalysis analysis = *dynamic_cast<const FORMImportanceSamplingAnalysis*>(&*item->getAnalysis().getImplementation());
    formTabWidget_ = new ApproximationResultTabWidget(analysis.getFORMResult(), analysis, this);
  }

  setParameters(item->getAnalysis());
  buildInterface();
}


SimulationReliabilityResultWindow::~SimulationReliabilityResultWindow()
{
  delete histogramConfigurationWidget_;
  delete convergenceGraphConfigurationWidget_;
  histogramConfigurationWidget_ = 0;
  convergenceGraphConfigurationWidget_ = 0;
}


void SimulationReliabilityResultWindow::setParameters(const Analysis & analysis)
{
  const SimulationReliabilityAnalysis * simuAnalysis = dynamic_cast<const SimulationReliabilityAnalysis*>(&*analysis.getImplementation());

  // - parameters names
  QStringList namesList;
  namesList << tr("Algorithm");
  if (dynamic_cast<const ImportanceSamplingAnalysis*>(&*analysis.getImplementation()))
    namesList << tr("Design point (standard space)");
  namesList << tr("Maximum coefficient of variation");
  namesList << tr("Maximum elapsed time");
  namesList << tr("Maximum calls");
  namesList << tr("Block size");
  namesList << tr("Seed");

  // - parameters values
  QStringList valuesList;

  // algo
  if (dynamic_cast<const MonteCarloReliabilityAnalysis*>(&*analysis.getImplementation()))
    valuesList << tr("Monte Carlo");
  else if (dynamic_cast<const ImportanceSamplingAnalysis*>(&*analysis.getImplementation()))
  {
    if (dynamic_cast<const FORMImportanceSamplingAnalysis*>(&*analysis.getImplementation()))
      valuesList << tr("FORM - Importance sampling");
    else
      valuesList << tr("Importance sampling");

    // starting point
    const Point startingPoint(dynamic_cast<const ImportanceSamplingAnalysis*>(&*analysis.getImplementation())->getStandardSpaceDesignPoint());
    QString startingPointText = "[";
    for (UnsignedInteger i=0; i<startingPoint.getDimension(); ++i)
    {
      startingPointText += QString::number(startingPoint[i]);
      if (i < startingPoint.getDimension()-1)
        startingPointText += "; ";
    }
    startingPointText += "]";
    valuesList << startingPointText;
  }
  else
    return;

  // stop criteria
  valuesList << QString::number(simuAnalysis->getMaximumCoefficientOfVariation());
  if (simuAnalysis->getMaximumElapsedTime() < (UnsignedInteger)std::numeric_limits<int>::max())
    valuesList << QString::number(simuAnalysis->getMaximumElapsedTime()) + "(s)";
  else
    valuesList << "- (s)";
  if (simuAnalysis->getMaximumCalls() < (UnsignedInteger)std::numeric_limits<int>::max())
    valuesList << QString::number(simuAnalysis->getMaximumCalls());
  else
    valuesList << "-";
  valuesList << QString::number(simuAnalysis->getBlockSize());

  // seed
  valuesList << QString::number(simuAnalysis->getSeed()); 

  if (namesList.size() == valuesList.size())
    parametersWidget_ = new ParametersWidget(tr("Threshold exceedance parameters"), namesList, valuesList);
}


void SimulationReliabilityResultWindow::buildInterface()
{
  setWindowTitle(tr("Threshold exceedance results"));

  // get output info
  QString outputName(QString::fromUtf8(result_.getSimulationResult().getEvent().getDescription()[0].c_str()));

  // main splitter
  QSplitter * mainWidget = new QSplitter(Qt::Horizontal);

  // - list outputs
  QGroupBox * outputsGroupBox = new QGroupBox(tr("Output"));
  QVBoxLayout * outputsLayoutGroupBox = new QVBoxLayout(outputsGroupBox);

  QListWidget * outputsListWidget = new QListWidget;
  outputsListWidget->addItems(QStringList() << outputName);
  outputsLayoutGroupBox->addWidget(outputsListWidget);

  mainWidget->addWidget(outputsGroupBox);
  mainWidget->setStretchFactor(0, 1);

  // tab widget
  tabWidget_ = new QTabWidget;

  // first tab : summary --------------------------------
  tabWidget_->addTab(getSummaryTab(), tr("Summary"));

  // second tab : output histogram --------------------------------
  tabWidget_->addTab(getHistogramTab(), tr("Histogram"));

  // third tab : convergence --------------------------------
  tabWidget_->addTab(getConvergenceTab(), tr("Convergence graph"));

  // fourth tab : FORM result --------------------------------
  if (formTabWidget_)
    tabWidget_->addTab(formTabWidget_, tr("FORM results"));

  // fifth tab : parameters --------------------------------
  if (parametersWidget_)
    tabWidget_->addTab(parametersWidget_, tr("Parameters"));

  //
  connect(tabWidget_, SIGNAL(currentChanged(int)), this, SLOT(showHideGraphConfigurationWidget(int)));
  mainWidget->addWidget(tabWidget_);
  mainWidget->setStretchFactor(1, 10);

  setWidget(mainWidget);
}


QWidget* SimulationReliabilityResultWindow::getSummaryTab()
{
  QWidget * tab = new QWidget;
  QVBoxLayout * tabLayout = new QVBoxLayout(tab);

  QScrollArea * scrollArea = new QScrollArea;
  scrollArea->setWidgetResizable(true);
  tabLayout->setSizeConstraint(QLayout::SetFixedSize);

  // stop criteria
  QGroupBox * parametersGroupBox = new QGroupBox(tr("Stop criteria"));
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
  tabLayout->addWidget(parametersGroupBox);

  // probability estimate table
  QGroupBox * groupBox = new QGroupBox(tr("Failure probability estimate"));
  QVBoxLayout * groupBoxLayout = new QVBoxLayout(groupBox);

  ResizableTableViewWithoutScrollBar * resultsTable = new ResizableTableViewWithoutScrollBar;
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

  tabLayout->addWidget(groupBox);

  scrollArea->setWidget(tab);

  return scrollArea;
}


QWidget* SimulationReliabilityResultWindow::getHistogramTab()
{
  QWidget * tab = new QWidget;
  QVBoxLayout * tabLayout = new QVBoxLayout(tab);

  // get output info
  QString outputName(QString::fromUtf8(result_.getSimulationResult().getEvent().getDescription()[0].c_str()));

  // plot histogram
  QVector<PlotWidget*> listHistogram;
  PlotWidget * plot = new PlotWidget("histogram");
  plot->plotHistogram(result_.getOutputSample(), 2, 0, tr("%1 distribution").arg(outputName));

  // plot threshold
  Sample threshold = Sample(2, 2);
  threshold[0][0] = result_.getSimulationResult().getEvent().getThreshold();
  threshold[1][0] = plot->axisInterval(QwtPlot::yLeft).minValue();
  threshold[1][0] = result_.getSimulationResult().getEvent().getThreshold();
  threshold[1][1] = plot->axisInterval(QwtPlot::yLeft).maxValue();
  plot->plotCurve(threshold, QPen(Qt::red), QwtPlotCurve::Lines, 0, tr("Threshold"));

  plot->setAxisTitle(QwtPlot::xBottom, tr("Values"));
  plot->setAxisTitle(QwtPlot::yLeft, tr("Number of simulations"));
  plot->insertLegend(new QwtLegend, QwtPlot::BottomLegend);
  plot->setTitle(tr("%1 output distribution").arg(outputName));

  listHistogram.append(plot);
  tabLayout->addWidget(plot);

  histogramConfigurationWidget_ = new GraphConfigurationWidget(listHistogram);

  return tab;
}


QWidget* SimulationReliabilityResultWindow::getConvergenceTab()
{
  // do not use a simple QWidget here otherwise it is not possible to resize the window
  ResizableStackedWidget * tab = new ResizableStackedWidget;

  QVector<PlotWidget*> listConvergenceGraph;
  PlotWidget * plot = new PlotWidget("convergence");
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

  listConvergenceGraph.append(plot);
  tab->addWidget(plot);

  convergenceGraphConfigurationWidget_ = new GraphConfigurationWidget(listConvergenceGraph);

  return tab;
}


void SimulationReliabilityResultWindow::showHideGraphConfigurationWidget(int indexTab)
{
  switch (indexTab)
  {
    // if a plotWidget is visible
    case 1: // histogram
      if (histogramConfigurationWidget_)
        if (!histogramConfigurationWidget_->isVisible())
          emit graphWindowActivated(histogramConfigurationWidget_);
      break;
    case 2: // convergence graph
      if (convergenceGraphConfigurationWidget_)
        if (!convergenceGraphConfigurationWidget_->isVisible())
          emit graphWindowActivated(convergenceGraphConfigurationWidget_);
      break;
    // if no plotWidget is visible
    default:
    {
      emit graphWindowDeactivated();
      break;
    }
  }
}


void SimulationReliabilityResultWindow::showHideGraphConfigurationWidget(Qt::WindowStates oldState, Qt::WindowStates newState)
{
  if (oldState == Qt::WindowMaximized)
    return;

  if (newState == Qt::WindowFullScreen || newState == (Qt::WindowActive|Qt::WindowMaximized))
    showHideGraphConfigurationWidget(tabWidget_->currentIndex());
  else if (newState == Qt::WindowNoState || newState == Qt::WindowMinimized || newState == (Qt::WindowActive|Qt::WindowMinimized))
    showHideGraphConfigurationWidget(-1);
}
}