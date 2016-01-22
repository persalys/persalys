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

  explicit PhysicalModel(const std::string & name);
  PhysicalModel(const std::string & name, const InputCollection & inputs, const OutputCollection & outputs);
  PhysicalModel(const PhysicalModelImplementation & implementation);
  PhysicalModel(const Implementation & p_implementation);
  PhysicalModel(PhysicalModelImplementation * p_implementation);
  PhysicalModel(const PhysicalModel & other);
  virtual PhysicalModel * clone() const;

  void addObserver(Observer * observer);
  void blockNotification(bool block, const std::string & notBlockedMessage="");

  std::string getName() const;
  void setName(const std::string & name);

  InputCollection getInputs() const;
  Input & getInputByName(const std::string & inputName);
  void setInputs(const InputCollection & inputs);
  void setInputDescription(const std::string & inputName, const std::string & description);
  void setInputValue(const std::string & inputName, const double & value);
  void setInputDistribution(const std::string & inputName, const OT::Distribution & distribution);
  void addInput(const Input & input);
  void removeInput(const std::string & inputName);
  OT::Description getInputNames();
  OT::Description getStochasticInputNames();
  bool hasInputNamed(const std::string & inputName);
  bool hasStochasticInputs();

  OutputCollection getOutputs() const;
  Output & getOutputByName(const std::string & outputName);
  void setOutputs(const OutputCollection & outputs);
  void setOutputDescription(const std::string & outputName, const std::string & description);
  void setOutputFormula(const std::string & outputName, const std::string & formula);
  void setOutputValue(const std::string & outputName, const double & value);
  void addOutput(const Output & output);
  void removeOutput(const std::string & outputName);
  OT::Description getOutputNames();
  bool hasOutputNamed(const std::string & outputName);

  OT::ComposedDistribution getComposedDistribution() const;
  OT::RandomVector getInputRandomVector();
  OT::RandomVector getOutputRandomVector(const OT::Description & outputNames);

  OT::NumericalMathFunction getFunction(const OT::Description & outputNames);
  OT::NumericalMathFunction getFunction();

  OT::Copula getCopula() const;
  void setCopula(const OT::Copula & copula);

  std::string dump() const;
};
}
#endif