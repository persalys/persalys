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
#include "otgui/ExportableTableView.hxx"
#include "otgui/GraphConfigurationWidget.hxx"
#include "otgui/PlotMatrixConfigurationWidget.hxx"

#include <QTabWidget>
#include <QPushButton>

namespace OTGUI {
class OTGUI_API DesignOfExperimentWindow : public OTguiSubWindow
{
  Q_OBJECT

public:
  DesignOfExperimentWindow(DesignOfExperimentItem * item);

  static QVector<PlotWidget*> GetListScatterPlots(const OT::NumericalSample & inS, const OT::NumericalSample & outS,
                                                  const QStringList inNames, const QStringList inAxisNames,
                                                  const QStringList outNames, const QStringList outAxisNames);

protected:
  void buildInterface();
  void addTabsForOutputs();

public slots:
  void evaluateOutputs();
  void updateWindowForOutputs();
  void showHideGraphConfigurationWidget(int indexTab);
  void showHideGraphConfigurationWidget(Qt::WindowStates, Qt::WindowStates);

private:
  DesignOfExperiment designOfExperiment_;
  QTabWidget * tabWidget_;
  ExportableTableView * tableView_;
  QPushButton * evaluateButton_;
  QComboBox * outputsComboBoxFirstTab_;
  PlotMatrixConfigurationWidget * plotMatrixConfigurationWidget_;
  PlotMatrixConfigurationWidget * plotMatrix_X_X_ConfigurationWidget_;
  GraphConfigurationWidget * graphConfigurationWidget_;
};
}
#endif
