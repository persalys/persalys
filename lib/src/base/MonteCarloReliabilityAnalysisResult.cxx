//                                               -*- C++ -*-
/**
 *  @brief Results of the reliability analysis with Monte Carlo method
 *
 *  Copyright 2015-2016 EDF
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
#include "otgui/MonteCarloReliabilityAnalysisResult.hxx"

#include "PersistentObjectFactory.hxx"

using namespace OT;

namespace OTGUI {

CLASSNAMEINIT(MonteCarloReliabilityAnalysisResult);

static Factory<MonteCarloReliabilityAnalysisResult> RegisteredFactory("MonteCarloReliabilityAnalysisResult");

/* Default constructor */
MonteCarloReliabilityAnalysisResult::MonteCarloReliabilityAnalysisResult()
  : PersistentObject()
{
}


/* Constructor with parameters */
MonteCarloReliabilityAnalysisResult::MonteCarloReliabilityAnalysisResult(const SimulationResult & simulationResults,
                                                                         const NumericalSample & outputSample,
                                                                         const NumericalSample & convergenceSample,
                                                                         const NumericalSample & convergenceSampleLowerBound,
                                                                         const NumericalSample & convergenceSampleUpperBound)
  : PersistentObject()
  , simulationResult_(simulationResults)
  , outputSample_(outputSample)
  , convergenceSample_(convergenceSample)
  , convergenceSampleLowerBound_(convergenceSampleLowerBound)
  , convergenceSampleUpperBound_(convergenceSampleUpperBound)
{
}


/* Virtual constructor */
MonteCarloReliabilityAnalysisResult* MonteCarloReliabilityAnalysisResult::clone() const
{
  return new MonteCarloReliabilityAnalysisResult(*this);
}


SimulationResult MonteCarloReliabilityAnalysisResult::getSimulationResult() const
{
  return simulationResult_;
}


NumericalSample MonteCarloReliabilityAnalysisResult::getOutputSample() const
{
  return outputSample_;
}


NumericalSample MonteCarloReliabilityAnalysisResult::getConvergenceSample() const
{
  return convergenceSample_;
}


NumericalSample MonteCarloReliabilityAnalysisResult::getConvergenceSampleLowerBound() const
{
  return convergenceSampleLowerBound_;
}


NumericalSample MonteCarloReliabilityAnalysisResult::getConvergenceSampleUpperBound() const
{
  return convergenceSampleUpperBound_;
}


/* Method save() stores the object through the StorageManager */
void MonteCarloReliabilityAnalysisResult::save(Advocate& adv) const
{
  PersistentObject::save(adv);
  adv.saveAttribute("simulationResult_", simulationResult_);
  adv.saveAttribute("outputSample_", outputSample_);
  adv.saveAttribute("convergenceSample_", convergenceSample_);
}


/* Method load() reloads the object from the StorageManager */
void MonteCarloReliabilityAnalysisResult::load(Advocate& adv)
{
  PersistentObject::load(adv);
  adv.loadAttribute("simulationResult_", simulationResult_);
  adv.loadAttribute("outputSample_", outputSample_);
  adv.loadAttribute("convergenceSample_", convergenceSample_);
}
}