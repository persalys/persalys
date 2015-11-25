// MonteCarloReliabilityAnalysis.hxx

#ifndef MONTECARLORELIABILITYANALYSIS_H
#define MONTECARLORELIABILITYANALYSIS_H

#include "AnalysisImplementation.hxx"
#include "LimitState.hxx"

#include "SimulationResult.hxx"

namespace OTGUI {
class MonteCarloReliabilityAnalysis : public AnalysisImplementation
{
  CLASSNAME;

public:
  MonteCarloReliabilityAnalysis(const std::string & name, const LimitState & limitState,
                                const OT::UnsignedInteger & maximumOuterSampling=1000);

  MonteCarloReliabilityAnalysis(const MonteCarloReliabilityAnalysis & other);
  virtual MonteCarloReliabilityAnalysis * clone() const;

  LimitState getLimitState() const;

  OT::UnsignedInteger getMaximumOuterSampling() const;
  void setMaximumOuterSampling(const OT::UnsignedInteger & maxi);

  double getMaximumCoefficientOfVariation() const;
  void setMaximumCoefficientOfVariation(const double & coef);

  OT::SimulationResult getResult() const;

  virtual void run();
  virtual std::string dump() const;

private:
  LimitState limitState_;
  OT::UnsignedInteger maximumOuterSampling_;
  double maximumCoefficientOfVariation_;
  OT::SimulationResult result_;
};
}
#endif