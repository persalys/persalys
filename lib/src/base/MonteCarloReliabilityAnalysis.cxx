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

#include "openturns/MonteCarlo.hxx"
#include "openturns/RandomGenerator.hxx"
#include "openturns/PersistentObjectFactory.hxx"

using namespace OT;

namespace OTGUI {

CLASSNAMEINIT(MonteCarloReliabilityAnalysis);

static Factory<MonteCarloReliabilityAnalysis> RegisteredFactory;

/* Default constructor */
MonteCarloReliabilityAnalysis::MonteCarloReliabilityAnalysis()
  : ReliabilityAnalysis()
  , maximumOuterSampling_(ResourceMap::GetAsNumericalScalar("Simulation-DefaultMaximumOuterSampling"))
  , maximumCoefficientOfVariation_(ResourceMap::GetAsNumericalScalar("Simulation-DefaultMaximumCoefficientOfVariation"))
  , blockSize_(ResourceMap::GetAsNumericalScalar("Simulation-DefaultBlockSize"))
  , seed_(ResourceMap::GetAsNumericalScalar("RandomGenerator-InitialSeed"))
{
}


/* Constructor with parameters */
MonteCarloReliabilityAnalysis::MonteCarloReliabilityAnalysis(const String & name,
                                                             const LimitState & limitState,
                                                             const UnsignedInteger & maximumOuterSampling)
  : ReliabilityAnalysis(name, limitState)
  , maximumOuterSampling_(maximumOuterSampling)
  , maximumCoefficientOfVariation_(ResourceMap::GetAsNumericalScalar("Simulation-DefaultMaximumCoefficientOfVariation"))
  , blockSize_(ResourceMap::GetAsNumericalScalar("Simulation-DefaultBlockSize"))
  , seed_(ResourceMap::GetAsNumericalScalar("RandomGenerator-InitialSeed"))
{
}


/* Virtual constructor */
MonteCarloReliabilityAnalysis* MonteCarloReliabilityAnalysis::clone() const
{
  return new MonteCarloReliabilityAnalysis(*this);
}


void MonteCarloReliabilityAnalysis::run()
{
  RandomGenerator::SetSeed(getSeed());

  Description outputDescription(1);
  outputDescription[0] = getLimitState().getOutputName();
  NumericalMathFunction function = getPhysicalModel().getRestrictedFunction(outputDescription);
  function.enableHistory();
  function.clearHistory();

  Event event(RandomVector(function, getPhysicalModel().getInputRandomVector()), getLimitState().getOperator(), getLimitState().getThreshold());
  event.setDescription(outputDescription);

  MonteCarlo algo = MonteCarlo(event);
  algo.setMaximumOuterSampling(maximumOuterSampling_);
  algo.setMaximumCoefficientOfVariation(maximumCoefficientOfVariation_);
  algo.setBlockSize(blockSize_);
  algo.run();

  // set results
  // get convergence graph at level 0.95
  Graph graph = algo.drawProbabilityConvergence();
  result_ = MonteCarloReliabilityResult(algo.getResult(),
                                                function.getHistoryOutput().getSample(),
                                                graph.getDrawables()[0].getData(),
                                                graph.getDrawables()[1].getData(),
                                                graph.getDrawables()[2].getData());
  function.disableHistory();

  notify("analysisFinished");
}


UnsignedInteger MonteCarloReliabilityAnalysis::getMaximumOuterSampling() const
{
  return maximumOuterSampling_;
}


void MonteCarloReliabilityAnalysis::setMaximumOuterSampling(const UnsignedInteger & maxi)
{
  maximumOuterSampling_ = maxi;
}


double MonteCarloReliabilityAnalysis::getMaximumCoefficientOfVariation() const
{
  return maximumCoefficientOfVariation_;
}


void MonteCarloReliabilityAnalysis::setMaximumCoefficientOfVariation(const double & coef)
{
  maximumCoefficientOfVariation_ = coef;
}


UnsignedInteger MonteCarloReliabilityAnalysis::getBlockSize() const
{
  return blockSize_;
}


void MonteCarloReliabilityAnalysis::setBlockSize(const UnsignedInteger & size)
{
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
  oss << getName() << " = otguibase.MonteCarloReliabilityAnalysis('" << getName() << "', " << getLimitState().getName();
  oss << ", " << maximumOuterSampling_ << ")\n";
  oss << getName() << ".setMaximumCoefficientOfVariation(" << maximumCoefficientOfVariation_ << ")\n";
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
  adv.saveAttribute("maximumOuterSampling_", maximumOuterSampling_);
  adv.saveAttribute("maximumCoefficientOfVariation_", maximumCoefficientOfVariation_);
  adv.saveAttribute("blockSize_", blockSize_);
  adv.saveAttribute("seed_", seed_);
  adv.saveAttribute("result_", result_);
}


/* Method load() reloads the object from the StorageManager */
void MonteCarloReliabilityAnalysis::load(Advocate & adv)
{
  ReliabilityAnalysis::load(adv);
  adv.loadAttribute("maximumOuterSampling_", maximumOuterSampling_);
  adv.loadAttribute("maximumCoefficientOfVariation_", maximumCoefficientOfVariation_);
  adv.loadAttribute("blockSize_", blockSize_);
  adv.loadAttribute("seed_", seed_);
  adv.loadAttribute("result_", result_);
}
}