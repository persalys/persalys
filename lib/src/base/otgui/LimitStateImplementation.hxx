// LimitStateImplementation.hxx

#ifndef LIMITSTATEIMPLEMENTATION_H
#define LIMITSTATEIMPLEMENTATION_H

#include "PhysicalModel.hxx"

#include "Event.hxx"

namespace OTGUI {
class LimitStateImplementation : public OT::PersistentObject, public Observable
{
public:
  typedef OT::Pointer<LimitStateImplementation>       Implementation;

  LimitStateImplementation(const OT::String & name, const PhysicalModel & physicalModel,
                           const OT::String & outputName,
                           const OT::ComparisonOperator & failure, const double & threshold);

  virtual LimitStateImplementation * clone() const;

  PhysicalModel getPhysicalModel() const;
  void setPhysicalModel(const PhysicalModel & physicalModel);

  OT::String getOutputName() const;
  void setOutputName(const OT::String & outputName);

  OT::ComparisonOperator getOperator() const;
  void setOperator(const OT::ComparisonOperator & comparisonOperator);

  double getThreshold() const;
  void setThreshold(const double & threshold);

  OT::Event getEvent();

  OT::String dump() const;

private:
  PhysicalModel physicalModel_;
  OT::String outputName_;
  OT::ComparisonOperator operator_;
  double threshold_;
};
}
#endif