//                                               -*- C++ -*-
/**
 *  @brief QMdiSubWindow to display the designs of experiments
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
#ifndef OTGUI_DESIGNOFEXPERIMENTWINDOW_HXX
#define OTGUI_DESIGNOFEXPERIMENTWINDOW_HXX

#include "OTguiSubWindow.hxx"
#include "otgui/DesignOfExperimentItem.hxx"
#include "otgui/DataTableView.hxx"
#include "otgui/GraphConfigurationWidget.hxx"
#include "otgui/PlotMatrixConfigurationWidget.hxx"
#include "otgui/NotEditableTableWidget.hxx"

#include <QTabWidget>
#include <QPushButton>

namespace OTGUI {
class DesignOfExperimentWindow : public OTguiSubWindow
{
  Q_OBJECT

public:
  DesignOfExperimentWindow(DesignOfExperimentItem * item);

protected:
  void buildInterface();
  void addTabsForOutputs();

public slots:
  void evaluateOutputs();
  void updateWindowForOutputs();
  void updateLabelsText(int indexOutput=0);
  void showHideGraphConfigurationWidget(int indexTab);
  void showHideGraphConfigurationWidget(Qt::WindowStates, Qt::WindowStates);

private:
  DesignOfExperiment designOfExperiment_;
  QTabWidget * tabWidget_;
  DataTableView * tableView_;
  QPushButton * evaluateButton_;
  NotEditableTableWidget * minMaxTable_;
  PlotMatrixConfigurationWidget * plotMatrixConfigurationWidget_;
  PlotMatrixConfigurationWidget * plotMatrix_X_X_ConfigurationWidget_;
  GraphConfigurationWidget * graphConfigurationWidget_;
};
}
#endif