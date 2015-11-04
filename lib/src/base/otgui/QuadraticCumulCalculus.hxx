// QuadraticCumulCalculus.hxx

#ifndef QUADRATICCUMULCALCULUS_H
#define QUADRATICCUMULCALCULUS_H

#include "Calculus.hxx"
#include "QuadraticCumulResult.hxx"

namespace OTGUI {
class QuadraticCumulCalculus : public CalculusImplementation
{
  CLASSNAME;

public:
  QuadraticCumulCalculus(const std::string & name, const PhysicalModel & physicalModel);

  QuadraticCumulCalculus(const QuadraticCumulCalculus & other);
  virtual QuadraticCumulCalculus * clone() const;

  OutputCollection getOutputs() const;
  void setOutputs(const OutputCollection & outputs);

  QuadraticCumulResult getResult() const;

  virtual void run();
  virtual std::string dump() const;
  virtual bool calculusLaunched() const;

private:
  OutputCollection outputs_;
  QuadraticCumulResult result_;
};
}
#endif