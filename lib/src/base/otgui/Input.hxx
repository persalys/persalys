// Input.hxx

#ifndef INPUT_H
#define INPUT_H

#include "Variable.hxx"

#include "Distribution.hxx"
#include "DiracFactory.hxx"

namespace OTGUI {
class Input : public Variable
{
  CLASSNAME;

public:
  Input(const OT::String & name="", const double & value=0., const OT::String & description="",
        const OT::Distribution & distribution=OT::Dirac());

  virtual Input * clone() const;

  virtual OT::String __repr__() const;

  OT::Distribution getDistribution() const;
  void setDistribution(const OT::Distribution & distribution);

  bool isStochastic() const;

  OT::String dumpDistribution() const;
  OT::String dump() const;

private:
  OT::Distribution distribution_;
};
typedef OT::Collection<Input> InputCollection;
}
#endif