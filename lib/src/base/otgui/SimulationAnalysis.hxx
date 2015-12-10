// SimulationAnalysis.hxx

#ifndef SIMULATIONANALYSIS_H
#define SIMULATIONANALYSIS_H

#include "AnalysisImplementation.hxx"

namespace OTGUI {
class SimulationAnalysis : public AnalysisImplementation
{
  CLASSNAME;

public:
  SimulationAnalysis(const std::string & name, const PhysicalModel & physicalModel, int nbSimu);

  SimulationAnalysis(const SimulationAnalysis & other);
  virtual SimulationAnalysis * clone() const;

  OutputCollection getOutputs() const;
  void setOutputs(const OutputCollection & outputs);

  int getNbSimulations() const;
  void setNbSimulations(const int nbSimu);

  OT::NumericalSample getInputSample();
  OT::NumericalSample getOutputSample(OT::NumericalSample inputSample) const;
  OT::NumericalSample getOutputSample(OT::NumericalSample inputSample, const OT::Description & outputNames) const;
  OT::Description getOutputNames() const;

private:
  OutputCollection outputs_;
  int nbSimulations_;
};
}
#endif