// SimulationCalculus.cxx

#include "otgui/SimulationCalculus.hxx"

#include "RandomGenerator.hxx"
#include "CorrelationAnalysis.hxx"

using namespace OT;

namespace OTGUI {

CLASSNAMEINIT(SimulationCalculus);

SimulationCalculus::SimulationCalculus(const std::string & name, const PhysicalModel & physicalModel,
                                       int nbSimulation)
 : CalculusImplementation(name, physicalModel)
 , outputs_(physicalModel.getOutputs())
 , nbSimulations_(nbSimulation)
{
//TODO ctr with outputNames (pas OutputCollection!) optionnel par défaut prendrait tous les outputs
}


SimulationCalculus::SimulationCalculus(const SimulationCalculus & other)
 : CalculusImplementation(other)
 , outputs_(other.outputs_)
 , nbSimulations_(other.nbSimulations_)
{
}


SimulationCalculus* SimulationCalculus::clone() const
{
  return new SimulationCalculus(*this);
}


OutputCollection SimulationCalculus::getOutputs() const
{
  return outputs_;
}


void SimulationCalculus::setOutputs(const OutputCollection & outputs)
{
  outputs_ = outputs;
}


int SimulationCalculus::getNbSimulations() const
{
  return nbSimulations_;
}


void SimulationCalculus::setNbSimulations(const int nbSimu)
{
  nbSimulations_ = nbSimu;
}


NumericalSample SimulationCalculus::getInputSample()
{
  NumericalSample inputSample(getPhysicalModel().getInputRandomVector().getSample(nbSimulations_));
  inputSample.setDescription(getPhysicalModel().getInputNames());
  return inputSample;
}


NumericalSample SimulationCalculus::getOutputSample(NumericalSample inputSample) const
{
  return getPhysicalModel().getFunction(outputs_)(inputSample);
}


NumericalSample SimulationCalculus::getOutputSample(NumericalSample inputSample, const OutputCollection & output) const
{
  return getPhysicalModel().getFunction(output)(inputSample);
}


Description SimulationCalculus::getOutputNames() const
{
  int nbOutputs = outputs_.getSize();
  Description outputNames = Description(nbOutputs);
  for (int i=0; i<nbOutputs; ++i)
    outputNames[i] = outputs_[i].getName();
  return outputNames;
}

}