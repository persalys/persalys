//                                               -*- C++ -*-
/**
 *  @brief Class for Reliability Analysis with the Monte Carlo method
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
#include "otgui/MonteCarloReliabilityAnalysis.hxx"

#include <openturns/MonteCarlo.hxx>
#include <openturns/PersistentObjectFactory.hxx>

using namespace OT;

namespace OTGUI {

CLASSNAMEINIT(MonteCarloReliabilityAnalysis);

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
  return MonteCarlo(event);
}
}