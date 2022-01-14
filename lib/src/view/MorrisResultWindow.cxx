//                                               -*- C++ -*-
/**
 *  @brief QMdiSubWindow for the results of MorrisAnalysis
 *
 *  Copyright 2015-2022 EDF-Phimeca
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
#include "persalys/MorrisResultWindow.hxx"

#include "persalys/MorrisAnalysis.hxx"
#include "persalys/QtTools.hxx"
#include "persalys/ResizableStackedWidget.hxx"
#include "persalys/MorrisResultWidget.hxx"

using namespace OT;

namespace PERSALYS
{

MorrisResultWindow::MorrisResultWindow(AnalysisItem * item, QWidget * parent)
  : DataAnalysisWindow(item, parent)
  , morrisResult_(dynamic_cast<MorrisAnalysis*>(item->getAnalysis().getImplementation().get())->getResult())
{
  initialize(item);

  buildInterface();
}


void MorrisResultWindow::initialize(AnalysisItem* item)
{
  MorrisAnalysis analysis(*dynamic_cast<MorrisAnalysis*>(item->getAnalysis().getImplementation().get()));

  designOfExperiment_ = morrisResult_.getDesignOfExperiment();

  titleLabel_->setText(tr("Morris method"));
  titleLabel_->setDocLink("user_manual/graphical_interface/deterministic_analysis/user_manual_deterministic_analysis.html#morrisresult");

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
  bool canUseParaview = false;
#ifdef PERSALYS_HAVE_PARAVIEW
  if (SubWindow::SupportsOpenGL_3_2())
  {
    addParaviewWidgetsTabs();
    canUseParaview = true;
  }
#endif
  if (!canUseParaview)
  {
    addTableTab();
    addScatterPlotsTab();
  }

  if (parametersWidget_)
    tabWidget_->addTab(parametersWidget_, tr("Parameters"));

  // tab : model description
  if (modelDescriptionWidget_)
    tabWidget_->addTab(modelDescriptionWidget_, tr("Model"));

  connect(tabWidget_, SIGNAL(currentChanged(int)), this, SLOT(updateVariablesListVisibility(int)));
}


void MorrisResultWindow::addEffectsTab()
{
  ResizableStackedWidget * stackWidget = new ResizableStackedWidget;
  for (UnsignedInteger i = 0; i < morrisResult_.getDesignOfExperiment().getOutputSample().getDimension(); ++i)
  {
    MorrisResultWidget * morrisWidget = new MorrisResultWidget(morrisResult_, i, this);
    stackWidget->addWidget(morrisWidget);
  }
  connect(variablesListWidget_, SIGNAL(currentRowChanged(int)), stackWidget, SLOT(setCurrentIndex(int)));

  tabWidget_->addTab(stackWidget, tr("Elementary effects"));
}


void MorrisResultWindow::updateVariablesListVisibility(int indexTab)
{
  variablesGroupBox_->setVisible(indexTab == 0);
}
}
