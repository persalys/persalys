// SRCCalculus.hxx

#ifndef SRCCALCULUS_H
#define SRCCALCULUS_H

#include "SimulationCalculus.hxx"
#include "SRCCalculusResult.hxx"

namespace OTGUI {
class SRCCalculus : public SimulationCalculus
{
  CLASSNAME;

public:
  SRCCalculus(const std::string & name, const PhysicalModel & physicalModel, int nbSimu=10000);

  SRCCalculus(const SRCCalculus & other);
  virtual SRCCalculus * clone() const;

  SRCCalculusResult getResult() const;

  virtual void run();
  virtual std::string dump() const;
  virtual bool calculusLaunched() const;

private:
  SRCCalculusResult result_;
};
}
#endif