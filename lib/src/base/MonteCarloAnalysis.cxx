// MonteCarloAnalysis.cxx

#include "otgui/MonteCarloAnalysis.hxx"
#include "RandomGenerator.hxx"

using namespace OT;

namespace OTGUI {

CLASSNAMEINIT(MonteCarloAnalysis);

MonteCarloAnalysis::MonteCarloAnalysis(const std::string & name, const PhysicalModel & physicalModel,
                                       int nbSimu, bool confidenceInterval, double level)
  : SimulationAnalysis(name, physicalModel, nbSimu)
  , isConfidenceIntervalRequired_(confidenceInterval)
  , levelConfidenceInterval_(level)
  , result_()
{
//TODO ctr with outputNames (pas OutputCollection!) optionnel par défaut prendrait tous les outputs
}


MonteCarloAnalysis::MonteCarloAnalysis(const MonteCarloAnalysis & other)
  : SimulationAnalysis(other)
  , isConfidenceIntervalRequired_(other.isConfidenceIntervalRequired_)
  , levelConfidenceInterval_(other.levelConfidenceInterval_)
  , result_(other.result_)
{
}


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
  levelConfidenceInterval_ = levelConfidenceInterval;
}


void MonteCarloAnalysis::run()
{
  RandomGenerator::SetSeed(0); //TODO seed in argument
  NumericalSample inputSample(getInputSample());
  // set results
  result_ = MonteCarloResult(inputSample, getOutputSample(inputSample));

  notify("analysisFinished");
}


MonteCarloResult MonteCarloAnalysis::getResult() const
{
  return result_;
}


std::string MonteCarloAnalysis::dump() const
{
  OSS oss;
  oss << getName() << " = otguibase.MonteCarloAnalysis('" << getName() << "', " << getPhysicalModel().getName();
  oss << ", " << getNbSimulations() << ")\n";

  return oss.str();
}


bool MonteCarloAnalysis::analysisLaunched() const
{
//   return outputSample_.getSize()!=0;
}
}