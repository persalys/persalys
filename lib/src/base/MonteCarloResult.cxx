// MonteCarloResult.cxx

#include "MonteCarloResult.hxx"
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

MonteCarloResult::MonteCarloResult(NumericalSample outputSample, NumericalSample inputSample,
                                   double level)
 : ParametricCalculusResult(outputSample, inputSample)
 , level_(level)
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
    firstQuartile_ = getResultSample().computeQuantile(0.25);
  return firstQuartile_;
}


NumericalPoint MonteCarloResult::getThirdQuartile()
{
  if (!thirdQuartile_.getSize())
    thirdQuartile_ = getResultSample().computeQuantile(0.75);
  return thirdQuartile_;
}


Interval MonteCarloResult::getMeanConfidenceInterval()
{
  if (!meanConfidenceInterval_.getDimension())
  {
    meanConfidenceInterval_ = Interval(getResultSample().getDimension());

    Normal X(0,1);
    double f = X.computeQuantile(level_/2, true)[0];
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


Interval MonteCarloResult::getStdConfidenceInterval()
{
  if (!stdConfidenceInterval_.getDimension())
  {
    stdConfidenceInterval_ = Interval(getResultSample().getDimension());

    // TODO : use Normal Distribution?
    int nbSimu = getResultSample().getSize();
    ChiSquare X(nbSimu-1);
    // low
    double f1 = X.computeQuantile(level_/2, true)[0];
    // up
    double f2 = X.computeQuantile(level_/2, false)[0];

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


Distribution MonteCarloResult::getFittedDistribution()
{
  KernelSmoothing gaussianKernel = KernelSmoothing();
  return gaussianKernel.build(getResultSample(), true);
}

}