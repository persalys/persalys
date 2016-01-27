// SimulationAnalysis.hxx

#ifndef SIMULATIONANALYSIS_H
#define SIMULATIONANALYSIS_H

#include "AnalysisImplementation.hxx"

namespace OTGUI {
class SimulationAnalysis : public AnalysisImplementation
{
  CLASSNAME;

public:
  SimulationAnalysis(const OT::String & name, const PhysicalModel & physicalModel, const OT::UnsignedInteger nbSimu);

  virtual SimulationAnalysis * clone() const;

  OutputCollection getOutputs() const;
  void setOutputs(const OutputCollection & outputs);

  OT::UnsignedInteger getNbSimulations() const;
  void setNbSimulations(const OT::UnsignedInteger nbSimu);

  OT::NumericalSample getInputSample();
  OT::Description getOutputNames() const;

  OT::UnsignedInteger getSeed() const;
  void setSeed(const OT::UnsignedInteger seed);

protected:
  OT::NumericalSample getOutputSample(OT::NumericalSample inputSample) const;
  OT::NumericalSample getOutputSample(OT::NumericalSample inputSample, const OT::Description & outputNames) const;

private:
  OutputCollection outputs_;
  OT::UnsignedInteger nbSimulations_;
  OT::UnsignedInteger seed_;
};
}
#endif