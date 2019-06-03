//                                               -*- C++ -*-
/**
 *  @brief Data analysis
 *
 *  Copyright 2015-2019 EDF-Phimeca
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
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
#include "persalys/DataAnalysis.hxx"

#include <openturns/Normal.hxx>
#include <openturns/ChiSquare.hxx>
#include <openturns/KernelSmoothing.hxx>
#include <openturns/PersistentObjectFactory.hxx>
#include <openturns/SpecFunc.hxx>

using namespace OT;

namespace PERSALYS
{

CLASSNAMEINIT(DataAnalysis)

static Factory<DataAnalysis> Factory_DataAnalysis;

/* Default constructor */
DataAnalysis::DataAnalysis()
  : DesignOfExperimentAnalysis()
  , isConfidenceIntervalRequired_(true)
  , levelConfidenceInterval_(0.95)
  , result_()
{

}


DataAnalysis::DataAnalysis(const String & name, const DesignOfExperiment & designOfExperiment)
  : DesignOfExperimentAnalysis(name, designOfExperiment)
  , isConfidenceIntervalRequired_(true)
  , levelConfidenceInterval_(0.95)
  , result_()
{
  if (designOfExperiment_.getSample().getSize())
    setInterestVariables(designOfExperiment_.getSample().getDescription());
}


DataAnalysis* DataAnalysis::clone() const
{
  return new DataAnalysis(*this);
}


void DataAnalysis::initialize()
{
  AnalysisImplementation::initialize();
  result_ = DataAnalysisResult(designOfExperiment_);
}


