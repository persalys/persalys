//                                               -*- C++ -*-
/**
 *  @brief Class for Reliability Analysis with the Monte Carlo method
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
#ifndef OTGUI_MONTECARLORELIABILITYANALYSIS_HXX
#define OTGUI_MONTECARLORELIABILITYANALYSIS_HXX

#include "ReliabilityAnalysis.hxx"
#include "LimitState.hxx"
#include "MonteCarloReliabilityAnalysisResult.hxx"

namespace OTGUI {
class OTGUI_API MonteCarloReliabilityAnalysis : public ReliabilityAnalysis
{
  CLASSNAME;

public:
  /** Default constructor */
  MonteCarloReliabilityAnalysis();
  /** Constructor with parameters */
  MonteCarloReliabilityAnalysis(const OT::String & name, const LimitState & limitState,
                                const OT::UnsignedInteger & maximumOuterSampling=1000);

  /** Virtual constructor */
  virtual MonteCarloReliabilityAnalysis * clone() const;

  OT::UnsignedInteger getMaximumOuterSampling() const;
  void setMaximumOuterSampling(const OT::UnsignedInteger & maxi);

  double getMaximumCoefficientOfVariation() const;
  void setMaximumCoefficientOfVariation(const double & coef);

  OT::UnsignedInteger getBlockSize() const;
  void setBlockSize(const OT::UnsignedInteger & size);

  OT::UnsignedInteger getSeed() const;
  void setSeed(const OT::UnsignedInteger seed);

  MonteCarloReliabilityAnalysisResult getResult() const;

  virtual void run();
  virtual OT::String getPythonScript() const;
  virtual bool analysisLaunched() const;

  /** Method save() stores the object through the StorageManager */
  void save(OT::Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  void load(OT::Advocate & adv);

private:
  OT::UnsignedInteger maximumOuterSampling_;
  double maximumCoefficientOfVariation_;
  OT::UnsignedInteger blockSize_;
  OT::UnsignedInteger seed_;
  MonteCarloReliabilityAnalysisResult result_;
};
}
#endif