//                                               -*- C++ -*-
/**
 *  @brief QMdiSubWindow for the results of MorrisAnalysis
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
#include "otgui/MorrisResultWindow.hxx"

#include "otgui/MorrisAnalysis.hxx"
#include "otgui/QtTools.hxx"
#include "otgui/ResizableStackedWidget.hxx"
#include "otgui/MorrisResultWidget.hxx"

using namespace OT;

namespace OTGUI
{

MorrisResultWindow::MorrisResultWindow(AnalysisItem * item, QWidget * parent)
  : DataAnalysisWindow(item, parent)
  , morrisResult_(dynamic_cast<MorrisAnalysis*>(item->getAnalysis().getImplementation().get())->getResult())
{
  initialize(item);

  setWindowTitle(tr("Morris analysis results"));

  buildInterface();
}


void MorrisResultWindow::initialize(AnalysisItem* item)
{
  MorrisAnalysis analysis(*dynamic_cast<MorrisAnalysis*>(item->getAnalysis().getImplementation().get()));

  try
  {
    designOfExperiment_ = DesignOfExperiment("fakeName", analysis.getPhysicalModel());
    designOfExperiment_.setInputSample(morrisResult_.getInputSample());
    designOfExperiment_.setOutputSample(morrisResult_.getOutputSample());
  }
  catch (std::exception& ex)
  {
    designOfExperiment_ = DesignOfExperiment();
    designOfExperiment_.setInputSample(morrisResult_.getInputSample());
    designOfExperiment_.setOutputSample(morrisResult_.getOutputSample());
  }

  // parameters widget
  setParameters(item->getAnalysis(), tr("Morris analysis parameters"));
}


void MorrisResultWindow::fillListWidget()
{
  variablesGroupBox_->setTitle(tr("Outputs"));
  variablesListWidget_->addItems(QtOT::DescriptionToStringList(designOfExperiment_.getOutputSample().getDescription()));
  variablesListWidget_->setCurrentRow(0);
}


void MorrisResultWindow::fillTabWidget()
{
  // -- effects tab --------------------------------
  addEffectsTab();

  // -- graph tabs
#ifdef OTGUI_HAVE_PARAVIEW
  addParaviewWidgetsTabs();
#else
  addTableTab();
  addScatterPlotsTab();
#endif
  if (parametersWidget_)
    tabWidget_->addTab(parametersWidget_, tr("Parameters"));
}


void MorrisResultWindow::addEffectsTab()
{
  ResizableStackedWidget * stackWidget = new ResizableStackedWidget;
  for (UnsignedInteger i = 0; i < morrisResult_.getOutputSample().getDimension(); ++i)
  {
    MorrisResultWidget * morrisWidget = new MorrisResultWidget(morrisResult_, i, this);
    stackWidget->addWidget(morrisWidget);
  }
  connect(variablesListWidget_, SIGNAL(currentRowChanged(int)), stackWidget, SLOT(setCurrentIndex(int)));

  tabWidget_->addTab(stackWidget, tr("Elementary effects"));
}
}
