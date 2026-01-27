//                                               -*- C++ -*-
/**
 *  @brief QMdiSubWindow for the analysis of data
 *
 *  Copyright 2015-2025 EDF-Phimeca
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
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
#ifndef PERSALYS_DATANALYSISWINDOW_HXX
#define PERSALYS_DATANALYSISWINDOW_HXX

#include "persalys/ResultWindow.hxx"
#include "persalys/DataAnalysisResult.hxx"
#include "persalys/WidgetBoundToDockWidget.hxx"

#ifdef PERSALYS_HAVE_PARAVIEW
#include "persalys/PVSpreadSheetViewWidget.hxx"
#endif

#include <QGroupBox>

namespace PERSALYS
{
class PERSALYS_VIEW_API DataAnalysisWindow : public ResultWindow
{
  Q_OBJECT

public:
  explicit DataAnalysisWindow(Item * item, QWidget *parent = nullptr);

protected:
  void initializeVariablesNames();
  void buildInterface();
  virtual void fillListWidget();
  virtual void fillTabWidget();
  virtual void addSummaryTab();
  void addPDF_CDFTab();
  void addBoxPlotTab();
  virtual void addDependenceTab();
#ifdef PERSALYS_HAVE_PARAVIEW
  virtual void addParaviewWidgetsTabs();
  virtual void addParaviewPlotWidgetsTabs(PVSpreadSheetViewWidget* pvSpreadSheet, const OT::Sample &sample);
#endif
  virtual void addPlotMatrixTab();
  void removePlotMatrixTab();
  virtual void addScatterPlotsTab();
  virtual void addTableTab();
  void addErrorTable();

public slots:
  virtual void updateVariablesListVisibility(int indexTab);
signals:
  void stateChanged(int);

protected:
  DesignOfExperiment    designOfExperiment_;
  DataAnalysisResult    result_;
  bool                  hasMaximumCV_;
  bool                  hasMaximumCILength_;
  QString               sampleSizeTitle_;
  QString               analysisStopCriteriaMessage_;
  QString               analysisErrorMessage_;
  OT::Sample            failedInputSample_;
  OT::Sample            notEvaluatedInputSample_;
  bool                  resultsSampleIsValid_;
  QStringList           inputNames_;
  QStringList           inAxisTitles_;
  QStringList           outputNames_;
  QStringList           outAxisTitles_;
  bool                  isConfidenceIntervalRequired_;
  double                levelConfidenceInterval_;
  bool                  showTable_;
  TitleLabel          * titleLabel_                       = nullptr;
  QGroupBox           * variablesGroupBox_                = nullptr;
  VariablesListWidget * variablesListWidget_              = nullptr;
  QTabWidget          * tabWidget_                        = nullptr;
  OT::Description       errorDescription_;
  QTabWidget          * tablesTabWidget_                  = nullptr;
  int                   summaryValuesListSampleSizeIndex_ = 0;
  bool                  doMultivariate_                   = true;     // can be set to false in DataAnalysisResultWindow

private:
  WidgetBoundToDockWidget * boundPlotMatrixWidget_ = nullptr;
  int plotMatrixTabIndex_ = -1;
  bool canUseParaview_ = false;
};
}
#endif
