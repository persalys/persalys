//                                               -*- C++ -*-
/**
 *  @brief Results of the reliability analysis using simulation methods
 *
 *  Copyright 2015-2021 EDF-Phimeca
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
#include "persalys/SimulationReliabilityResult.hxx"

#include <openturns/PersistentObjectFactory.hxx>

using namespace OT;

namespace PERSALYS
{

CLASSNAMEINIT(SimulationReliabilityResult)

static Factory<SimulationReliabilityResult> Factory_SimulationReliabilityResult;

/* Default constructor */
SimulationReliabilityResult::SimulationReliabilityResult()
  : EvaluationResult()
{
}


/* Constructor with parameters */
SimulationReliabilityResult::SimulationReliabilityResult(const ProbabilitySimulationResult& simulationResults,
    const Sample& outputSample,
    const Sample& convergenceSample,
    const Sample& convergenceSampleLowerBound,
    const Sample& convergenceSampleUpperBound
                                                        )
  : EvaluationResult()
  , simulationResult_(simulationResults)
  , convergenceSample_(convergenceSample)
  , convergenceSampleLowerBound_(convergenceSampleLowerBound)
  , convergenceSampleUpperBound_(convergenceSampleUpperBound)
{
  designOfExperiment_.setOutputSample(outputSample);
}


/* Virtual constructor */
SimulationReliabilityResult* SimulationReliabilityResult::clone() const
{
  return new SimulationReliabilityResult(*this);
}


ProbabilitySimulationResult SimulationReliabilityResult::getSimulationResult() const
{
  return simulationResult_;
}


Sample SimulationReliabilityResult::getConvergenceSample() const
{
  return convergenceSample_;
}


Sample SimulationReliabilityResult::getConvergenceSampleLowerBound() const
{
  return convergenceSampleLowerBound_;
}


Sample SimulationReliabilityResult::getConvergenceSampleUpperBound() const
{
  return convergenceSampleUpperBound_;
}


/* String converter */
String SimulationReliabilityResult::__repr__() const
{
  OSS oss;
  oss << "class=" << getClassName()
      << " name=" << getName()
      << " simulationResult=" << getSimulationResult();
  return oss;
}


/* Method save() stores the object through the StorageManager */
void SimulationReliabilityResult::save(Advocate& adv) const
{
  EvaluationResult::save(adv);
  adv.saveAttribute("simulationResult_", simulationResult_);
  adv.saveAttribute("convergenceSample_", convergenceSample_);
  adv.saveAttribute("convergenceSampleLowerBound_", convergenceSampleLowerBound_);
  adv.saveAttribute("convergenceSampleUpperBound_", convergenceSampleUpperBound_);
}


/* Method load() reloads the object from the StorageManager */
void SimulationReliabilityResult::load(Advocate& adv)
{
  EvaluationResult::load(adv);
  adv.loadAttribute("simulationResult_", simulationResult_);
  adv.loadAttribute("convergenceSample_", convergenceSample_);
  adv.loadAttribute("convergenceSampleLowerBound_", convergenceSampleLowerBound_);
  adv.loadAttribute("convergenceSampleUpperBound_", convergenceSampleUpperBound_);
  // can open older xml files
  if (!designOfExperiment_.getSample().getSize())
  {
    Sample outS;
    adv.loadAttribute("outputSample_", outS);
    designOfExperiment_.setOutputSample(outS);
  }
}
}
