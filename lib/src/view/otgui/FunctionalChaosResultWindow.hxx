//                                               -*- C++ -*-
/**
 *  @brief QMdiSubWindow for the results of functional chaos
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
#ifndef OTGUI_FUNCTIONALCHAOSRESULTWINDOW_HXX
#define OTGUI_FUNCTIONALCHAOSRESULTWINDOW_HXX

#include "otgui/ResultWindow.hxx"
#include "otgui/MetaModelFunctionalChaosResult.hxx"
#include "otgui/GraphConfigurationWidget.hxx"

namespace OTGUI {
class FunctionalChaosResultWindow : public ResultWindow
{
  Q_OBJECT

public:
  FunctionalChaosResultWindow(AnalysisItem * item);

protected:
  void setParameters(const Analysis & analysis);
  void buildInterface();

public slots:
  void showHideGraphConfigurationWidget(int indexTab);
  void showHideGraphConfigurationWidget(Qt::WindowStates, Qt::WindowStates);

private:
  MetaModelFunctionalChaosResult result_;
  OT::NumericalSample outputSample_;
  QTabWidget * tabWidget_;
  GraphConfigurationWidget * metaModelPlotsConfigurationWidget_;
  GraphConfigurationWidget * sobolIndicesPlotsConfigurationWidget_;
};
}
#endif