//                                               -*- C++ -*-
/**
 *  @brief QMdiSubWindow for the results of the reliability analysis using simulation method
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
#ifndef OTGUI_SIMULATIONRELIABILITYRESULTWINDOW_HXX
#define OTGUI_SIMULATIONRELIABILITYRESULTWINDOW_HXX

#include "otgui/ResultWindow.hxx"
#include "otgui/SimulationReliabilityResult.hxx"
#include "otgui/GraphConfigurationWidget.hxx"

namespace OTGUI {
class OTGUI_API SimulationReliabilityResultWindow : public ResultWindow
{
  Q_OBJECT

public:
  SimulationReliabilityResultWindow(AnalysisItem * item);

  virtual ~SimulationReliabilityResultWindow();

protected:
  void setParameters(const Analysis & analysis);
  void buildInterface();
  QWidget * getSummaryTab();
  QWidget * getHistogramTab();
  QWidget * getConvergenceTab();

public slots:
  void showHideGraphConfigurationWidget(int indexTab);
  void showHideGraphConfigurationWidget(Qt::WindowStates, Qt::WindowStates);

private:
  SimulationReliabilityResult result_;
  QTabWidget * tabWidget_;
  QTabWidget * formTabWidget_;
  GraphConfigurationWidget * histogramConfigurationWidget_;
  GraphConfigurationWidget * convergenceGraphConfigurationWidget_;
};
}
#endif