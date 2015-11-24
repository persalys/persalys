// LimitState.hxx

#ifndef LIMITSTATE_H
#define LIMITSTATE_H

#include "PhysicalModel.hxx"

#include <Less.hxx>
#include <Event.hxx>

namespace OTGUI {
class LimitState : public OT::PersistentObject
{
  CLASSNAME;

public:
  LimitState(const std::string & name, const PhysicalModel & physicalModel,
             const std::string & outputName,
             const OT::ComparisonOperator & comparisonOperator=OT::Less(),
             const double & threshold=0.);
  LimitState(const LimitState & other);
  virtual LimitState * clone() const;

  virtual ~LimitState();

  std::string getName() const;
  PhysicalModel getPhysicalModel() const;
  OT::Event getEvent();

  std::string dump() const;

private:
  std::string name_;
  PhysicalModel physicalModel_;
  Output output_;
  OT::ComparisonOperator comparisonOperator_;
  double threshold_;
};
}
#endif