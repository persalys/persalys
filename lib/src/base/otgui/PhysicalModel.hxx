// PhysicalModel.hxx

#ifndef PHYSICALMODEL_H
#define PHYSICALMODEL_H

#include "PhysicalModelImplementation.hxx"

namespace OTGUI {
class PhysicalModel : public OT::TypedInterfaceObject<PhysicalModelImplementation>
{
public:
  typedef OT::Pointer<PhysicalModelImplementation>       Implementation;

  explicit PhysicalModel(const std::string & name);
  PhysicalModel(const std::string & name, const InputCollection & inputs, const OutputCollection & outputs);
  PhysicalModel(const PhysicalModelImplementation & implementation);
  PhysicalModel(const Implementation & p_implementation);
  PhysicalModel(PhysicalModelImplementation * p_implementation);
  PhysicalModel(const PhysicalModel & other);
  virtual PhysicalModel * clone() const;

  virtual ~PhysicalModel();

  void addObserver(Observer * observer);
  void blockNotification(bool block, const std::string & notBlockedMessage="");

  std::string getName() const;
  void setName(const std::string & name);

  InputCollection getInputs() const;
  Input & getInputByName(const std::string & inputName);
  void setInputs(const InputCollection & inputs);
  void updateInputDescription(const std::string & inputName, const std::string & description);
  void updateInputValue(const std::string & inputName, const double & value);
  void updateInputDistribution(const std::string & inputName, const OT::Distribution & distribution);
  void addInput(const Input & input);
  void removeInput(const std::string & inputName);
  OT::Description getInputNames();
  bool hasAnInputNamed(const std::string & inputName);
  bool hasStochasticInputs();

  OutputCollection getOutputs() const;
  Output & getOutputByName(const std::string & outputName);
  void setOutputs(const OutputCollection & outputs);
  void updateOutputDescription(const std::string & outputName, const std::string & description);
  void updateOutputFormula(const std::string & outputName, const std::string & formula);
  void addOutput(const Output & output);
  void removeOutput(const std::string & outputName);
  void updateFunction();
  OT::Description getOutputNames();
  bool hasAnOutputNamed(const std::string & outputName);

  OT::ComposedDistribution getComposedDistribution() const;
  OT::RandomVector getInputRandomVector();
  OT::RandomVector getOutputRandomVector(const OutputCollection & outputs);

  OT::NumericalMathFunction getFunction(const OutputCollection & outputs);
  OT::NumericalMathFunction getFunction();

  std::string dump() const;

};
}
#endif