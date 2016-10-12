//                                               -*- C++ -*-
/**
 *  @brief QMdiSubWindow for the results of functional chaos
 *
 *  Copyright 2015-2016 EDF-Phimeca
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
#include "otgui/FunctionalChaosResultWindow.hxx"

#include "otgui/FunctionalChaosAnalysis.hxx"
#include "otgui/SobolResultWindowWidget.hxx"
#include "otgui/ResizableStackedWidget.hxx"
#include "otgui/ResizableTableViewWithoutScrollBar.hxx"

#include <QVBoxLayout>
#include <QGroupBox>
#include <QHeaderView>

using namespace OT;

namespace OTGUI {
  
FunctionalChaosResultWindow::FunctionalChaosResultWindow(AnalysisItem * item)
  : ResultWindow(item)
  , result_(dynamic_cast<FunctionalChaosAnalysis*>(&*item->getAnalysis().getImplementation())->getResult())
  , outputSample_(dynamic_cast<FunctionalChaosAnalysis*>(&*item->getAnalysis().getImplementation())->getDesignOfExperiment().getOutputSample())
{
  setParameters(item->getAnalysis());
  buildInterface();
}


void FunctionalChaosResultWindow::setParameters(const Analysis & analysis)
{
  const FunctionalChaosAnalysis * chaos = dynamic_cast<const FunctionalChaosAnalysis*>(&*analysis.getImplementation());

  // ParametersWidget
  QStringList namesList;
  namesList << tr("Algorithm");
  namesList << tr("Chaos degree");
  namesList << tr("Sparse");
  namesList << tr("Leave-one-out validation");

  QStringList valuesList;
  valuesList << tr("Functional chaos");
  valuesList << QString::number(chaos->getChaosDegree());
  valuesList << (chaos->getSparseChaos()? tr("yes") : tr("no"));
  valuesList << (chaos->isLeaveOneOutValidation()? tr("yes") : tr("no"));

  parametersWidget_ = new ParametersWidget(tr("Metamodel creation parameters:"), namesList, valuesList);
}


void FunctionalChaosResultWindow::buildInterface()
{
  tabWidget_ = new QTabWidget;

  const UnsignedInteger outputDimension = result_.getSobolResult().getOutputNames().getSize();
  QStringList outputNames;
  for (UnsignedInteger i=0; i<outputDimension; ++i)
    outputNames << QString::fromUtf8(result_.getSobolResult().getOutputNames()[i].c_str());

  // first tab : METAMODEL GRAPH --------------------------------
  QWidget * tab = new QWidget;
  QVBoxLayout * metaModelPlotLayout = new QVBoxLayout(tab);

  QVector<PlotWidget*> listMetaModels;
  ResizableStackedWidget * plotsStackedWidget = new ResizableStackedWidget;
  for (UnsignedInteger i=0; i<outputDimension; ++i)
  {
    PlotWidget *plot = new PlotWidget;
    plot->plotScatter(result_.getMetaModelOutputSample().getMarginal(i), outputSample_.getMarginal(i));
    NumericalSample lineSample(result_.getMetaModelOutputSample().getMarginal(i));
    lineSample.stack(lineSample);
    plot->plotCurve(lineSample, QPen(Qt::black, 1));
    plot->setTitle(tr("Metamodel:") + " " + outputNames[i]);
    plot->setAxisTitle(QwtPlot::xBottom, tr("Metamodel"));
    plot->setAxisTitle(QwtPlot::yLeft, tr("Physical model"));
    listMetaModels.append(plot);
    plotsStackedWidget->addWidget(plot);
  }
  metaModelPlotLayout->addWidget(plotsStackedWidget);
  metaModelPlotsConfigurationWidget_ = new GraphConfigurationWidget(listMetaModels, QStringList(), outputNames, GraphConfigurationWidget::MetaModel);
  connect(metaModelPlotsConfigurationWidget_, SIGNAL(currentPlotChanged(int)), plotsStackedWidget, SLOT(setCurrentIndex(int)));

  tabWidget_->addTab(tab, tr("Scatter plot"));

  // second tab : RESULTS --------------------------------
  QScrollArea * scrollArea = new QScrollArea;
  scrollArea->setWidgetResizable(true);
  tab = new QWidget;
  QVBoxLayout * resultsLayout = new QVBoxLayout(tab);
  resultsLayout->setSizeConstraint(QLayout::SetFixedSize);

  // -- output name --
  QHBoxLayout * headLayout = new QHBoxLayout;
  QLabel * outputName = new QLabel(tr("Output"));
  headLayout->addWidget(outputName);
  QComboBox * outputsComboBox = new QComboBox;
  outputsComboBox->addItems(outputNames);
  headLayout->addWidget(outputsComboBox);
  headLayout->addStretch();
  resultsLayout->addLayout(headLayout);

  // -- validation --
  if (result_.getMetaModelOutputSampleLeaveOneOut().getSize())
  {
    QGroupBox * validationGroupBox = new QGroupBox(tr("Leave-one-out validation"));
    QVBoxLayout * validationGroupBoxLayout = new QVBoxLayout(validationGroupBox);
    ResizableStackedWidget * validationStackedWidget = new ResizableStackedWidget;

    for (UnsignedInteger outputIndex=0; outputIndex<outputDimension; ++outputIndex)
    {
      ResizableTableViewWithoutScrollBar * validationTableView = new ResizableTableViewWithoutScrollBar;
      validationTableView->horizontalHeader()->hide();
      validationTableView->verticalHeader()->hide();
      CustomStandardItemModel * momentsEstimationsTable = new CustomStandardItemModel(2, 2);
      validationTableView->setModel(momentsEstimationsTable);
      // - vertical header
      momentsEstimationsTable->setNotEditableHeaderItem(0, 0, tr("Residual"));
      momentsEstimationsTable->setNotEditableHeaderItem(1, 0, tr("Q2"));

      // - residual/q2 values
      momentsEstimationsTable->setNotEditableItem(0, 1, result_.getErrorQ2LeaveOneOut()[outputIndex]);
      momentsEstimationsTable->setNotEditableItem(1, 1, result_.getQ2LeaveOneOut()[outputIndex]);

      validationTableView->resizeToContents();

      validationStackedWidget->addWidget(validationTableView);
    }
    connect(outputsComboBox, SIGNAL(currentIndexChanged(int)), validationStackedWidget, SLOT(setCurrentIndex(int)));
    validationGroupBoxLayout->addWidget(validationStackedWidget);

    resultsLayout->addWidget(validationGroupBox);
  }

  // -- moments --
  QGroupBox * momentsGroupBox = new QGroupBox(tr("Moments estimates"));
  QVBoxLayout * momentsGroupBoxLayout = new QVBoxLayout(momentsGroupBox);
  ResizableStackedWidget * momentsStackedWidget = new ResizableStackedWidget;

  for (UnsignedInteger outputIndex=0; outputIndex<outputDimension; ++outputIndex)
  {
    ResizableTableViewWithoutScrollBar * momentsEstimationsTableView = new ResizableTableViewWithoutScrollBar;
    momentsEstimationsTableView->horizontalHeader()->hide();
    momentsEstimationsTableView->verticalHeader()->hide();
    CustomStandardItemModel * momentsEstimationsTable = new CustomStandardItemModel(3, 2);
    momentsEstimationsTableView->setModel(momentsEstimationsTable);
    // - vertical header
    momentsEstimationsTable->setNotEditableHeaderItem(0, 0, tr("Estimate"));
    momentsEstimationsTable->setNotEditableHeaderItem(1, 0, tr("Mean"));
    momentsEstimationsTable->setNotEditableHeaderItem(2, 0, tr("Variance"));
    // - horizontal header
    momentsEstimationsTable->setNotEditableHeaderItem(0, 1, tr("Value"));
    // - moments values
    momentsEstimationsTable->setNotEditableItem(1, 1, result_.getMean()[outputIndex]);
    momentsEstimationsTable->setNotEditableItem(2, 1, result_.getVariance()[outputIndex]);

    momentsEstimationsTableView->resizeToContents();

    momentsStackedWidget->addWidget(momentsEstimationsTableView);
  }
  connect(outputsComboBox, SIGNAL(currentIndexChanged(int)), momentsStackedWidget, SLOT(setCurrentIndex(int)));
  momentsGroupBoxLayout->addWidget(momentsStackedWidget);

  resultsLayout->addWidget(momentsGroupBox);
  scrollArea->setWidget(tab);
  tabWidget_->addTab(scrollArea, tr("Summary"));
  

  // third tab : SOBOL INDICES --------------------------------
  SobolResultWindowWidget * sobolScrollArea = new SobolResultWindowWidget(result_.getSobolResult());
  tabWidget_->addTab(sobolScrollArea, tr("Sobol indices"));
  sobolIndicesPlotsConfigurationWidget_ = new GraphConfigurationWidget(sobolScrollArea->getPlotWidgets(), QStringList(), outputNames, GraphConfigurationWidget::SensitivityIndices);
  connect(sobolIndicesPlotsConfigurationWidget_, SIGNAL(currentPlotChanged(int)), sobolScrollArea, SIGNAL(currentPlotChanged(int)));
  connect(sobolIndicesPlotsConfigurationWidget_, SIGNAL(currentPlotChanged(int)), sobolScrollArea, SLOT(setCurrentIndex(int)));

  // fourth tab : PARAMETERS --------------------------------
  if (parametersWidget_)
    tabWidget_->addTab(parametersWidget_, tr("Parameters"));

  //
  connect(tabWidget_, SIGNAL(currentChanged(int)), this, SLOT(showHideGraphConfigurationWidget(int)));
  setWidget(tabWidget_);
}


void FunctionalChaosResultWindow::showHideGraphConfigurationWidget(int indexTab)
{
  switch (indexTab)
  {
    // if a plotWidget is visible
    case 0: // metamodel graph
      if (metaModelPlotsConfigurationWidget_)
        if (!metaModelPlotsConfigurationWidget_->isVisible())
          emit graphWindowActivated(metaModelPlotsConfigurationWidget_);
      break;
    case 2: // sobol indices graph
      if (sobolIndicesPlotsConfigurationWidget_)
        if (!sobolIndicesPlotsConfigurationWidget_->isVisible())
          emit graphWindowActivated(sobolIndicesPlotsConfigurationWidget_);
      break;
    // if no plotWidget is visible
    default:
    {
      emit graphWindowDeactivated();
      break;
    }
  }
}


void FunctionalChaosResultWindow::showHideGraphConfigurationWidget(Qt::WindowStates oldState, Qt::WindowStates newState)
{
  if (oldState == Qt::WindowMaximized)
    return;

  if (newState == Qt::WindowFullScreen || newState == (Qt::WindowActive|Qt::WindowMaximized))
    showHideGraphConfigurationWidget(tabWidget_->currentIndex());
  else if (newState == Qt::WindowNoState || newState == Qt::WindowMinimized || newState == (Qt::WindowActive|Qt::WindowMinimized))
    showHideGraphConfigurationWidget(-1);
}
}