// ParametricCalculus.hxx

#ifndef PARAMETRICCALCULUS_H
#define PARAMETRICCALCULUS_H

#include "Calculus.hxx"
#include "PhysicalModel.hxx"
#include "NumericalSample.hxx"
#include "NumericalPoint.hxx"

namespace OTGUI {
class ParametricCalculus : public CalculusImplementation
{
  CLASSNAME;

public:
  ParametricCalculus(const std::string & name, const PhysicalModel & physicalModel);
  ParametricCalculus(const std::string & name, const PhysicalModel & physicalModel,
                     const OT::NumericalPoint & infBounds, const OT::NumericalPoint & supBounds,
                     const OT::Indices & nbValues);
  ParametricCalculus(const ParametricCalculus & other);
  virtual ParametricCalculus * clone() const;

  virtual ~ParametricCalculus();

  void computeParameters(const InputCollection & inputs);
  void computeInputSample();

  OT::NumericalSample getInputSample() const;
  void setInputSample(const OT::NumericalSample & inputSample);
  OT::NumericalSample getResultSample() const;

  OT::NumericalPoint getInfBounds() const;
  void setInfBound(const int & index, const double & infBounds);
  OT::NumericalPoint getSupBounds() const;
  void setSupBound(const int & index, const double & supBounds);
  OT::Indices getNbValues() const;
  void setNbValues(const int & index, const int & nbValues);

  virtual void run();
  virtual std::string dump() const;
  virtual bool calculusLaunched() const;

private:
  OT::NumericalSample inputSample_;
  OT::NumericalSample resultSample_;
  OT::NumericalPoint infBounds_;
  OT::NumericalPoint supBounds_;
  OT::Indices nbValues_;
};
}
#endif