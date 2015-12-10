// AnalyticalPhysicalModel.hxx

#ifndef ANALYTICALPHYSICALMODEL_H
#define ANALYTICALPHYSICALMODEL_H

#include "PhysicalModelImplementation.hxx"

namespace OTGUI {
class AnalyticalPhysicalModel : public PhysicalModelImplementation
{
  CLASSNAME;

public:
  AnalyticalPhysicalModel(const std::string & name);
  AnalyticalPhysicalModel(const std::string & name, const InputCollection & inputs,
                          const OutputCollection & outputs);
  AnalyticalPhysicalModel(const AnalyticalPhysicalModel & other);
  virtual AnalyticalPhysicalModel * clone() const;

  void setInputs(const InputCollection & inputs);
  void addInput(const Input & input);
  void removeInput(const std::string & inputName);

  void setOutputs(const OutputCollection & outputs);
  void addOutput(const Output & output);

  OT::Description getFormulas();
  virtual OT::NumericalMathFunction getFunction(const OT::Description & outputNames);
  virtual OT::NumericalMathFunction getFunction();

  std::string dump() const;

protected:
  void updateFunction();

private:
  OT::NumericalMathFunction function_;
  OT::Description formulas_;
};
}
#endif