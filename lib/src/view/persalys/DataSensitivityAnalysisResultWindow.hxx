//                                               -*- C++ -*-
/**
 *  @brief QMdiSubWindow for the sensitivity analysis of a data model
 *
 *  Copyright 2015-2026 EDF-Phimeca
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

#ifndef PERSALYS_DATASENSITIVITYANALYSISRESULTWINDOW_HXX
#define PERSALYS_DATASENSITIVITYANALYSISRESULTWINDOW_HXX

#include "persalys/ResultWindow.hxx"
#include "persalys/DataSensitivityAnalysisResult.hxx"

#include <QTabWidget>

namespace PERSALYS
{
class PERSALYS_VIEW_API DataSensitivityAnalysisResultWindow : public ResultWindow
{
  Q_OBJECT

public:
  explicit DataSensitivityAnalysisResultWindow(AnalysisItem * item, QWidget *parent = nullptr);

protected:
  void initialize(const AnalysisItem* item);
  void initializeVariablesNames();
  void buildInterface();

private:
  void addSobolTab(QTabWidget * tabWidget);
  void addSRCTab(QTabWidget * tabWidget);
  void addHSICTab(QTabWidget * tabWidget, DataSensitivityAnalysisResult::HSICType type);

protected:
  DataSensitivityAnalysisResult result_;
  DesignOfExperiment designOfExperiment_;
  QStringList inputNames_;
  QStringList inAxisTitles_;
  QStringList outAxisTitles_;
  QStringList outputNames_;
  VariablesListWidget * outputsListWidget_ = nullptr;
};

} // namespace PERSALYS

#endif // PERSALYS_DATASENSITIVITYANALYSISWINDOW_HXX