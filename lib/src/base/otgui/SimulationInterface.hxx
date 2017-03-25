//                                               -*- C++ -*-
/**
 *  @brief Abstract top-level class for ot simulation 
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
#ifndef OTGUI_SIMULATIONINTERFACE_HXX
#define OTGUI_SIMULATIONINTERFACE_HXX

#include "otgui/OTGuiprivate.hxx"

#include <openturns/Simulation.hxx>

namespace OTGUI {
class OTGUI_API SimulationInterface : public OT::TypedInterfaceObject<OT::Simulation>
{
public:
  typedef OT::Pointer<OT::Simulation> Implementation;

  /** Default constructor */
  SimulationInterface();
  /** Default constructor */
  SimulationInterface(const OT::Simulation & implementation);
  /** Constructor from implementation */
  SimulationInterface(const Implementation & p_implementation);
  /** Constructor from implementation pointer */
  SimulationInterface(OT::Simulation * p_implementation);

  /** Virtual constructor */
  virtual SimulationInterface * clone() const;

  /** Result accessor */
  OT::SimulationResult getResult() const;

  /** Maximum sample size accessor */
  void setMaximumOuterSampling(const OT::UnsignedInteger maximumOuterSampling);

  /** Maximum coefficient of variation accessor */
  void setMaximumCoefficientOfVariation(const OT::NumericalScalar maximumCoefficientOfVariation);

  /** Block size accessor */
  void setBlockSize(const OT::UnsignedInteger blockSize);

  /** Convergence strategy accessor */
  void setConvergenceStrategy(const OT::HistoryStrategy & convergenceStrategy);

  /** Performs the actual computation. */
  void run();

  /** Draw the probability convergence at the given level */
  OT::Graph drawProbabilityConvergence(const OT::NumericalScalar level = OT::ResourceMap::GetAsNumericalScalar("SimulationResult-DefaultConfidenceLevel")) const;

  /** Progress callback */
  typedef void (*ProgressCallback)(OT::NumericalScalar, void * data);
  void setProgressCallback(ProgressCallback callBack, void * data = 0);

  /** Stop callback */
  typedef OT::Bool (*StopCallback)(void * data);
  void setStopCallback(StopCallback callBack, void * data = 0);
};
}
#endif