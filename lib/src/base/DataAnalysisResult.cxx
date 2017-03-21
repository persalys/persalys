//                                               -*- C++ -*-
/**
 *  @brief Results of a data analysis
 *
 *  Copyright 2015-2017 EDF-Phimeca
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
#include "otgui/DataAnalysisResult.hxx"

#include "openturns/Normal.hxx"
#include "openturns/ChiSquare.hxx"
#include "openturns/KernelSmoothing.hxx"
#include "openturns/PersistentObjectFactory.hxx"

using namespace OT;

namespace OTGUI{

CLASSNAMEINIT(DataAnalysisResult);

static Factory<DataAnalysisResult> RegisteredFactory;

/* Default constructor */
DataAnalysisResult::DataAnalysisResult()
  : DataSample()
  , elapsedTime_(0.)
{

}


DataAnalysisResult::DataAnalysisResult(const NumericalSample& inputSample, const NumericalSample& outputSample)
  : DataSample(inputSample, outputSample)
  , min_(inputSample.getDimension()+(outputSample.getSize()?outputSample.getDimension():0))
  , max_(inputSample.getDimension()+(outputSample.getSize()?outputSample.getDimension():0))
  , mean_(inputSample.getDimension()+(outputSample.getSize()?outputSample.getDimension():0))
  , coefficientOfVariation_(inputSample.getDimension()+(outputSample.getSize()?outputSample.getDimension():0))
  , median_(inputSample.getDimension()+(outputSample.getSize()?outputSample.getDimension():0))
  , standardDeviation_(inputSample.getDimension()+(outputSample.getSize()?outputSample.getDimension():0))
  , variance_(inputSample.getDimension()+(outputSample.getSize()?outputSample.getDimension():0))
  , skewness_(inputSample.getDimension()+(outputSample.getSize()?outputSample.getDimension():0))
  , kurtosis_(inputSample.getDimension()+(outputSample.getSize()?outputSample.getDimension():0))
  , firstQuartile_(inputSample.getDimension()+(outputSample.getSize()?outputSample.getDimension():0))
  , thirdQuartile_(inputSample.getDimension()+(outputSample.getSize()?outputSample.getDimension():0))
  , meanConfidenceInterval_(inputSample.getDimension()+(outputSample.getSize()?outputSample.getDimension():0))
  , stdConfidenceInterval_(inputSample.getDimension()+(outputSample.getSize()?outputSample.getDimension():0))
  , outliers_(inputSample.getDimension()+(outputSample.getSize()?outputSample.getDimension():0))
  , pdf_(inputSample.getDimension()+(outputSample.getSize()?outputSample.getDimension():0))
  , cdf_(inputSample.getDimension()+(outputSample.getSize()?outputSample.getDimension():0))
  , elapsedTime_(0.)
{
  meanConfidenceInterval_.setFiniteLowerBound(Interval::BoolCollection(inputSample.getDimension()+(outputSample.getSize()?outputSample.getDimension():0), false));
  meanConfidenceInterval_.setFiniteUpperBound(Interval::BoolCollection(inputSample.getDimension()+(outputSample.getSize()?outputSample.getDimension():0), false));
  stdConfidenceInterval_.setFiniteLowerBound(Interval::BoolCollection(inputSample.getDimension()+(outputSample.getSize()?outputSample.getDimension():0), false));
  stdConfidenceInterval_.setFiniteUpperBound(Interval::BoolCollection(inputSample.getDimension()+(outputSample.getSize()?outputSample.getDimension():0), false));
}


DataAnalysisResult* DataAnalysisResult::clone() const
{
  return new DataAnalysisResult(*this);
}


DataAnalysisResult::NumericalPointCollection DataAnalysisResult::getMin() const
{
  return min_;
}


DataAnalysisResult::NumericalPointCollection DataAnalysisResult::getMax() const
{
  return max_;
}


DataAnalysisResult::NumericalPointCollection DataAnalysisResult::getMean() const
{
  return mean_;
}


DataAnalysisResult::NumericalPointCollection DataAnalysisResult::getCoefficientOfVariation() const
{
  return coefficientOfVariation_;
}


DataAnalysisResult::NumericalPointCollection DataAnalysisResult::getMedian() const
{
  return median_;
}


DataAnalysisResult::NumericalPointCollection DataAnalysisResult::getStandardDeviation() const
{
  return standardDeviation_;
}


DataAnalysisResult::NumericalPointCollection DataAnalysisResult::getVariance() const
{
  return variance_;
}


