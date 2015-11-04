// SRCCalculus.cxx

#include "otgui/SRCCalculus.hxx"

#include "RandomGenerator.hxx"
#include "CorrelationAnalysis.hxx"

using namespace OT;

namespace OTGUI {

CLASSNAMEINIT(SRCCalculus);

SRCCalculus::SRCCalculus(const std::string & name, const PhysicalModel & physicalModel, int nbSimu)
 : SimulationCalculus(name, physicalModel, nbSimu)
 , result_()
{
}


SRCCalculus::SRCCalculus(const SRCCalculus & other)
 : SimulationCalculus(other)
 , result_(other.result_)
{
}


SRCCalculus* SRCCalculus::clone() const
{
  return new SRCCalculus(*this);
}


void SRCCalculus::run()
{
  RandomGenerator::SetSeed(0); //TODO seed in argument
  NumericalSample inputSample(getInputSample());

  // set results
  NumericalSample indices(0, inputSample.getDimension());

  for (int i=0; i<getOutputs().getSize(); ++i)
  {
    OutputCollection output_i;
    output_i.add(getOutputs()[i]);
    indices.add(CorrelationAnalysis::SRC(inputSample, getOutputSample(inputSample, output_i)));
  }

  indices.setDescription(inputSample.getDescription());
  result_ = SRCCalculusResult(indices, getOutputNames());

  notify("calculusFinished");
}


SRCCalculusResult SRCCalculus::getResult() const
{
  return result_;
}


std::string SRCCalculus::dump() const
{
  std::string result;
  result += getName()+ " = otguibase.SRCCalculus('" + getName() + "', " + getPhysicalModel().getName() + ")\n";
  return result;
}


bool SRCCalculus::calculusLaunched() const
{
  return result_.getIndices().getSize()!=0;
}


}