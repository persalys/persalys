//                                               -*- C++ -*-
/**
 *  @brief Computes Monte Carlo analysis
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
#include "otgui/MonteCarloAnalysis.hxx"

#include "otgui/DataAnalysis.hxx"
#include "otgui/DataModel.hxx"

#include <openturns/RandomGenerator.hxx>
#include <openturns/PersistentObjectFactory.hxx>
#include <openturns/SpecFunc.hxx>

using namespace OT;

namespace OTGUI
{

CLASSNAMEINIT(MonteCarloAnalysis)

static Factory<MonteCarloAnalysis> Factory_MonteCarloAnalysis;

/* Default constructor */
MonteCarloAnalysis::MonteCarloAnalysis()
  : SimulationAnalysis()
  , WithStopCriteriaAnalysis()
  , isConfidenceIntervalRequired_(true)
  , levelConfidenceInterval_(0.95)
  , result_()
{
}


/* Constructor with parameters */
MonteCarloAnalysis::MonteCarloAnalysis(const String& name, const PhysicalModel& physicalModel)
  : SimulationAnalysis(name, physicalModel)
  , WithStopCriteriaAnalysis()
  , isConfidenceIntervalRequired_(true)
  , levelConfidenceInterval_(0.95)
  , result_()
{
}


/* Virtual constructor */
MonteCarloAnalysis* MonteCarloAnalysis::clone() const
{
  MonteCarloAnalysis * newAnalysis = new MonteCarloAnalysis(*this);
  newAnalysis->designOfExperiment_ = designOfExperiment_.getImplementation()->clone();
  return newAnalysis;
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
    throw InvalidArgumentException(HERE) << "MonteCarloAnalysis::setLevelConfidenceInterval : the level of the confidence interval must be superior or equal to 0. and inferior to 1.";
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
  if (getMaximumCalls() < getBlockSize())
    throw InvalidValueException(HERE) << "The maximum calls number (" << getMaximumCalls()
                                      << ") can not be inferior to the block size (" << getBlockSize() << ")";
  if (!getPhysicalModel().getRestrictedFunction(getInterestVariables()).getOutputDescription().getSize())
    throw InvalidDimensionException(HERE) << "The outputs to be analysed "
                                          << getInterestVariables() << " are not outputs of the model "
                                          << getPhysicalModel().getOutputNames();

  // initialization
  RandomGenerator::SetSeed(getSeed());

  Sample effectiveInputSample(0, getPhysicalModel().getStochasticInputNames().getSize());
  effectiveInputSample.setDescription(getPhysicalModel().getStochasticInputNames());
  Sample outputSample(0, getInterestVariables().getSize());
  outputSample.setDescription(getInterestVariables());

  const bool maximumOuterSamplingSpecified = getMaximumCalls() < (UnsignedInteger)std::numeric_limits<int>::max();
  const UnsignedInteger maximumOuterSampling = maximumOuterSamplingSpecified ? static_cast<UnsignedInteger>(ceil(1.0 * getMaximumCalls() / getBlockSize())) : (UnsignedInteger)std::numeric_limits<int>::max();
  const UnsignedInteger modulo = maximumOuterSamplingSpecified ? getMaximumCalls() % getBlockSize() : 0;
  const UnsignedInteger lastBlockSize = modulo == 0 ? getBlockSize() : modulo;

  Scalar coefficientOfVariation = -1.0;
  clock_t elapsedTime = 0;
  const clock_t startTime = clock();
  UnsignedInteger outerSampling = 0;

  // We loop if there remains some outer sampling and the coefficient of variation is greater than the limit or has not been computed yet.
  while (!stopRequested_
          && (outerSampling < maximumOuterSampling)
          && (coefficientOfVariation == -1.0 || coefficientOfVariation > getMaximumCoefficientOfVariation())
          && (static_cast<UnsignedInteger>(elapsedTime) < getMaximumElapsedTime() * CLOCKS_PER_SEC))
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
    oss << "Elapsed time = " << (float) elapsedTime / CLOCKS_PER_SEC << " s\n";
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
      blockOutputSample = computeOutputSample(blockInputSample);
    }
    catch (std::exception & ex)
    {
      failedInputSample_ = blockInputSample;
      warningMessage_ = ex.what();
    }

    // if SymbolicPhysicalModel find NaN and inf
    // for ex: in case of zero division the Symbolic models do not raise error
    // TODO rm this section with the next OT version (cf: https://github.com/openturns/openturns/pull/600)
    if (!failedInputSample_.getSize() && getPhysicalModel().getImplementation()->getClassName() == "SymbolicPhysicalModel")
    {
      for (UnsignedInteger j = 0; j < blockInputSample.getSize(); ++j)
      {
        for (UnsignedInteger k = 0; k < getInterestVariables().getSize(); ++k)
        {
          if (!SpecFunc::IsNormal(blockOutputSample(j, k)))
          {
            failedInputSample_ = blockInputSample;
            warningMessage_ = "At least a point failed. "
                            + getInterestVariables()[k]
                            + Point(blockInputSample[j]).__str__()
                            + " = "
                            + (OSS() << blockOutputSample(j, k)).str();
            break;
          }
        }
        if (failedInputSample_.getSize())
          break;
      }
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
      const Point empiricalStd(outputSample.computeStandardDeviationPerComponent());

      Scalar coefOfVar(0.);
      for (UnsignedInteger i = 0; i < outputSample.getDimension(); ++i)
      {
        if (std::abs(empiricalMean[i]) > SpecFunc::Precision)
        {
          const Scalar sigma_i = empiricalStd[i] / sqrt(outputSample.getSize());
          coefOfVar = std::max(sigma_i / std::abs(empiricalMean[i]), coefOfVar);
        }
        else
        {
          coefOfVar = -1;
        }
      }
      coefficientOfVariation = coefOfVar;
    }
    elapsedTime = clock() - startTime;
    ++outerSampling;
  }

  // check
  if (!outputSample.getSize())
    throw InvalidValueException(HERE) << "Monte Carlo Analysis failed. The output sample is empty. " << warningMessage_;

  // set design of experiments
  designOfExperiment_.setInputSample(effectiveInputSample);
  designOfExperiment_.setOutputSample(outputSample);

  // compute data analysis
  DataAnalysis dataAnalysis("", designOfExperiment_);
  dataAnalysis.setIsConfidenceIntervalRequired(isConfidenceIntervalRequired());
  dataAnalysis.setLevelConfidenceInterval(levelConfidenceInterval_);
  dataAnalysis.run();

  // set result
  result_ = dataAnalysis.getResult();
  result_.elapsedTime_ = (float) elapsedTime / CLOCKS_PER_SEC;
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
  param.add("Maximum coefficient of variation", getMaximumCoefficientOfVariation());
  String time = "- (s)";
  if (getMaximumCalls() < (UnsignedInteger)std::numeric_limits<int>::max())
    time = (OSS() << getMaximumElapsedTime()).str() + "(s)";
  param.add("Maximum elapsed time", time);
  String maxCalls = "-";
  if (getMaximumCalls() < (UnsignedInteger)std::numeric_limits<int>::max())
    maxCalls = (OSS() << getMaximumCalls()).str();
  param.add("Maximum calls", maxCalls);
  param.add("Block size", getBlockSize());
  param.add("Seed", getSeed());

  return param;
}


