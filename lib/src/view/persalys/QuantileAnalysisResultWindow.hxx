//                                               -*- C++ -*-
/**
 *  @brief QMdiSubWindow for the results of quantile analysis
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
#ifndef PERSALYS_QUANTILEANALYSISRESULTWINDOW_HXX
#define PERSALYS_QUANTILEANALYSISRESULTWINDOW_HXX

#include "persalys/ResultWindow.hxx"
#include "persalys/QuantileAnalysisResult.hxx"

namespace PERSALYS
{
  class PERSALYS_VIEW_API QuantileAnalysisResultWindow : public ResultWindow
  {
    Q_OBJECT

  public:
    QuantileAnalysisResultWindow(AnalysisItem * item, QWidget *parent = nullptr);

  protected:
    void buildInterface(const OT::Description& varNames);
    void fillTabWidget();
    void addQuantileTab();
    void addPlotTab();

  private:
    QuantileAnalysisResult result_;
    TitleLabel * titleLabel_ = nullptr;
    QTabWidget * tabWidget_ = nullptr;
    VariablesListWidget * variablesListWidget_ = nullptr;
  };
}
#endif
