//                                               -*- C++ -*-
/**
 *  @brief QMdiSubWindow to display the input sample of the designs of experiments
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
 *  You should have received a copy of the GNU Lesser General Public
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
#include "otgui/DesignOfExperimentInputWindow.hxx"

namespace OTGUI
{

DesignOfExperimentInputWindow::DesignOfExperimentInputWindow(DesignOfExperimentDefinitionItem * item, QWidget * parent)
  : DataAnalysisWindow(item, parent)
{
  designOfExperiment_.setInputSample(item->getOriginalInputSample());

  // parameters widget
  setParameters(item->getAnalysis(), tr("Design of experiments creation parameters"));

  setWindowTitle(tr("Design of experiments"));

  buildInterface();
}


void DesignOfExperimentInputWindow::fillTabWidget()
{
  variablesGroupBox_->setVisible(false);

#ifdef OTGUI_HAVE_PARAVIEW
  addParaviewWidgetsTabs();
#else
  // tab: Table --------------------------------
  addTableTab();
  if (designOfExperiment_.getSample().getDimension() > 1 && designOfExperiment_.getSample().getSize() > 1)
  {
    // tab: plot C
    addPlotMatrixTab();
    // tab: scatter plots
    addScatterPlotsTab();
  }
#endif

  // tab: Parameters --------------------------------
  if (parametersWidget_)
    tabWidget_->addTab(parametersWidget_, tr("Parameters"));
}
}
