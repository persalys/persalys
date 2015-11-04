// ParametricCalculus.hxx

#ifndef PARAMETRICCALCULUS_H
#define PARAMETRICCALCULUS_H

#include "SimulationCalculus.hxx"
#include "ParametricCalculusResult.hxx"

namespace OTGUI {
class ParametricCalculus : public SimulationCalculus
{
  CLASSNAME;

public:
  ParametricCalculus(const std::string & name, const PhysicalModel & physicalModel);
  ParametricCalculus(const std::string & name, const PhysicalModel & physicalModel,
                     const OT::NumericalPoint & infBounds, const OT::NumericalPoint & supBounds,
                     const OT::Indices & nbValues);
  ParametricCalculus(const ParametricCalculus & other);
  virtual ParametricCalculus * clone() const;

  void initializeParameters(const InputCollection & inputs);
  void updateParameters();

  OT::NumericalSample getInputSample();
  void setInputSample(const OT::NumericalSample & inputSample);

  OT::NumericalPoint getInfBounds() const;
  void setInfBound(const int & index, const double & infBounds);
  OT::NumericalPoint getSupBounds() const;
  void setSupBound(const int & index, const double & supBounds);
  OT::Indices getNbValues() const;
  void setNbValues(const int & index, const int & nbValues);

  ParametricCalculusResult getResult() const;

  virtual void run();
  virtual std::string dump() const;
  virtual bool calculusLaunched() const;

private:
  OT::Description inputNames_;
  OT::NumericalSample inputSample_;
  ParametricCalculusResult result_;
  OT::NumericalPoint infBounds_;
  OT::NumericalPoint supBounds_;
  OT::Indices nbValues_;
};
}
#endif