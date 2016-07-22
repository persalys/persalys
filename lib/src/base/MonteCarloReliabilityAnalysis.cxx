//                                               -*- C++ -*-
/**
 *  @brief Class for Reliability Analysis with the Monte Carlo method
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
#include "otgui/MonteCarloReliabilityAnalysis.hxx"

#include "MonteCarlo.hxx"
#include "RandomGenerator.hxx"
#include "PersistentObjectFactory.hxx"

using namespace OT;

namespace OTGUI {

CLASSNAMEINIT(MonteCarloReliabilityAnalysis);

static Factory<MonteCarloReliabilityAnalysis> RegisteredFactory("MonteCarloReliabilityAnalysis");

/* Default constructor */
MonteCarloReliabilityAnalysis::MonteCarloReliabilityAnalysis()
  : ReliabilityAnalysis()
  , maximumCalls_(std::numeric_limits<int>::max())
  , maximumCoefficientOfVariation_(0.01)
  , maximumElapsedTime_(60) // in seconds
  , blockSize_(ResourceMap::GetAsNumericalScalar("Simulation-DefaultBlockSize"))
  , seed_(ResourceMap::GetAsNumericalScalar("RandomGenerator-InitialSeed"))
{
}


/* Constructor with parameters */
MonteCarloReliabilityAnalysis::MonteCarloReliabilityAnalysis(const String & name,
                                                             const LimitState & limitState)
  : ReliabilityAnalysis(name, limitState)
  , maximumCalls_(std::numeric_limits<int>::max())
  , maximumCoefficientOfVariation_(0.01)
  , maximumElapsedTime_(60) // in seconds
  , blockSize_(ResourceMap::GetAsNumericalScalar("Simulation-DefaultBlockSize"))
  , seed_(ResourceMap::GetAsNumericalScalar("RandomGenerator-InitialSeed"))
{
}


/* Virtual constructor */
MonteCarloReliabilityAnalysis* MonteCarloReliabilityAnalysis::clone() const
{
  return new MonteCarloReliabilityAnalysis(*this);
}


bool MonteCarloReliabilityAnalysis::Stop(void * p)
{
  TimeCriteria * arg = (TimeCriteria*)p;
  arg->elapsedTime_ = clock()-arg->startTime_;
  // stop algorithm when the elapsed time is superior to the max elapsed time
  if (arg->elapsedTime_ > arg->maxElapsedTime_)
    return true;
  return false;
}


void MonteCarloReliabilityAnalysis::run()
{
  RandomGenerator::SetSeed(getSeed());

  Description outputName(1);
  outputName[0] = getLimitState().getOutputName();
  NumericalMathFunction function(getPhysicalModel().getRestrictedFunction(outputName));
  function.enableHistory();
  function.clearHistory();

  Event event(RandomVector(function, getPhysicalModel().getInputRandomVector()), getLimitState().getOperator(), getLimitState().getThreshold());
  event.setDescription(outputName);

  MonteCarlo algo = MonteCarlo(event);
  UnsignedInteger maximumOuterSampling = std::numeric_limits<int>::max();
  if (maximumCalls_ < std::numeric_limits<int>::max())
  {
    algo.setConvergenceStrategy(Compact(maximumCalls_)); // TODO: propose in wizard the convergence sample's size?
    maximumOuterSampling = static_cast<UnsignedInteger>(ceil(1.0 * maximumCalls_ / blockSize_));
  }
  algo.setMaximumOuterSampling(maximumOuterSampling);
  algo.setMaximumCoefficientOfVariation(maximumCoefficientOfVariation_);
  algo.setBlockSize(blockSize_);

  TimeCriteria data(maximumElapsedTime_);
  algo.setStopCallback(&Stop, &data);

  algo.run();

  // set results
  // get convergence graph at level 0.95
  Graph graph = algo.drawProbabilityConvergence();
  result_ = MonteCarloReliabilityResult(algo.getResult(),
                                        function.getHistoryOutput().getSample(),
                                        graph.getDrawables()[0].getData(),
                                        graph.getDrawables()[1].getData(),
                                        graph.getDrawables()[2].getData());

  result_.setElapsedTime((float)data.elapsedTime_/CLOCKS_PER_SEC);

  function.disableHistory();

  notify("analysisFinished");
}


