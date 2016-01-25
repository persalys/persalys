// SRCAnalysis.cxx

#include "otgui/SRCAnalysis.hxx"

#include "RandomGenerator.hxx"
#include "CorrelationAnalysis.hxx"

using namespace OT;

namespace OTGUI {

CLASSNAMEINIT(SRCAnalysis);

SRCAnalysis::SRCAnalysis(const std::string & name, const PhysicalModel & physicalModel, int nbSimu)
 : SimulationAnalysis(name, physicalModel, nbSimu)
 , result_()
{
}


SRCAnalysis::SRCAnalysis(const SRCAnalysis & other)
 : SimulationAnalysis(other)
 , result_(other.result_)
{
}


SRCAnalysis* SRCAnalysis::clone() const
{
  return new SRCAnalysis(*this);
}


void SRCAnalysis::run()
{
  RandomGenerator::SetSeed(0); //TODO seed in argument
  NumericalSample inputSample(getInputSample());

  // set results
  NumericalSample indices(0, inputSample.getDimension());

  for (int i=0; i<getOutputs().getSize(); ++i)
  {
    Description outputName(1);
    outputName[0] = getOutputs()[i].getName();
    indices.add(CorrelationAnalysis::SRC(inputSample, getOutputSample(inputSample, outputName)));
  }

  indices.setDescription(inputSample.getDescription());
  result_ = SRCResult(indices, getOutputNames());

  notify("analysisFinished");
}


SRCResult SRCAnalysis::getResult() const
{
  return result_;
}


std::string SRCAnalysis::dump() const
{
  std::string result;
  result += getName()+ " = otguibase.SRCAnalysis('" + getName() + "', " + getPhysicalModel().getName() + ")\n";
  return result;
}


bool SRCAnalysis::analysisLaunched() const
{
  return result_.getIndices().getSize()!=0;
}
}