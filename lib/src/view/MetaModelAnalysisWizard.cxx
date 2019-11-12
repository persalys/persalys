//                                               -*- C++ -*-
/**
 *  @brief QWizard to define a metamodel analysis
 *
 *  Copyright 2015-2019 EDF-Phimeca
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
#include "persalys/MetaModelAnalysisWizard.hxx"

#include "persalys/StudyItem.hxx"
#include "persalys/ModelEvaluation.hxx"
#include "persalys/DesignOfExperimentAnalysis.hxx"

using namespace OT;

namespace PERSALYS
{

MetaModelAnalysisWizard::MetaModelAnalysisWizard(const Analysis& analysis, const bool isGeneralWizard, QWidget* parent)
  : AnalysisWizard(analysis, parent)
  , doeList_()
  , introPage_(0)
  , krigingPage_(0)
  , functionalChaosPage_(0)
  , validationPage_(0)
{
  DesignOfExperimentAnalysis * doeAnalysis = dynamic_cast<DesignOfExperimentAnalysis*>(analysis_.getImplementation().get());
  Q_ASSERT(doeAnalysis);
  const DesignOfExperiment doe = doeAnalysis->getDesignOfExperiment();

  // set list of design of experiments items
  if (isGeneralWizard && doe.hasPhysicalModel())
  {
    PhysicalModel model(doe.getPhysicalModel());

    // get list of Designs Of Experiment of the current physical model
    if (Observer * obs = model.getImplementation().get()->getObserver("Study"))
    {
      StudyImplementation * study = dynamic_cast<StudyImplementation*>(obs);
      Q_ASSERT(study);
      for (UnsignedInteger i = 0; i < study->getAnalyses().getSize(); ++i)
      {
        DesignOfExperimentEvaluation * doeEval = dynamic_cast<DesignOfExperimentEvaluation *>(study->getAnalyses()[i].getImplementation().get());
        ModelEvaluation * modelEval = dynamic_cast<ModelEvaluation *>(study->getAnalyses()[i].getImplementation().get());
        if (doeEval && !modelEval && doeEval->getPhysicalModel() == model && doeEval->hasValidResult())
        {
          doeList_.append(doeEval->getResult().getDesignOfExperiment());
        }
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
  docLink_ = "user_manual/graphical_interface/data_analysis/user_manual_data_analysis.html#metamodelwizard";

  // intro page : doe/ouputs/method
  introPage_ = new MetaModelIntroPage(this);
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

  // validation page
  validationPage_ = new MetaModelValidationPage(this);
  validationPage_->initialize(analysis_);
  setPage(Page_Validation, validationPage_);
  connect(introPage_, SIGNAL(designOfExperimentChanged(DesignOfExperiment)), validationPage_, SLOT(updateInputSampleSize(DesignOfExperiment)));

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
    case Page_ChaosMethod:
    case Page_KrigingMethod:
      return Page_Validation;
    default:
      return -1;
  }
}


Analysis MetaModelAnalysisWizard::getAnalysis() const
{
  const String analysisName = analysis_.getName();
  const DesignOfExperiment design = introPage_->getDesignOfExperiment();

  Analysis analysis;
  switch (introPage_->getMethodId())
  {
    case MetaModelIntroPage::Chaos:
      analysis = functionalChaosPage_->getAnalysis(analysisName, design);
      break;
    case MetaModelIntroPage::Kriging:
      analysis = krigingPage_->getAnalysis(analysisName, design);
      break;
    default:
      throw InvalidValueException(HERE) << "MetaModelAnalysisWizard::getAnalysis no analysis";
  }
  analysis.getImplementation()->setInterestVariables(introPage_->getInterestVariables());
  validationPage_->updateMetamodelValidation(analysis);
  return analysis;
}
}