UnsignedInteger MonteCarloReliabilityAnalysis::getMaximumCalls() const
{
  return maximumCalls_;
}


void MonteCarloReliabilityAnalysis::setMaximumCalls(const UnsignedInteger maxi)
{
  if (maxi < blockSize_)
    throw InvalidValueException(HERE) << "The maximum calls can not be inferior to the block size";
  maximumCalls_ = maxi;
}


double MonteCarloReliabilityAnalysis::getMaximumCoefficientOfVariation() const
{
  return maximumCoefficientOfVariation_;
}


void MonteCarloReliabilityAnalysis::setMaximumCoefficientOfVariation(const double coef)
{
  maximumCoefficientOfVariation_ = coef;
}


UnsignedInteger MonteCarloReliabilityAnalysis::getMaximumElapsedTime() const
{
  return maximumElapsedTime_;
}


void MonteCarloReliabilityAnalysis::setMaximumElapsedTime(const UnsignedInteger seconds)
{
  maximumElapsedTime_ = seconds;
}


UnsignedInteger MonteCarloReliabilityAnalysis::getBlockSize() const
{
  return blockSize_;
}


void MonteCarloReliabilityAnalysis::setBlockSize(const UnsignedInteger size)
{
  if (size > maximumCalls_)
    throw InvalidValueException(HERE) << "The block size can not be superior to the maximum calls";
  blockSize_ = size;
}


UnsignedInteger MonteCarloReliabilityAnalysis::getSeed() const
{
  return seed_;
}


void MonteCarloReliabilityAnalysis::setSeed(const UnsignedInteger seed)
{
  seed_ = seed;
}


MonteCarloReliabilityResult MonteCarloReliabilityAnalysis::getResult() const
{
  return result_;
}


String MonteCarloReliabilityAnalysis::getPythonScript() const
{
  OSS oss;
  oss << getName() << " = otguibase.MonteCarloReliabilityAnalysis('" << getName() << "', " << getLimitState().getName() << ")\n";
  if (maximumCalls_ != std::numeric_limits<int>::max())
    oss << getName() << ".setMaximumCalls(" << maximumCalls_ << ")\n";
  oss << getName() << ".setMaximumCoefficientOfVariation(" << maximumCoefficientOfVariation_ << ")\n";
  if (maximumElapsedTime_ != std::numeric_limits<int>::max())
    oss << getName() << ".setMaximumElapsedTime(" << maximumElapsedTime_ << ")\n";
  oss << getName() << ".setBlockSize(" << blockSize_ << ")\n";
  oss << getName() << ".setSeed(" << seed_ << ")\n";

  return oss;
}


bool MonteCarloReliabilityAnalysis::analysisLaunched() const
{
  return getResult().getSimulationResult().getOuterSampling() != 0;
}


/* Method save() stores the object through the StorageManager */
void MonteCarloReliabilityAnalysis::save(Advocate & adv) const
{
  ReliabilityAnalysis::save(adv);
  adv.saveAttribute("maximumCalls_", maximumCalls_);
  adv.saveAttribute("maximumCoefficientOfVariation_", maximumCoefficientOfVariation_);
  adv.saveAttribute("maximumElapsedTime_", maximumElapsedTime_);
  adv.saveAttribute("blockSize_", blockSize_);
  adv.saveAttribute("seed_", seed_);
  adv.saveAttribute("result_", result_);
}


/* Method load() reloads the object from the StorageManager */
void MonteCarloReliabilityAnalysis::load(Advocate & adv)
{
  ReliabilityAnalysis::load(adv);
  adv.loadAttribute("maximumCalls_", maximumCalls_);
  adv.loadAttribute("maximumCoefficientOfVariation_", maximumCoefficientOfVariation_);
  adv.loadAttribute("maximumElapsedTime_", maximumElapsedTime_);
  adv.loadAttribute("blockSize_", blockSize_);
  adv.loadAttribute("seed_", seed_);
  adv.loadAttribute("result_", result_);
}
}