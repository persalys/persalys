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
  explicit PhysicalModelImplementation(const OT::String & name);
  PhysicalModelImplementation(const OT::String & name, const InputCollection & inputs,
                              const OutputCollection & outputs);

  virtual PhysicalModelImplementation * clone() const;

  InputCollection getInputs() const;
  Input & getInputByName(const OT::String & inputName);
  virtual void setInputs(const InputCollection & inputs);
  void setInputDescription(const OT::String & inputName, const OT::String & description);
  void setInputValue(const OT::String & inputName, const double & value);
  void setInputDistribution(const OT::String & inputName, const OT::Distribution & distribution);
  virtual void addInput(const Input & input);
  virtual void removeInput(const OT::String & inputName);
  OT::Description getInputNames();
  OT::Description getStochasticInputNames();
  bool hasInputNamed(const OT::String & inputName);
  bool hasStochasticInputs();

  OutputCollection getOutputs() const;
  Output & getOutputByName(const OT::String & outputName);
  virtual void setOutputs(const OutputCollection & outputs);
  void setOutputDescription(const OT::String & outputName, const OT::String & description);
  virtual void setOutputFormula(const OT::String & outputName, const OT::String & formula);
  void setOutputValue(const OT::String & outputName, const double & value);
  virtual void addOutput(const Output & output);
  virtual void removeOutput(const OT::String & outputName);
  OT::Description getOutputNames();
  bool hasOutputNamed(const OT::String & outputName);

  OT::ComposedDistribution getComposedDistribution() const;
  OT::RandomVector getInputRandomVector();
  OT::RandomVector getOutputRandomVector(const OT::Description & outputNames);

  virtual OT::NumericalMathFunction getFunction();
  virtual OT::NumericalMathFunction getFunction(const OT::Description & outputNames);
  OT::NumericalMathFunction getFunction(const OT::String & outputName);
  OT::NumericalMathFunction getRestrictedFunction();
  OT::NumericalMathFunction getRestrictedFunction(const OT::Description & outputNames);

  OT::Copula getCopula() const;
  void setCopula(const OT::Copula & copula);

  virtual OT::String dump() const;

protected:
  void updateCopula();
  OT::String dumpProbaModel() const;
  OT::String dumpCopula() const;

private:
  InputCollection inputs_;
  OutputCollection outputs_;
  OT::Copula copula_;
};
}
#endif