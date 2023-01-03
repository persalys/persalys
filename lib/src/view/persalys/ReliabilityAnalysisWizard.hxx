//                                               -*- C++ -*-
/**
 *  @brief QWizard to define a reliability analysis
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
#ifndef PERSALYS_RELIABILITYANALYSISWIZARD_HXX
#define PERSALYS_RELIABILITYANALYSISWIZARD_HXX

#include "persalys/AnalysisWizard.hxx"
#include "persalys/ReliabilityIntroPage.hxx"
#include "persalys/SimulationReliabilityPage.hxx"
#include "persalys/ApproximationReliabilityPage.hxx"

namespace PERSALYS
{
class PERSALYS_VIEW_API ReliabilityAnalysisWizard : public AnalysisWizard
{
  Q_OBJECT

  friend class TestReliabilityAnalysisWizard;

public:
  enum {Page_Intro, Page_SimuMethod, Page_ApproxMethod};

  ReliabilityAnalysisWizard(const Analysis& analysis, const bool isGeneralWizard = false, QWidget* parent = 0);

  virtual Analysis getAnalysis() const;
  virtual int nextId() const;

protected:
  void buildInterface();

private:
  QList<LimitState> limitStateList_;
  ReliabilityIntroPage * introPage_;
  SimulationReliabilityPage * simulationPage_;
  ApproximationReliabilityPage * approximationPage_;
};
}
#endif
