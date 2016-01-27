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
  YACSPhysicalModel(const OT::String & name, const OT::String & fileName);

  virtual YACSPhysicalModel * clone() const;

  void setInputs(const InputCollection & inputs);
  void addInput(const Input & input);
  void removeInput(const OT::String & inputName);

  void setOutputs(const OutputCollection & outputs);
  void addOutput(const Output & output);
  void removeOutput(const OT::String & outputName);

  OT::String getXMLFileName() const;
  void setXMLFileName(const OT::String & fileName);

  OT::NumericalMathFunction getFunction(const OT::Description & outputNames);
  OT::NumericalMathFunction getFunction();

  OT::String dump() const;

protected:
  void updateData();

private:
  YACSEvaluation evaluation_;
};
}
#endif