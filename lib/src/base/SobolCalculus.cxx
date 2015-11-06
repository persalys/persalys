// SobolCalculus.cxx

#include "otgui/SobolCalculus.hxx"

#include "RandomGenerator.hxx"
#include "SensitivityAnalysis.hxx"

using namespace OT;

namespace OTGUI {

CLASSNAMEINIT(SobolCalculus);

SobolCalculus::SobolCalculus(const std::string & name, const PhysicalModel & physicalModel, int nbSimu)
 : SimulationCalculus(name, physicalModel, nbSimu)
 , result_()
{
//TODO ctr with outputNames (pas OutputCollection!) optionnel par défaut prendrait tous les outputs
}


SobolCalculus::SobolCalculus(const SobolCalculus & other)
 : SimulationCalculus(other)
 , result_(other.result_)
{
}


SobolCalculus* SobolCalculus::clone() const
{
  return new SobolCalculus(*this);
}


void SobolCalculus::run()
{
  RandomGenerator::SetSeed(0); //TODO seed in argument
  NumericalSample inputSample1(getInputSample());
  NumericalSample inputSample2(getInputSample());

  SensitivityAnalysis sensitivityAnalysis = SensitivityAnalysis(inputSample1, inputSample2, getPhysicalModel().getFunction(getOutputs()));

  // set results
  NumericalSample firstOrderIndices(1, sensitivityAnalysis.getFirstOrderIndices(0));
  NumericalSample totalOrderIndices(1, sensitivityAnalysis.getTotalOrderIndices(0));

  for (int i=1; i<getOutputs().getSize(); ++i)
  {
    firstOrderIndices.add(sensitivityAnalysis.getFirstOrderIndices(i));
    totalOrderIndices.add(sensitivityAnalysis.getTotalOrderIndices(i));
  }

  firstOrderIndices.setDescription(getPhysicalModel().getInputNames());
  result_ = SobolCalculusResult(firstOrderIndices, totalOrderIndices, getOutputNames());

  notify("calculusFinished");
}


SobolCalculusResult SobolCalculus::getResult() const
{
  return result_;
}


std::string SobolCalculus::dump() const
{
  std::string result;
  result += getName()+ " = otguibase.SobolCalculus('" + getName() + "', " + getPhysicalModel().getName() + ")\n";
  return result;
}


bool SobolCalculus::calculusLaunched() const
{
  return result_.getFirstOrderIndices().getSize()!=0;
}


}