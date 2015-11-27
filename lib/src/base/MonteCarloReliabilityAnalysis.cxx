// MonteCarloReliabilityAnalysis.cxx

#include "otgui/MonteCarloReliabilityAnalysis.hxx"

#include "MonteCarlo.hxx"
#include "RandomGenerator.hxx"

using namespace OT;

namespace OTGUI {

CLASSNAMEINIT(MonteCarloReliabilityAnalysis);

MonteCarloReliabilityAnalysis::MonteCarloReliabilityAnalysis(const std::string & name,
                                                             const LimitState & limitState,
                                                             const UnsignedInteger & maximumOuterSampling)
 : ReliabilityAnalysis(name, limitState)
 , maximumOuterSampling_(maximumOuterSampling)
 , maximumCoefficientOfVariation_(ResourceMap::GetAsNumericalScalar("Simulation-DefaultMaximumCoefficientOfVariation"))
 , result_()
{
}


MonteCarloReliabilityAnalysis::MonteCarloReliabilityAnalysis(const MonteCarloReliabilityAnalysis & other)
 : ReliabilityAnalysis(other)
 , maximumOuterSampling_(other.maximumOuterSampling_)
 , maximumCoefficientOfVariation_(other.maximumCoefficientOfVariation_)
 , result_(other.result_)
{
}


MonteCarloReliabilityAnalysis* MonteCarloReliabilityAnalysis::clone() const
{
  return new MonteCarloReliabilityAnalysis(*this);
}


void MonteCarloReliabilityAnalysis::run()
{
  RandomGenerator::SetSeed(0); //TODO seed in argument
  MonteCarlo algo = MonteCarlo(getLimitState().getEvent());
  algo.setMaximumOuterSampling(maximumOuterSampling_);
  algo.setMaximumCoefficientOfVariation(maximumCoefficientOfVariation_);
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


SimulationResult MonteCarloReliabilityAnalysis::getResult() const
{
  return result_;
}


std::string MonteCarloReliabilityAnalysis::dump() const
{
  std::string result;
  result += getName()+ " = otguibase.MonteCarloReliabilityAnalysis('" + getName() + "', " + getLimitState().getName();
  OSS oss1;
  oss1 << maximumOuterSampling_;
  result += ", " + oss1.str() + ")\n";
  OSS oss2;
  oss2 << maximumCoefficientOfVariation_;
  result += getName() + ".setMaximumCoefficientOfVariation(" + oss2.str() + ")\n";
  return result;
}
}