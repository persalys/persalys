//                                               -*- C++ -*-
/**
 *  @brief Computes Monte Carlo analysis
 *
 *  Copyright 2015-2025 EDF-Phimeca
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
#include "persalys/MonteCarloAnalysis.hxx"
#include "persalys/DataAnalysis.hxx"
#include "persalys/DataModel.hxx"

#include <openturns/RandomGenerator.hxx>
#include <openturns/PersistentObjectFactory.hxx>
#include <openturns/SpecFunc.hxx>
#include <openturns/DistFunc.hxx>

#include <limits>

using namespace OT;

namespace PERSALYS
{

CLASSNAMEINIT(MonteCarloAnalysis)

static Factory<MonteCarloAnalysis> Factory_MonteCarloAnalysis;

/* Default constructor */
MonteCarloAnalysis::MonteCarloAnalysis()
  : SimulationAnalysis()
  , WithStopCriteriaAnalysis()
  , result_()
{
  isDeterministicAnalysis_ = false;
}


/* Constructor with parameters */
MonteCarloAnalysis::MonteCarloAnalysis(const String& name, const PhysicalModel& physicalModel)
  : SimulationAnalysis(name, physicalModel)
  , WithStopCriteriaAnalysis()
  , result_()
{
  isDeterministicAnalysis_ = false;
}


/* Virtual constructor */
MonteCarloAnalysis* MonteCarloAnalysis::clone() const
{
  return new MonteCarloAnalysis(*this);
}


Scalar MonteCarloAnalysis::getMaximumConfidenceIntervalLength() const
{
  return maximumConfidenceIntervalLength_;
}


void MonteCarloAnalysis::setMaximumConfidenceIntervalLength(const Scalar length)
{
  maximumConfidenceIntervalLength_ = length;
}


bool MonteCarloAnalysis::isConfidenceIntervalRequired() const
{
  return isConfidenceIntervalRequired_;
}


void MonteCarloAnalysis::setIsConfidenceIntervalRequired(const bool isConfidenceIntervalRequired)
{
  isConfidenceIntervalRequired_ = isConfidenceIntervalRequired;
}


double MonteCarloAnalysis::getLevelConfidenceInterval() const
{
  return levelConfidenceInterval_;
}


void MonteCarloAnalysis::setLevelConfidenceInterval(const double levelConfidenceInterval)
{
  if (levelConfidenceInterval < 0. || levelConfidenceInterval >= 1.)
    throw InvalidArgumentException(HERE) << "MonteCarloAnalysis::setLevelConfidenceInterval : the level of the confidence interval must be greater or equal to 0. and less than 1.";
  levelConfidenceInterval_ = levelConfidenceInterval;
}


void MonteCarloAnalysis::initialize()
{
  SimulationAnalysis::initialize();
  result_ = DataAnalysisResult();
}


