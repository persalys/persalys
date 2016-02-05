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

  OT::Description getFormulas();
  virtual OT::NumericalMathFunction getFunction();

  OT::String dump() const;
};
}
#endif