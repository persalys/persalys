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
  QuadraticCumulAnalysis(const OT::String & name, const PhysicalModel & physicalModel);

  QuadraticCumulAnalysis(const QuadraticCumulAnalysis & other);
  virtual QuadraticCumulAnalysis * clone() const;

  OutputCollection getOutputs() const;
  void setOutputs(const OutputCollection & outputs);

  QuadraticCumulResult getResult() const;

  virtual void run();
  virtual OT::String dump() const;
  virtual bool analysisLaunched() const;

private:
  OutputCollection outputs_;
  QuadraticCumulResult result_;
};
}
#endif