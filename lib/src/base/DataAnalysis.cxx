//                                               -*- C++ -*-
/**
 *  @brief Data analysis
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
#include "otgui/DataAnalysis.hxx"

#include "openturns/Normal.hxx"
#include "openturns/ChiSquare.hxx"
#include "openturns/KernelSmoothing.hxx"
#include "openturns/PersistentObjectFactory.hxx"

using namespace OT;

namespace OTGUI{

CLASSNAMEINIT(DataAnalysis);

static Factory<DataAnalysis> RegisteredFactory;

/* Default constructor */
DataAnalysis::DataAnalysis()
  : DesignOfExperimentAnalysis()
  , isConfidenceIntervalRequired_(false)
  , levelConfidenceInterval_(0.)
{

}


DataAnalysis::DataAnalysis(const OT::String & name, const DesignOfExperiment & designOfExperiment)
  : DesignOfExperimentAnalysis(name, designOfExperiment)
  , isConfidenceIntervalRequired_(false)
  , levelConfidenceInterval_(0.)
{
  
}


DataAnalysis* DataAnalysis::clone() const
{
  return new DataAnalysis(*this);
}


void DataAnalysis::run()
{
  const NumericalSample sample(getDesignOfExperiment().getSample());

  if (sample.getSize())
  {
    result_.setInputSample(getDesignOfExperiment().getInputSample());
    result_.setOutputSample(getDesignOfExperiment().getOutputSample());
    result_.min_ = sample.getMin();
    result_.max_ = sample.getMax();
    result_.mean_ = sample.computeMean();
    result_.median_ = sample.computeMedian();
    result_.standardDeviation_ = sample.computeStandardDeviationPerComponent();
    result_.variance_ = sample.computeVariance();
    result_.skewness_ = NumericalPoint(sample.getDimension(), std::numeric_limits<double>::max());
    try
    {
      if (!(sample.getSize() < 2))
        result_.skewness_ = sample.computeSkewness();
    }
    catch (std::exception)
    {
      // nothing
    }
    result_.kurtosis_ = NumericalPoint(sample.getDimension(), std::numeric_limits<double>::max());
    try
    {
      if (!(sample.getSize() < 3))
        result_.kurtosis_ = sample.computeKurtosis();
    }
    catch (std::exception)
    {
      // nothing
    }
    result_.firstQuartile_ = sample.computeQuantilePerComponent(0.25);
    result_.thirdQuartile_ = sample.computeQuantilePerComponent(0.75);
    result_.coefficientOfVariation_ = ComputeCoefficientOfVariation(sample, result_.mean_, result_.standardDeviation_);
    if (isConfidenceIntervalRequired_)
    {
      result_.meanConfidenceInterval_ = ComputeMeanConfidenceInterval(sample, result_.mean_, result_.standardDeviation_, levelConfidenceInterval_);
      result_.stdConfidenceInterval_ = ComputeStdConfidenceInterval(sample, result_.variance_, levelConfidenceInterval_);
    }
    result_.outliers_ = ComputeOutliers(sample, result_.firstQuartile_, result_.thirdQuartile_);

    DataSample::NumericalSampleCollection pdf;
    DataSample::NumericalSampleCollection cdf;
    ComputeFittedDistributionPDF_CDF(sample, pdf, cdf);
    result_.pdf_ = pdf;
    result_.cdf_ = cdf;

    notify("analysisFinished");
  }
  else
  {
    throw InvalidDimensionException(HERE) << "Impossible to analyse an empty sample";
  }
}


bool DataAnalysis::isConfidenceIntervalRequired() const
{
  return isConfidenceIntervalRequired_;
}


void DataAnalysis::setIsConfidenceIntervalRequired(const bool isRequired)
{
  isConfidenceIntervalRequired_ = isRequired;
}


double DataAnalysis::getLevelConfidenceInterval() const
{
  return levelConfidenceInterval_;
}


void DataAnalysis::setLevelConfidenceInterval(const double level)
{
  levelConfidenceInterval_ = level;
}


DataAnalysisResult DataAnalysis::getResult() const
{
  return result_;
}


NumericalPoint DataAnalysis::ComputeCoefficientOfVariation(const OT::NumericalSample & sample,
                                                           const NumericalPoint & empiricalMean,
                                                           const NumericalPoint & empiricalStd)
{
  if (empiricalMean.getSize() != empiricalStd.getSize())
    throw InvalidDimensionException(HERE) << "In computeCoefficientOfVariation: The mean and the standard deviation must have the same size";

  if (empiricalMean.getSize() != sample.getDimension())
    throw InvalidDimensionException(HERE) << "In computeCoefficientOfVariation: The mean must have the same dimension as the sample";

  NumericalPoint coefficientOfVariation(sample.getDimension());

  for (UnsignedInteger i=0; i<sample.getDimension(); ++i)
    coefficientOfVariation[i] = empiricalStd[i] / sqrt(sample.getSize()) / std::abs(empiricalMean[i]);

  return coefficientOfVariation;
}


