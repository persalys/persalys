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
#include "otgui/SobolResultWindowWidget.hxx"

#include <QVBoxLayout>

using namespace OT;

namespace OTGUI {
  
SobolResultWindow::SobolResultWindow(AnalysisItem * item)
  : ResultWindow(item)
  , result_(dynamic_cast<SobolAnalysis*>(&*item->getAnalysis().getImplementation())->getResult())
  , plotsConfigurationWidget_(0)
  , warningMessage_("")
{
  setParameters(item->getAnalysis());
  buildInterface();
}


SobolResultWindow::~SobolResultWindow()
{
  delete plotsConfigurationWidget_;
  plotsConfigurationWidget_ = 0;
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

  parametersWidget_ = new ParametersWidget(tr("Sensitivity analysis parameters:"), namesList, valuesList);
}


void SobolResultWindow::buildInterface()
{
  tabWidget_ = new QTabWidget;

  // first tab --------------------------------
  SobolResultWindowWidget * scrollArea = new SobolResultWindowWidget(result_, warningMessage_);
  tabWidget_->addTab(scrollArea, tr("Result"));

  QStringList outputNames;
  for (UnsignedInteger i=0; i<result_.getOutputNames().getSize(); ++i)
    outputNames << QString::fromUtf8(result_.getOutputNames()[i].c_str());

  plotsConfigurationWidget_ = new GraphConfigurationWidget(scrollArea->getPlotWidgets(), QStringList(), outputNames, GraphConfigurationWidget::SensitivityIndices);
  connect(plotsConfigurationWidget_, SIGNAL(currentPlotChanged(int)), scrollArea, SIGNAL(currentPlotChanged(int)));
  connect(plotsConfigurationWidget_, SIGNAL(currentPlotChanged(int)), scrollArea, SLOT(setCurrentIndex(int)));

  // second tab --------------------------------
  if (result_.getElapsedTime() > 0. && result_.getCallsNumber())
  {
    QWidget * tab = new QWidget;
    QVBoxLayout * tabLayout = new QVBoxLayout(tab);

    // elapsed time
    QLabel * elapsedTimeLabel = new QLabel(tr("Elapsed time:") + " " + QString::number(result_.getElapsedTime()) + " s");
    elapsedTimeLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);
    tabLayout->addWidget(elapsedTimeLabel);

    // sample size
    QLabel * nbSimuLabel = new QLabel(tr("Number of calls:") + " " + QString::number(result_.getCallsNumber()));
    nbSimuLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);
    tabLayout->addWidget(nbSimuLabel);

    // coefficient Of Variation
    QLabel * coefLabel = new QLabel(tr("Coefficient of variation:") + " " + QString::number(result_.getCoefficientOfVariation()));
    coefLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);
    tabLayout->addWidget(coefLabel);

    tabLayout->addStretch();
    tabWidget_->addTab(tab, tr("Summary"));
  }

  // third tab --------------------------------
  if (parametersWidget_)
    tabWidget_->addTab(parametersWidget_, tr("Parameters"));

  //
  connect(tabWidget_, SIGNAL(currentChanged(int)), this, SLOT(showHideGraphConfigurationWidget(int)));
  setWidget(tabWidget_);
}


void SobolResultWindow::showHideGraphConfigurationWidget(int indexTab)
{
  switch (indexTab)
  {
    // if a plotWidget is visible
    case 0: // sobol indices graph
      if (plotsConfigurationWidget_)
        if (!plotsConfigurationWidget_->isVisible())
          emit graphWindowActivated(plotsConfigurationWidget_);
      break;
    // if no plotWidget is visible
    default:
    {
      emit graphWindowDeactivated();
      break;
    }
  }
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