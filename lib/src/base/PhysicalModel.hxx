// PhysicalModel.hxx

#ifndef PHYSICALMODEL_H
#define PHYSICALMODEL_H

#include "PhysicalModelImplementation.hxx"

namespace OTGUI {
class PhysicalModel
{
public:
  explicit PhysicalModel(const std::string & name);
  PhysicalModel(const std::string & name, const InputCollection & inputs, const OutputCollection & outputs);
  PhysicalModel(const PhysicalModel & other);
  virtual PhysicalModel * clone() const;

  virtual ~PhysicalModel();

  void addObserver(Observer * observer);

  std::string getName() const;
  void setName(const std::string & name);

  InputCollection getInputs() const;
  void setInputs(const InputCollection & inputs);
  void addInput(Input input);

  OutputCollection getOutputs() const;
  void setOutputs(const OutputCollection & outputs);
  void addOutput(Output output);

  OT::NumericalMathFunction getFunction() const;
  void setFunction(const OT::NumericalMathFunction & function);

  bool checkInputs();
  bool checkOutputs();

  void loadDataWithYACS(const std::string & fileName);
  std::string dump() const;

private:
  PhysicalModelImplementation * p_implementation_;
};
}
#endif