String MonteCarloAnalysis::getPythonScript() const
{
  OSS oss;
  oss.setPrecision(12);
  oss << getName() << " = otguibase.MonteCarloAnalysis('" << getName() << "', " << getPhysicalModel().getName() << ")\n";
  if (getInterestVariables().getSize() < getPhysicalModel().getSelectedOutputsNames().getSize())
  {
    oss << "interestVariables = " << Parameters::GetOTDescriptionStr(getInterestVariables()) << "\n";
    oss << getName() << ".setInterestVariables(interestVariables)\n";
  }
  if (getMaximumCalls() < (UnsignedInteger)std::numeric_limits<int>::max())
    oss << getName() << ".setMaximumCalls(" << getMaximumCalls() << ")\n";
  oss << getName() << ".setMaximumCoefficientOfVariation(" << getMaximumCoefficientOfVariation() << ")\n";
  if (getMaximumElapsedTime() < (UnsignedInteger)std::numeric_limits<int>::max())
    oss << getName() << ".setMaximumElapsedTime(" << getMaximumElapsedTime() << ")\n";
  oss << getName() << ".setBlockSize(" << getBlockSize() << ")\n";
  oss << getName() << ".setSeed(" << getSeed() << ")\n";

  return oss;
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
      << " blockSize=" << getBlockSize();
  return oss;
}


/* Method save() stores the object through the StorageManager */
void MonteCarloAnalysis::save(Advocate & adv) const
{
  SimulationAnalysis::save(adv);
  WithStopCriteriaAnalysis::save(adv);
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
}
}
