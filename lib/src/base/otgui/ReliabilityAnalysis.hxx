// ReliabilityAnalysis.hxx

#ifndef RELIABILITYANALYSIS_H
#define RELIABILITYANALYSIS_H

#include "AnalysisImplementation.hxx"
#include "LimitState.hxx"

namespace OTGUI {
class ReliabilityAnalysis : public AnalysisImplementation
{
public:
  ReliabilityAnalysis(const std::string & name, const LimitState & limitState);
  ReliabilityAnalysis(const ReliabilityAnalysis & other);
  virtual ReliabilityAnalysis * clone() const;

  virtual ~ReliabilityAnalysis();

  LimitState getLimitState() const;

private:
  LimitState limitState_;
};
}
#endif