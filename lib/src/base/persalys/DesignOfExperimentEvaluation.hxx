//                                               -*- C++ -*-
/**
 *  @brief Evaluates the design of experiments
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
#ifndef PERSALYS_DESIGNOFEXPERIMENTEVALUATION_HXX
#define PERSALYS_DESIGNOFEXPERIMENTEVALUATION_HXX

#include "SimulationAnalysis.hxx"
#include "DataAnalysisResult.hxx"

namespace PERSALYS
{
class PERSALYS_BASE_API DesignOfExperimentEvaluation : public SimulationAnalysis
{
  CLASSNAME

public:
  /** Default constructor */
  DesignOfExperimentEvaluation();

  /** Constructor with parameters */
  DesignOfExperimentEvaluation(const OT::String& name, const PhysicalModel& physicalModel);

  /** Virtual constructor */
  DesignOfExperimentEvaluation * clone() const override;

  /** Object name accessor */
  void setName(const OT::String& name) override;

  void removeAllObservers() override;

  virtual OT::Sample getOriginalInputSample() const;

  OT::Sample getNotEvaluatedInputSample() const;

  void setDesignOfExperiment(const DesignOfExperiment& designOfExperiment);
  DataAnalysisResult getResult() const;

  Parameters getParameters() const override;
  bool hasValidResult() const override;
  bool canBeLaunched(OT::String &errorMessage) const override;

  static bool StopRequested(void* state);

  /** Method save() stores the object through the StorageManager */
  void save(OT::Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(OT::Advocate & adv) override;

protected:
  void initialize() override;
  void launch() override;

protected:
  mutable OT::Sample originalInputSample_;
  DataAnalysisResult result_;
};
}
#endif
