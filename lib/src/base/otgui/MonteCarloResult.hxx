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

  typedef OT::Collection<OT::NumericalPoint> NumericalPointCollection;
  typedef OT::Collection<OT::Distribution> DistributionCollection;

  MonteCarloResult();
  MonteCarloResult(OT::NumericalSample inputSample, OT::NumericalSample outputSample);

  virtual ~MonteCarloResult();

  OT::NumericalPoint getMean();
  OT::NumericalPoint getMedian();
  OT::NumericalPoint getStandardDeviation();
  OT::NumericalPoint getVariance();
  OT::NumericalPoint getSkewness();
  OT::NumericalPoint getKurtosis();
  OT::NumericalPoint getFirstQuartile();
  OT::NumericalPoint getThirdQuartile();
  OT::Interval getMeanConfidenceInterval(const double level=0.95);
  OT::Interval getStdConfidenceInterval(const double level=0.95);
  NumericalPointCollection getOutliers();

  DistributionCollection getFittedDistribution();

private:
  double levelConfidenceInterval_;
  OT::NumericalPoint mean_;
  OT::NumericalPoint median_;
  OT::NumericalPoint standardDeviation_;
  OT::NumericalPoint variance_;
  OT::NumericalPoint skewness_;
  OT::NumericalPoint kurtosis_;
  OT::NumericalPoint firstQuartile_;
  OT::NumericalPoint thirdQuartile_;
  OT::Interval meanConfidenceInterval_;
  OT::Interval stdConfidenceInterval_;
  NumericalPointCollection outliers_;
};
}
#endif