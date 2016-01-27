// MonteCarloReliabilityAnalysis.cxx

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