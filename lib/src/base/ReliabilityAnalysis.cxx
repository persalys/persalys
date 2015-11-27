// ReliabilityAnalysis.cxx

#include "otgui/ReliabilityAnalysis.hxx"

using namespace OT;

namespace OTGUI {

ReliabilityAnalysis::ReliabilityAnalysis(const std::string & name, const LimitState & limitState)
 : AnalysisImplementation(name, limitState.getPhysicalModel())
 , limitState_(limitState)
{
  setIsReliabilityAnalysis(true);
}


ReliabilityAnalysis::ReliabilityAnalysis(const ReliabilityAnalysis & other)
 : AnalysisImplementation(other)
 , limitState_(other.limitState_)
{
  setIsReliabilityAnalysis(true);
}


ReliabilityAnalysis* ReliabilityAnalysis::clone() const
{
  return new ReliabilityAnalysis(*this);
}


ReliabilityAnalysis::~ReliabilityAnalysis()
{

}


LimitState ReliabilityAnalysis::getLimitState() const
{
  return limitState_;
}
}