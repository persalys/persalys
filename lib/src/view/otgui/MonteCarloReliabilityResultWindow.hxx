//                                               -*- C++ -*-
/**
 *  @brief QMdiSubWindow for the results of the reliability analysis by Monte Carlo
 *
 *  Copyright 2015-2016 EDF
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
#ifndef OTGUI_MONTECARLORELIABILITYRESULTWINDOW_HXX
#define OTGUI_MONTECARLORELIABILITYRESULTWINDOW_HXX

#include "otgui/OTguiSubWindow.hxx"
#include "otgui/AnalysisItem.hxx"
#include "otgui/MonteCarloReliabilityAnalysisResult.hxx"
#include "otgui/GraphConfigurationWidget.hxx"

namespace OTGUI {
class MonteCarloReliabilityResultWindow : public OTguiSubWindow
{
  Q_OBJECT

public:
  MonteCarloReliabilityResultWindow(AnalysisItem * item);

protected:
  void buildInterface();

public slots:
  void showHideGraphConfigurationWidget(int indexTab);
  void showHideGraphConfigurationWidget(Qt::WindowStates, Qt::WindowStates);

private:
  MonteCarloReliabilityAnalysisResult result_;
  QTabWidget * tabWidget_;
  GraphConfigurationWidget * histogramConfigurationWidget_;
  GraphConfigurationWidget * convergenceGraphConfigurationWidget_;
};
}
#endif