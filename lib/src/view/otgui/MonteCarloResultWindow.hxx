//                                               -*- C++ -*-
/**
 *  @brief QMdiSubWindow for the results of Monte Carlo
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
#ifndef OTGUI_MONTECARLORESULTWINDOW_HXX
#define OTGUI_MONTECARLORESULTWINDOW_HXX

#include "otgui/OTguiSubWindow.hxx"
#include "otgui/AnalysisItem.hxx"
#include "otgui/MonteCarloResult.hxx"
#include "otgui/GraphConfigurationWidget.hxx"
#include "otgui/PlotMatrixConfigurationWidget.hxx"

#include <QComboBox>
#include <QDoubleSpinBox>
#include <QLabel>

namespace OTGUI {
class MonteCarloResultWindow : public OTguiSubWindow
{
  Q_OBJECT

public:
  MonteCarloResultWindow(AnalysisItem * item);

protected:
  void buildInterface();
  void updateResultWidgets(int indexOutput=0);

public slots:
  void outputFirstTabChanged(int indexOutput);
  void probaValueChanged(double proba);
  void quantileValueChanged(double quantile);
  void showHideGraphConfigurationWidget(int indexTab);
  void showHideGraphConfigurationWidget(Qt::WindowStates, Qt::WindowStates);

private:
  MonteCarloResult result_;
  PhysicalModel physicalModel_;
  bool isConfidenceIntervalRequired_;
  double levelConfidenceInterval_;
  QTabWidget * tabWidget_;
  QComboBox * outputsComboBoxFirstTab_;
  GraphConfigurationWidget * pdf_cdfPlotsConfigurationWidget_;
  GraphConfigurationWidget * boxPlotsConfigurationWidget_;
  GraphConfigurationWidget * scatterPlotsConfigurationWidget_;
  PlotMatrixConfigurationWidget * plotMatrixConfigurationWidget_;
  PlotMatrixConfigurationWidget * plotMatrix_X_X_ConfigurationWidget_;
  QTableWidget * minMaxTable_;
  QTableWidget * momentsEstimationsTable_;
  QLabel * minLabel_;
  QLabel * maxLabel_;
  QDoubleSpinBox * probaSpinBox_;
  QDoubleSpinBox * quantileSpinBox_;
};
}
#endif