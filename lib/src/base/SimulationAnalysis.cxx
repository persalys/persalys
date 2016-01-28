// SimulationAnalysis.cxx

#include "otgui/SimulationAnalysis.hxx"

#include "RandomGenerator.hxx"
#include "CorrelationAnalysis.hxx"

using namespace OT;

namespace OTGUI {

CLASSNAMEINIT(SimulationAnalysis);

SimulationAnalysis::SimulationAnalysis(const String & name, const PhysicalModel & physicalModel,
                                       const UnsignedInteger nbSimulation)
  : AnalysisImplementation(name, physicalModel)
  , outputs_(physicalModel.getOutputs())
  , nbSimulations_(nbSimulation)
  , seed_(ResourceMap::GetAsNumericalScalar("RandomGenerator-InitialSeed"))
{
//TODO ctr with outputNames (pas OutputCollection!) optionnel par défaut prendrait tous les outputs
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


UnsignedInteger SimulationAnalysis::getNbSimulations() const
{
  return nbSimulations_;
}


void SimulationAnalysis::setNbSimulations(const UnsignedInteger nbSimu)
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
  Description outputNames(nbOutputs);
  for (int i=0; i<nbOutputs; ++i)
    outputNames[i] = outputs_[i].getName();
  return outputNames;
}


UnsignedInteger SimulationAnalysis::getSeed() const
{
  return seed_;
}


void SimulationAnalysis::setSeed(const UnsignedInteger seed)
{
  seed_ = seed;
}
}