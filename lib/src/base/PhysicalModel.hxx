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
  bool updateInputs(const InputCollection & inputs);
  void addInput(Input input);

  OutputCollection getOutputs() const;
  void setOutputs(const OutputCollection & outputs);
  bool updateOutputs(const OutputCollection& outputs);
  void addOutput(Output output);

  OT::RandomVector getInputRandomVector();
  OT::RandomVector getOutputRandomVector(const OutputCollection & outputs);

  OT::NumericalMathFunction getFunction(const OutputCollection & outputs) const;
  OT::NumericalMathFunction getFunction() const;
  void setFunction(const OT::NumericalMathFunction & function);

  bool checkInputs();
  bool checkOutputs();

  std::string dump() const;

};
}
#endif