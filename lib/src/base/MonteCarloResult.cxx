// MonteCarloResult.cxx

#include "otgui/MonteCarloResult.hxx"
#include "Normal.hxx"
#include "ChiSquare.hxx"
#include "KernelSmoothing.hxx"

using namespace OT;

namespace OTGUI{

CLASSNAMEINIT(MonteCarloResult);

MonteCarloResult::MonteCarloResult()
  : SimulationAnalysisResult()
  , levelConfidenceInterval_(0.)
{
}

MonteCarloResult::MonteCarloResult(NumericalSample inputSample, NumericalSample outputSample)
  : SimulationAnalysisResult(inputSample, outputSample)
  , levelConfidenceInterval_(0.95)
{
}
  

MonteCarloResult* MonteCarloResult::clone() const
{
  return new MonteCarloResult(*this);
}


NumericalPoint MonteCarloResult::getMean()
{
  if (!mean_.getSize())
    mean_ = getOutputSample().computeMean();
  return mean_;
}


NumericalPoint MonteCarloResult::getMedian()
{
  if (!median_.getSize())
    median_ = getOutputSample().computeMedian();
  return median_;
}


NumericalPoint MonteCarloResult::getStandardDeviation()
{
  if (!standardDeviation_.getSize())
    standardDeviation_ = getOutputSample().computeStandardDeviationPerComponent();
  return standardDeviation_;
}


NumericalPoint MonteCarloResult::getVariance()
{
  if (!variance_.getSize())
    variance_ = getOutputSample().computeVariance();
  return variance_;
}


NumericalPoint MonteCarloResult::getSkewness()
{
  if (!skewness_.getSize())
  {
    skewness_ = NumericalPoint(getOutputSample().getDimension());
    if (!(getOutputSample().getSize() < 2))
      skewness_ = getOutputSample().computeSkewness();
  }
  return skewness_;
}


NumericalPoint MonteCarloResult::getKurtosis()
{
  if (!kurtosis_.getSize())
  {
    kurtosis_ = NumericalPoint(getOutputSample().getDimension());
    if (!(getOutputSample().getSize() < 3))
      kurtosis_ = getOutputSample().computeKurtosis();
  }
  return kurtosis_;
}


NumericalPoint MonteCarloResult::getFirstQuartile()
{
  if (!firstQuartile_.getSize())
    firstQuartile_ = getOutputSample().computeQuantilePerComponent(0.25);
  return firstQuartile_;
}


NumericalPoint MonteCarloResult::getThirdQuartile()
{
  if (!thirdQuartile_.getSize())
    thirdQuartile_ = getOutputSample().computeQuantilePerComponent(0.75);
  return thirdQuartile_;
}


Interval MonteCarloResult::getMeanConfidenceInterval(const double level)
{
  if (!meanConfidenceInterval_.getDimension() || levelConfidenceInterval_ != level)
  {
    levelConfidenceInterval_ = level;
    meanConfidenceInterval_ = Interval(getOutputSample().getDimension());

    Normal X(0,1);
    double f = X.computeQuantile((1-levelConfidenceInterval_)/2, true)[0];
    NumericalPoint delta = f * getStandardDeviation() / sqrt(getOutputSample().getSize());
    
    NumericalPoint lowerBounds(getOutputSample().getDimension());
    NumericalPoint upperBounds(getOutputSample().getDimension());

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
    stdConfidenceInterval_ = Interval(getOutputSample().getDimension());

    // TODO : use Normal Distribution?
    int nbSimu = getOutputSample().getSize();
    ChiSquare X(nbSimu-1);
    // low
    double f1 = X.computeQuantile((1-levelConfidenceInterval_)/2, true)[0];
    // up
    double f2 = X.computeQuantile((1-levelConfidenceInterval_)/2, false)[0];

    stdConfidenceInterval_ = Interval(getOutputSample().getDimension());
    NumericalPoint lowerBounds(getOutputSample().getDimension());
    NumericalPoint upperBounds(getOutputSample().getDimension());

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
    for (int i=0; i<getOutputSample().getDimension(); ++i)
    {
      NumericalPoint outliersOfIthOutput;
      double Q1 = getFirstQuartile()[i];
      double Q3 = getThirdQuartile()[i];
      double lowerBound = Q1 - 1.5 * (Q3 - Q1);
      double upperBound = Q3 + 1.5 * (Q3 - Q1);

      for (int j=0; j<getOutputSample().getSize(); ++j)
        if (getOutputSample()[j][i] < lowerBound || getOutputSample()[j][i] > upperBound)
          outliersOfIthOutput.add(getOutputSample()[j][i]);

      outliers_.add(outliersOfIthOutput);
    }
  }
  return outliers_;
}


MonteCarloResult::DistributionCollection MonteCarloResult::getFittedDistribution()
{
  KernelSmoothing gaussianKernel = KernelSmoothing();

  DistributionCollection distributions(getOutputSample().getDimension());

  for (int i=0; i<getOutputSample().getDimension(); ++i)
    distributions[i] = gaussianKernel.build(getOutputSample().getMarginal(i), true);

  return distributions;
}
}