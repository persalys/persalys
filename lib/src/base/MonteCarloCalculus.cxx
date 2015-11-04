// MonteCarloCalculus.cxx

#include "otgui/MonteCarloCalculus.hxx"
#include "RandomGenerator.hxx"

using namespace OT;

namespace OTGUI {

CLASSNAMEINIT(MonteCarloCalculus);

MonteCarloCalculus::MonteCarloCalculus(const std::string & name, const PhysicalModel & physicalModel,
                                       int nbSimu, bool confidenceInterval, double level)
 : CalculusImplementation(name, physicalModel)
 , outputs_(physicalModel.getOutputs())
 , nbSimulations_(nbSimu)
 , isConfidenceIntervalRequired_(confidenceInterval)
 , levelConfidenceInterval_(level)
 , result_()
{
//TODO ctr with outputNames (pas OutputCollection!) optionnel par défaut prendrait tous les outputs
}


MonteCarloCalculus::MonteCarloCalculus(const MonteCarloCalculus & other)
 : CalculusImplementation(other)
 , outputs_(other.outputs_)
 , nbSimulations_(other.nbSimulations_)
 , isConfidenceIntervalRequired_(other.isConfidenceIntervalRequired_)
 , levelConfidenceInterval_(other.levelConfidenceInterval_)
 , result_(other.result_)
{
}


MonteCarloCalculus* MonteCarloCalculus::clone() const
{
  return new MonteCarloCalculus(*this);
}


OutputCollection MonteCarloCalculus::getOutputs() const
{
  return outputs_;
}


void MonteCarloCalculus::setOutputs(const OutputCollection & outputs)
{
  outputs_ = outputs;
}


int MonteCarloCalculus::getNbSimulations() const
{
  return nbSimulations_;
}


void MonteCarloCalculus::setNbSimulations(const int nbSimu)
{
  nbSimulations_ = nbSimu;
}


bool MonteCarloCalculus::isConfidenceIntervalRequired() const
{
  return isConfidenceIntervalRequired_;
}


void MonteCarloCalculus::setIsConfidenceIntervalRequired(const bool isConfidenceIntervalRequired)
{
  isConfidenceIntervalRequired_ = isConfidenceIntervalRequired;
}


double MonteCarloCalculus::getLevelConfidenceInterval() const
{
  return levelConfidenceInterval_;
}


void MonteCarloCalculus::setLevelConfidenceInterval(const double levelConfidenceInterval)
{
  levelConfidenceInterval_ = levelConfidenceInterval;
}


void MonteCarloCalculus::run()
{
  RandomGenerator::SetSeed(0); //TODO seed in argument
  NumericalSample inputSample(getPhysicalModel().getInputRandomVector().getSample(nbSimulations_));
  inputSample.setDescription(getPhysicalModel().getFunction().getInputDescription());
  // set results
  result_ = MonteCarloResult(getPhysicalModel().getFunction(outputs_)(inputSample), inputSample);

  notify("calculusFinished");
}


MonteCarloResult MonteCarloCalculus::getResult() const
{
  return result_;
}


std::string MonteCarloCalculus::dump() const
{
  std::string result;
  OSS oss;
  oss << nbSimulations_;
  result += getName()+ " = otguibase.MonteCarloCalculus('" + getName() + "', " + getPhysicalModel().getName();
  result += ", " + oss.str() + ")\n";
  return result;
}


bool MonteCarloCalculus::calculusLaunched() const
{
//   return resultSample_.getSize()!=0;
}


}