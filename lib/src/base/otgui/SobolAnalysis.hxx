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
  SobolAnalysis(const OT::String & name, const PhysicalModel & physicalModel, const OT::UnsignedInteger nbSimu=10000);

  virtual SobolAnalysis * clone() const;

  SobolResult getResult() const;

  virtual void run();
  virtual OT::String dump() const;
  virtual bool analysisLaunched() const;

private:
  SobolResult result_;
};
}
#endif