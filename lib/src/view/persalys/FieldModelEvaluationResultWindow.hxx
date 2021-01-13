//                                               -*- C++ -*-
/**
 *  @brief QMdiSubWindow for the results of an evaluation of the model
 *
 *  Copyright 2015-2021 EDF-Phimeca
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
#ifndef PERSALYS_FIELDMODELEVALUATIONRESULTWINDOW_HXX
#define PERSALYS_FIELDMODELEVALUATIONRESULTWINDOW_HXX

#include "ResultWindow.hxx"
#include "persalys/AnalysisItem.hxx"
#include "persalys/FieldMonteCarloResult.hxx"

#include <QTabWidget>

namespace PERSALYS
{
class PERSALYS_VIEW_API FieldModelEvaluationResultWidget : public QWidget
{
  Q_OBJECT

public:
  FieldModelEvaluationResultWidget(const OT::Sample& inputSample, const OT::ProcessSample& ps, QWidget *parent = 0);
  FieldModelEvaluationResultWidget(const OT::Sample& inputSample, const OT::ProcessSample& ps, const OT::Sample& meanS, const OT::Sample& lowQS, const OT::Sample& upQS, QWidget *parent = 0);

  VariablesListWidget * getOutListWidget();
  QTabWidget * getTabWidget();

protected:
  void buildInterface();
  void addWidgetsTabs();
#ifdef PERSALYS_HAVE_PARAVIEW
  void addParaviewWidgetsTabs();
#endif

private:
  VariablesListWidget * outListWidget_;
  QTabWidget * tabWidget_;
  OT::Sample inputSample_;
  OT::ProcessSample processSample_;
  OT::Sample meanSample_;
  OT::Sample lowerQuantileSample_;
  OT::Sample upperQuantileSample_;
};


class PERSALYS_VIEW_API FieldModelEvaluationResultWindow : public ResultWindow
{
  Q_OBJECT

public:
  FieldModelEvaluationResultWindow(AnalysisItem * item, QWidget * parent = 0);

protected:
  void buildInterface();

private:
  FieldModelEvaluationResultWidget * mainWidget_;
  OT::Sample inputSample_;
  OT::ProcessSample result_;
};


class PERSALYS_VIEW_API FieldCentralTendencyResultWindow : public ResultWindow
{
  Q_OBJECT

public:
  FieldCentralTendencyResultWindow(AnalysisItem * item, QWidget * parent = 0);

protected:
  void buildInterface();
  void addDecompositionTab();
  void addCorrelationTab();

private:
  OT::Sample inputSample_;
  FieldMonteCarloResult result_;
  QString errorMessage_;
  FieldModelEvaluationResultWidget * mainWidget_;
};
}
#endif
