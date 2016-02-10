//                                               -*- C++ -*-
/**
 *  @brief QMdiSubWindow for the results of SRCAnalysis
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
#ifndef OTGUI_SRCCRESULTWINDOW_HXX
#define OTGUI_SRCRESULTWINDOW_HXX

#include "otgui/OTguiSubWindow.hxx"
#include "otgui/SensitivityAnalysisItem.hxx"
#include "otgui/SRCResult.hxx"
#include "otgui/GraphConfigurationWidget.hxx"

#include <QStackedLayout>

namespace OTGUI {
class SRCResultWindow : public OTguiSubWindow
{
  Q_OBJECT

public:
  SRCResultWindow(SensitivityAnalysisItem * item);

protected:
  void buildInterface();

public slots:
  void updateIndicesPlot(int section, Qt::SortOrder order);
  void showHideGraphConfigurationWidget(Qt::WindowStates, Qt::WindowStates);

private:
  SRCResult result_;
  std::vector<std::map<double, int> > indices_;
  QStackedLayout * frameLayout_;
  GraphConfigurationWidget * plotsConfigurationWidget_;
  QVector<PlotWidget*> listPlotWidgets_;
};
}
#endif