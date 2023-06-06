//                                               -*- C++ -*-
/**
 *  @brief QMdiSubWindow for the results of MO optim
 *
 *  Copyright 2015-2023 EDF-Phimeca
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
#ifndef PERSALYS_MULTIOBJECTIVEOPTIMIZATIONRESULTWINDOW_HXX
#define PERSALYS_MULTIOBJECTIVEOPTIMIZATIONRESULTWINDOW_HXX

//#include "persalys/ResultWindow.hxx"
#include "persalys/DataAnalysisWindow.hxx"
#include "persalys/MultiObjectiveOptimizationAnalysis.hxx"
//#include "persalys/AnalysisItem.hxx"
//#include "persalys/DesignOfExperiment.hxx"

//#include <openturns/OptimizationResult.hxx>

namespace PERSALYS
{
  class PERSALYS_VIEW_API MultiObjectiveOptimizationResultWindow : public  DataAnalysisWindow //ResultWindow
{
  Q_OBJECT

public:
  MultiObjectiveOptimizationResultWindow(AnalysisItem * item, QWidget *parent = nullptr);

protected:
  void initialize(AnalysisItem * item);
#ifdef PERSALYS_HAVE_PARAVIEW
  void addParaviewWidgetsTabs();
#endif
  //void buildInterface();

private:
  MultiObjectiveOptimizationAnalysisResult result_;
  TitleLabel * titleLabel_;
  QTabWidget * tabWidget_;
  QStringList inputNames_;
  QStringList outputNames_;
  QStringList inAxisTitles_;
  QStringList outAxisTitles_;


};
}
#endif
