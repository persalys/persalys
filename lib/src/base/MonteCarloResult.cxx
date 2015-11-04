// MonteCarloResult.cxx

#include "otgui/MonteCarloResult.hxx"
#include "Normal.hxx"
#include "ChiSquare.hxx"
#include "KernelSmoothing.hxx"

using namespace OT;

namespace OTGUI{

CLASSNAMEINIT(MonteCarloResult);

MonteCarloResult::MonteCarloResult()
  : ParametricCalculusResult()
{
}

MonteCarloResult::MonteCarloResult(NumericalSample inputSample, NumericalSample outputSample)
 : ParametricCalculusResult(inputSample, outputSample)
 , levelConfidenceInterval_(0.95)
{
}
  

MonteCarloResult::~MonteCarloResult()
{
}


NumericalPoint MonteCarloResult::getMean()
{
  if (!mean_.getSize())
    mean_ = getResultSample().computeMean();
  return mean_;
}


NumericalPoint MonteCarloResult::getMedian()
{
  if (!median_.getSize())
    median_ = getResultSample().computeMedian();
  return median_;
}


NumericalPoint MonteCarloResult::getStandardDeviation()
{
  if (!standardDeviation_.getSize())
    standardDeviation_ = getResultSample().computeStandardDeviationPerComponent();
  return standardDeviation_;
}


NumericalPoint MonteCarloResult::getVariance()
{
  if (!variance_.getSize())
    variance_ = getResultSample().computeVariance();
  return variance_;
}


NumericalPoint MonteCarloResult::getSkewness()
{
  if (!skewness_.getSize())
  {
    skewness_ = NumericalPoint(getResultSample().getDimension());
    if (!(getResultSample().getSize() < 2))
      skewness_ = getResultSample().computeSkewness();
  }
  return skewness_;
}


NumericalPoint MonteCarloResult::getKurtosis()
{
  if (!kurtosis_.getSize())
  {
    kurtosis_ = NumericalPoint(getResultSample().getDimension());
    if (!(getResultSample().getSize() < 3))
      kurtosis_ = getResultSample().computeKurtosis();
  }
  return kurtosis_;
}


NumericalPoint MonteCarloResult::getFirstQuartile()
{
  if (!firstQuartile_.getSize())
    firstQuartile_ = getResultSample().computeQuantilePerComponent(0.25);
  return firstQuartile_;
}


NumericalPoint MonteCarloResult::getThirdQuartile()
{
  if (!thirdQuartile_.getSize())
    thirdQuartile_ = getResultSample().computeQuantilePerComponent(0.75);
  return thirdQuartile_;
}


Interval MonteCarloResult::getMeanConfidenceInterval(const double level)
{
  if (!meanConfidenceInterval_.getDimension() || levelConfidenceInterval_ != level)
  {
    levelConfidenceInterval_ = level;
    meanConfidenceInterval_ = Interval(getResultSample().getDimension());

    Normal X(0,1);
    double f = X.computeQuantile((1-levelConfidenceInterval_)/2, true)[0];
    NumericalPoint delta = f * getStandardDeviation() / sqrt(getResultSample().getSize());
    
    NumericalPoint lowerBounds(getResultSample().getDimension());
    NumericalPoint upperBounds(getResultSample().getDimension());

    for (int i=0; i<meanConfidenceInterval_.getDimension(); ++i)
    {
      // low
      lowerBounds[i] = getMean()[i] - delta[i];
      // up
      upperBounds[i] = getMean()[i] + delta[i];
    }
    meanConfidenceInterval_.setLowerBound(lowerBounds);
    meanConfidenceInterval_.setUpperBound(upperBounds);
  }
  return meanConfidenceInterval_;
}


Interval MonteCarloResult::getStdConfidenceInterval(const double level)
{
  if (!stdConfidenceInterval_.getDimension() || levelConfidenceInterval_ != level)
  {
    levelConfidenceInterval_ = level;
    stdConfidenceInterval_ = Interval(getResultSample().getDimension());

    // TODO : use Normal Distribution?
    int nbSimu = getResultSample().getSize();
    ChiSquare X(nbSimu-1);
    // low
    double f1 = X.computeQuantile((1-levelConfidenceInterval_)/2, true)[0];
    // up
    double f2 = X.computeQuantile((1-levelConfidenceInterval_)/2, false)[0];

    stdConfidenceInterval_ = Interval(getResultSample().getDimension());
    NumericalPoint lowerBounds(getResultSample().getDimension());
    NumericalPoint upperBounds(getResultSample().getDimension());

    for (int i=0; i<stdConfidenceInterval_.getDimension(); ++i)
    {
      //low
      lowerBounds[i] = sqrt((nbSimu - 1) * getVariance()[i] / f1);
      //up
      upperBounds[i] = sqrt((nbSimu - 1) * getVariance()[i] / f2);
    }
    stdConfidenceInterval_.setLowerBound(lowerBounds);
    stdConfidenceInterval_.setUpperBound(upperBounds);
  }
  return stdConfidenceInterval_;
}


MonteCarloResult::NumericalPointCollection MonteCarloResult::getOutliers()
{
  if (!outliers_.getSize())
  {
    outliers_.clear();
    for (int i=0; i<getResultSample().getDimension(); ++i)
    {
      NumericalPoint outliersOfIthOutput;
      double Q1 = getFirstQuartile()[i];
      double Q3 = getThirdQuartile()[i];
      double lowerBound = Q1 - 1.5 * (Q3 - Q1);
      double upperBound = Q3 + 1.5 * (Q3 - Q1);

      for (int j=0; j<getResultSample().getSize(); ++j)
        if (getResultSample()[j][i] < lowerBound || getResultSample()[j][i] > upperBound)
          outliersOfIthOutput.add(getResultSample()[j][i]);

      outliers_.add(outliersOfIthOutput);
    }
  }
  return outliers_;
}


MonteCarloResult::DistributionCollection MonteCarloResult::getFittedDistribution()
{
  KernelSmoothing gaussianKernel = KernelSmoothing();

  DistributionCollection distributions(getResultSample().getDimension());

  for (int i=0; i<getResultSample().getDimension(); ++i)
    distributions[i] = gaussianKernel.build(getResultSample().getMarginal(i), true);

  return distributions;
}

}