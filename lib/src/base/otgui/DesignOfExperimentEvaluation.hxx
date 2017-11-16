//                                               -*- C++ -*-
/**
 *  @brief Evaluates the design of experiments
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

#include "SimulationAnalysis.hxx"

namespace OTGUI
{
class OTGUI_API DesignOfExperimentEvaluation : public SimulationAnalysis
{
  CLASSNAME

public:
  /** Default constructor */
  DesignOfExperimentEvaluation();

  /** Constructor with parameters */
  DesignOfExperimentEvaluation(const OT::String& name, const PhysicalModel& physicalModel);

  /** Virtual constructor */
  virtual DesignOfExperimentEvaluation * clone() const;

  /** Object name accessor */
  virtual void setName(const OT::String& name);

  virtual OT::Sample getOriginalInputSample() const;
  void setDesignOfExperiment(const DesignOfExperiment & designOfExperiment);

  OT::Sample getNotEvaluatedInputSample() const;

  virtual Parameters getParameters() const;

  /** Method save() stores the object through the StorageManager */
  void save(OT::Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  void load(OT::Advocate & adv);

protected:
  virtual void launch();

protected:
  mutable OT::Sample originalInputSample_;
};
}
#endif
