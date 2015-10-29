// PhysicalModelImplementation.hxx

#ifndef PHYSICALMODELIMPLEMENTATION_H
#define PHYSICALMODELIMPLEMENTATION_H

#include "Input.hxx"
#include "Output.hxx"
#include "Observable.hxx"

#include "NumericalMathFunction.hxx"
#include "RandomVector.hxx"
#include "ComposedDistribution.hxx"

#include <QList>
#include <vector>

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
  bool updateInputs(const InputCollection & inputs);
  void addInput(Input input);

  OutputCollection getOutputs() const;
  void setOutputs(const OutputCollection & outputs);
  bool updateOutputs(const OutputCollection& outputs);
  void addOutput(Output output);

  OT::ComposedDistribution getComposedDistribution() const;
  OT::RandomVector getInputRandomVector();
  OT::RandomVector getOutputRandomVector(const OutputCollection & outputs);

  OT::NumericalMathFunction getFunction(const OutputCollection & outputs) const;
  OT::NumericalMathFunction getFunction() const;
  void setFunction(const OT::NumericalMathFunction & function);

  virtual bool checkInputs();
  virtual bool checkOutputs();

  std::string dump() const;

private:
  std::string name_;
  InputCollection inputs_;
  OutputCollection outputs_;
  OT::NumericalMathFunction function_;
  OT::Description listInput_;
  OT::Description listOutput_;
  OT::Description listFormula_;
};
}
#endif