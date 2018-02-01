//                                               -*- C++ -*-
/**
 *  @brief QWizard to define a reliability analysis
 *
 *  Copyright 2015-2018 EDF-Phimeca
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
#ifndef OTGUI_RELIABILITYANALYSISWIZARD_HXX
#define OTGUI_RELIABILITYANALYSISWIZARD_HXX

#include "otgui/AnalysisWizard.hxx"
#include "otgui/IntroReliabilityPage.hxx"
#include "otgui/SimulationReliabilityPage.hxx"
#include "otgui/ApproximationReliabilityPage.hxx"

namespace OTGUI
{
class OTGUI_API ReliabilityAnalysisWizard : public AnalysisWizard
{
  Q_OBJECT

public:
  enum {Page_Intro, Page_SimuMethod, Page_ApproxMethod, Page_FORM};

  ReliabilityAnalysisWizard(const Analysis& analysis, const bool isGeneralWizard = false, QWidget* parent = 0);

  virtual int nextId() const;
  virtual bool validateCurrentPage();

protected:
  void buildInterface();

private:
  QList<LimitState> limitStateList_;
  IntroReliabilityPage * introPage_;
  SimulationReliabilityPage * simulationPage_;
  ApproximationReliabilityPage * approximationPage_;
  ApproximationReliabilityPage * formPage_;
};
}
#endif