Interval DataAnalysis::ComputeMeanConfidenceInterval(const OT::NumericalSample & sample,
                                                     const OT::NumericalPoint & empiricalMean,
                                                     const NumericalPoint & empiricalStd,
                                                     const double level)
{
  if (empiricalMean.getSize() != empiricalStd.getSize())
    throw InvalidDimensionException(HERE) << "In computeCoefficientOfVariation: The mean and the standard deviation must have the same size";

  if (empiricalMean.getSize() != sample.getDimension())
    throw InvalidDimensionException(HERE) << "In computeMeanConfidenceInterval: The mean must have the same dimension as the sample";

  Interval meanConfidenceInterval(sample.getDimension());

  const Normal X(0,1);
  const double f = X.computeQuantile((1-level)/2, true)[0];
  NumericalPoint delta(f * empiricalStd / sqrt(sample.getSize()));

  meanConfidenceInterval.setLowerBound(empiricalMean - delta);
  meanConfidenceInterval.setUpperBound(empiricalMean + delta);

  return meanConfidenceInterval;
}


Interval DataAnalysis::ComputeStdConfidenceInterval(const OT::NumericalSample & sample,
                                                    const OT::NumericalPoint & variance,
                                                    const double level)
{
  if (variance.getSize() != sample.getDimension())
    throw InvalidDimensionException(HERE) << "In computeStdConfidenceInterval: The variance must have the same dimension as the sample";

  Interval stdConfidenceInterval(sample.getDimension());

  // TODO : use Normal Distribution?
  const UnsignedInteger nbSimu = sample.getSize();
  const ChiSquare X(nbSimu-1);
  // low
  const double f1 = X.computeQuantile((1-level)/2, true)[0];
  // up
  const double f2 = X.computeQuantile((1-level)/2, false)[0];

  NumericalPoint lowerBounds(sample.getDimension());
  NumericalPoint upperBounds(sample.getDimension());

  for (UnsignedInteger i=0; i<sample.getDimension(); ++i)
  {
    //low
    lowerBounds[i] = sqrt((nbSimu - 1) * variance[i] / f1);
    //up
    upperBounds[i] = sqrt((nbSimu - 1) * variance[i] / f2);
  }
  stdConfidenceInterval.setLowerBound(lowerBounds);
  stdConfidenceInterval.setUpperBound(upperBounds);

  return stdConfidenceInterval;
}




DataAnalysisResult::NumericalPointCollection DataAnalysis::ComputeOutliers(const OT::NumericalSample & sample,
                                                                           const OT::NumericalPoint & firstQuartile,
                                                                           const OT::NumericalPoint & thirdQuartile)
{
  if (firstQuartile.getSize() != thirdQuartile.getSize())
    throw InvalidDimensionException(HERE) << "In computeOutliers: The first quartile must have the same dimension as the third quartile";

  if (firstQuartile.getSize() != sample.getDimension())
    throw InvalidDimensionException(HERE) << "In computeOutliers: The third quartile must have the same dimension as the sample";

  DataAnalysisResult::NumericalPointCollection outliers(sample.getDimension());

  const NumericalPoint lowerBound(firstQuartile - 1.5 * (thirdQuartile-firstQuartile));
  const NumericalPoint upperBound(thirdQuartile + 1.5 * (thirdQuartile-firstQuartile));

  for (UnsignedInteger i=0; i<sample.getSize(); ++i)
    for (UnsignedInteger j=0; j<sample.getDimension(); ++j)
      if (sample[i][j] < lowerBound[j] || sample[i][j] > upperBound[j])
        outliers[j].add(sample[i][j]);

  return outliers;
}


void DataAnalysis::ComputeFittedDistributionPDF_CDF(const OT::NumericalSample & sample,
                                                    DataSample::NumericalSampleCollection & pdf,
                                                    DataSample::NumericalSampleCollection & cdf)
{
  KernelSmoothing gaussianKernel;
  for (UnsignedInteger i=0; i<sample.getDimension(); ++i)
  {
    try
    {
      Distribution fittedDistribution(gaussianKernel.build(sample.getMarginal(i)));
      pdf.add(fittedDistribution.drawPDF().getDrawable(0).getData());
      cdf.add(fittedDistribution.drawCDF().getDrawable(0).getData());
    }
    catch (std::exception)
    {
      pdf.add(NumericalSample());
      cdf.add(NumericalSample());
    }
  }
}


String DataAnalysis::getPythonScript() const
{
  OSS oss;
  oss << getName() << " = otguibase.DataAnalysis('" << getName() << "', " << getDesignOfExperiment().getName() << ")\n";

  return oss;
}


bool DataAnalysis::analysisLaunched() const
{
  return getResult().getMean().getSize() != 0;
}


/* String converter */
String DataAnalysis::__repr__() const
{
  OSS oss;
  oss << "class=" << GetClassName()
      << " name=" << getName()
      << " designOfExperiment=class=" << getDesignOfExperiment().GetClassName() << " name=" << getDesignOfExperiment().getName()
      << " isConfidenceIntervalRequired=" << isConfidenceIntervalRequired()
      << " levelConfidenceInterval=" << getLevelConfidenceInterval();
  return oss;
}


/* Method save() stores the object through the StorageManager */
void DataAnalysis::save(Advocate & adv) const
{
  DesignOfExperimentAnalysis::save(adv);
  adv.saveAttribute("isConfidenceIntervalRequired_", isConfidenceIntervalRequired_);
  adv.saveAttribute("levelConfidenceInterval_", levelConfidenceInterval_);
  adv.saveAttribute("result_", result_);
}


/* Method load() reloads the object from the StorageManager */
void DataAnalysis::load(Advocate & adv)
{
  DesignOfExperimentAnalysis::load(adv);
  adv.loadAttribute("isConfidenceIntervalRequired_", isConfidenceIntervalRequired_);
  adv.loadAttribute("levelConfidenceInterval_", levelConfidenceInterval_);
  adv.loadAttribute("result_", result_);
}
}