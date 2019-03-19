//                                               -*- C++ -*-
/**
 *  @brief QWizard introducing the methods to define designs of experiments
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
#include "otgui/DesignOfExperimentWizard.hxx"

namespace OTGUI
{

DesignOfExperimentWizard::DesignOfExperimentWizard(const Analysis & designOfExperiment, QWidget *parent)
  : AnalysisWizard(designOfExperiment, parent)
  , introPage_(0)
  , gridPage_(0)
  , probaPage_(0)
  , importPage_(0)
{
  buildInterface();
}


void DesignOfExperimentWizard::buildInterface()
{
  setWindowTitle(tr("Design of experiments"));

  introPage_ = new DesignOfExperimentIntroPage(this);
  introPage_->initialize(analysis_);
  setPage(Page_Intro, introPage_);

  gridPage_ = new GridDesignPage(this);
  gridPage_->initialize(analysis_);
  setPage(Page_Deterministic, gridPage_);

  probaPage_ = new ProbabilisticDesignPage;
  probaPage_->initialize(analysis_);
  setPage(Page_Probabilistic, probaPage_);

  importPage_ = new ImportedDesignPage(this);
  importPage_->initialize(analysis_);
  setPage(Page_Import, importPage_);

  setStartId(Page_Intro);
}


int DesignOfExperimentWizard::nextId() const
{
  switch (currentId())
  {
    case Page_Intro:
      return introPage_->nextId();
    case Page_Deterministic:/* Falls through. */
    case Page_Probabilistic:/* Falls through. */
    case Page_Import:
      return -1;
    default:
      return -1;
  }
}


Analysis DesignOfExperimentWizard::getAnalysis() const
{
  Analysis analysis;
  switch (introPage_->getMethodId())
  {
    case DesignOfExperimentIntroPage::Deterministic:
      analysis = gridPage_->getAnalysis();
      break;
    case DesignOfExperimentIntroPage::Probabilistic:
    {
      // get the physical model
      PhysicalModel model = dynamic_cast<const PhysicalModelAnalysis*>(analysis_.getImplementation().get())->getPhysicalModel();
      analysis = probaPage_->getAnalysis(analysis_.getName(), model);
      break;
    }
    case DesignOfExperimentIntroPage::Import:
      analysis = importPage_->getAnalysis();
      break;
    default:
      throw InvalidValueException(HERE) << "ReliabilityAnalysisWizard::getAnalysis no analysis";
  }

  dynamic_cast<DesignOfExperimentEvaluation*>(analysis.getImplementation().get())->resetResult();

  return analysis;
}
}
