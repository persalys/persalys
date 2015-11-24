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
  void setInputs(const InputCollection & inputs);
  bool updateInput(int row, const Input & input, bool updateProbaModel=true);
  void addInput(Input input);
  void newInput(const Input & input);
  void removeInput(int row);
  virtual bool checkInputs();
  OT::Description getInputNames() const;
  bool hasStochasticInputs();

  OutputCollection getOutputs() const;
  Output getOutputByName(const std::string & outputName) const;
  void setOutputs(const OutputCollection & outputs);
  bool updateOutput(int row, const Output & output);
  void addOutput(Output output);
  void newOutput(const Output & output);
  void removeOutput(int row);
  virtual bool checkOutputs();

  OT::ComposedDistribution getComposedDistribution() const;
  OT::RandomVector getInputRandomVector();
  OT::RandomVector getOutputRandomVector(const OutputCollection & outputs);

  OT::NumericalMathFunction getFunction(const OutputCollection & outputs) const;
  OT::NumericalMathFunction getFunction() const;
  void setFunction(const OT::NumericalMathFunction & function);

  std::string dump() const;

private:
  std::string name_;
  InputCollection inputs_;
  OutputCollection outputs_;
  OT::NumericalMathFunction function_;
  OT::Description inputNames_;
  OT::Description outputNames_;
  OT::Description listFormula_;
};
}
#endif