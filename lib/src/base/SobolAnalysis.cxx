// SobolAnalysis.cxx

#include "otgui/SobolAnalysis.hxx"

#include "RandomGenerator.hxx"
#include "SensitivityAnalysis.hxx"

using namespace OT;

namespace OTGUI {

CLASSNAMEINIT(SobolAnalysis);

SobolAnalysis::SobolAnalysis(const String & name, const PhysicalModel & physicalModel, const UnsignedInteger nbSimu)
  : SimulationAnalysis(name, physicalModel, nbSimu)
{
//TODO ctr with outputNames (pas OutputCollection!) optionnel par défaut prendrait tous les outputs
}


SobolAnalysis* SobolAnalysis::clone() const
{
  return new SobolAnalysis(*this);
}


void SobolAnalysis::run()
{
  RandomGenerator::SetSeed(getSeed());
  NumericalSample inputSample1(getInputSample());
  NumericalSample inputSample2(getInputSample());

  SensitivityAnalysis sensitivityAnalysis = SensitivityAnalysis(inputSample1, inputSample2, getPhysicalModel().getRestrictedFunction(getOutputNames()));

  // set results
  Collection<SymmetricMatrix> secondOrderIndices;
  secondOrderIndices.add(sensitivityAnalysis.getSecondOrderIndices(0));
  NumericalSample firstOrderIndices(1, sensitivityAnalysis.getFirstOrderIndices(0));
  NumericalSample totalOrderIndices(1, sensitivityAnalysis.getTotalOrderIndices(0));

  for (UnsignedInteger i=1; i<getOutputs().getSize(); ++i)
  {
    secondOrderIndices.add(sensitivityAnalysis.getSecondOrderIndices(i));
    firstOrderIndices.add(sensitivityAnalysis.getFirstOrderIndices(i));
    totalOrderIndices.add(sensitivityAnalysis.getTotalOrderIndices(i));
  }

  firstOrderIndices.setDescription(getPhysicalModel().getStochasticInputNames());
  result_ = SobolResult(firstOrderIndices, secondOrderIndices, totalOrderIndices, getOutputNames());

  notify("analysisFinished");
}


SobolResult SobolAnalysis::getResult() const
{
  return result_;
}


String SobolAnalysis::dump() const
{
  OSS oss;
  oss << getName() << " = otguibase.SobolAnalysis('" << getName() << "', " << getPhysicalModel().getName();
  oss << ", " << getNbSimulations() << ")\n";
  oss << getName() << ".setSeed(" << getSeed() << ")\n";

  return oss;
}


bool SobolAnalysis::analysisLaunched() const
{
  return result_.getFirstOrderIndices().getSize()!=0;
}
}