void MonteCarloAnalysis::launch()
{
  // check
  if (!getPhysicalModel().getRestrictedFunction(getInterestVariables()).getOutputDescription().getSize())
    throw InvalidDimensionException(HERE) << "The outputs of interest "
                                          << getInterestVariables() << " are not outputs of the model "
                                          << getPhysicalModel().getOutputNames();

  // initialization
  RandomGenerator::SetSeed(getSeed());

  Sample effectiveInputSample(0, getPhysicalModel().getStochasticInputNames().getSize());
  effectiveInputSample.setDescription(getPhysicalModel().getStochasticInputNames());
  Sample outputSample(0, getInterestVariables().getSize());
  outputSample.setDescription(getInterestVariables());
  Function function(getPhysicalModel().getRestrictedFunction(getInterestVariables()));

  const bool maximumOuterSamplingSpecified = getMaximumCalls() < (UnsignedInteger)std::numeric_limits<int>::max();
  const UnsignedInteger maximumOuterSampling = maximumOuterSamplingSpecified ? static_cast<UnsignedInteger>(ceil(1.0 * getMaximumCalls() / getBlockSize())) : (UnsignedInteger)std::numeric_limits<int>::max();
  const UnsignedInteger modulo = maximumOuterSamplingSpecified ? getMaximumCalls() % getBlockSize() : 0;
  const UnsignedInteger lastBlockSize = modulo == 0 ? getBlockSize() : modulo;

  Scalar coefficientOfVariation = -1.0;
  Scalar confidenceInterval = -1.0;
  UnsignedInteger outerSampling = 0;
  TimeCriteria timeCriteria;
  const Scalar maxTime = getMaximumElapsedTime() > 0 ? getMaximumElapsedTime() : std::numeric_limits<double>::max();

  // We loop if there remains some outer sampling and the coefficient of variation is greater than the limit or has not been computed yet.
  while (!stopRequested_
         && (outerSampling < maximumOuterSampling)
         && (coefficientOfVariation == -1.0 || coefficientOfVariation > getMaximumCoefficientOfVariation())
         && (confidenceInterval == -1.0 || confidenceInterval > getMaximumConfidenceIntervalLength())
         && (timeCriteria.getElapsedTime() < maxTime))
  {
    // progress
    if (getMaximumCalls() < (UnsignedInteger)std::numeric_limits<int>::max())
    {
      progressValue_ = (int) (outerSampling * 100 / maximumOuterSampling);
      notify("progressValueChanged");
    }
    // information message
    OSS oss;
    oss << "Number of iterations = " << outputSample.getSize() << "\n";
    oss << "Coefficient of variation = " << coefficientOfVariation << "\n";
    oss << "Confidence interval length = " << confidenceInterval << "\n";
    oss << "Elapsed time = " << timeCriteria.getElapsedTime() << " s\n";
    informationMessage_ = oss;
    notify("informationMessageUpdated");

    // the last block can be smaller
    const UnsignedInteger effectiveBlockSize = outerSampling < (maximumOuterSampling - 1) ? getBlockSize() : lastBlockSize;

    // get block input sample
    const Sample blockInputSample(generateInputSample(effectiveBlockSize));

    // Perform a block of simulations
    Sample blockOutputSample;
    try
    {
      blockOutputSample = function(blockInputSample);
    }
    catch (std::exception & ex)
    {
      failedInputSample_ = blockInputSample;
      warningMessage_ = ex.what();
    }

    if (!failedInputSample_.getSize())
    {
      // if succeed fill samples
      outputSample.add(blockOutputSample);
      effectiveInputSample.add(blockInputSample);
    }
    else
    {
      // exit the while section. Stop the analysis
      break;
    }

    // stop criteria
    if (getBlockSize() != 1 || (getBlockSize() == 1 && outerSampling))
    {
      const Point empiricalMean(outputSample.computeMean());
      const Point empiricalStd(outputSample.computeStandardDeviation());

      Scalar coefOfVar(0.);
      Scalar CI(0.);
      for (UnsignedInteger i = 0; i < outputSample.getDimension(); ++i)
      {
        if (std::abs(empiricalMean[i]) > SpecFunc::Precision)
        {
          const Scalar sigma_i = empiricalStd[i] / sqrt(outputSample.getSize());
          coefOfVar = std::max(sigma_i / std::abs(empiricalMean[i]), coefOfVar);
          CI = 2.0 * sigma_i * DistFunc::qNormal(0.5 * (1 + levelConfidenceInterval_));
        }
        else
        {
          coefOfVar = -1;
          CI = -1;
        }
      }
      coefficientOfVariation = coefOfVar;
      confidenceInterval = CI;
    }
    timeCriteria.incrementElapsedTime();
    ++outerSampling;
  }

  // check
  if (!outputSample.getSize())
    throw InvalidValueException(HERE) << "Monte Carlo Analysis failed. The output sample is empty. " << warningMessage_;

  // set design of experiments
  result_.designOfExperiment_.setInputSample(effectiveInputSample);
  result_.designOfExperiment_.setOutputSample(outputSample);

  // compute data analysis
  DataAnalysis dataAnalysis("", result_.designOfExperiment_);
  dataAnalysis.setIsConfidenceIntervalRequired(isConfidenceIntervalRequired());
  dataAnalysis.setLevelConfidenceInterval(levelConfidenceInterval_);
  dataAnalysis.run();

  // set result
  result_ = dataAnalysis.getResult();
  timeCriteria.incrementElapsedTime();
  result_.elapsedTime_ = timeCriteria.getElapsedTime();
}


