//                                               -*- C++ -*-
/**
 *  @brief QWizard to define a ScreeningAnalysis
 *
 *  Copyright 2015-2022 EDF-Phimeca
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
#include "persalys/ScreeningAnalysisWizard.hxx"

using namespace OT;

namespace PERSALYS
{

ScreeningAnalysisWizard::ScreeningAnalysisWizard(const Analysis& analysis, QWidget* parent)
  : AnalysisWizard(analysis, parent)
  , introPage_(0)
  , morrisPage_(0)
  , morrisSecondPage_(0)
{
  buildInterface();
}


void ScreeningAnalysisWizard::buildInterface()
{
  setWindowTitle(tr("Screening analysis"));
  docLink_ = "user_manual/graphical_interface/deterministic_analysis/user_manual_deterministic_analysis.html#screeningwizard";

  // First Page: choose the method
  introPage_ = new ScreeningIntroPage(this);
  introPage_->initialize(analysis_);
  setPage(Page_Intro, introPage_);

  // Second Page: Morris page
  morrisPage_ = new MorrisPage(this);
  morrisPage_->initialize(analysis_);
  setPage(Page_Morris, morrisPage_);

  // Third Page: second Morris page
  morrisSecondPage_ = new MorrisSecondPage(this);
  morrisSecondPage_->initialize(analysis_);
  setPage(Page_Morris2, morrisSecondPage_);

  setStartId(Page_Intro);
}


int ScreeningAnalysisWizard::nextId() const
{
  switch (currentId())
  {
    case Page_Intro:
      return introPage_->nextId();
    case Page_Morris:
      return morrisPage_->nextId();
    default:
      return -1;
  }
}


Analysis ScreeningAnalysisWizard::getAnalysis() const
{
  MorrisAnalysis analysis(morrisPage_->getAnalysis().getName(), morrisPage_->getAnalysis().getPhysicalModel());
  analysis.setBounds(morrisPage_->getAnalysis().getBounds());
  analysis.setTrajectoriesNumber(morrisSecondPage_->getTrajectoriesNumber());
  analysis.setLevel(morrisSecondPage_->getLevel());
  analysis.setSeed(morrisSecondPage_->getSeed());
  analysis.setBlockSize(morrisSecondPage_->getBlockSize());
  analysis.setInterestVariables(introPage_->getInterestVariables());
  return analysis;
}
}
