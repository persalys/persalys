//                                               -*- C++ -*-
/**
 *  @brief Abstract top-level class for all physical models
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
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
#include "otgui/SimulationInterface.hxx"

using namespace OT;

namespace OTGUI
{

/* Default constructor */
SimulationInterface::SimulationInterface()
  : TypedInterfaceObject<ProbabilitySimulationAlgorithm>(new OT::ProbabilitySimulationAlgorithm())
{
}


/* Default constructor */
SimulationInterface::SimulationInterface(const ProbabilitySimulationAlgorithm & implementation)
  : TypedInterfaceObject<ProbabilitySimulationAlgorithm>(implementation.clone())
{

}


/* Constructor from implementation */
SimulationInterface::SimulationInterface(const Implementation & p_implementation)
  : TypedInterfaceObject<ProbabilitySimulationAlgorithm>(p_implementation)
{
  // Initialize any other class members here
  // At last, allocate memory space if needed, but go to destructor to free it
}

/* Constructor from implementation pointer */
SimulationInterface::SimulationInterface(ProbabilitySimulationAlgorithm * p_implementation)
  : TypedInterfaceObject<ProbabilitySimulationAlgorithm>(p_implementation)
{
  // Initialize any other class members here
  // At last, allocate memory space if needed, but go to destructor to free it
}


SimulationInterface* SimulationInterface::clone() const
{
  return new SimulationInterface(*this);
}


ProbabilitySimulationResult SimulationInterface::getResult() const
{
  return getImplementation()->getResult();
}


void SimulationInterface::setMaximumOuterSampling(const UnsignedInteger maximumOuterSampling)
{
  getImplementation()->setMaximumOuterSampling(maximumOuterSampling);
}


void SimulationInterface::setMaximumCoefficientOfVariation(const Scalar maximumCoefficientOfVariation)
{
  getImplementation()->setMaximumCoefficientOfVariation(maximumCoefficientOfVariation);
}


void SimulationInterface::setBlockSize(const UnsignedInteger blockSize)
{
  getImplementation()->setBlockSize(blockSize);
}


void SimulationInterface::setConvergenceStrategy(const HistoryStrategy& convergenceStrategy)
{
  getImplementation()->setConvergenceStrategy(convergenceStrategy);
}


void SimulationInterface::run()
{
  getImplementation()->run();
}


Graph SimulationInterface::drawProbabilityConvergence(const Scalar level) const
{
  return getImplementation()->drawProbabilityConvergence(level);
}


void SimulationInterface::setProgressCallback(SimulationInterface::ProgressCallback callBack, void* data)
{
  getImplementation()->setProgressCallback(callBack, data);
}


void SimulationInterface::setStopCallback(SimulationInterface::StopCallback callBack, void* data)
{
  getImplementation()->setStopCallback(callBack, data);
}
}
