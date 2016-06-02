//                                               -*- C++ -*-
/**
 *  @brief Results of a Monte Carlo analysis 
 *
 *  Copyright 2015-2016 EDF-Phimeca
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
#include "otgui/MonteCarloResult.hxx"
#include "Normal.hxx"
#include "ChiSquare.hxx"
#include "KernelSmoothing.hxx"
#include "PersistentObjectFactory.hxx"

using namespace OT;

namespace OTGUI{

CLASSNAMEINIT(MonteCarloResult);

static Factory<MonteCarloResult> RegisteredFactory("MonteCarloResult");

/* Default constructor */
MonteCarloResult::MonteCarloResult()
  : SimulationAnalysisResult()
  , levelConfidenceInterval_(0.)
{
}


/* Constructor with parameters */
MonteCarloResult::MonteCarloResult(NumericalSample inputSample, NumericalSample outputSample)
  : SimulationAnalysisResult(inputSample, outputSample)
  , levelConfidenceInterval_(0.95)
{
  mean_ = getOutputSample().computeMean();
  median_ = getOutputSample().computeMedian();
  standardDeviation_ = getOutputSample().computeStandardDeviationPerComponent();
  variance_ = getOutputSample().computeVariance();
  skewness_ = NumericalPoint(getOutputSample().getDimension());
  if (!(getOutputSample().getSize() < 2))
    skewness_ = getOutputSample().computeSkewness();
  kurtosis_ = NumericalPoint(getOutputSample().getDimension());
  if (!(getOutputSample().getSize() < 3))
    kurtosis_ = getOutputSample().computeKurtosis();
  firstQuartile_ = getOutputSample().computeQuantilePerComponent(0.25);
  thirdQuartile_ = getOutputSample().computeQuantilePerComponent(0.75);
  computeMeanConfidenceInterval(levelConfidenceInterval_);
  computeStdConfidenceInterval(levelConfidenceInterval_);
  computeOutliers();
  computeFittedDistribution();
}
  

/* Virtual constructor */
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
    computeMeanConfidenceInterval(level);
  return meanConfidenceInterval_;
}


void MonteCarloResult::computeMeanConfidenceInterval(const double level)
{
  levelConfidenceInterval_ = level;
  meanConfidenceInterval_ = Interval(getOutputSample().getDimension());

  Normal X(0,1);
  double f = X.computeQuantile((1-levelConfidenceInterval_)/2, true)[0];
  NumericalPoint delta = f * getStandardDeviation() / sqrt(getOutputSample().getSize());

  NumericalPoint lowerBounds(getOutputSample().getDimension());
  NumericalPoint upperBounds(getOutputSample().getDimension());

  for (UnsignedInteger i=0; i<meanConfidenceInterval_.getDimension(); ++i)
  {
    // low
    lowerBounds[i] = getMean()[i] - delta[i];
    // up
    upperBounds[i] = getMean()[i] + delta[i];
  }
  meanConfidenceInterval_.setLowerBound(lowerBounds);
  meanConfidenceInterval_.setUpperBound(upperBounds);
}


Interval MonteCarloResult::getStdConfidenceInterval(const double level)
{
  if (!stdConfidenceInterval_.getDimension() || levelConfidenceInterval_ != level)
    computeStdConfidenceInterval(level);
  return stdConfidenceInterval_;
}


void MonteCarloResult::computeStdConfidenceInterval(const double level)
{
  levelConfidenceInterval_ = level;
  stdConfidenceInterval_ = Interval(getOutputSample().getDimension());

  // TODO : use Normal Distribution?
  UnsignedInteger nbSimu = getOutputSample().getSize();
  ChiSquare X(nbSimu-1);
  // low
  double f1 = X.computeQuantile((1-levelConfidenceInterval_)/2, true)[0];
  // up
  double f2 = X.computeQuantile((1-levelConfidenceInterval_)/2, false)[0];

  stdConfidenceInterval_ = Interval(getOutputSample().getDimension());
  NumericalPoint lowerBounds(getOutputSample().getDimension());
  NumericalPoint upperBounds(getOutputSample().getDimension());

  for (UnsignedInteger i=0; i<stdConfidenceInterval_.getDimension(); ++i)
  {
    //low
    lowerBounds[i] = sqrt((nbSimu - 1) * getVariance()[i] / f1);
    //up
    upperBounds[i] = sqrt((nbSimu - 1) * getVariance()[i] / f2);
  }
  stdConfidenceInterval_.setLowerBound(lowerBounds);
  stdConfidenceInterval_.setUpperBound(upperBounds);
}


