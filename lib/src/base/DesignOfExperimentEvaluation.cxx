//                                               -*- C++ -*-
/**
 *  @brief Evaluates the design of experiment
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
#include "otgui/DesignOfExperimentEvaluation.hxx"

using namespace OT;

namespace OTGUI {

CLASSNAMEINIT(DesignOfExperimentEvaluation);

/* Constructor with parameters */
DesignOfExperimentEvaluation::DesignOfExperimentEvaluation(const DesignOfExperiment& design)
  : DesignOfExperimentAnalysis("evaluation", design)
  , Observer()
{
  if (!design.hasPhysicalModel())
    throw InvalidArgumentException(HERE) << "The design of experiment must be built from a physical model";

  AnalysisImplementation::setInterestVariables(design.getPhysicalModel().getSelectedOutputsNames());
}


/* Virtual constructor */
DesignOfExperimentEvaluation* DesignOfExperimentEvaluation::clone() const
{
  return new DesignOfExperimentEvaluation(*this);
}


void DesignOfExperimentEvaluation::run()
{
  getDesignOfExperiment().getImplementation()->addObserver(this);
  getDesignOfExperiment().run();
  getDesignOfExperiment().getImplementation()->removeObserver(this);
}


bool DesignOfExperimentEvaluation::analysisLaunched() const
{
  return getDesignOfExperiment().getOutputSample().getSize() > 0;
}


void DesignOfExperimentEvaluation::stop()
{
  getDesignOfExperiment().stop();
}


void DesignOfExperimentEvaluation::update(Observable* source, const String& message)
{
  if (message == "analysisFinished")
  {
    notify("analysisFinished");
  }
  else if (message == "analysisBadlyFinished")
  {
    errorMessage_ = getDesignOfExperiment().getErrorMessage();
    notify("analysisBadlyFinished");
  }
  else if (message == "progressValueChanged")
  {
    progressValue_ = designOfExperiment_.getProgressValue();
    notify("progressValueChanged");
  }
}
}