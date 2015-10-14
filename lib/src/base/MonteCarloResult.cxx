// MonteCarloResult.cxx

#include "MonteCarloResult.hxx"
#include "Normal.hxx"
#include "ChiSquare.hxx"
#include "KernelSmoothing.hxx"

namespace OTGUI{

CLASSNAMEINIT(MonteCarloResult);

MonteCarloResult::MonteCarloResult()
  : ParametricCalculusResult()
{
}

MonteCarloResult::MonteCarloResult(OT::NumericalSample outputSample, OT::NumericalSample inputSample,
                                   int nbSimu, bool confidenceInterval, double level)
 : ParametricCalculusResult(outputSample, inputSample)
{
  mean_ = outputSample.computeMean();
  standardDeviation_ = outputSample.computeStandardDeviationPerComponent();
  variance_ = outputSample.computeVariance();
  skewness_ = OT::NumericalPoint(outputSample.getDimension());
  if (!(outputSample.getSize() < 2))
    skewness_ = outputSample.computeSkewness();
  kurtosis_ = OT::NumericalPoint(outputSample.getDimension());
  if (!(outputSample.getSize() < 3))
    kurtosis_ = outputSample.computeKurtosis();

  firstQuartile_ = outputSample.computeQuantile(0.25);
  thirdQuartile_ = outputSample.computeQuantile(0.75);

  meanConfidenceInterval_ = OT::NumericalSample(outputSample.getDimension(), 2);
  stdConfidenceInterval_ = OT::NumericalSample(outputSample.getDimension(), 2);
  if (confidenceInterval)
  {
    computeMeanConfidenceInterval(nbSimu, level);
    computeStdConfidenceInterval(nbSimu, level);
  }
}
  

MonteCarloResult::~MonteCarloResult()
{
}


OT::NumericalPoint MonteCarloResult::getMean() const
{
  return mean_;
}


OT::NumericalPoint MonteCarloResult::getStandardDeviation() const
{
  return standardDeviation_;
}


OT::NumericalPoint MonteCarloResult::getVariance() const
{
  return variance_;
}


OT::NumericalPoint MonteCarloResult::getSkewness() const
{
  return skewness_;
}


OT::NumericalPoint MonteCarloResult::getKurtosis() const
{
  return kurtosis_;
}


OT::NumericalPoint MonteCarloResult::getFirstQuartile() const
{
  return firstQuartile_;
}


OT::NumericalPoint MonteCarloResult::getThirdQuartile() const
{
  return thirdQuartile_;
}


OT::NumericalSample MonteCarloResult::getMeanConfidenceInterval() const
{
  return meanConfidenceInterval_;
}


OT::NumericalSample MonteCarloResult::getStdConfidenceInterval() const
{
  return stdConfidenceInterval_;
}


OT::Distribution MonteCarloResult::getFittedDistribution() const
{
  OT::KernelSmoothing gaussianKernel = OT::KernelSmoothing();
  return gaussianKernel.build(getResultSample(), true);
}


void MonteCarloResult::computeMeanConfidenceInterval(int nbSimu, double level)
{
  OT::Normal X(0,1);
  double f = X.computeQuantile(level/2, true)[0];
  OT::NumericalPoint delta = f * standardDeviation_ / sqrt(nbSimu);

  for (int i=0; i<meanConfidenceInterval_.getSize(); ++i)
  {
    // low
    meanConfidenceInterval_[i][0] = mean_[i] - delta[i];
    // up
    meanConfidenceInterval_[i][1] = mean_[i] + delta[i];
  }
}


void MonteCarloResult::computeStdConfidenceInterval(int nbSimu, double level)
{
  // TODO : use Normal Distribution?
  OT::ChiSquare X(nbSimu-1);
  // low
  double f1 = X.computeQuantile(level/2, true)[0];
  // up
  double f2 = X.computeQuantile(level/2, false)[0];

  for (int i=0; i<stdConfidenceInterval_.getSize(); ++i)
  {
    //low
    stdConfidenceInterval_[i][0] = sqrt((nbSimu - 1) * variance_[i] / f1);
    //up
    stdConfidenceInterval_[i][1] = sqrt((nbSimu - 1) * variance_[i] / f2);
  }
}

}