//                                               -*- C++ -*-
/**
 *  @brief QWizard to define a central tendency analysis
 *
 *  Copyright 2015-2017 EDF-Phimeca
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
#include "otgui/CentralTendencyWizard.hxx"

#include "otgui/TaylorExpansionMomentsAnalysis.hxx"

using namespace OT;

namespace OTGUI
{

CentralTendencyWizard::CentralTendencyWizard(const Analysis& analysis, QWidget* parent)
  : AnalysisWizard(analysis, parent)
{
  buildInterface();
}


void CentralTendencyWizard::buildInterface()
{
  setWindowTitle(tr("Central tendency"));

  // First Page: choose the method
  introPage_ = new IntroCentralTendencyPage(this);
  introPage_->initialize(analysis_);
  setPage(Page_Intro, introPage_);

  // Second Page: monte carlo method
  monteCarloPage_ = new MonteCarloPage(this);
  monteCarloPage_->initialize(analysis_);
  setPage(Page_MonteCarlo, monteCarloPage_);

  setStartId(Page_Intro);
}


int CentralTendencyWizard::nextId() const
{
  switch (currentId())
  {
    case Page_Intro:
      return introPage_->nextId();
    case Page_MonteCarlo:
      return monteCarloPage_->nextId();
    default:
      return -1;
  }
}


Analysis CentralTendencyWizard::getAnalysis() const
{
  // get the physical model
  PhysicalModel model = dynamic_cast<const PhysicalModelAnalysis*>(analysis_.getImplementation().get())->getPhysicalModel();

  if (currentId() == Page_MonteCarlo)
  {
    Analysis analysis = monteCarloPage_->getAnalysis(analysis_.getName(), model);
    analysis.getImplementation()->setInterestVariables(introPage_->getInterestVariables());
    return analysis;
  }
  else
  {
    TaylorExpansionMomentsAnalysis analysis(analysis_.getName(), model);
    analysis.setInterestVariables(introPage_->getInterestVariables());
    return analysis;
  }
}
}