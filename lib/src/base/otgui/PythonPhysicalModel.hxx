// PythonPhysicalModel.hxx

#ifndef OTGUI_PYTHONPHYSICALMODEL_H
#define OTGUI_PYTHONPHYSICALMODEL_H

#include "PhysicalModelImplementation.hxx"

namespace OTGUI {
class PythonPhysicalModel : public PhysicalModelImplementation
{
  CLASSNAME;

public:
  PythonPhysicalModel(const std::string & name);

  PythonPhysicalModel(const std::string & name,
                      const InputCollection & inputs,
                      const OutputCollection & outputs,
                      const std::string & code);

  virtual PythonPhysicalModel * clone() const;

  void setCode(const OT::String & code);
  OT::String getCode() const;
  virtual OT::NumericalMathFunction getFunction(const OT::Description & outputNames);
  virtual OT::NumericalMathFunction getFunction();

  std::string dump() const;

protected:
  void updateFunction();

private:
  mutable OT::NumericalMathFunction function_;
  OT::String code_;
};

}
#endif