void DataAnalysis::launch()
{
  // get sample to analyse
  const Sample sample(getDesignOfExperiment().getSample());

  if (!sample.getSize())
    throw InvalidDimensionException(HERE) << "The sample is empty";

  for (UnsignedInteger i = 0; i < sample.getDimension(); ++i)
  {
    if (stopRequested_)
      break;

    progressValue_ = (int) (i * 100 / sample.getSize());
    notify("progressValueChanged");

    // min/max
    result_.min_.add(sample.getMarginal(i).getMin());
    result_.max_.add(sample.getMarginal(i).getMax());

    // moments
    result_.mean_.add(sample.getMarginal(i).computeMean());
    result_.median_.add(sample.getMarginal(i).computeMedian());

    result_.standardDeviation_.add(Point());
    result_.coefficientOfVariation_.add(Point());
    result_.variance_.add(Point());
    result_.skewness_.add(Point());
    result_.kurtosis_.add(Point());
    try
    {
      result_.standardDeviation_[i] = sample.getMarginal(i).computeStandardDeviationPerComponent();
      if (std::abs(result_.mean_[i][0]) > SpecFunc::Precision)
        result_.coefficientOfVariation_[i] = result_.standardDeviation_[i] / std::abs(result_.mean_[i][0]);
      result_.variance_[i] = sample.getMarginal(i).computeVariance();
      result_.skewness_[i] = sample.getMarginal(i).computeSkewness();
      result_.kurtosis_[i] = sample.getMarginal(i).computeKurtosis();
    }
    catch (std::exception &)
    {
      // nothing
    }

    // quartiles
    result_.firstQuartile_.add(sample.getMarginal(i).computeQuantilePerComponent(0.25));
    result_.thirdQuartile_.add(sample.getMarginal(i).computeQuantilePerComponent(0.75));

    // Confidence Intervals
    if (isConfidenceIntervalRequired_)
    {
      // mean Confidence Interval
      Point meanLowerBounds(result_.meanConfidenceInterval_.getLowerBound());
      Point meanUpperBounds(result_.meanConfidenceInterval_.getUpperBound());
      meanLowerBounds.add(0.);
      meanUpperBounds.add(1.);

      Interval::BoolCollection meanFiniteLowerBounds(result_.meanConfidenceInterval_.getFiniteLowerBound());
      Interval::BoolCollection meanFiniteUpperBounds(result_.meanConfidenceInterval_.getFiniteUpperBound());
      meanFiniteLowerBounds.add(false);
      meanFiniteUpperBounds.add(false);

      result_.meanConfidenceInterval_ = Interval(result_.min_.getSize());

      if (result_.standardDeviation_[i].getDimension())
      {
        const Normal X(0, 1);
        const double f = X.computeQuantile((1 - levelConfidenceInterval_) / 2, true)[0];
        double delta(f * result_.standardDeviation_[i][0] / sqrt(sample.getSize()));

        meanLowerBounds[i] = result_.mean_[i][0] - delta;
        meanUpperBounds[i] = result_.mean_[i][0] + delta;
        result_.meanConfidenceInterval_.setLowerBound(meanLowerBounds);
        result_.meanConfidenceInterval_.setUpperBound(meanUpperBounds);

        meanFiniteLowerBounds[i] = true;
        meanFiniteUpperBounds[i] = true;
        result_.meanConfidenceInterval_.setFiniteLowerBound(meanFiniteLowerBounds);
        result_.meanConfidenceInterval_.setFiniteUpperBound(meanFiniteUpperBounds);
      }

      // std Confidence Interval
      Point stdLowerBounds(result_.stdConfidenceInterval_.getLowerBound());
      Point stdUpperBounds(result_.stdConfidenceInterval_.getUpperBound());
      stdLowerBounds.add(0.);
      stdUpperBounds.add(1.);

      Interval::BoolCollection stdFiniteLowerBounds(result_.stdConfidenceInterval_.getFiniteLowerBound());
      Interval::BoolCollection stdFiniteUpperBounds(result_.stdConfidenceInterval_.getFiniteUpperBound());
      stdFiniteLowerBounds.add(false);
      stdFiniteUpperBounds.add(false);

      result_.stdConfidenceInterval_ = Interval(result_.min_.getSize());

      if (result_.variance_[i].getDimension() && sample.getSize() > 1)
      {
        // TODO : use Normal Distribution?
        const UnsignedInteger nbSimu = sample.getSize();
        const ChiSquare X(nbSimu - 1);
        // low
        const double f1 = X.computeQuantile((1 - levelConfidenceInterval_) / 2, true)[0];
        // up
        const double f2 = X.computeQuantile((1 - levelConfidenceInterval_) / 2, false)[0];

        //low
        stdLowerBounds[i] = sqrt((nbSimu - 1) * result_.variance_[i][0] / f1);
        //up
        stdUpperBounds[i] = sqrt((nbSimu - 1) * result_.variance_[i][0] / f2);

        result_.stdConfidenceInterval_.setLowerBound(stdLowerBounds);
        result_.stdConfidenceInterval_.setUpperBound(stdUpperBounds);

        stdFiniteLowerBounds[i] = true;
        stdFiniteUpperBounds[i] = true;
        result_.stdConfidenceInterval_.setFiniteLowerBound(stdFiniteLowerBounds);
        result_.stdConfidenceInterval_.setFiniteUpperBound(stdFiniteUpperBounds);
      }
    }

    // outliers
    result_.outliers_.add(Point());
    const double lowerBound(result_.firstQuartile_[i][0] - 1.5 * (result_.thirdQuartile_[i][0] - result_.firstQuartile_[i][0]));
    const double upperBound(result_.thirdQuartile_[i][0] + 1.5 * (result_.thirdQuartile_[i][0] - result_.firstQuartile_[i][0]));
    for (UnsignedInteger j = 0; j < sample.getSize(); ++j)
      if (sample(j, i) < lowerBound || sample(j, i) > upperBound)
        result_.outliers_[i].add(sample(j, i));

    // pdf/cdf
    result_.pdf_.add(Sample());
    result_.cdf_.add(Sample());
    try
    {
      KernelSmoothing gaussianKernel;
      Distribution fittedDistribution(gaussianKernel.build(sample.getMarginal(i)));
      result_.pdf_[i] = fittedDistribution.drawPDF().getDrawable(0).getData();
      result_.cdf_[i] = fittedDistribution.drawCDF().getDrawable(0).getData();
    }
    catch (std::exception &)
    {
    }
  }

  // post processing
  const UnsignedInteger nbAnalysedVar = result_.min_.getSize();

  // initialisation C.I
  if (!isConfidenceIntervalRequired_)
  {
    result_.meanConfidenceInterval_ = Interval(nbAnalysedVar);
    result_.meanConfidenceInterval_.setFiniteLowerBound(Interval::BoolCollection(nbAnalysedVar, false));
    result_.meanConfidenceInterval_.setFiniteUpperBound(Interval::BoolCollection(nbAnalysedVar, false));
    result_.stdConfidenceInterval_ = Interval(nbAnalysedVar);
    result_.stdConfidenceInterval_.setFiniteLowerBound(Interval::BoolCollection(nbAnalysedVar, false));
    result_.stdConfidenceInterval_.setFiniteUpperBound(Interval::BoolCollection(nbAnalysedVar, false));
  }

  if (nbAnalysedVar == sample.getDimension())
  {
    result_.designOfExperiment_.setInputSample(designOfExperiment_.getInputSample());
    result_.designOfExperiment_.setOutputSample(designOfExperiment_.getOutputSample());
  }
  else // if the user stops the analysis before the end
  {
    // input sample
    if (designOfExperiment_.getInputSample().getSize())
    {
      if (nbAnalysedVar <= designOfExperiment_.getInputSample().getDimension())
      {
        Indices inputIndices(nbAnalysedVar);
        inputIndices.fill();
        result_.designOfExperiment_.setInputSample(designOfExperiment_.getInputSample().getMarginal(inputIndices));
      }
      else
        result_.designOfExperiment_.setInputSample(designOfExperiment_.getInputSample());
    }

    // output sample
    const int nbAnalysedOutputs = nbAnalysedVar - (designOfExperiment_.getInputSample().getSize() > 0 ? designOfExperiment_.getInputSample().getDimension() : 0);

    if (nbAnalysedOutputs > 0)
    {
      Indices outputIndices(nbAnalysedOutputs);
      outputIndices.fill();
      result_.designOfExperiment_.setOutputSample(designOfExperiment_.getOutputSample().getMarginal(outputIndices));
    }
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


String DataAnalysis::getPythonScript() const
{
  OSS oss;
  oss << getName() << " = persalys.DataAnalysis('" << getName() << "', " << getDesignOfExperiment().getName() << ")\n";

  return oss;
}


bool DataAnalysis::hasValidResult() const
{
  return getResult().getMean().getSize() != 0;
}


/* String converter */
String DataAnalysis::__repr__() const
{
  OSS oss;
  oss << "class=" << GetClassName()
      << " name=" << getName()
      << " designOfExperiment=" << getDesignOfExperiment().getName()
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
