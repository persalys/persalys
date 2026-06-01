//                                               -*- C++ -*-
/**
 *  @brief QMdiSubWindow for the results of the reliability analysis using simulation method
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
#include "persalys/SimulationReliabilityResultWindow.hxx"

#include "persalys/MonteCarloReliabilityAnalysis.hxx"
#include "persalys/ImportanceSamplingAnalysis.hxx"
#include "persalys/FORMImportanceSamplingAnalysis.hxx"
#include "persalys/SimulationReliabilityAnalysis.hxx"
#include "persalys/ReliabilityAnalysis.hxx"
#include "persalys/ParametersTableView.hxx"
#include "persalys/ApproximationResultTabWidget.hxx"
#include "persalys/ResizableStackedWidget.hxx"
#include "persalys/GraphConfigurationWidget.hxx"
#include "persalys/WidgetBoundToDockWidget.hxx"
#include "persalys/TranslationManager.hxx"
#include "persalys/ExportableTableView.hxx"

#include <openturns/Less.hxx>
#include <openturns/LessOrEqual.hxx>
#include <openturns/Greater.hxx>
#include <openturns/GreaterOrEqual.hxx>

#include <qwt_legend.h>
#include <qwt_scale_engine.h>

#include <QVBoxLayout>
#include <QScrollArea>
#include <QSplitter>
#include <QGroupBox>

using namespace OT;

namespace PERSALYS
{

/** Format a single limit-state event as "outputName op threshold". */
static QString formatLimitStateEntry(const OT::String & outputName,
                                     const OT::ComparisonOperator & op,
                                     double threshold)
{
  QString opStr;
  const OT::String opName = op.getImplementation()->getClassName();
  if      (opName == "LessOrEqual")    opStr = "<=";
  else if (opName == "Greater")        opStr = ">";
  else if (opName == "GreaterOrEqual") opStr = ">=";
  else                                 opStr = "<";
  return QString("%1 %2 %3")
    .arg(QString::fromUtf8(outputName.c_str()), opStr, QString::number(threshold));
}

