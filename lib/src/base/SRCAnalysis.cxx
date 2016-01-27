// SRCAnalysis.cxx

#include "otgui/SRCAnalysis.hxx"

#include "RandomGenerator.hxx"
#include "CorrelationAnalysis.hxx"

using namespace OT;

namespace OTGUI {

CLASSNAMEINIT(SRCAnalysis);

SRCAnalysis::SRCAnalysis(const String & name, const PhysicalModel & physicalModel, const UnsignedInteger nbSimu)
  : SimulationAnalysis(name, physicalModel, nbSimu)
{
}


SRCAnalysis* SRCAnalysis::clone() const
{
  return new SRCAnalysis(*this);
}


void SRCAnalysis::run()
{
  RandomGenerator::SetSeed(getSeed());
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


String SRCAnalysis::dump() const
{
  OSS oss;
  oss << getName() << " = otguibase.SRCAnalysis('" << getName() << "', " << getPhysicalModel().getName();
  oss << ", " << getNbSimulations() << ")\n";
  oss << getName() << ".setSeed(" << getSeed() << ")\n";

  return oss;
}


bool SRCAnalysis::analysisLaunched() const
{
  return result_.getIndices().getSize()!=0;
}
}