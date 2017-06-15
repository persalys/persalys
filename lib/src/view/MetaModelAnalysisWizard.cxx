//                                               -*- C++ -*-
/**
 *  @brief QWizard to define a metamodel analysis
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
#include "otgui/MetaModelAnalysisWizard.hxx"

#include "otgui/OTStudyItem.hxx"
#include "otgui/DesignOfExperimentAnalysis.hxx"

using namespace OT;

namespace OTGUI {

MetaModelAnalysisWizard::MetaModelAnalysisWizard(OTguiItem* item, const Analysis& analysis, const bool isGeneralWizard, QWidget* parent)
  : AnalysisWizard(analysis, parent)
  , doeList_()
  , introPage_(0)
  , krigingPage_(0)
  , functionalChaosPage_(0)
{
  // set list of designs of experiment
  const DesignOfExperiment doe = dynamic_cast<DesignOfExperimentAnalysis*>(analysis_.getImplementation().get())->getDesignOfExperiment();
  if (isGeneralWizard)
  {
    for (UnsignedInteger i=0; i<item->getParentOTStudyItem()->getOTStudy().getDesignOfExperiments().getSize(); ++i)
    {
      const DesignOfExperiment design_i = item->getParentOTStudyItem()->getOTStudy().getDesignOfExperiments()[i];
      if (design_i.getPhysicalModel().getImplementation().get() == doe.getPhysicalModel().getImplementation().get() &&
          design_i.getOutputSample().getSize()
         )
      {
        doeList_.append(design_i);
      }
    }
  }
  else
  {
    doeList_.append(doe);
  }

  // build widgets
  buildInterface();
}


void MetaModelAnalysisWizard::buildInterface()
{
  setWindowTitle(tr("Metamodel"));

  // intro page : doe/ouputs/method
  introPage_ = new IntroMetaModelPage(this);
  setPage(Page_Intro, introPage_);

  // chaos page
  functionalChaosPage_ = new FunctionalChaosPage(this);
  functionalChaosPage_->initialize(analysis_);
  setPage(Page_ChaosMethod, functionalChaosPage_);
  connect(introPage_, SIGNAL(designOfExperimentChanged(DesignOfExperiment)), functionalChaosPage_, SLOT(updateInputSampleSize(DesignOfExperiment)));

  // kriging page
  krigingPage_ = new KrigingPage(this);
  setPage(Page_KrigingMethod, krigingPage_);
  connect(introPage_, SIGNAL(designOfExperimentChanged(DesignOfExperiment)), krigingPage_, SLOT(updateCovarianceModel(DesignOfExperiment)));

  // initialization
  introPage_->initialize(analysis_, doeList_);
  krigingPage_->initialize(analysis_);

  setStartId(Page_Intro);
}


int MetaModelAnalysisWizard::nextId() const
{
  switch (currentId())
  {
    case Page_Intro:
      return introPage_->nextId();
    default:
      return -1;
  }
}


bool MetaModelAnalysisWizard::validateCurrentPage()
{
  if (currentId() == Page_ChaosMethod || currentId() == Page_KrigingMethod)
  {
    const DesignOfExperiment design = introPage_->getDesignOfExperiment();

    if (currentId() == Page_ChaosMethod)
      analysis_ = functionalChaosPage_->getAnalysis(analysis_.getName(), design);
    else
      analysis_ = krigingPage_->getAnalysis(analysis_.getName(), design);

    analysis_.getImplementation()->setInterestVariables(introPage_->getInterestVariables());
  }
  return QWizard::validateCurrentPage();
}
}