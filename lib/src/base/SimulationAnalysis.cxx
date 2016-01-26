// SimulationAnalysis.cxx

#include "otgui/SimulationAnalysis.hxx"

#include "RandomGenerator.hxx"
#include "CorrelationAnalysis.hxx"

using namespace OT;

namespace OTGUI {

CLASSNAMEINIT(SimulationAnalysis);

SimulationAnalysis::SimulationAnalysis(const std::string & name, const PhysicalModel & physicalModel,
                                       int nbSimulation)
  : AnalysisImplementation(name, physicalModel)
  , outputs_(physicalModel.getOutputs())
  , nbSimulations_(nbSimulation)
{
//TODO ctr with outputNames (pas OutputCollection!) optionnel par défaut prendrait tous les outputs
}


SimulationAnalysis::SimulationAnalysis(const SimulationAnalysis & other)
  : AnalysisImplementation(other)
  , outputs_(other.outputs_)
  , nbSimulations_(other.nbSimulations_)
{
}


SimulationAnalysis* SimulationAnalysis::clone() const
{
  return new SimulationAnalysis(*this);
}


OutputCollection SimulationAnalysis::getOutputs() const
{
  return outputs_;
}


void SimulationAnalysis::setOutputs(const OutputCollection & outputs)
{
  outputs_ = outputs;
}


int SimulationAnalysis::getNbSimulations() const
{
  return nbSimulations_;
}


void SimulationAnalysis::setNbSimulations(const int nbSimu)
{
  nbSimulations_ = nbSimu;
}


NumericalSample SimulationAnalysis::getInputSample()
{
  NumericalSample inputSample(getPhysicalModel().getInputRandomVector().getSample(nbSimulations_));
  inputSample.setDescription(getPhysicalModel().getStochasticInputNames());
  return inputSample;
}


NumericalSample SimulationAnalysis::getOutputSample(NumericalSample inputSample) const
{
  return getPhysicalModel().getRestrictedFunction(getOutputNames())(inputSample);
}


NumericalSample SimulationAnalysis::getOutputSample(NumericalSample inputSample, const Description & outputNames) const
{
  return getPhysicalModel().getRestrictedFunction(outputNames)(inputSample);
}


Description SimulationAnalysis::getOutputNames() const
{
  int nbOutputs = outputs_.getSize();
  Description outputNames = Description(nbOutputs);
  for (int i=0; i<nbOutputs; ++i)
    outputNames[i] = outputs_[i].getName();
  return outputNames;
}
}