// YACSPhysicalModel.hxx

#ifndef YACSPHYSICALMODEL_H
#define YACSPHYSICALMODEL_H

#include "PhysicalModelImplementation.hxx"
#include "YACSEvaluation.hxx"

#ifdef OTGUI_HAVE_YACS
#include "YACSEvalSession.hxx"
#endif

namespace OTGUI {
class YACSPhysicalModel : public PhysicalModelImplementation
{
  CLASSNAME;

public:
  YACSPhysicalModel(const std::string & name, const std::string & fileName);

  virtual YACSPhysicalModel * clone() const;

  void setInputs(const InputCollection & inputs);
  void addInput(const Input & input);
  void removeInput(const std::string & inputName);

  void setOutputs(const OutputCollection & outputs);
  void addOutput(const Output & output);
  void removeOutput(const std::string & outputName);

  std::string getXMLFileName() const;
  void setXMLFileName(const std::string & fileName);

  OT::NumericalMathFunction getFunction(const OT::Description & outputNames);
  OT::NumericalMathFunction getFunction();

  std::string dump() const;

protected:
  void updateData();

private:
  YACSEvaluation evaluation_;
};
}
#endif