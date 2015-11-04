// SobolCalculus.hxx

#ifndef SOBOLCALCULUS_H
#define SOBOLCALCULUS_H

#include "Calculus.hxx"
#include "SobolCalculusResult.hxx"

namespace OTGUI {
class SobolCalculus : public CalculusImplementation
{
  CLASSNAME;

public:
  SobolCalculus(const std::string & name, const PhysicalModel & physicalModel, int nbSimu=10000);

  SobolCalculus(const SobolCalculus & other);
  virtual SobolCalculus * clone() const;

  OutputCollection getOutputs() const;
  void setOutputs(const OutputCollection & outputs);

  int getNbSimulations() const;
  void setNbSimulations(const int nbSimu);

  SobolCalculusResult getResult() const;

  virtual void run();
  virtual std::string dump() const;
  virtual bool calculusLaunched() const;

private:
  OutputCollection outputs_;
  int nbSimulations_;
  SobolCalculusResult result_;
};
}
#endif