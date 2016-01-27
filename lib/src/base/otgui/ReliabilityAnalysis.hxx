// ReliabilityAnalysis.hxx

#ifndef RELIABILITYANALYSIS_H
#define RELIABILITYANALYSIS_H

#include "AnalysisImplementation.hxx"
#include "LimitState.hxx"

namespace OTGUI {
class ReliabilityAnalysis : public AnalysisImplementation
{
public:
  ReliabilityAnalysis(const OT::String & name, const LimitState & limitState);

  virtual ReliabilityAnalysis * clone() const;

  LimitState getLimitState() const;

private:
  LimitState limitState_;
};
}
#endif