DataAnalysisResult::NumericalPointCollection DataAnalysisResult::getSkewness() const
{
  return skewness_;
}


DataAnalysisResult::NumericalPointCollection DataAnalysisResult::getKurtosis() const
{
  return kurtosis_;
}


DataAnalysisResult::NumericalPointCollection DataAnalysisResult::getFirstQuartile() const
{
  return firstQuartile_;
}


DataAnalysisResult::NumericalPointCollection DataAnalysisResult::getThirdQuartile() const
{
  return thirdQuartile_;
}


Interval DataAnalysisResult::getMeanConfidenceInterval() const
{
  return meanConfidenceInterval_;
}


Interval DataAnalysisResult::getStdConfidenceInterval() const
{
  return stdConfidenceInterval_;
}


DataAnalysisResult::NumericalPointCollection DataAnalysisResult::getOutliers() const
{
  return outliers_;
}


DataSample::NumericalSampleCollection DataAnalysisResult::getPDF() const
{
  return pdf_;
}


DataSample::NumericalSampleCollection DataAnalysisResult::getCDF() const
{
  return cdf_;
}


double DataAnalysisResult::getElapsedTime() const
{
  return elapsedTime_;
}


/* String converter */
String DataAnalysisResult::__repr__() const
{
  OSS oss;
  oss << "class=" << GetClassName()
      << " name=" << getName()
      << " min=" << getMin()
      << " max=" << getMax()
      << " mean=" << getMean()
      << " median=" << getMedian()
      << " standardDeviation=" << getStandardDeviation()
      << " coefficientOfVariation=" << getCoefficientOfVariation()
      << " variance=" << getVariance()
      << " skewness=" << getSkewness()
      << " kurtosis=" << getKurtosis()
      << " firstQuartile=" << getFirstQuartile()
      << " thirdQuartile=" << getThirdQuartile()
      << " meanConfidenceInterval=" << getMeanConfidenceInterval()
      << " stdConfidenceInterval=" << getStdConfidenceInterval();
  return oss;
}


/* Method save() stores the object through the StorageManager */
void DataAnalysisResult::save(Advocate & adv) const
{
  DataSample::save(adv);
  adv.saveAttribute("min_", min_);
  adv.saveAttribute("max_", max_);
  adv.saveAttribute("mean_", mean_);
  adv.saveAttribute("median_", median_);
  adv.saveAttribute("standardDeviation_", standardDeviation_);
  adv.saveAttribute("coefficientOfVariation_", coefficientOfVariation_);
  adv.saveAttribute("variance_", variance_);
  adv.saveAttribute("skewness_", skewness_);
  adv.saveAttribute("kurtosis_", kurtosis_);
  adv.saveAttribute("firstQuartile_", firstQuartile_);
  adv.saveAttribute("thirdQuartile_", thirdQuartile_);
  adv.saveAttribute("meanConfidenceInterval_", meanConfidenceInterval_);
  adv.saveAttribute("stdConfidenceInterval_", stdConfidenceInterval_);
  adv.saveAttribute("outliers_", outliers_);
  adv.saveAttribute("pdf_", pdf_);
  adv.saveAttribute("cdf_", cdf_);
  adv.saveAttribute("elapsedTime_", elapsedTime_);
}


/* Method load() reloads the object from the StorageManager */
void DataAnalysisResult::load(Advocate & adv)
{
  DataSample::load(adv);
  adv.loadAttribute("min_", min_);
  adv.loadAttribute("max_", max_);
  adv.loadAttribute("mean_", mean_);
  adv.loadAttribute("median_", median_);
  adv.loadAttribute("standardDeviation_", standardDeviation_);
  adv.loadAttribute("coefficientOfVariation_", coefficientOfVariation_);
  adv.loadAttribute("variance_", variance_);
  adv.loadAttribute("skewness_", skewness_);
  adv.loadAttribute("kurtosis_", kurtosis_);
  adv.loadAttribute("firstQuartile_", firstQuartile_);
  adv.loadAttribute("thirdQuartile_", thirdQuartile_);
  adv.loadAttribute("meanConfidenceInterval_", meanConfidenceInterval_);
  adv.loadAttribute("stdConfidenceInterval_", stdConfidenceInterval_);
  adv.loadAttribute("outliers_", outliers_);
  adv.loadAttribute("pdf_", pdf_);
  adv.loadAttribute("cdf_", cdf_);
  adv.loadAttribute("elapsedTime_", elapsedTime_);
}
}