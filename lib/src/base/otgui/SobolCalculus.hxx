// SobolCalculus.hxx

#ifndef SOBOLCALCULUS_H
#define SOBOLCALCULUS_H

#include "SimulationCalculus.hxx"
#include "SobolCalculusResult.hxx"

namespace OTGUI {
class SobolCalculus : public SimulationCalculus
{
  CLASSNAME;

public:
  SobolCalculus(const std::string & name, const PhysicalModel & physicalModel, int nbSimu=10000);

  SobolCalculus(const SobolCalculus & other);
  virtual SobolCalculus * clone() const;

  SobolCalculusResult getResult() const;

  virtual void run();
  virtual std::string dump() const;
  virtual bool calculusLaunched() const;

private:
  SobolCalculusResult result_;
};
}
#endif