MonteCarloResult::NumericalPointCollection MonteCarloResult::getOutliers()
{
  if (!outliers_.getSize())
    computeOutliers();
  return outliers_;
}


void MonteCarloResult::computeOutliers()
{
  outliers_.clear();
  for (UnsignedInteger i=0; i<getOutputSample().getDimension(); ++i)
  {
    NumericalPoint outliersOfIthOutput;
    double Q1 = getFirstQuartile()[i];
    double Q3 = getThirdQuartile()[i];
    double lowerBound = Q1 - 1.5 * (Q3 - Q1);
    double upperBound = Q3 + 1.5 * (Q3 - Q1);

    for (UnsignedInteger j=0; j<getOutputSample().getSize(); ++j)
      if (getOutputSample()[j][i] < lowerBound || getOutputSample()[j][i] > upperBound)
        outliersOfIthOutput.add(getOutputSample()[j][i]);

    outliers_.add(outliersOfIthOutput);
  }
}


MonteCarloResult::DistributionCollection MonteCarloResult::getFittedDistribution()
{
  if (!fittedDistribution_.getSize())
    computeFittedDistribution();
  return fittedDistribution_;
}


void MonteCarloResult::computeFittedDistribution()
{
  fittedDistribution_ = DistributionCollection();
  KernelSmoothing gaussianKernel;
    for (UnsignedInteger i=0; i<getOutputSample().getDimension(); ++i)
      fittedDistribution_.add(gaussianKernel.build(getOutputSample().getMarginal(i), true));
}


/* Method save() stores the object through the StorageManager */
void MonteCarloResult::save(Advocate & adv) const
{
  SimulationAnalysisResult::save(adv);
  adv.saveAttribute("levelConfidenceInterval_", levelConfidenceInterval_);
  adv.saveAttribute("mean_", mean_);
  adv.saveAttribute("median_", median_);
  adv.saveAttribute("standardDeviation_", standardDeviation_);
  adv.saveAttribute("variance_", variance_);
  adv.saveAttribute("skewness_", skewness_);
  adv.saveAttribute("kurtosis_", kurtosis_);
  adv.saveAttribute("firstQuartile_", firstQuartile_);
  adv.saveAttribute("thirdQuartile_", thirdQuartile_);
  adv.saveAttribute("meanConfidenceInterval_", meanConfidenceInterval_);
  adv.saveAttribute("stdConfidenceInterval_", stdConfidenceInterval_);
  adv.saveAttribute("fittedDistributions_", fittedDistribution_);
  adv.saveAttribute("outliers_", outliers_);
}


/* Method load() reloads the object from the StorageManager */
void MonteCarloResult::load(Advocate & adv)
{
  SimulationAnalysisResult::load(adv);
  adv.loadAttribute("levelConfidenceInterval_", levelConfidenceInterval_);
  adv.loadAttribute("mean_", mean_);
  adv.loadAttribute("median_", median_);
  adv.loadAttribute("standardDeviation_", standardDeviation_);
  adv.loadAttribute("variance_", variance_);
  adv.loadAttribute("skewness_", skewness_);
  adv.loadAttribute("kurtosis_", kurtosis_);
  adv.loadAttribute("firstQuartile_", firstQuartile_);
  adv.loadAttribute("thirdQuartile_", thirdQuartile_);
  adv.loadAttribute("meanConfidenceInterval_", meanConfidenceInterval_);
  adv.loadAttribute("stdConfidenceInterval_", stdConfidenceInterval_);
  adv.loadAttribute("fittedDistributions_", fittedDistribution_);
  adv.loadAttribute("outliers_", outliers_);
}
}