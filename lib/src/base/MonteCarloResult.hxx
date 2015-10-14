// MonteCarloResult.hxx

#ifndef MONTECARLORESULT_H
#define MONTECARLORESULT_H

#include "ParametricCalculusResult.hxx"
#include "Distribution.hxx"

namespace OTGUI {
class MonteCarloResult : public ParametricCalculusResult
{
  CLASSNAME;

public:
  MonteCarloResult();
  MonteCarloResult(OT::NumericalSample outputSample, OT::NumericalSample inputSample,
                   int nbSimu=10000, bool confidenceInterval=true, double level=0.05);

  virtual ~MonteCarloResult();

  OT::NumericalPoint getMean() const;
  OT::NumericalPoint getStandardDeviation() const;
  OT::NumericalPoint getVariance() const;
  OT::NumericalPoint getSkewness() const;
  OT::NumericalPoint getKurtosis() const;
  OT::NumericalPoint getFirstQuartile() const;
  OT::NumericalPoint getThirdQuartile() const;
  OT::NumericalSample getMeanConfidenceInterval() const;
  OT::NumericalSample getStdConfidenceInterval() const;

  OT::Distribution getFittedDistribution() const;

protected:
  void computeMeanConfidenceInterval(int nbSimu, double level);
  void computeStdConfidenceInterval(int nbSimu, double level);

private:
  OT::NumericalPoint mean_;
  OT::NumericalPoint standardDeviation_;
  OT::NumericalPoint variance_;
  OT::NumericalPoint skewness_;
  OT::NumericalPoint kurtosis_;
  OT::NumericalPoint firstQuartile_;
  OT::NumericalPoint thirdQuartile_;
  OT::NumericalSample meanConfidenceInterval_;
  OT::NumericalSample stdConfidenceInterval_;
};
}
#endif