//                                               -*- C++ -*-
/**
 *  @brief Results of the reliability analysis using simulation methods
 *
 *  Copyright 2015-2026 EDF-Phimeca
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
#ifndef PERSALYS_SIMULATIONRELIABILITYRESULT_HXX
#define PERSALYS_SIMULATIONRELIABILITYRESULT_HXX

#include "EvaluationResult.hxx"

#include <openturns/ProbabilitySimulationResult.hxx>

namespace PERSALYS
{
class PERSALYS_BASE_API SimulationReliabilityResult : public EvaluationResult
{
  CLASSNAME

public:
  friend class SimulationReliabilityAnalysis;

  /** Default constructor */
  SimulationReliabilityResult() = default;

  /** Constructor with parameters */
  SimulationReliabilityResult(const OT::ProbabilitySimulationResult& simulationResults,
                              const OT::Collection<OT::Sample> &inSamples,
                              const OT::Collection<OT::Sample> &outSamples,
                              const OT::Sample& convergenceSample,
                              const OT::Sample& convergenceSampleLowerBound,
                              const OT::Sample& convergenceSampleUpperBound,
                              bool sharedInputs = false);

  /** Virtual constructor */
  SimulationReliabilityResult * clone() const override;

  OT::ProbabilitySimulationResult getSimulationResult() const;
  OT::Sample getConvergenceSample() const;
  OT::Sample getConvergenceSampleLowerBound() const;
  OT::Sample getConvergenceSampleUpperBound() const;
  OT::Collection<OT::Sample> getInputSamples() const;
  OT::Collection<OT::Sample> getOutputSamples() const;

  /** String converter */
  OT::String __repr__() const override;

  /** Method save() stores the object through the StorageManager */
  void save(OT::Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(OT::Advocate & adv) override;

private:
  OT::PersistentCollection<OT::Sample> inputSamples_;
  OT::PersistentCollection<OT::Sample> outputSamples_;
  OT::ProbabilitySimulationResult simulationResult_;
  OT::Sample convergenceSample_;
  OT::Sample convergenceSampleLowerBound_;
  OT::Sample convergenceSampleUpperBound_;
  OT::Bool sharedInputs_ = false;
};
}
#endif
