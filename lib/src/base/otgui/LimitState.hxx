// LimitState.hxx

#ifndef LIMITSTATE_H
#define LIMITSTATE_H

#include "LimitStateImplementation.hxx"

#include <Less.hxx>
#include <Event.hxx>

namespace OTGUI {
class LimitState : public OT::TypedInterfaceObject<LimitStateImplementation>
{
  CLASSNAME;

public:
  typedef OT::Pointer<LimitStateImplementation>       Implementation;

  LimitState(const std::string & name, const PhysicalModel & physicalModel,
             const std::string & outputName,
             const OT::ComparisonOperator & failure=OT::Less(),
             const double & threshold=0.);
  LimitState(const LimitStateImplementation & implementation);
  LimitState(const Implementation & p_implementation);
  LimitState(LimitStateImplementation * p_implementation); 

  virtual LimitState * clone() const;

  void addObserver(Observer * observer);
  void blockNotification(bool block, const std::string & notBlockedMessage="");

  PhysicalModel getPhysicalModel() const;
  void setPhysicalModel(const PhysicalModel & physicalModel);

  std::string getOutputName() const;
  void setOutputName(const std::string & outputName);

  OT::ComparisonOperator getOperator() const;
  void setOperator(const OT::ComparisonOperator & comparisonOperator);

  double getThreshold() const;
  void setThreshold(const double & threshold);

  OT::Event getEvent();

  std::string dump() const;
};
}
#endif