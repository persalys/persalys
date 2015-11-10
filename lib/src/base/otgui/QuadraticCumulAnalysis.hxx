// QuadraticCumulAnalysis.hxx

#ifndef QUADRATICCUMULANALYSIS_H
#define QUADRATICCUMULANALYSIS_H

#include "Analysis.hxx"
#include "QuadraticCumulResult.hxx"

namespace OTGUI {
class QuadraticCumulAnalysis : public AnalysisImplementation
{
  CLASSNAME;

public:
  QuadraticCumulAnalysis(const std::string & name, const PhysicalModel & physicalModel);

  QuadraticCumulAnalysis(const QuadraticCumulAnalysis & other);
  virtual QuadraticCumulAnalysis * clone() const;

  OutputCollection getOutputs() const;
  void setOutputs(const OutputCollection & outputs);

  QuadraticCumulResult getResult() const;

  virtual void run();
  virtual std::string dump() const;
  virtual bool analysisLaunched() const;

private:
  OutputCollection outputs_;
  QuadraticCumulResult result_;
};
}
#endif