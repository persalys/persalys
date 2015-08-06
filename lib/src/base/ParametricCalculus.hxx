// ParametricCalculus.hxx

#ifndef PARAMETRICCALCULUS_H
#define PARAMETRICCALCULUS_H

#include "Calculus.hxx"
#include "PhysicalModel.hxx"
#include "NumericalSample.hxx"
#include "NumericalPoint.hxx"

namespace OTGUI {
class ParametricCalculus : public Calculus
{
public:
  ParametricCalculus(std::string name, const PhysicalModel & physicalModel);
  ParametricCalculus(std::string name, const PhysicalModel & physicalModel,
                     const OT::NumericalPoint & infBounds, const OT::NumericalPoint & supBounds,
                     const OT::Indices & nbValues);
  ParametricCalculus(const ParametricCalculus & other);
  virtual ParametricCalculus * clone() const;
  virtual ~ParametricCalculus();

  void computeParameters(InputCollection inputs);
  void computeInputSample();

  OT::NumericalSample getInputSample() const;
  void setInputSample(OT::NumericalSample inputSample);
  OT::NumericalSample getResultSample() const;

  OT::NumericalPoint getInfBounds() const;
  void setInfBound(int index, double infBounds);
  OT::NumericalPoint getSupBounds() const;
  void setSupBound(int index, double supBounds);
  OT::Indices getNbValues() const;
  void setNbValues(int index, int nbValues);

  void run();
  std::string dump() const;

private:
  OT::NumericalSample inputSample_;
  OT::NumericalSample resultSample_;
  OT::NumericalPoint infBounds_;
  OT::NumericalPoint supBounds_;
  OT::Indices nbValues_;
};
}
#endif