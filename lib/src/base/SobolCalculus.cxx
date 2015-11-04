// SobolCalculus.cxx

#include "otgui/SobolCalculus.hxx"

#include "RandomGenerator.hxx"
#include "SensitivityAnalysis.hxx"

using namespace OT;

namespace OTGUI {

CLASSNAMEINIT(SobolCalculus);

SobolCalculus::SobolCalculus(const std::string & name, const PhysicalModel & physicalModel, int nbSimu)
 : CalculusImplementation(name, physicalModel)
 , outputs_(physicalModel.getOutputs())
 , nbSimulations_(nbSimu)
 , result_()
{
//TODO ctr with outputNames (pas OutputCollection!) optionnel par défaut prendrait tous les outputs
}


SobolCalculus::SobolCalculus(const SobolCalculus & other)
 : CalculusImplementation(other)
 , outputs_(other.outputs_)
 , nbSimulations_(other.nbSimulations_)
 , result_(other.result_)
{
}


SobolCalculus* SobolCalculus::clone() const
{
  return new SobolCalculus(*this);
}


OutputCollection SobolCalculus::getOutputs() const
{
  return outputs_;
}


void SobolCalculus::setOutputs(const OutputCollection & outputs)
{
  outputs_ = outputs;
}


int SobolCalculus::getNbSimulations() const
{
  return nbSimulations_;
}


void SobolCalculus::setNbSimulations(const int nbSimu)
{
  nbSimulations_ = nbSimu;
}


void SobolCalculus::run()
{
  RandomGenerator::SetSeed(0); //TODO seed in argument
  NumericalSample inputSample1(getPhysicalModel().getInputRandomVector().getSample(nbSimulations_));
  NumericalSample inputSample2(getPhysicalModel().getInputRandomVector().getSample(nbSimulations_));
  inputSample1.setDescription(getPhysicalModel().getFunction().getInputDescription());
  inputSample2.setDescription(getPhysicalModel().getFunction().getInputDescription());
  SensitivityAnalysis sensitivityAnalysis = SensitivityAnalysis(inputSample1, inputSample2, getPhysicalModel().getFunction(outputs_));
  // set results
  NumericalSample firstOrderIndices(1, sensitivityAnalysis.getFirstOrderIndices(0));
  NumericalSample totalOrderIndices(1, sensitivityAnalysis.getTotalOrderIndices(0));
  for (int i=1; i<outputs_.getSize(); ++i)
  {
    firstOrderIndices.add(sensitivityAnalysis.getFirstOrderIndices(i));
    totalOrderIndices.add(sensitivityAnalysis.getTotalOrderIndices(i));
  }
  firstOrderIndices.setDescription(getPhysicalModel().getFunction().getInputDescription());
  result_ = SobolCalculusResult(firstOrderIndices, totalOrderIndices,
                                getPhysicalModel().getFunction().getOutputDescription());

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