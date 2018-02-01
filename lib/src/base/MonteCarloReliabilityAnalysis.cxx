//                                               -*- C++ -*-
/**
 *  @brief Class for Reliability Analysis with the Monte Carlo method
 *
 *  Copyright 2015-2018 EDF-Phimeca
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
#include "otgui/MonteCarloReliabilityAnalysis.hxx"

#include <openturns/ProbabilitySimulationAlgorithm.hxx>
#include <openturns/PersistentObjectFactory.hxx>
#include <openturns/MonteCarloExperiment.hxx>

using namespace OT;

namespace OTGUI
{

CLASSNAMEINIT(MonteCarloReliabilityAnalysis)

static Factory<MonteCarloReliabilityAnalysis> Factory_MonteCarloReliabilityAnalysis;

/* Default constructor */
MonteCarloReliabilityAnalysis::MonteCarloReliabilityAnalysis()
  : SimulationReliabilityAnalysis()
{
}


/* Constructor with parameters */
MonteCarloReliabilityAnalysis::MonteCarloReliabilityAnalysis(const String& name,
    const LimitState& limitState)
  : SimulationReliabilityAnalysis(name, limitState)
{
}


/* Virtual constructor */
MonteCarloReliabilityAnalysis* MonteCarloReliabilityAnalysis::clone() const
{
  return new MonteCarloReliabilityAnalysis(*this);
}


SimulationInterface MonteCarloReliabilityAnalysis::getSimulationAlgorithm(const Event& event)
{
  const MonteCarloExperiment experiment;
  return ProbabilitySimulationAlgorithm(event, experiment);
}


Parameters MonteCarloReliabilityAnalysis::getParameters() const
{
  Parameters param;

  param.add("Algorithm", "Monte Carlo");
  param.add("Output of interest", getLimitState().getOutputName());
  param.add("Maximum coefficient of variation", getMaximumCoefficientOfVariation());
  String time = "- (s)";
  if (getMaximumCalls() < (UnsignedInteger)std::numeric_limits<int>::max())
    time = (OSS() << getMaximumElapsedTime()).str() + "(s)";
  param.add("Maximum elapsed time", time);
  String maxCalls = "-";
  if (getMaximumCalls() < (UnsignedInteger)std::numeric_limits<int>::max())
    maxCalls = (OSS() << getMaximumCalls()).str();
  param.add("Maximum calls", maxCalls);
  param.add("Block size", getBlockSize());
  param.add("Seed", getSeed());

  return param;
}
}
