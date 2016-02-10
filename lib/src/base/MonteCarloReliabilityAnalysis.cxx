//                                               -*- C++ -*-
/**
 *  @brief Class for Reliability Analysis with the Monte Carlo method
 *
 *  Copyright 2015-2016 EDF
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

using namespace OT;

namespace OTGUI {

CLASSNAMEINIT(MonteCarloReliabilityAnalysis);

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


MonteCarloReliabilityAnalysis* MonteCarloReliabilityAnalysis::clone() const
{
  return new MonteCarloReliabilityAnalysis(*this);
}


void MonteCarloReliabilityAnalysis::run()
{
  RandomGenerator::SetSeed(getSeed());
  MonteCarlo algo = MonteCarlo(getLimitState().getEvent());
  algo.setMaximumOuterSampling(maximumOuterSampling_);
  algo.setMaximumCoefficientOfVariation(maximumCoefficientOfVariation_);
  algo.setBlockSize(blockSize_);
  algo.run();

  // set results
  result_ = algo.getResult();

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


SimulationResult MonteCarloReliabilityAnalysis::getResult() const
{
  return result_;
}


String MonteCarloReliabilityAnalysis::dump() const
{
  OSS oss;
  oss << getName() << " = otguibase.MonteCarloReliabilityAnalysis('" << getName() << "', " << getLimitState().getName();
  oss << ", " << maximumOuterSampling_ << ")\n";
  oss << getName() << ".setMaximumCoefficientOfVariation(" << maximumCoefficientOfVariation_ << ")\n";
  oss << getName() << ".setBlockSize(" << blockSize_ << ")\n";
  oss << getName() << ".setSeed(" << seed_ << ")\n";

  return oss;
}
}