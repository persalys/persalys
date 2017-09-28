//                                               -*- C++ -*-
/**
 *  @brief QMdiSubWindow for the results of the reliability analysis using simulation method
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
#ifndef OTGUI_SIMULATIONRELIABILITYRESULTWINDOW_HXX
#define OTGUI_SIMULATIONRELIABILITYRESULTWINDOW_HXX

#include "otgui/ResultWindow.hxx"
#include "otgui/SimulationReliabilityResult.hxx"

namespace OTGUI
{
class OTGUI_API SimulationReliabilityResultWindow : public ResultWindow
{
  Q_OBJECT

public:
  SimulationReliabilityResultWindow(AnalysisItem * item, QWidget * parent = 0);

protected:
  void buildInterface();
  QWidget * getSummaryTab();
  QWidget * getHistogramTab();
  QWidget * getConvergenceTab();

private:
  SimulationReliabilityResult result_;
  QTabWidget * formTabWidget_;
};
}
#endif