SimulationReliabilityResultWindow::SimulationReliabilityResultWindow(AnalysisItem * item, QWidget * parent)
  : ResultWindow(item, parent)
  , result_(dynamic_cast<SimulationReliabilityAnalysis*>(item->getAnalysis().getImplementation().get())->getResult())
{
  // title
  const QString methodName = TranslationManager::GetTranslatedParameterName(item->getAnalysis().getImplementation()->getParameters()[0].second);
  titleLabel_ = new TitleLabel(methodName);

  // Compute limit state display names
  const auto * reliabilityAnalysis = dynamic_cast<const ReliabilityAnalysis*>(item->getAnalysis().getImplementation().get());
  if (reliabilityAnalysis)
  {
    const LimitState & ls = reliabilityAnalysis->getLimitState();
    const Description outputNames = ls.getOutputNames();
    for (UnsignedInteger i = 0; i < outputNames.getSize(); ++i)
    {
      limitStateNames_ << formatLimitStateEntry(outputNames[i], ls.getOperator(i), ls.getThreshold(i));
      eventThresholds_ << ls.getThreshold(i);
    }
  }

  // FORM result widget
  if (const auto * formISAnalysis = dynamic_cast<const FORMImportanceSamplingAnalysis*>(item->getAnalysis().getImplementation().get()))
  {
    if (formISAnalysis->getLimitState().isSystemLimitState())
    {
      const Description outputNames = formISAnalysis->getLimitState().getOutputNames();
      formTabWidget_ = new ApproximationResultTabWidget(formISAnalysis->getMultiFORMResult(), *formISAnalysis, outputNames, this);
    }
    else
    {
      formTabWidget_ = new ApproximationResultTabWidget(formISAnalysis->getFORMResult(), *formISAnalysis, this);
    }
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

  // get output info (fallback label for single-event case)
  QString outputName(QString::fromUtf8(result_.getSimulationResult().getEvent().getDescription()[0].c_str()));

  // main splitter
  QSplitter * mainWidget = new QSplitter(Qt::Horizontal);

  // - list of limit states
  QGroupBox * outputsGroupBox = new QGroupBox(tr("Limit state"));
  QVBoxLayout * outputsLayoutGroupBox = new QVBoxLayout(outputsGroupBox);

  VariablesListWidget * outputsListWidget = new VariablesListWidget;

  // Use computed limit state display names when available
  QStringList displayNames = limitStateNames_;
  if (displayNames.isEmpty())
  {
    for (UnsignedInteger i = 0; i < result_.getOutputSamples().getSize(); ++i)
      if (!result_.getOutputSamples()[i].getDescription().isEmpty())
        displayNames << QString::fromUtf8(result_.getOutputSamples()[i].getDescription()[0].c_str());
  }
  if (displayNames.isEmpty())
    displayNames << outputName;

  outputsListWidget->addItems(displayNames);
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

  // tab : table of values --------------------------------
  if (result_.getDesignOfExperiment().getSample().getSize() > 0)
    tabWidget->addTab(ExportableTableView::GetSampleTableViewWidget(getItem(), result_.getDesignOfExperiment().getSample()), tr("Table"));

  // tab : model description --------------------------------
  if (modelDescriptionWidget_)
    tabWidget->addTab(modelDescriptionWidget_, tr("Model"));

  // Wire limit state selection to dynamic content
  // Histogram stacked widget (system events only, set by getHistogramTab())
  if (histogramStack_)
    connect(outputsListWidget, &VariablesListWidget::currentRowChanged,
            histogramStack_, &QStackedWidget::setCurrentIndex);

  // FORM results sub-tabs (system FORM IS only)
  if (formTabWidget_)
  {
    if (auto * approxWidget = dynamic_cast<ApproximationResultTabWidget*>(formTabWidget_))
      connect(outputsListWidget, &VariablesListWidget::currentRowChanged,
              approxWidget, &ApproximationResultTabWidget::setCurrentEvent);
  }

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
    valuesList << QtOT::FormatDuration(result_.getElapsedTime());
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
  CustomStandardItemModel * resultsTableModel = new CustomStandardItemModel(3, 3, resultsTable);
  resultsTable->setModel(resultsTableModel);

  // horizontal header
  resultsTableModel->setNotEditableHeaderItem(0, 0, tr("Estimate"));
  resultsTableModel->setNotEditableHeaderItem(0, 1, tr("Value"));
  const Scalar defaultLevel = ResourceMap::GetAsScalar("ProbabilitySimulationResult-DefaultConfidenceLevel") * 100;
  resultsTableModel->setNotEditableHeaderItem(0, 2, tr("Confidence interval\nat") + " " + QString::number(defaultLevel) + "%");

  // Failure probability
  const Scalar pfEstimate = result_.getSimulationResult().getProbabilityEstimate();

  resultsTableModel->setNotEditableHeaderItem(1, 0, tr("Failure probability"));
  resultsTableModel->setNotEditableItem(1, 1, pfEstimate);

  // - confidence interval
  const Interval pfCI(result_.getSimulationResult().getProbabilityDistribution().computeBilateralConfidenceInterval(0.95));
  resultsTableModel->setNotEditableItem(1, 2, pfCI.__str__().c_str());

  // Coefficient of variation
  resultsTableModel->setNotEditableHeaderItem(2, 0, tr("Coefficient of variation"));
  resultsTableModel->setNotEditableItem(2, 1, result_.getSimulationResult().getCoefficientOfVariation());

  // resize to contents
  resultsTable->resizeToContents();

  groupBoxLayout->addWidget(resultsTable);

  tabLayout->addWidget(groupBox, 0, Qt::AlignTop);
  tabLayout->addStretch();

  scrollArea->setWidget(tab);

  return scrollArea;
}


QWidget* SimulationReliabilityResultWindow::getHistogramTab()
{
  auto * scrollArea = new QScrollArea;
  scrollArea->setWidgetResizable(true);

  const Collection<Sample> outSamples = result_.getOutputSamples();

  if (outSamples.getSize() > 1)
  {
    // For system events: one page per limit state, driven by limit state list
    histogramStack_ = new QStackedWidget;

    for (UnsignedInteger i = 0; i < outSamples.getSize(); ++i)
    {
      auto * page       = new QWidget;
      auto * pageLayout = new QVBoxLayout(page);

      if (outSamples[i].getSize() > 0)
      {
        const QString subOutputName(QString::fromUtf8(outSamples[i].getDescription()[0].c_str()));
        auto * plot = new PlotWidget(tr("histogram%1").arg(i));
        plot->plotHistogram(outSamples[i], PlotWidget::Other, 0, tr("%1 distribution").arg(subOutputName));

        // plot threshold line
        if (i < static_cast<UnsignedInteger>(eventThresholds_.size()))
        {
          const double thr = eventThresholds_[i];
          Sample thresholdLine(2, 2);
          thresholdLine(0, 0) = thr;
          thresholdLine(0, 1) = plot->axisInterval(QwtPlot::yLeft).minValue();
          thresholdLine(1, 0) = thr;
          thresholdLine(1, 1) = plot->axisInterval(QwtPlot::yLeft).maxValue();
          plot->plotCurve(thresholdLine, QPen(Qt::red), QwtPlotCurve::Lines, 0, tr("Threshold"));
          plot->insertLegend(new QwtLegend, QwtPlot::BottomLegend);
        }

        plot->setAxisTitle(QwtPlot::xBottom, tr("Values"));
        plot->setAxisTitle(QwtPlot::yLeft, tr("Number of simulations"));
        plot->setTitle(tr("%1 output distribution").arg(subOutputName));

        auto * histogramSettingWidget = new SimpleGraphSetting(plot, this);
        pageLayout->addWidget(new WidgetBoundToDockWidget(plot, histogramSettingWidget, this));
      }

      histogramStack_->addWidget(page);
    }

    scrollArea->setWidget(histogramStack_);
  }
  else
  {
    const QString outputName(QString::fromUtf8(result_.getSimulationResult().getEvent().getDescription()[0].c_str()));

    auto * plot = new PlotWidget(tr("histogram"));
    plot->plotHistogram(result_.getDesignOfExperiment().getOutputSample(), PlotWidget::Other, 0, tr("%1 distribution").arg(outputName));

    // plot threshold
    Sample threshold(2, 2);
    threshold(0, 0) = result_.getSimulationResult().getEvent().getThreshold();
    threshold(0, 1) = plot->axisInterval(QwtPlot::yLeft).minValue();
    threshold(1, 0) = result_.getSimulationResult().getEvent().getThreshold();
    threshold(1, 1) = plot->axisInterval(QwtPlot::yLeft).maxValue();
    plot->plotCurve(threshold, QPen(QColor("#D55E00")), QwtPlotCurve::Lines, 0, tr("Threshold"));

    plot->setAxisTitle(QwtPlot::xBottom, tr("Values"));
    plot->setAxisTitle(QwtPlot::yLeft, tr("Number of simulations"));
    plot->insertLegend(new QwtLegend, QwtPlot::BottomLegend);
    plot->setTitle(tr("%1 output distribution").arg(outputName));

    auto * histogramSettingWidget = new SimpleGraphSetting(plot, this);

    scrollArea->setWidget(new WidgetBoundToDockWidget(plot, histogramSettingWidget, this));
  }

  return scrollArea;
}


QWidget* SimulationReliabilityResultWindow::getConvergenceTab()
{
  QScrollArea * scrollArea = new QScrollArea;
  scrollArea->setWidgetResizable(true);

  // do not use a simple QWidget here otherwise it is not possible to resize the window
  ResizableStackedWidget * stackedWidget = new ResizableStackedWidget;

  PlotWidget * plot = new PlotWidget(tr("convergence"));
  // plot pf convergence
  // Okabe-Ito: vermillion for main estimate, dark blue for confidence interval bounds
  plot->plotCurve(result_.getConvergenceSample(), QPen(QColor("#D55E00")), QwtPlotCurve::Lines, 0, tr("Probability estimate"));
  // plot lower bound
  if (result_.getConvergenceSampleLowerBound().getSize())
    plot->plotCurve(result_.getConvergenceSampleLowerBound(), QPen(QColor("#0072B2")), QwtPlotCurve::Lines, 0, tr("Lower bound"));
  // plot upper bound
  if (result_.getConvergenceSampleUpperBound().getSize())
    plot->plotCurve(result_.getConvergenceSampleUpperBound(), QPen(QColor("#0072B2")), QwtPlotCurve::Lines, 0, tr("Upper bound"));

  plot->setTitle(tr("Probability estimate convergence graph at level 0.95"));
  plot->setAxisTitle(QwtPlot::yLeft, tr("Estimate"));
  plot->setAxisTitle(QwtPlot::xBottom, tr("Number of calls"));
  plot->insertLegend(new QwtLegend, QwtPlot::BottomLegend);
#if (QWT_VERSION >= 0x060100)
  QwtLogScaleEngine * scaleEngin = new QwtLogScaleEngine();
#else
  QwtScaleEngine * scaleEngin = new QwtLog10ScaleEngine();
#endif
  plot->setAxisScaleEngine(QwtPlot::xBottom, scaleEngin);

  stackedWidget->addWidget(plot);

  SimpleGraphSetting * convergenceGraphSettingWidget = new SimpleGraphSetting(plot, this);

  scrollArea->setWidget(new WidgetBoundToDockWidget(stackedWidget, convergenceGraphSettingWidget, this));

  return scrollArea;
}
}
