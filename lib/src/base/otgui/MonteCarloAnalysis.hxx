// MonteCarloAnalysis.hxx

#ifndef MONTECARLOANALYSIS_H
#define MONTECARLOANALYSIS_H

#include "SimulationAnalysis.hxx"
#include "MonteCarloResult.hxx"

namespace OTGUI {
class MonteCarloAnalysis : public SimulationAnalysis
{
  CLASSNAME;

public:
  MonteCarloAnalysis(const std::string & name, const PhysicalModel & physicalModel,
                     int nbSimu=10000, bool confidenceInterval=true, double level=0.95);

  MonteCarloAnalysis(const MonteCarloAnalysis & other);
  virtual MonteCarloAnalysis * clone() const;

  bool isConfidenceIntervalRequired() const;
  void setIsConfidenceIntervalRequired(const bool isConfidenceIntervalRequired);

  double getLevelConfidenceInterval() const;
  void setLevelConfidenceInterval(const double levelConfidenceInterval);

  MonteCarloResult getResult() const;

  virtual void run();
  virtual std::string dump() const;
  virtual bool analysisLaunched() const;

private:
  bool isConfidenceIntervalRequired_;
  double levelConfidenceInterval_;
  MonteCarloResult result_;
};
}
#endif