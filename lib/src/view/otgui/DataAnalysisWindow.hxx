//                                               -*- C++ -*-
/**
 *  @brief QMdiSubWindow for the analysis of data
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
#ifndef OTGUI_DATANALYSISWINDOW_HXX
#define OTGUI_DATANALYSISWINDOW_HXX

#include "otgui/ResultWindow.hxx"
#include "otgui/DataAnalysisResult.hxx"
#include "otgui/GraphConfigurationWidget.hxx"
#include "otgui/PlotMatrixConfigurationWidget.hxx"
#include "otgui/DoubleSpinBox.hxx"

#include <QComboBox>

namespace OTGUI {
class OTGUI_API DataAnalysisWindow : public ResultWindow
{
  Q_OBJECT

public:
  DataAnalysisWindow(AnalysisItem * item);

protected:
  virtual void initialize(AnalysisItem * item) = 0;
  void buildInterface();
  void setParameters(const Analysis & analysis) {};
  QWidget* getPDF_CDFWidget();
  QWidget* getBoxPlotWidget();
  QWidget* getScatterPlotsWidget();

public slots:
  void updateSpinBoxes(int indexOutput=0);
  void probaValueChanged(double proba);
  void quantileValueChanged(double quantile);
  void showHideGraphConfigurationWidget(int indexTab);
  void showHideGraphConfigurationWidget(Qt::WindowStates, Qt::WindowStates);

protected:
  DataAnalysisResult result_;
  bool resultsSampleIsValid_;
  QString sampleSizeTitle_;
  QStringList stochInputNames_;
  QStringList inAxisTitles_;
  QStringList outputNames_;
  QStringList outAxisTitles_;
  bool isConfidenceIntervalRequired_;
  double levelConfidenceInterval_;
  QTabWidget * tabWidget_;
  QComboBox * variablesComboBox_;
  GraphConfigurationWidget * pdf_cdfPlotsConfigurationWidget_;
  GraphConfigurationWidget * boxPlotsConfigurationWidget_;
  GraphConfigurationWidget * scatterPlotsConfigurationWidget_;
  PlotMatrixConfigurationWidget * plotMatrixConfigurationWidget_;
  PlotMatrixConfigurationWidget * plotMatrix_X_X_ConfigurationWidget_;
  DoubleSpinBox * probaSpinBox_;
  DoubleSpinBox * quantileSpinBox_;
  int tabOffset_;
};
}
#endif