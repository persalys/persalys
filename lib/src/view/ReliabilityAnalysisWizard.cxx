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
#include "otgui/ReliabilityAnalysisWizard.hxx"

#include "otgui/FORMImportanceSamplingAnalysis.hxx"
#include "otgui/OTStudyItem.hxx"

using namespace OT;

namespace OTGUI
{

ReliabilityAnalysisWizard::ReliabilityAnalysisWizard(const Analysis& analysis, const bool isGeneralWizard, QWidget* parent)
  : AnalysisWizard(analysis, parent)
  , limitStateList_()
  , introPage_(0)
  , simulationPage_(0)
  , approximationPage_(0)
  , formPage_(0)
{
  const LimitState limitState = dynamic_cast<ReliabilityAnalysis*>(analysis_.getImplementation().get())->getLimitState();

  if (isGeneralWizard)
  {
    if (Observer * obs = limitState.getImplementation().get()->getObserver("OTStudy"))
    {
      OTStudyImplementation * study = dynamic_cast<OTStudyImplementation*>(obs);
      Q_ASSERT(study);
      for (UnsignedInteger i = 0; i < study->getLimitStates().getSize(); ++i)
      {
        const LimitState limitState_i = study->getLimitStates()[i];
        if (limitState_i.getPhysicalModel() == limitState.getPhysicalModel())
          limitStateList_.append(limitState_i);
      }
    }
  }
  else
  {
    limitStateList_.append(limitState);
  }
  buildInterface();
}


void ReliabilityAnalysisWizard::buildInterface()
{
  setWindowTitle(tr("Threshold exceedance"));

  // First Page: choose the limit state and the method of reliability analysis
  introPage_ = new IntroReliabilityPage(this);
  introPage_->initialize(analysis_, limitStateList_);
  setPage(Page_Intro, introPage_);

  // Second Page: simulation methods
  simulationPage_ = new SimulationReliabilityPage(this);
  simulationPage_->initialize(analysis_);
  setPage(Page_SimuMethod, simulationPage_);
  connect(introPage_, SIGNAL(methodChanged(int)), simulationPage_, SLOT(updateMethod(int)));

  // Second Page: approximation methods
  approximationPage_ = new ApproximationReliabilityPage(this);
  approximationPage_->initialize(analysis_);
  setPage(Page_ApproxMethod, approximationPage_);

  // third page: FORM page
  formPage_ = new ApproximationReliabilityPage(this);
  formPage_->initialize(analysis_);
  setPage(Page_FORM, formPage_);

  setStartId(Page_Intro);
}


int ReliabilityAnalysisWizard::nextId() const
{
  switch (currentId())
  {
    case Page_Intro:
      return introPage_->nextId();
    case Page_SimuMethod:
      return simulationPage_->nextId();
    default:
      return -1;
  }
}


bool ReliabilityAnalysisWizard::validateCurrentPage()
{
  if (currentId() == Page_SimuMethod || currentId() == Page_ApproxMethod)
  {
    const LimitState limitState = introPage_->getLimitState();

    if (currentId() == Page_SimuMethod)
      analysis_ = simulationPage_->getAnalysis(analysis_.getName(), limitState);
    else
      analysis_ = approximationPage_->getAnalysis(analysis_.getName(), limitState);
  }
  else if (currentId() == Page_FORM)
  {
    FORMImportanceSamplingAnalysis * formIS_ptr = dynamic_cast<FORMImportanceSamplingAnalysis*>(analysis_.getImplementation().get());
    if (formIS_ptr)
    {
      FORMImportanceSamplingAnalysis analysis(*formIS_ptr);
      analysis.setOptimizationAlgorithm(formPage_->getOptimizationAlgorithm());
      analysis.setPhysicalStartingPoint(formPage_->getOptimizationAlgorithm().getStartingPoint());
      analysis_ = analysis;
    }
  }

  return QWizard::validateCurrentPage();
}
}
