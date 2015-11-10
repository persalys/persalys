// SobolAnalysis.hxx

#ifndef SOBOLANALYSIS_H
#define SOBOLANALYSIS_H

#include "SimulationAnalysis.hxx"
#include "SobolResult.hxx"

namespace OTGUI {
class SobolAnalysis : public SimulationAnalysis
{
  CLASSNAME;

public:
  SobolAnalysis(const std::string & name, const PhysicalModel & physicalModel, int nbSimu=10000);

  SobolAnalysis(const SobolAnalysis & other);
  virtual SobolAnalysis * clone() const;

  SobolResult getResult() const;

  virtual void run();
  virtual std::string dump() const;
  virtual bool analysisLaunched() const;

private:
  SobolResult result_;
};
}
#endif