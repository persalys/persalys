// SRCAnalysis.hxx

#ifndef SRCANALYSIS_H
#define SRCANALYSIS_H

#include "SimulationAnalysis.hxx"
#include "SRCResult.hxx"

namespace OTGUI {
class SRCAnalysis : public SimulationAnalysis
{
  CLASSNAME;

public:
  SRCAnalysis(const std::string & name, const PhysicalModel & physicalModel, int nbSimu=10000);

  SRCAnalysis(const SRCAnalysis & other);
  virtual SRCAnalysis * clone() const;

  SRCResult getResult() const;

  virtual void run();
  virtual std::string dump() const;
  virtual bool analysisLaunched() const;

private:
  SRCResult result_;
};
}
#endif