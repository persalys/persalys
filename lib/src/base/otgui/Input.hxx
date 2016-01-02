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
  Input(const std::string & name="", const double & value=0., const std::string & description="",
        const OT::Distribution & distribution=OT::Dirac());
  Input(const Input & other);
  virtual Input * clone() const;

  virtual OT::String __repr__() const;

  virtual ~Input();

  OT::Distribution getDistribution() const;
  void setDistribution(const OT::Distribution & distribution);

  bool isStochastic() const;

  std::string dump() const;

private:
  OT::Distribution distribution_;
};
typedef OT::Collection<Input> InputCollection;
}
#endif