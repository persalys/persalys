// PhysicalModelImplementation.hxx

#ifndef PHYSICALMODELIMPLEMENTATION_H
#define PHYSICALMODELIMPLEMENTATION_H

#include "Input.hxx"
#include "Output.hxx"
#include "Observable.hxx"

#include "NumericalMathFunction.hxx"
#include "RandomVector.hxx"
#include "ComposedDistribution.hxx"

namespace OTGUI {
class PhysicalModelImplementation : public OT::PersistentObject, public Observable
{
public:
  explicit PhysicalModelImplementation(const std::string & name);
  PhysicalModelImplementation(const std::string & name, const InputCollection & inputs,
                              const OutputCollection & outputs);
  PhysicalModelImplementation(const PhysicalModelImplementation & other);
  PhysicalModelImplementation(const PhysicalModelImplementation * other);

  virtual PhysicalModelImplementation * clone() const;

  virtual ~PhysicalModelImplementation();

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
  OT::Description getFormulas();
  bool hasAnOutputNamed(const std::string & outputName);

  OT::ComposedDistribution getComposedDistribution() const;
  OT::RandomVector getInputRandomVector();
  OT::RandomVector getOutputRandomVector(const OutputCollection & outputs);

  OT::NumericalMathFunction getFunction(const OutputCollection & outputs);
  OT::NumericalMathFunction getFunction();

  std::string dump() const;

private:
  std::string name_;
  InputCollection inputs_;
  OutputCollection outputs_;
  OT::NumericalMathFunction function_;
  OT::Description inputNames_;
  OT::Description outputNames_;
};
}
#endif