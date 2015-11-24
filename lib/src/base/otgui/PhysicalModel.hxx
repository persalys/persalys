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

  std::string getName() const;
  void setName(const std::string & name);

  InputCollection getInputs() const;
  void setInputs(const InputCollection & inputs);
  bool updateInput(int row, const Input & input, bool updateProbaModel=true);
  void addInput(Input input);
  void newInput(const Input & input);
  void removeInput(int row);
  bool checkInputs();
  OT::Description getInputNames() const;
  bool hasStochasticInputs();

  OutputCollection getOutputs() const;
  Output getOutputByName(const std::string & outputName) const;
  void setOutputs(const OutputCollection & outputs);
  bool updateOutput(int row, const Output & output);
  void addOutput(Output output);
  void newOutput(const Output & output);
  void removeOutput(int row);
  bool checkOutputs();

  OT::ComposedDistribution getComposedDistribution() const;
  OT::RandomVector getInputRandomVector();
  OT::RandomVector getOutputRandomVector(const OutputCollection & outputs);

  OT::NumericalMathFunction getFunction(const OutputCollection & outputs) const;
  OT::NumericalMathFunction getFunction() const;
  void setFunction(const OT::NumericalMathFunction & function);

  std::string dump() const;

};
}
#endif