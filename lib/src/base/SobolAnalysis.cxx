// SobolAnalysis.cxx

#include "otgui/SobolAnalysis.hxx"

#include "RandomGenerator.hxx"
#include "SensitivityAnalysis.hxx"

using namespace OT;

namespace OTGUI {

CLASSNAMEINIT(SobolAnalysis);

SobolAnalysis::SobolAnalysis(const std::string & name, const PhysicalModel & physicalModel, int nbSimu)
 : SimulationAnalysis(name, physicalModel, nbSimu)
 , result_()
{
//TODO ctr with outputNames (pas OutputCollection!) optionnel par défaut prendrait tous les outputs
}


SobolAnalysis::SobolAnalysis(const SobolAnalysis & other)
 : SimulationAnalysis(other)
 , result_(other.result_)
{
}


SobolAnalysis* SobolAnalysis::clone() const
{
  return new SobolAnalysis(*this);
}


void SobolAnalysis::run()
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
  result_ = SobolResult(firstOrderIndices, totalOrderIndices, getOutputNames());

  notify("analysisFinished");
}


SobolResult SobolAnalysis::getResult() const
{
  return result_;
}


std::string SobolAnalysis::dump() const
{
  std::string result;
  result += getName()+ " = otguibase.SobolAnalysis('" + getName() + "', " + getPhysicalModel().getName() + ")\n";
  return result;
}


bool SobolAnalysis::analysisLaunched() const
{
  return result_.getFirstOrderIndices().getSize()!=0;
}


}