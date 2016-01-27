// AnalyticalPhysicalModel.hxx

#ifndef ANALYTICALPHYSICALMODEL_H
#define ANALYTICALPHYSICALMODEL_H

#include "PhysicalModelImplementation.hxx"

namespace OTGUI {
class AnalyticalPhysicalModel : public PhysicalModelImplementation
{
  CLASSNAME;

public:
  AnalyticalPhysicalModel(const OT::String & name);
  AnalyticalPhysicalModel(const OT::String & name, const InputCollection & inputs,
                          const OutputCollection & outputs);
  AnalyticalPhysicalModel(const AnalyticalPhysicalModel & other);
  virtual AnalyticalPhysicalModel * clone() const;

  void setInputs(const InputCollection & inputs);
  void addInput(const Input & input);
  void removeInput(const OT::String & inputName);

  void setOutputs(const OutputCollection & outputs);
  void setOutputFormula(const OT::String & outputName, const OT::String & formula);
  void addOutput(const Output & output);
  void removeOutput(const OT::String & outputName);

  OT::Description getFormulas();
  virtual OT::NumericalMathFunction getFunction();

  OT::String dump() const;

protected:
  void updateFunction();

private:
  OT::NumericalMathFunction function_;
  OT::Description formulas_;
};
}
#endif