// PhysicalModel.hxx

#ifndef PHYSICALMODEL_H
#define PHYSICALMODEL_H

#include "PhysicalModelImplementation.hxx"

namespace OTGUI {
class PhysicalModel : public OT::TypedInterfaceObject<PhysicalModelImplementation>
{
  CLASSNAME;

public:
  typedef OT::Pointer<PhysicalModelImplementation>       Implementation;

  explicit PhysicalModel(const OT::String & name);
  PhysicalModel(const OT::String & name, const InputCollection & inputs, const OutputCollection & outputs);
  PhysicalModel(const PhysicalModelImplementation & implementation);
  PhysicalModel(const Implementation & p_implementation);
  PhysicalModel(PhysicalModelImplementation * p_implementation);

  virtual PhysicalModel * clone() const;

  void addObserver(Observer * observer);
  void blockNotification(bool block, const OT::String & notBlockedMessage="");

  InputCollection getInputs() const;
  Input & getInputByName(const OT::String & inputName);
  void setInputs(const InputCollection & inputs);
  void setInputDescription(const OT::String & inputName, const OT::String & description);
  void setInputValue(const OT::String & inputName, const double & value);
  void setInputDistribution(const OT::String & inputName, const OT::Distribution & distribution);
  void addInput(const Input & input);
  void removeInput(const OT::String & inputName);
  OT::Description getInputNames();
  OT::Description getStochasticInputNames();
  bool hasInputNamed(const OT::String & inputName);
  bool hasStochasticInputs();

  OutputCollection getOutputs() const;
  Output & getOutputByName(const OT::String & outputName);
  void setOutputs(const OutputCollection & outputs);
  void setOutputDescription(const OT::String & outputName, const OT::String & description);
  void setOutputFormula(const OT::String & outputName, const OT::String & formula);
  void setOutputValue(const OT::String & outputName, const double & value);
  void addOutput(const Output & output);
  void removeOutput(const OT::String & outputName);
  OT::Description getOutputNames();
  bool hasOutputNamed(const OT::String & outputName);

  OT::ComposedDistribution getComposedDistribution() const;
  OT::RandomVector getInputRandomVector();
  OT::RandomVector getOutputRandomVector(const OT::Description & outputNames);

  OT::NumericalMathFunction getFunction();
  OT::NumericalMathFunction getFunction(const OT::Description & outputNames);
  OT::NumericalMathFunction getFunction(const OT::String & outputName);
  OT::NumericalMathFunction getRestrictedFunction();
  OT::NumericalMathFunction getRestrictedFunction(const OT::Description & outputNames);

  OT::Copula getCopula() const;
  void setCopula(const OT::Copula & copula);

  OT::String dump() const;
};
}
#endif