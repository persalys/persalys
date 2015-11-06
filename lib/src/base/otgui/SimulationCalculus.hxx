// SimulationCalculus.hxx

#ifndef SIMULATIONCALCULUS_H
#define SIMULATIONCALCULUS_H

#include "CalculusImplementation.hxx"

namespace OTGUI {
class SimulationCalculus : public CalculusImplementation
{
  CLASSNAME;

public:
  SimulationCalculus(const std::string & name, const PhysicalModel & physicalModel, int nbSimu);

  SimulationCalculus(const SimulationCalculus & other);
  virtual SimulationCalculus * clone() const;

  OutputCollection getOutputs() const;
  void setOutputs(const OutputCollection & outputs);

  int getNbSimulations() const;
  void setNbSimulations(const int nbSimu);

  OT::NumericalSample getInputSample();
  OT::NumericalSample getOutputSample(OT::NumericalSample inputSample) const;
  OT::NumericalSample getOutputSample(OT::NumericalSample inputSample, const OutputCollection & output) const;
  OT::Description getOutputNames() const;

private:
  OutputCollection outputs_;
  int nbSimulations_;
};
}
#endif