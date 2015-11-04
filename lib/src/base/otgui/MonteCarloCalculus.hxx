// MonteCarloCalculus.hxx

#ifndef MONTECARLOCALCULUS_H
#define MONTECARLOCALCULUS_H

#include "SimulationCalculus.hxx"
#include "MonteCarloResult.hxx"

namespace OTGUI {
class MonteCarloCalculus : public SimulationCalculus
{
  CLASSNAME;

public:
  MonteCarloCalculus(const std::string & name, const PhysicalModel & physicalModel,
                     int nbSimu=10000, bool confidenceInterval=true, double level=0.95);

  MonteCarloCalculus(const MonteCarloCalculus & other);
  virtual MonteCarloCalculus * clone() const;

  bool isConfidenceIntervalRequired() const;
  void setIsConfidenceIntervalRequired(const bool isConfidenceIntervalRequired);

  double getLevelConfidenceInterval() const;
  void setLevelConfidenceInterval(const double levelConfidenceInterval);

  MonteCarloResult getResult() const;

  virtual void run();
  virtual std::string dump() const;
  virtual bool calculusLaunched() const;

private:
  bool isConfidenceIntervalRequired_;
  double levelConfidenceInterval_;
  MonteCarloResult result_;
};
}
#endif