DataAnalysisResult MonteCarloAnalysis::getResult() const
{
  return result_;
}


Parameters MonteCarloAnalysis::getParameters() const
{
  Parameters param;

  param.add("Algorithm", "Monte Carlo");
  param.add("Outputs of interest", getInterestVariables().__str__());
  if (isConfidenceIntervalRequired())
    param.add("Confidence level", (OSS() << getLevelConfidenceInterval() * 100).str() + "%");
  param.add(WithStopCriteriaAnalysis::getParameters());
  param.add("Maximum confidence interval length", getMaximumConfidenceIntervalLength());
  param.add(SimulationAnalysis::getParameters());

  return param;
}


String MonteCarloAnalysis::getPythonScript() const
{
  OSS oss;
  oss.setPrecision(12);
  oss << getName() << " = persalys.MonteCarloAnalysis('" << getName() << "', " << getPhysicalModel().getName() << ")\n";
  if (getInterestVariables().getSize() < getPhysicalModel().getSelectedOutputsNames().getSize())
  {
    oss << "interestVariables = " << Parameters::GetOTDescriptionStr(getInterestVariables()) << "\n";
    oss << getName() << ".setInterestVariables(interestVariables)\n";
  }
  if (getMaximumCalls() < (UnsignedInteger)std::numeric_limits<int>::max())
    oss << getName() << ".setMaximumCalls(" << getMaximumCalls() << ")\n";
  oss << getName() << ".setMaximumCoefficientOfVariation(" << getMaximumCoefficientOfVariation() << ")\n";
  oss << getName() << ".setMaximumElapsedTime(" << getMaximumElapsedTime() << ")\n";
  oss << getName() << ".setBlockSize(" << getBlockSize() << ")\n";
  oss << getName() << ".setSeed(" << getSeed() << ")\n";

  return oss;
}


bool MonteCarloAnalysis::hasValidResult() const
{
  return result_.getDesignOfExperiment().getSample().getSize() != 0;
}


/* String converter */
String MonteCarloAnalysis::__repr__() const
{
  OSS oss;
  oss << PhysicalModelAnalysis::__repr__()
      << " isConfidenceIntervalRequired=" << isConfidenceIntervalRequired()
      << " levelConfidenceInterval=" << getLevelConfidenceInterval()
      << WithStopCriteriaAnalysis::__repr__()
      << " seed=" << getSeed()
      << " maximumConfidenceIntervalLength=" << getMaximumConfidenceIntervalLength()
      << " blockSize=" << getBlockSize();
  return oss;
}


/* Method save() stores the object through the StorageManager */
void MonteCarloAnalysis::save(Advocate & adv) const
{
  SimulationAnalysis::save(adv);
  WithStopCriteriaAnalysis::save(adv);
  adv.saveAttribute("maximumConfidenceIntervalLength_", maximumConfidenceIntervalLength_);
  adv.saveAttribute("isConfidenceIntervalRequired_", isConfidenceIntervalRequired_);
  adv.saveAttribute("levelConfidenceInterval_", levelConfidenceInterval_);
  adv.saveAttribute("result_", result_);
}


/* Method load() reloads the object from the StorageManager */
void MonteCarloAnalysis::load(Advocate & adv)
{
  SimulationAnalysis::load(adv);
  WithStopCriteriaAnalysis::load(adv);
  adv.loadAttribute("isConfidenceIntervalRequired_", isConfidenceIntervalRequired_);
  adv.loadAttribute("levelConfidenceInterval_", levelConfidenceInterval_);
  adv.loadAttribute("result_", result_);
  // can open older xml files
  if (!result_.getDesignOfExperiment().getSample().getSize())
  {
    DesignOfExperiment experiment;
    adv.loadAttribute("designOfExperiment_", experiment);
    result_.designOfExperiment_ = experiment;
  }
  if (adv.hasAttribute("maximumConfidenceIntervalLength_"))
    adv.loadAttribute("maximumConfidenceIntervalLength_", maximumConfidenceIntervalLength_);
}
}
