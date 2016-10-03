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

  QStringList strList;
  strList << tr("Sensitivity analysis parameters :") + "\n";
  strList << tr("Algorithm : ") + tr("Sobol");
  strList << tr("Maximum coefficient of variation : ") + QString::number(sobolAnalysis->getMaximumCoefficientOfVariation());
  if (sobolAnalysis->getMaximumElapsedTime() < std::numeric_limits<int>::max())
    strList << tr("Maximum elapsed time : ") + QString::number(sobolAnalysis->getMaximumElapsedTime()) + "(s)";
  else
    strList << tr("Maximum elapsed time : ") + "- (s)";
  if (sobolAnalysis->getMaximumCalls() < std::numeric_limits<int>::max())
    strList << tr("Maximum calls : ") + QString::number(sobolAnalysis->getMaximumCalls());
  else
    strList << tr("Maximum calls : ") + "-";

  strList << tr("Block size : ") + QString::number(sobolAnalysis->getBlockSize());
  strList << tr("Seed : ") + QString::number(sobolAnalysis->getSeed());

  parameters_ = strList.join("\n");
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
  tabWidget_->addTab(buildParametersTextEdit(), tr("Parameters"));

  //
  connect(tabWidget_, SIGNAL(currentChanged(int)), this, SLOT(showHideGraphConfigurationWidget(int)));
  setWidget(tabWidget_);
}


void SobolResultWindow::showHideGraphConfigurationWidget(int indexTab)
{
  switch (indexTab)
  {
    // if a plotWidget is visible
    case 0:
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
  if (oldState == 2)
    return;

  if (newState == 4 || newState == 10)
    showHideGraphConfigurationWidget(tabWidget_->currentIndex());
  else if (newState == 0 || newState == 1 || newState == 9)
    showHideGraphConfigurationWidget(-1);
}
}