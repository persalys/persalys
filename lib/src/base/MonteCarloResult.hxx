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
                   double level=0.95);

  virtual ~MonteCarloResult();

  OT::NumericalPoint getMean();
  OT::NumericalPoint getStandardDeviation();
  OT::NumericalPoint getVariance();
  OT::NumericalPoint getSkewness();
  OT::NumericalPoint getKurtosis();
  OT::NumericalPoint getFirstQuartile();
  OT::NumericalPoint getThirdQuartile();
  OT::Interval getMeanConfidenceInterval();
  OT::Interval getStdConfidenceInterval();

  OT::Distribution getFittedDistribution();

private:
  double level_;
  OT::NumericalPoint mean_;
  OT::NumericalPoint standardDeviation_;
  OT::NumericalPoint variance_;
  OT::NumericalPoint skewness_;
  OT::NumericalPoint kurtosis_;
  OT::NumericalPoint firstQuartile_;
  OT::NumericalPoint thirdQuartile_;
  OT::Interval meanConfidenceInterval_;
  OT::Interval stdConfidenceInterval_;
};
}
#endif