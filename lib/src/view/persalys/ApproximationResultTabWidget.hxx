//                                               -*- C++ -*-
/**
 *  @brief QTabWidget for the FORM results
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
#ifndef PERSALYS_APPROXIMATIONRESULTTABWIDGET_HXX
#define PERSALYS_APPROXIMATIONRESULTTABWIDGET_HXX

#include "persalys/ReliabilityAnalysis.hxx"

#include <openturns/FORMResult.hxx>
#include <openturns/MultiFORMResult.hxx>
#include <openturns/SORMResult.hxx>

#include <QTabWidget>
#include <QScrollArea>
#include <QStackedWidget>

namespace PERSALYS
{
class PERSALYS_VIEW_API ApproximationResultTabWidget : public QTabWidget
{
  Q_OBJECT

public:
  enum Method {FORM, SORM, SystemFORM};

  ApproximationResultTabWidget(const OT::FORMResult& result,
                               const ReliabilityAnalysis& analysis,
                               QWidget* parent = nullptr);
  ApproximationResultTabWidget(const OT::MultiFORMResult& result,
                               const ReliabilityAnalysis& analysis,
                               const OT::Description& outputNames,
                               QWidget* parent = nullptr);
  ApproximationResultTabWidget(const OT::SORMResult& result,
                               const ReliabilityAnalysis& analysis,
                               QWidget* parent = nullptr);

public slots:
  /** Switch the visible per-event content (system FORM only). */
  void setCurrentEvent(int index);

protected:
  void buildInterface();
  void buildSystemFORMInterface();

private:
  Method                  method_;
  OT::FORMResult          formResult_;
  OT::SORMResult          sormResult_;
  OT::AnalyticalResult    result_;
  OT::MultiFORMResult     multiFORMResult_;
  OT::Description         outputNames_;
  QScrollArea           * parametersWidget_         = nullptr;
  OT::UnsignedInteger     maximumEvaluationNumber_  = 0;
  // System FORM: stacked widgets driven by limit state selection
  QStackedWidget        * summaryOptimizationStack_ = nullptr;
  QStackedWidget        * designPointStack_         = nullptr;
};
}
#endif
