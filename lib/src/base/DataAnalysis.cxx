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
#include "openturns/SpecFunc.hxx"

using namespace OT;

namespace OTGUI{

CLASSNAMEINIT(DataAnalysis);

static Factory<DataAnalysis> RegisteredFactory;

/* Default constructor */
DataAnalysis::DataAnalysis()
  : DesignOfExperimentAnalysis()
  , isConfidenceIntervalRequired_(true)
  , levelConfidenceInterval_(0.95)
{

}


DataAnalysis::DataAnalysis(const String & name, const DesignOfExperiment & designOfExperiment)
  : DesignOfExperimentAnalysis(name, designOfExperiment)
  , isConfidenceIntervalRequired_(true)
  , levelConfidenceInterval_(0.95)
{
}


DataAnalysis* DataAnalysis::clone() const
{
  return new DataAnalysis(*this);
}


void DataAnalysis::run()
{
  const NumericalSample sample(getDesignOfExperiment().getSample());

  if (!sample.getSize())
    throw InvalidDimensionException(HERE) << "The sample is empty";

  result_ = DataAnalysisResult(getDesignOfExperiment().getInputSample(), getDesignOfExperiment().getOutputSample());

  for (UnsignedInteger i=0; i<sample.getDimension(); ++i)
  {
    // min/max
    result_.min_[i] = sample.getMarginal(i).getMin();
    result_.max_[i] = sample.getMarginal(i).getMax();

    // moments
    result_.mean_[i] = sample.getMarginal(i).computeMean();
    result_.median_[i] = sample.getMarginal(i).computeMedian();

    try
    {
      result_.standardDeviation_[i] = sample.getMarginal(i).computeStandardDeviationPerComponent();
      if (std::abs(result_.mean_[i][0]) > SpecFunc::Precision)
        result_.coefficientOfVariation_[i] = result_.standardDeviation_[i] / sqrt(sample.getSize()) / std::abs(result_.mean_[i][0]);
      result_.variance_[i] = sample.getMarginal(i).computeVariance();
      result_.skewness_[i] = sample.getMarginal(i).computeSkewness();
      result_.kurtosis_[i] = sample.getMarginal(i).computeKurtosis();
    }
    catch (std::exception)
    {
      // nothing
    }

    // quartiles
    result_.firstQuartile_[i] = sample.getMarginal(i).computeQuantilePerComponent(0.25);
    result_.thirdQuartile_[i] = sample.getMarginal(i).computeQuantilePerComponent(0.75);

    // mean Confidence Interval
    if (isConfidenceIntervalRequired_ && result_.standardDeviation_[i].getDimension())
    {
      const Normal X(0,1);
      const double f = X.computeQuantile((1-levelConfidenceInterval_)/2, true)[0];
      double delta(f * result_.standardDeviation_[i][0] / sqrt(sample.getSize()));

      NumericalPoint lowerBounds(result_.meanConfidenceInterval_.getLowerBound());
      NumericalPoint upperBounds(result_.meanConfidenceInterval_.getUpperBound());
      lowerBounds[i] = result_.mean_[i][0] - delta;
      upperBounds[i] = result_.mean_[i][0] + delta;
      result_.meanConfidenceInterval_.setLowerBound(lowerBounds);
      result_.meanConfidenceInterval_.setUpperBound(upperBounds);

      Interval::BoolCollection finiteLowerBounds(result_.meanConfidenceInterval_.getFiniteLowerBound());
      Interval::BoolCollection finiteUpperBounds(result_.meanConfidenceInterval_.getFiniteUpperBound());
      finiteLowerBounds[i] = true;
      finiteUpperBounds[i] = true;
      result_.meanConfidenceInterval_.setFiniteLowerBound(finiteLowerBounds);
      result_.meanConfidenceInterval_.setFiniteUpperBound(finiteUpperBounds);
    }
    // std Confidence Interval
    if (isConfidenceIntervalRequired_ && result_.variance_[i].getDimension() && sample.getSize() > 1)
    {
      // TODO : use Normal Distribution?
      const UnsignedInteger nbSimu = sample.getSize();
      const ChiSquare X(nbSimu-1);
      // low
      const double f1 = X.computeQuantile((1-levelConfidenceInterval_)/2, true)[0];
      // up
      const double f2 = X.computeQuantile((1-levelConfidenceInterval_)/2, false)[0];

      NumericalPoint lowerBounds(result_.stdConfidenceInterval_.getLowerBound());
      NumericalPoint upperBounds(result_.stdConfidenceInterval_.getUpperBound());

      //low
      lowerBounds[i] = sqrt((nbSimu - 1) * result_.variance_[i][0] / f1);
      //up
      upperBounds[i] = sqrt((nbSimu - 1) * result_.variance_[i][0] / f2);

      result_.stdConfidenceInterval_.setLowerBound(lowerBounds);
      result_.stdConfidenceInterval_.setUpperBound(upperBounds);

      Interval::BoolCollection finiteLowerBounds(result_.stdConfidenceInterval_.getFiniteLowerBound());
      Interval::BoolCollection finiteUpperBounds(result_.stdConfidenceInterval_.getFiniteUpperBound());
      finiteLowerBounds[i] = true;
      finiteUpperBounds[i] = true;
      result_.stdConfidenceInterval_.setFiniteLowerBound(finiteLowerBounds);
      result_.stdConfidenceInterval_.setFiniteUpperBound(finiteUpperBounds);
    }

    // outliers
    const double lowerBound(result_.firstQuartile_[i][0] - 1.5 * (result_.thirdQuartile_[i][0]-result_.firstQuartile_[i][0]));
    const double upperBound(result_.thirdQuartile_[i][0] + 1.5 * (result_.thirdQuartile_[i][0]-result_.firstQuartile_[i][0]));
    for (UnsignedInteger j=0; j<sample.getSize(); ++j)
      if (sample[j][i] < lowerBound || sample[j][i] > upperBound)
        result_.outliers_[i].add(sample[j][i]);

    // pdf/cdf
    try
    {
      KernelSmoothing gaussianKernel;
      Distribution fittedDistribution(gaussianKernel.build(sample.getMarginal(i)));
      result_.pdf_[i] = fittedDistribution.drawPDF().getDrawable(0).getData();
      result_.cdf_[i] = fittedDistribution.drawCDF().getDrawable(0).getData();
    }
    catch (std::exception)
    {
    }
  }

  notify("analysisFinished");
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