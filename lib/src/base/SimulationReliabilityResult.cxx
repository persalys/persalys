//                                               -*- C++ -*-
/**
 *  @brief Results of the reliability analysis using simulation methods
 *
 *  Copyright 2015-2016 EDF-Phimeca
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
#include "otgui/SimulationReliabilityResult.hxx"

#include <openturns/PersistentObjectFactory.hxx>

using namespace OT;

namespace OTGUI {

CLASSNAMEINIT(SimulationReliabilityResult);

static Factory<SimulationReliabilityResult> Factory_SimulationReliabilityResult;

/* Default constructor */
SimulationReliabilityResult::SimulationReliabilityResult()
  : PersistentObject()
  , elapsedTime_(0.)
{
}


/* Constructor with parameters */
SimulationReliabilityResult::SimulationReliabilityResult(const SimulationResult& simulationResults,
                                                         const NumericalSample& outputSample,
                                                         const NumericalSample& convergenceSample,
                                                         const NumericalSample& convergenceSampleLowerBound,
                                                         const NumericalSample& convergenceSampleUpperBound
                                                        )
  : PersistentObject()
  , simulationResult_(simulationResults)
  , outputSample_(outputSample)
  , convergenceSample_(convergenceSample)
  , convergenceSampleLowerBound_(convergenceSampleLowerBound)
  , convergenceSampleUpperBound_(convergenceSampleUpperBound)
  , elapsedTime_(0.)
{
}


/* Virtual constructor */
SimulationReliabilityResult* SimulationReliabilityResult::clone() const
{
  return new SimulationReliabilityResult(*this);
}


SimulationResult SimulationReliabilityResult::getSimulationResult() const
{
  return simulationResult_;
}


NumericalSample SimulationReliabilityResult::getOutputSample() const
{
  return outputSample_;
}


NumericalSample SimulationReliabilityResult::getConvergenceSample() const
{
  return convergenceSample_;
}


NumericalSample SimulationReliabilityResult::getConvergenceSampleLowerBound() const
{
  return convergenceSampleLowerBound_;
}


NumericalSample SimulationReliabilityResult::getConvergenceSampleUpperBound() const
{
  return convergenceSampleUpperBound_;
}


NumericalScalar SimulationReliabilityResult::getElapsedTime() const
{
  return elapsedTime_;
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
  PersistentObject::save(adv);
  adv.saveAttribute("simulationResult_", simulationResult_);
  adv.saveAttribute("outputSample_", outputSample_);
  adv.saveAttribute("convergenceSample_", convergenceSample_);
  adv.saveAttribute("convergenceSampleLowerBound_", convergenceSampleLowerBound_);
  adv.saveAttribute("convergenceSampleUpperBound_", convergenceSampleUpperBound_);
  adv.saveAttribute("elapsedTime_", elapsedTime_);
}


/* Method load() reloads the object from the StorageManager */
void SimulationReliabilityResult::load(Advocate& adv)
{
  PersistentObject::load(adv);
  adv.loadAttribute("simulationResult_", simulationResult_);
  adv.loadAttribute("outputSample_", outputSample_);
  adv.loadAttribute("convergenceSample_", convergenceSample_);
  adv.loadAttribute("convergenceSampleLowerBound_", convergenceSampleLowerBound_);
  adv.loadAttribute("convergenceSampleUpperBound_", convergenceSampleUpperBound_);
  adv.loadAttribute("elapsedTime_", elapsedTime_);
}
}