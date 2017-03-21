//                                               -*- C++ -*-
/**
 *  @brief QMdiSubWindow for the results of functional chaos
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
#include "otgui/FunctionalChaosResultWindow.hxx"

#include "otgui/FunctionalChaosAnalysis.hxx"
#include "otgui/ResizableStackedWidget.hxx"
#include "otgui/MetaModelValidationWidget.hxx"
#include "otgui/SensitivityResultWidget.hxx"

#include <QGroupBox>
#include <QSplitter>
#include <QScrollArea>

using namespace OT;

namespace OTGUI {

FunctionalChaosResultWindow::FunctionalChaosResultWindow(AnalysisItem * item)
  : ResultWindow(item)
  , result_(dynamic_cast<FunctionalChaosAnalysis*>(&*item->getAnalysis().getImplementation())->getResult())
  , outputsListWidget_(0)
  , tabWidget_(0)
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

  parametersWidget_ = new ParametersWidget(tr("Metamodel creation parameters"), namesList, valuesList);
}


void FunctionalChaosResultWindow::buildInterface()
{
  setWindowTitle(tr("Functional chaos results"));

  // get output info
  const UnsignedInteger outputDimension = result_.getSobolResult().getOutputNames().getSize();
  QStringList outputNames;
  for (UnsignedInteger i=0; i<outputDimension; ++i)
    outputNames << QString::fromUtf8(result_.getSobolResult().getOutputNames()[i].c_str());

  // main splitter
  QSplitter * mainWidget = new QSplitter(Qt::Horizontal);

  // - list outputs
  QGroupBox * outputsGroupBox = new QGroupBox(tr("Outputs"));
  QVBoxLayout * outputsLayoutGroupBox = new QVBoxLayout(outputsGroupBox);

  outputsListWidget_ = new QListWidget;
  outputsListWidget_->addItems(outputNames);
  outputsLayoutGroupBox->addWidget(outputsListWidget_);

  mainWidget->addWidget(outputsGroupBox);
  mainWidget->setStretchFactor(0, 1);

  // - tab widget
  tabWidget_ = new QTabWidget;

  // first tab : METAMODEL GRAPH --------------------------------
  QWidget * tab = new QWidget;
  QVBoxLayout * metaModelPlotLayout = new QVBoxLayout(tab);

  ResizableStackedWidget * plotsStackedWidget = new ResizableStackedWidget;
  connect(outputsListWidget_, SIGNAL(currentRowChanged(int)), plotsStackedWidget, SLOT(setCurrentIndex(int)));
  for (UnsignedInteger i=0; i<outputDimension; ++i)
  {
    MetaModelValidationWidget * validationWidget = new MetaModelValidationWidget(i,
                                                                                 result_.getMetaModelOutputSample().getMarginal(i),
                                                                                 result_.getOutputSample().getMarginal(i),
                                                                                 result_.getFunctionalChaosResult().getResiduals()[i],
                                                                                 result_.getFunctionalChaosResult().getRelativeErrors()[i],
                                                                                 tr("R2")
                                                                                );

    plotsStackedWidget->addWidget(validationWidget);
    connect(validationWidget, SIGNAL(graphWindowActivated(QWidget*)), this, SIGNAL(graphWindowActivated(QWidget*)));
    connect(outputsListWidget_, SIGNAL(currentRowChanged(int)), validationWidget, SLOT(showHideGraphConfigurationWidget(int)));
    connect(this, SIGNAL(stateChanged(int)), validationWidget, SLOT(showHideGraphConfigurationWidget(int)));
  }
  metaModelPlotLayout->addWidget(plotsStackedWidget);

  tabWidget_->addTab(tab, tr("Metamodel"));

  // second tab : MOMENTS --------------------------------
  QWidget * momentsWidget = new QWidget;
  QVBoxLayout * momentsWidgetLayout = new QVBoxLayout(momentsWidget);
  QGroupBox * momentsGroupBox = new QGroupBox(tr("Moments estimates"));
  QVBoxLayout * momentsGroupBoxLayout = new QVBoxLayout(momentsGroupBox);
  ResizableStackedWidget * momentsStackedWidget = new ResizableStackedWidget;

  for (UnsignedInteger outputIndex=0; outputIndex<outputDimension; ++outputIndex)
  {
    ResizableTableViewWithoutScrollBar * momentsEstimationsTableView = new ResizableTableViewWithoutScrollBar;
    momentsEstimationsTableView->horizontalHeader()->hide();
    momentsEstimationsTableView->verticalHeader()->hide();
    CustomStandardItemModel * momentsEstimationsTable = new CustomStandardItemModel(3, 2, momentsEstimationsTableView);
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
  momentsGroupBoxLayout->addWidget(momentsStackedWidget);

  connect(outputsListWidget_, SIGNAL(currentRowChanged(int)), momentsStackedWidget, SLOT(setCurrentIndex(int)));

  momentsWidgetLayout->addWidget(momentsGroupBox);
  tabWidget_->addTab(momentsWidget, tr("Moments"));

  // third tab : SOBOL INDICES --------------------------------
  QScrollArea * sobolScrollArea = new QScrollArea;
  sobolScrollArea->setWidgetResizable(true);
  QWidget * widget = new QWidget;
  QVBoxLayout * vbox = new QVBoxLayout(widget);
  ResizableStackedWidget * sobolStackedWidget = new ResizableStackedWidget;
  connect(outputsListWidget_, SIGNAL(currentRowChanged(int)), sobolStackedWidget, SLOT(setCurrentIndex(int)));
  for (UnsignedInteger i=0; i<outputDimension; ++i)
  {
    SensitivityResultWidget * sobolResultWidget = new SensitivityResultWidget(i,
                                                                              result_.getSobolResult().getFirstOrderIndices()[i],
                                                                              result_.getSobolResult().getTotalIndices()[i],
                                                                              result_.getSobolResult().getInputNames(),
                                                                              result_.getSobolResult().getOutputNames()[i],
                                                                              SensitivityResultWidget::Sobol
                                                                             );
    sobolStackedWidget->addWidget(sobolResultWidget);
    connect(sobolResultWidget, SIGNAL(graphWindowActivated(QWidget*)), this, SIGNAL(graphWindowActivated(QWidget*)));
    connect(outputsListWidget_, SIGNAL(currentRowChanged(int)), sobolResultWidget, SLOT(showHideGraphConfigurationWidget(int)));
    connect(this, SIGNAL(stateChanged(int)), sobolResultWidget, SLOT(showHideGraphConfigurationWidget(int)));
  }
  vbox->addWidget(sobolStackedWidget);
  sobolScrollArea->setWidget(widget);
  tabWidget_->addTab(sobolScrollArea, tr("Sobol indices"));

  // fourth tab : VALIDATION --------------------------------
  if (result_.getMetaModelOutputSampleLeaveOneOut().getSize())
  {
    QTabWidget * validationTabWidget = new QTabWidget;

    tab = new QWidget;
    metaModelPlotLayout = new QVBoxLayout(tab);

    ResizableStackedWidget * plotsLOOStackedWidget = new ResizableStackedWidget;
    connect(outputsListWidget_, SIGNAL(currentRowChanged(int)), plotsLOOStackedWidget, SLOT(setCurrentIndex(int)));
    for (UnsignedInteger i=0; i<outputDimension; ++i)
    {
      MetaModelValidationWidget * validationWidget = new MetaModelValidationWidget(i,
                                                                                   result_.getMetaModelOutputSampleLeaveOneOut().getMarginal(i),
                                                                                   result_.getOutputSample().getMarginal(i),
                                                                                   result_.getErrorQ2LeaveOneOut()[i],
                                                                                   result_.getQ2LeaveOneOut()[i],
                                                                                   tr("Q2")
                                                                                  );
      plotsLOOStackedWidget->addWidget(validationWidget);
  
      connect(validationWidget, SIGNAL(graphWindowActivated(QWidget*)), this, SIGNAL(graphWindowActivated(QWidget*)));
      connect(outputsListWidget_, SIGNAL(currentRowChanged(int)), validationWidget, SLOT(showHideGraphConfigurationWidget(int)));
      connect(this, SIGNAL(stateChanged(int)), validationWidget, SLOT(showHideGraphConfigurationWidget(int)));
    }
    metaModelPlotLayout->addWidget(plotsLOOStackedWidget);

    validationTabWidget->addTab(plotsLOOStackedWidget, tr("Leave-one-out"));
    tabWidget_->addTab(validationTabWidget, tr("Validation"));
  }

  // fourth/fifth tab : PARAMETERS --------------------------------
  if (parametersWidget_)
    tabWidget_->addTab(parametersWidget_, tr("Parameters"));

  // set widgets
  connect(tabWidget_, SIGNAL(currentChanged(int)), this, SLOT(showHideGraphConfigurationWidget(int)));

  mainWidget->addWidget(tabWidget_);
  mainWidget->setStretchFactor(1, 10);
  outputsListWidget_->setCurrentRow(0);
  setWidget(mainWidget);
}


void FunctionalChaosResultWindow::showHideGraphConfigurationWidget(int indexTab)
{
  if (indexTab == 0 || // metamodel
      indexTab == 2 || // sobol
      indexTab == 3    // validation
     )
    emit stateChanged(outputsListWidget_->currentRow());
  else
    emit graphWindowDeactivated();
}


void FunctionalChaosResultWindow::showHideGraphConfigurationWidget(Qt::WindowStates oldState, Qt::WindowStates newState)
{
  if (oldState == Qt::WindowMaximized)
    return;

  if (newState == Qt::WindowFullScreen || newState == (Qt::WindowActive|Qt::WindowMaximized))
    showHideGraphConfigurationWidget(tabWidget_->currentIndex());
  else if (newState == Qt::WindowNoState || newState == Qt::WindowMinimized || newState == (Qt::WindowActive|Qt::WindowMinimized))
    emit graphWindowDeactivated();
}
}