//                                               -*- C++ -*-
/**
 *  @brief Class for Reliability Analysis with the Monte Carlo method
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
#ifndef PERSALYS_MONTECARLORELIABILITYANALYSIS_HXX
#define PERSALYS_MONTECARLORELIABILITYANALYSIS_HXX

#include "SimulationReliabilityAnalysis.hxx"

namespace PERSALYS
{
class PERSALYS_BASE_API MonteCarloReliabilityAnalysis : public SimulationReliabilityAnalysis
{
  CLASSNAME

public:
  /** Default constructor */
  MonteCarloReliabilityAnalysis();
  /** Constructor with parameters */
  MonteCarloReliabilityAnalysis(const OT::String& name, const LimitState& limitState);

  /** Virtual constructor */
  MonteCarloReliabilityAnalysis * clone() const override;

  Parameters getParameters() const override;

protected:
  SimulationInterface getSimulationAlgorithm(const OT::RandomVector & event) override;
};
}
#endif
