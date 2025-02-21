//                                               -*- C++ -*-
/**
 *  @brief Abstract top-level class for ot simulation
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
#ifndef PERSALYS_SIMULATIONINTERFACE_HXX
#define PERSALYS_SIMULATIONINTERFACE_HXX

#include "persalys/PersalysPrivate.hxx"

#include <openturns/ProbabilitySimulationAlgorithm.hxx>

namespace PERSALYS
{
class PERSALYS_BASE_API SimulationInterface : public OT::TypedInterfaceObject<OT::ProbabilitySimulationAlgorithm>
{
public:
  typedef OT::Pointer<OT::ProbabilitySimulationAlgorithm> Implementation;

  /** Default constructor */
  SimulationInterface();
  /** Default constructor */
  SimulationInterface(const OT::ProbabilitySimulationAlgorithm & implementation);
  /** Constructor from implementation */
  SimulationInterface(const Implementation & p_implementation);
  /** Constructor from implementation pointer */
  SimulationInterface(OT::ProbabilitySimulationAlgorithm * p_implementation);

  /** Virtual constructor */
  virtual SimulationInterface * clone() const;

  /** Result accessor */
  OT::ProbabilitySimulationResult getResult() const;

  /** Maximum sample size accessor */
  void setMaximumOuterSampling(const OT::UnsignedInteger maximumOuterSampling);

  /** Maximum coefficient of variation accessor */
  void setMaximumCoefficientOfVariation(const OT::Scalar maximumCoefficientOfVariation);

  /** Maximum time accessor */
  void setMaximumTimeDuration(const OT::Scalar maximumTimeDuration);

  /** Block size accessor */
  void setBlockSize(const OT::UnsignedInteger blockSize);

  /** Convergence strategy accessor */
  void setConvergenceStrategy(const OT::HistoryStrategy & convergenceStrategy);

  /** Performs the actual computation. */
  void run();

  /** Draw the probability convergence at the given level */
  OT::Graph drawProbabilityConvergence(const OT::Scalar level = OT::ResourceMap::GetAsScalar("ProbabilitySimulationResult-DefaultConfidenceLevel")) const;

  /** Progress callback */
  typedef void (*ProgressCallback)(OT::Scalar, void * data);
  void setProgressCallback(ProgressCallback callBack, void * data = 0);

  /** Stop callback */
  typedef OT::Bool (*StopCallback)(void * data);
  void setStopCallback(StopCallback callBack, void * data = 0);
};
}
#endif
