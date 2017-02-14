//                                               -*- C++ -*-
/**
 *  @brief QMdiSubWindow for the results of SobolAnalysis
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
#include "otgui/SobolResultWindow.hxx"

#include "otgui/SobolAnalysis.hxx"
#include "otgui/SensitivityResultWidget.hxx"
#include "otgui/ResizableStackedWidget.hxx"

#include <QGroupBox>
#include <QScrollArea>
#include <QSplitter>

using namespace OT;

namespace OTGUI {
  
SobolResultWindow::SobolResultWindow(AnalysisItem * item)
  : ResultWindow(item)
  , result_(dynamic_cast<SobolAnalysis*>(&*item->getAnalysis().getImplementation())->getResult())
  , outputsListWidget_(0)
  , tabWidget_(0)
  , warningMessage_("")
{
  setParameters(item->getAnalysis());
  buildInterface();
}


void SobolResultWindow::setParameters(const Analysis & analysis)
{
  const SobolAnalysis * sobolAnalysis = dynamic_cast<const SobolAnalysis*>(&*analysis.getImplementation());

  // add warning if the model has not an independent copula
  if (!sobolAnalysis->getPhysicalModel().getComposedDistribution().hasIndependentCopula())
  {
    warningMessage_ = tr("The model has not an independent copula, the result could be false.");
  }

  // ParametersWidget
  QStringList namesList;
  namesList << tr("Algorithm");
  namesList << tr("Maximum coefficient of variation");
  namesList << tr("Maximum elapsed time");
  namesList << tr("Maximum calls");
  namesList << tr("Block size");
  namesList << tr("Seed");

  QStringList valuesList;
  valuesList << tr("Sobol");
  valuesList << QString::number(sobolAnalysis->getMaximumCoefficientOfVariation());
  if (sobolAnalysis->getMaximumElapsedTime() < (UnsignedInteger)std::numeric_limits<int>::max())
    valuesList << QString::number(sobolAnalysis->getMaximumElapsedTime()) + "(s)";
  else
    valuesList << "- (s)";
  if (sobolAnalysis->getMaximumCalls() < (UnsignedInteger)std::numeric_limits<int>::max())
    valuesList << QString::number(sobolAnalysis->getMaximumCalls());
  else
    valuesList << "-";
  valuesList << QString::number(sobolAnalysis->getBlockSize());
  valuesList << QString::number(sobolAnalysis->getSeed());

  parametersWidget_ = new ParametersWidget(tr("Sensitivity analysis parameters"), namesList, valuesList);
}


void SobolResultWindow::buildInterface()
{
  setWindowTitle(tr("Sobol' analysis results"));

  // get output info
  const UnsignedInteger outputDimension = result_.getOutputNames().getSize();
  QStringList outputNames;
  for (UnsignedInteger i=0; i<outputDimension; ++i)
    outputNames << QString::fromUtf8(result_.getOutputNames()[i].c_str());

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

  // first tab --------------------------------
  QScrollArea * scrollArea = new QScrollArea;
  scrollArea->setWidgetResizable(true);
  QWidget * widget = new QWidget;
  QVBoxLayout * vbox = new QVBoxLayout(widget);

  ResizableStackedWidget * stackedWidget = new ResizableStackedWidget;
  connect(outputsListWidget_, SIGNAL(currentRowChanged(int)), stackedWidget, SLOT(setCurrentIndex(int)));

  for (UnsignedInteger i=0; i<result_.getOutputNames().getSize(); ++i)
  {
    SensitivityResultWidget * indicesResultWidget = new SensitivityResultWidget(i,
                                                                                result_.getFirstOrderIndices()[i],
                                                                                result_.getTotalIndices()[i],
                                                                                result_.getInputNames(),
                                                                                result_.getOutputNames()[i],
                                                                                SensitivityResultWidget::Sobol
                                                                               );
    stackedWidget->addWidget(indicesResultWidget);
    connect(indicesResultWidget, SIGNAL(graphWindowActivated(QWidget*)), this, SIGNAL(graphWindowActivated(QWidget*)));
    connect(outputsListWidget_, SIGNAL(currentRowChanged(int)), indicesResultWidget, SLOT(showHideGraphConfigurationWidget(int)));
    connect(this, SIGNAL(stateChanged(int)), indicesResultWidget, SLOT(showHideGraphConfigurationWidget(int)));
  }
  vbox->addWidget(stackedWidget);

  // add a warning (if the model has not an independent copula when doing a SensitivityAnalysis)
  if (!warningMessage_.isEmpty())
  {
    QLabel * warningLabel = new QLabel(QString("<font color=red>%1</font>").arg(warningMessage_));
    vbox->addWidget(warningLabel);
  }
  scrollArea->setWidget(widget);
  tabWidget_->addTab(scrollArea, tr("Result"));

  // second tab --------------------------------
  if (result_.getElapsedTime() > 0. && result_.getCallsNumber())
  {
    // stop criteria
    QStringList namesList;
    namesList << tr("Elapsed time");
    namesList << tr("Number of calls");
    namesList << tr("Coefficient of variation");

    QStringList valuesList;
    valuesList << QString::number(result_.getElapsedTime()) + " s";
    valuesList << QString::number(result_.getCallsNumber());
    valuesList << QString::number(result_.getCoefficientOfVariation());

    ParametersWidget * parametersWidget = new ParametersWidget(tr("Stop criteria"), namesList, valuesList, true, true);

    tabWidget_->addTab(parametersWidget, tr("Summary"));
  }

  // third tab --------------------------------
  if (parametersWidget_)
    tabWidget_->addTab(parametersWidget_, tr("Parameters"));

  // set widgets
  connect(tabWidget_, SIGNAL(currentChanged(int)), this, SLOT(showHideGraphConfigurationWidget(int)));

  mainWidget->addWidget(tabWidget_);
  mainWidget->setStretchFactor(1, 10);
  outputsListWidget_->setCurrentRow(0);
  setWidget(mainWidget);
}


void SobolResultWindow::showHideGraphConfigurationWidget(int indexTab)
{
  if (indexTab == 0) // indices
    emit stateChanged(outputsListWidget_->currentRow());
  else
    emit graphWindowDeactivated();
}


void SobolResultWindow::showHideGraphConfigurationWidget(Qt::WindowStates oldState, Qt::WindowStates newState)
{
  if (oldState == Qt::WindowMaximized)
    return;

  if (newState == Qt::WindowFullScreen || newState == (Qt::WindowActive|Qt::WindowMaximized))
    showHideGraphConfigurationWidget(tabWidget_->currentIndex());
  else if (newState == Qt::WindowNoState || newState == Qt::WindowMinimized || newState == (Qt::WindowActive|Qt::WindowMinimized))
    showHideGraphConfigurationWidget(-1);
}
}