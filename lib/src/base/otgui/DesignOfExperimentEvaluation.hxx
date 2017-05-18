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
#ifndef OTGUI_DESIGNOFEXPERIMENTEVALUATION_HXX
#define OTGUI_DESIGNOFEXPERIMENTEVALUATION_HXX

#include "DesignOfExperimentAnalysis.hxx"

namespace OTGUI {
class OTGUI_API DesignOfExperimentEvaluation : public DesignOfExperimentAnalysis, public Observer
{
  CLASSNAME;
  /** Fake analysis to be enable to launch the evaluation of the DOE in a separate thread from StudyTreeView */

public:
  /** Constructor with parameters */
  DesignOfExperimentEvaluation(const DesignOfExperiment& design);

  /** Virtual constructor */
  virtual DesignOfExperimentEvaluation * clone() const;

  virtual void run();
  virtual bool analysisLaunched() const;
  virtual void stop();

  virtual void update(Observable * source, const OT::String & message);
};
}
#endif