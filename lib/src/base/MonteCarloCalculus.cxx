// MonteCarloCalculus.cxx

#include "MonteCarloCalculus.hxx"
#include "RandomGenerator.hxx"

using namespace OT;

namespace OTGUI {

CLASSNAMEINIT(MonteCarloCalculus);

MonteCarloCalculus::MonteCarloCalculus(const std::string & name, const PhysicalModel & physicalModel,
                                       int nbSimu, bool confidenceInterval, double level)
 : CalculusImplementation(name, physicalModel)
 , outputs_(physicalModel.getOutputs())
 , nbSimulations_(nbSimu)
 , computeCondidenceInterval_(confidenceInterval)
 , levelConfidenceInterval_(level)
 , result_()
{
//TODO ctr with outputNames (pas OutputCollection!) optionnel par défaut prendrait tous les outputs
}


MonteCarloCalculus::MonteCarloCalculus(const MonteCarloCalculus & other)
 : CalculusImplementation(other)
 , outputs_(other.outputs_)
 , nbSimulations_(other.nbSimulations_)
 , computeCondidenceInterval_(other.computeCondidenceInterval_)
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


void MonteCarloCalculus::run()
{
  RandomGenerator::SetSeed(0); //TODO seed in argument
  NumericalSample inputSample(getPhysicalModel().getInputRandomVector().getSample(nbSimulations_));
  // set results
  result_ = MonteCarloResult(getPhysicalModel().getFunction(outputs_)(inputSample), inputSample,
                             levelConfidenceInterval_);

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