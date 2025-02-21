//                                               -*- C++ -*-
/**
 *  @brief QWizard to define a reliability analysis
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
#include "persalys/ReliabilityAnalysisWizard.hxx"

#include "persalys/FORMImportanceSamplingAnalysis.hxx"
#include "persalys/MonteCarloReliabilityAnalysis.hxx"
#include "persalys/FORMAnalysis.hxx"
#include "persalys/SORMAnalysis.hxx"
#include "persalys/StudyItem.hxx"

using namespace OT;

namespace PERSALYS
{

ReliabilityAnalysisWizard::ReliabilityAnalysisWizard(const Analysis& analysis, const bool isGeneralWizard, QWidget* parent)
  : AnalysisWizard(analysis, parent)
  , limitStateList_()
  , introPage_(0)
  , simulationPage_(0)
  , approximationPage_(0)
{
  const LimitState limitState = dynamic_cast<ReliabilityAnalysis*>(analysis_.getImplementation().get())->getLimitState();

  if (isGeneralWizard)
  {
    if (Observer * obs = limitState.getImplementation().get()->getObserver("Study"))
    {
      StudyImplementation * study = dynamic_cast<StudyImplementation*>(obs);
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
  docLink_ = "user_manual/graphical_interface/probabilistic_analysis/user_manual_probabilistic_analysis.html#thresholdexceedancewizard";

  // First Page: choose the limit state and the method of reliability analysis
  introPage_ = new ReliabilityIntroPage(this);
  introPage_->initialize(analysis_, limitStateList_);
  setPage(Page_Intro, introPage_);

  // Second Page: simulation methods
  simulationPage_ = new SimulationReliabilityPage(this);
  simulationPage_->initialize(analysis_);
  setPage(Page_SimuMethod, simulationPage_);

  // Second/third Page: approximation methods
  approximationPage_ = new ApproximationReliabilityPage(this);
  approximationPage_->initialize(analysis_);
  setPage(Page_ApproxMethod, approximationPage_);

  setStartId(Page_Intro);
}


int ReliabilityAnalysisWizard::nextId() const
{
  switch (currentId())
  {
    case Page_Intro:
      return introPage_->nextId();
    case Page_SimuMethod:
    {
      if (introPage_->getMethodId() == ReliabilityIntroPage::FORM_IS)
        return ReliabilityAnalysisWizard::Page_ApproxMethod;
      return -1;
    }
    default:
      return -1;
  }
}


Analysis ReliabilityAnalysisWizard::getAnalysis() const
{
  const String analysisName = analysis_.getName();
  const LimitState limitState = introPage_->getLimitState();

  Analysis analysis;
  switch (introPage_->getMethodId())
  {
    case ReliabilityIntroPage::MonteCarlo:
      analysis = MonteCarloReliabilityAnalysis(analysisName, limitState);
      break;
    case ReliabilityIntroPage::FORM_IS:
      analysis = FORMImportanceSamplingAnalysis(analysisName, limitState);
      break;
    case ReliabilityIntroPage::FORM:
      analysis = FORMAnalysis(analysisName, limitState);
      break;
    case ReliabilityIntroPage::SORM:
      analysis = SORMAnalysis(analysisName, limitState);
      break;
    default:
      throw InvalidValueException(HERE) << "ReliabilityAnalysisWizard::getAnalysis no analysis";
  }
  simulationPage_->updateAnalysis(analysis);
  approximationPage_->updateAnalysis(analysis);
  return analysis;
}
}
