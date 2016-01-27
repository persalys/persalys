// MonteCarloReliabilityAnalysis.hxx

#ifndef MONTECARLORELIABILITYANALYSIS_H
#define MONTECARLORELIABILITYANALYSIS_H

#include "ReliabilityAnalysis.hxx"
#include "LimitState.hxx"

#include "SimulationResult.hxx"

namespace OTGUI {
class MonteCarloReliabilityAnalysis : public ReliabilityAnalysis
{
  CLASSNAME;

public:
  MonteCarloReliabilityAnalysis(const OT::String & name, const LimitState & limitState,
                                const OT::UnsignedInteger & maximumOuterSampling=1000);

  virtual MonteCarloReliabilityAnalysis * clone() const;

  OT::UnsignedInteger getMaximumOuterSampling() const;
  void setMaximumOuterSampling(const OT::UnsignedInteger & maxi);

  double getMaximumCoefficientOfVariation() const;
  void setMaximumCoefficientOfVariation(const double & coef);

  OT::UnsignedInteger getBlockSize() const;
  void setBlockSize(const OT::UnsignedInteger & size);

  OT::UnsignedInteger getSeed() const;
  void setSeed(const OT::UnsignedInteger seed);

  OT::SimulationResult getResult() const;

  virtual void run();
  virtual OT::String dump() const;

private:
  OT::UnsignedInteger maximumOuterSampling_;
  double maximumCoefficientOfVariation_;
  OT::UnsignedInteger blockSize_;
  OT::UnsignedInteger seed_;
  OT::SimulationResult result_;
};
}
#endif