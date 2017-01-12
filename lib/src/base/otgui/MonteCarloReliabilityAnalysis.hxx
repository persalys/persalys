//                                               -*- C++ -*-
/**
 *  @brief Class for Reliability Analysis with the Monte Carlo method
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
#ifndef OTGUI_MONTECARLORELIABILITYANALYSIS_HXX
#define OTGUI_MONTECARLORELIABILITYANALYSIS_HXX

#include "ReliabilityAnalysis.hxx"
#include "WithStopCriteriaAnalysis.hxx"
#include "MonteCarloReliabilityResult.hxx"

namespace OTGUI {
class OTGUI_API MonteCarloReliabilityAnalysis : public ReliabilityAnalysis, public WithStopCriteriaAnalysis
{
  CLASSNAME;

public:
  /** Default constructor */
  MonteCarloReliabilityAnalysis();
  /** Constructor with parameters */
  MonteCarloReliabilityAnalysis(const OT::String & name, const LimitState & limitState);

  /** Virtual constructor */
  virtual MonteCarloReliabilityAnalysis * clone() const;

  OT::UnsignedInteger getSeed() const;
  void setSeed(const OT::UnsignedInteger seed);

  MonteCarloReliabilityResult getResult() const;

  virtual void run();
  virtual OT::String getPythonScript() const;
  virtual bool analysisLaunched() const;

  virtual void stop();

  /** String converter */
  virtual OT::String __repr__() const;

  /** Method save() stores the object through the StorageManager */
  void save(OT::Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  void load(OT::Advocate & adv);

private:
  OT::UnsignedInteger seed_;
  TimeCriteria * timeCriteria_;
  MonteCarloReliabilityResult result_;
};
}
#endif