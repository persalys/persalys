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
 : AnalysisImplementation(name, limitState.getPhysicalModel())
 , limitState_(limitState)
 , maximumOuterSampling_(maximumOuterSampling)
 , maximumCoefficientOfVariation_(ResourceMap::GetAsNumericalScalar("Simulation-DefaultMaximumCoefficientOfVariation"))
 , result_()
{
}


MonteCarloReliabilityAnalysis::MonteCarloReliabilityAnalysis(const MonteCarloReliabilityAnalysis & other)
 : AnalysisImplementation(other)
 , limitState_(other.limitState_)
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
  MonteCarlo algo = MonteCarlo(limitState_.getEvent());
  algo.setMaximumOuterSampling(maximumOuterSampling_);
  algo.setMaximumCoefficientOfVariation(maximumCoefficientOfVariation_);
  algo.run();
  // set results
  result_ = algo.getResult();

  notify("analysisFinished");
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
  result += limitState_.dump();
  result += getName()+ " = otguibase.MonteCarloReliabilityAnalysis('" + getName() + "', " + limitState_.getName();
  OSS oss1;
  oss1 << maximumOuterSampling_;
  result += ", " + oss1.str() + ")\n";
  OSS oss2;
  oss2 << maximumCoefficientOfVariation_;
  result += getName() + ".setMaximumCoefficientOfVariation(" + oss2.str() + ")\n";
  return result;
}
}