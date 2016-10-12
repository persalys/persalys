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

#include "otgui/DataAnalysis.hxx"
#include "otgui/DataModel.hxx"

#include "openturns/RandomGenerator.hxx"
#include "openturns/PersistentObjectFactory.hxx"

using namespace OT;

namespace OTGUI {

CLASSNAMEINIT(MonteCarloAnalysis);

static Factory<MonteCarloAnalysis> RegisteredFactory;

/* Default constructor */
MonteCarloAnalysis::MonteCarloAnalysis()
  : SimulationAnalysis()
  , WithStopCriteriaAnalysis()
  , isConfidenceIntervalRequired_(true)
  , levelConfidenceInterval_(0.95)
{
}


/* Constructor with parameters */
MonteCarloAnalysis::MonteCarloAnalysis(const String & name, const PhysicalModel & physicalModel)
  : SimulationAnalysis(name, physicalModel)
  , WithStopCriteriaAnalysis()
  , isConfidenceIntervalRequired_(true)
  , levelConfidenceInterval_(0.95)
{
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
  // clear result
  result_ = DataAnalysisResult();

  // initialization
  RandomGenerator::SetSeed(getSeed());

  NumericalSample effectiveInputSample(0, getPhysicalModel().getInputNames().getSize());
  effectiveInputSample.setDescription(getPhysicalModel().getInputNames());
  NumericalSample outputSample(0, getPhysicalModel().getOutputNames().getSize()); // TODO only required outputs
  outputSample.setDescription(getPhysicalModel().getOutputNames());

  const bool maximumOuterSamplingSpecified = getMaximumCalls() < (UnsignedInteger)std::numeric_limits<int>::max();
  const UnsignedInteger maximumOuterSampling = maximumOuterSamplingSpecified ? static_cast<UnsignedInteger>(ceil(1.0 * getMaximumCalls() / getBlockSize())) : (UnsignedInteger)std::numeric_limits<int>::max();
  const UnsignedInteger modulo = maximumOuterSamplingSpecified ? getMaximumCalls() % getBlockSize() : 0;
  const UnsignedInteger lastBlockSize = modulo == 0 ? getBlockSize() : modulo;

  NumericalScalar coefficientOfVariation = -1.0;
  clock_t elapsedTime = 0;
  const clock_t startTime = clock();
  UnsignedInteger outerSampling = 0;

  // We loop if there remains some outer sampling and the coefficient of variation is greater than the limit or has not been computed yet.
  while ((outerSampling < maximumOuterSampling)
     && ((coefficientOfVariation == -1.0) || (coefficientOfVariation > getMaximumCoefficientOfVariation()))
     &&  (elapsedTime < getMaximumElapsedTime()*CLOCKS_PER_SEC))
  {
    // the last block can be smaller
    const UnsignedInteger effectiveBlockSize = outerSampling < (maximumOuterSampling - 1) ? getBlockSize() : lastBlockSize;

    // Perform a block of simulation
    const NumericalSample blockInputSample(generateInputSample(effectiveBlockSize));
    effectiveInputSample.add(blockInputSample);

    const NumericalSample blockOutputSample(computeOutputSample(blockInputSample));
    outputSample.add(blockOutputSample);

    // stop criteria
    if ((getMaximumCoefficientOfVariation() != -1) &&
        (getBlockSize() != 1 || (getBlockSize() == 1 && outerSampling)))
    {
      const NumericalPoint empiricalMean(outputSample.computeMean());
      const NumericalPoint empiricalStd(outputSample.computeStandardDeviationPerComponent());
      NumericalScalar coefOfVar(0.);
      for (UnsignedInteger i=0; i<outputSample.getDimension(); ++i)
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
  if (outputSample.getSize())
  {
    DataAnalysis dataAnalysis("", DataModel("", effectiveInputSample, outputSample));
    dataAnalysis.setIsConfidenceIntervalRequired(isConfidenceIntervalRequired());
    dataAnalysis.setLevelConfidenceInterval(levelConfidenceInterval_);
    dataAnalysis.run();
    result_ = dataAnalysis.getResult();
    result_.setElapsedTime((float)elapsedTime / CLOCKS_PER_SEC);

    notify("analysisFinished");
  }
  else
    throw InvalidValueException(HERE) << "MonteCarloAnalysis::run : The output sample is empty";
}


DataAnalysisResult MonteCarloAnalysis::getResult() const
{
  return result_;
}


String MonteCarloAnalysis::getPythonScript() const
{
  OSS oss;
  oss << getName() << " = otguibase.MonteCarloAnalysis('" << getName() << "', " << getPhysicalModel().getName() << ")\n";
  if (getMaximumCalls() < (UnsignedInteger)std::numeric_limits<int>::max())
    oss << getName() << ".setMaximumCalls(" << getMaximumCalls() << ")\n";
  oss << getName() << ".setMaximumCoefficientOfVariation(" << getMaximumCoefficientOfVariation() << ")\n";
  if (getMaximumElapsedTime() < (UnsignedInteger)std::numeric_limits<int>::max())
    oss << getName() << ".setMaximumElapsedTime(" << getMaximumElapsedTime() << ")\n";
  oss << getName() << ".setBlockSize(" << getBlockSize() << ")\n";
  oss << getName() << ".setSeed(" << getSeed() << ")\n";

  return oss;
}


bool MonteCarloAnalysis::analysisLaunched() const
{
  return result_.getOutputSample().getSize() != 0;
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