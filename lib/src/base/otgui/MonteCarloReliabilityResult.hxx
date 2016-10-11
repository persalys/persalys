//                                               -*- C++ -*-
/**
 *  @brief Results of the reliability analysis with Monte Carlo method 
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
#ifndef OTGUI_MONTECARLORELIABILITYANALYSISRESULT_HXX
#define OTGUI_MONTECARLORELIABILITYANALYSISRESULT_HXX

#include "openturns/SimulationResult.hxx"
#include "otgui/OTGuiprivate.hxx"

namespace OTGUI {
class OTGUI_API MonteCarloReliabilityResult : public OT::PersistentObject
{
  CLASSNAME;

public:
  /** Default constructor */
  MonteCarloReliabilityResult();
  /** Constructor with parameters */
  MonteCarloReliabilityResult(const OT::SimulationResult & simulationResults,
                                      const OT::NumericalSample & outputSample,
                                      const OT::NumericalSample & convergenceSample,
                                      const OT::NumericalSample & convergenceSampleLowerBound,
                                      const OT::NumericalSample & convergenceSampleUpperBound);

  /** Virtual constructor */
  virtual MonteCarloReliabilityResult * clone() const;

  OT::SimulationResult getSimulationResult() const;
  OT::NumericalSample getOutputSample() const;
  OT::NumericalSample getConvergenceSample() const;
  OT::NumericalSample getConvergenceSampleLowerBound() const;
  OT::NumericalSample getConvergenceSampleUpperBound() const;

  double getElapsedTime() const;
  void setElapsedTime(const double seconds);

  /** String converter */
  virtual OT::String __repr__() const;

  /** Method save() stores the object through the StorageManager */
  void save(OT::Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  void load(OT::Advocate & adv);

private:
  OT::SimulationResult simulationResult_;
  OT::NumericalSample outputSample_;
  OT::NumericalSample convergenceSample_;
  OT::NumericalSample convergenceSampleLowerBound_;
  OT::NumericalSample convergenceSampleUpperBound_;
  double elapsedTime_;
};
}
#endif
