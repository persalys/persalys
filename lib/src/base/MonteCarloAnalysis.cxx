//                                               -*- C++ -*-
/**
 *  @brief Computes Monte Carlo analysis 
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
#include "otgui/MonteCarloAnalysis.hxx"

#include "RandomGenerator.hxx"
#include "PersistentObjectFactory.hxx"

using namespace OT;

namespace OTGUI {

CLASSNAMEINIT(MonteCarloAnalysis);

static Factory<MonteCarloAnalysis> RegisteredFactory("MonteCarloAnalysis");

/* Default constructor */
MonteCarloAnalysis::MonteCarloAnalysis()
  : SimulationAnalysis()
  , isConfidenceIntervalRequired_(true)
  , levelConfidenceInterval_(0.95)
  , maximumCoefficientOfVariation_(0.01)
  , maximumElapsedTime_(60) // in seconds
  , blockSize_(ResourceMap::GetAsNumericalScalar("Simulation-DefaultBlockSize"))
{
}


/* Constructor with parameters */
MonteCarloAnalysis::MonteCarloAnalysis(const String & name, const PhysicalModel & physicalModel,
                                       const UnsignedInteger nbSimu, bool confidenceInterval, double level)
  : SimulationAnalysis(name, physicalModel, nbSimu)
  , isConfidenceIntervalRequired_(confidenceInterval)
  , maximumCoefficientOfVariation_(0.01)
  , maximumElapsedTime_(60) // in seconds
  , blockSize_(ResourceMap::GetAsNumericalScalar("Simulation-DefaultBlockSize"))
{
  setLevelConfidenceInterval(level);
//TODO ctr with outputNames (pas OutputCollection!) optionnel par défaut prendrait tous les outputs
}


/* Virtual constructor */
MonteCarloAnalysis* MonteCarloAnalysis::clone() const
{
  return new MonteCarloAnalysis(*this);
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


void MonteCarloAnalysis::run()
{
  RandomGenerator::SetSeed(getSeed());

  NumericalSample effectiveInputSample(0, getPhysicalModel().getInputNames().getSize());
  effectiveInputSample.setDescription(getPhysicalModel().getInputNames());
  NumericalSample outputSample(0, getPhysicalModel().getOutputNames().getSize()); // TODO only required outputs
  outputSample.setDescription(getPhysicalModel().getOutputNames());

  const bool maximumOuterSamplingSpecified = getNbSimulations() < std::numeric_limits<int>::max();
  const UnsignedInteger maximumOuterSampling = maximumOuterSamplingSpecified ? static_cast<UnsignedInteger>(ceil(1.0 * getNbSimulations() / blockSize_)) : std::numeric_limits<int>::max();
  const UnsignedInteger modulo = maximumOuterSamplingSpecified ? getNbSimulations() % blockSize_ : 0;
  const UnsignedInteger lastBlockSize = modulo == 0 ? blockSize_ : modulo;

  NumericalScalar coefficientOfVariation = -1.0;
  clock_t elapsedTime = 0;
  const clock_t startTime = clock();
  UnsignedInteger outerSampling = 0;

  // We loop if there remains some outer sampling and the coefficient of variation is greater than the limit or has not been computed yet.
  while ((outerSampling < maximumOuterSampling)
     && ((coefficientOfVariation == -1.0) || (coefficientOfVariation > getMaximumCoefficientOfVariation()))
     &&  (elapsedTime < maximumElapsedTime_*CLOCKS_PER_SEC))
  {
    // the last block can be smaller
    const UnsignedInteger effectiveBlockSize = outerSampling < (maximumOuterSampling - 1) ? blockSize_ : lastBlockSize;

    // Perform a block of simulation
    const NumericalSample blockInputSample(getInputSample(effectiveBlockSize));
    effectiveInputSample.add(blockInputSample);

    const NumericalSample blockOutputSample(getOutputSample(blockInputSample));
    outputSample.add(blockOutputSample);

    // stop criteria
    if ((getMaximumCoefficientOfVariation() != -1) && (blockSize_ != 1 || (blockSize_ == 1 && outerSampling)))
    {
      NumericalPoint empiricalMean = outputSample.computeMean();
      NumericalPoint empiricalStd = outputSample.computeStandardDeviationPerComponent();
      NumericalScalar coefOfVar(0.);
      for (int i=0; i<outputSample.getDimension(); ++i)
      {
        const NumericalScalar sigma_i = empiricalStd[i] / sqrt(outputSample.getSize());
        coefOfVar = std::max(sigma_i / empiricalMean[i], coefOfVar);
      }
      coefficientOfVariation = coefOfVar;
    }
    elapsedTime = clock() - startTime;
    ++outerSampling;
  }

  // set results
  result_ = MonteCarloResult(effectiveInputSample, outputSample);
  result_.setElapsedTime((float)elapsedTime / CLOCKS_PER_SEC);

  notify("analysisFinished");
}


double MonteCarloAnalysis::getMaximumCoefficientOfVariation() const
{
  return maximumCoefficientOfVariation_;
}


void MonteCarloAnalysis::setMaximumCoefficientOfVariation(const double coef)
{
  maximumCoefficientOfVariation_ = coef;
}


UnsignedInteger MonteCarloAnalysis::getMaximumElapsedTime() const
{
  return maximumElapsedTime_;
}


void MonteCarloAnalysis::setMaximumElapsedTime(const UnsignedInteger seconds)
{
  maximumElapsedTime_ = seconds;
}


UnsignedInteger MonteCarloAnalysis::getBlockSize() const
{
  return blockSize_;
}


void MonteCarloAnalysis::setBlockSize(const UnsignedInteger size)
{
  if (size > getNbSimulations()) // TODO check also in getMaximumCalls/NbSimulations
    throw InvalidValueException(HERE) << "The block size can not be superior to the maximum number of simulations";
  blockSize_ = size;
}


MonteCarloResult MonteCarloAnalysis::getResult() const
{
  return result_;
}


String MonteCarloAnalysis::getPythonScript() const
{
  OSS oss;
  oss << getName() << " = otguibase.MonteCarloAnalysis('" << getName() << "', " << getPhysicalModel().getName();
  oss << ", " << getNbSimulations() << ")\n";
  oss << getName() << ".setSeed(" << getSeed() << ")\n";

  return oss;
}


bool MonteCarloAnalysis::analysisLaunched() const
{
  return getResult().getOutputSample().getSize() != 0;
}


/* Method save() stores the object through the StorageManager */
void MonteCarloAnalysis::save(Advocate & adv) const
{
  SimulationAnalysis::save(adv);
  adv.saveAttribute("isConfidenceIntervalRequired_", isConfidenceIntervalRequired_);
  adv.saveAttribute("levelConfidenceInterval_", levelConfidenceInterval_);
  adv.saveAttribute("result_", result_);
}


/* Method load() reloads the object from the StorageManager */
void MonteCarloAnalysis::load(Advocate & adv)
{
  SimulationAnalysis::load(adv);
  adv.loadAttribute("isConfidenceIntervalRequired_", isConfidenceIntervalRequired_);
  adv.loadAttribute("levelConfidenceInterval_", levelConfidenceInterval_);
  adv.loadAttribute("result_", result_);
}
}