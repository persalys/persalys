// TaylorExpansionsMomentsAnalysis.hxx

#ifndef OTGUI_TAYLOREXPANSIONSMOMENTSANALYSIS_H
#define OTGUI_TAYLOREXPANSIONSMOMENTSANALYSIS_H

#include "Analysis.hxx"
#include "TaylorExpansionsMomentsResult.hxx"

namespace OTGUI {
class TaylorExpansionsMomentsAnalysis : public AnalysisImplementation
{
  CLASSNAME;

public:
  TaylorExpansionsMomentsAnalysis(const OT::String & name, const PhysicalModel & physicalModel);

  virtual TaylorExpansionsMomentsAnalysis * clone() const;

  OutputCollection getOutputs() const;
  void setOutputs(const OutputCollection & outputs);

  TaylorExpansionsMomentsResult getResult() const;

  virtual void run();
  virtual OT::String dump() const;
  virtual bool analysisLaunched() const;

private:
  OutputCollection outputs_;
  TaylorExpansionsMomentsResult result_;
};
}
#endif