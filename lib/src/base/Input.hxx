// Input.hxx

#ifndef INPUT_H
#define INPUT_H

#include "Variable.hxx"
#include "Distribution.hxx"
#include "DiracFactory.hxx"

namespace OTGUI {
class Input : public Variable
{
public:
  Input();
  Input(const std::string & name, const double & value, const std::string & description="",
        const OT::Distribution & distribution=OT::Dirac());
  Input(const Input & other);
  virtual Input * clone() const;

  virtual ~Input();

  OT::Distribution getDistribution() const;
  void setDistribution(const OT::Distribution & distribution);

  std::string dump() const;

private:
  OT::Distribution distribution_;
};
typedef OT::Collection<Input> InputCollection;
}
#endif