//                                               -*- C++ -*-
/**
 *  @brief QMdiSubWindow to display the designs of experiments
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
#ifndef OTGUI_DESIGNOFEXPERIMENTWINDOW_HXX
#define OTGUI_DESIGNOFEXPERIMENTWINDOW_HXX

#include "ResultWindow.hxx"
#include "otgui/AnalysisItem.hxx"
#include "otgui/PlotWidget.hxx"

#include <QTabWidget>
#include <QGroupBox>

namespace OTGUI {
class OTGUI_API DesignOfExperimentWindow : public ResultWindow
{
  Q_OBJECT

public:
  DesignOfExperimentWindow(AnalysisItem * item, QWidget * parent=0);

  static QVector<PlotWidget*> GetListScatterPlots(const OT::Sample& inS,
                                                  const OT::Sample& notValidInS,
                                                  const OT::Sample& outS,
                                                  const QStringList inNames,
                                                  const QStringList inAxisNames,
                                                  const QStringList outNames,
                                                  const QStringList outAxisNames);

protected:
  void buildInterface();
  void addMinMaxTab();
#ifdef OTGUI_HAVE_PARAVIEW
  void addParaviewWidgetsTabs();
#endif
  void addTablesTab();
  void addPlotMatrixTab();
  void addScatterPlotsTab();
  void getScatterPlotLabels(const OT::Sample& inS,
                            QStringList& inNames,
                            QStringList& inAxisNames,
                            QStringList& outNames,
                            QStringList& outAxisNames);

public slots:
  void updateVariablesListVisibility(int indexTab);

private:
  DesignOfExperiment designOfExperiment_;
  QGroupBox * variablesGroupBox_;
  OTguiListWidget * variablesListWidget_;
  QTabWidget * tabWidget_;
};
}
#endif
