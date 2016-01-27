// AnalysisImplementation.cxx

#include "otgui/AnalysisImplementation.hxx"

using namespace OT;

namespace OTGUI {

AnalysisImplementation::AnalysisImplementation(const String & name, const PhysicalModel & physicalModel)
  : PersistentObject()
  , Observable()
  , physicalModel_(physicalModel)
  , isReliabilityAnalysis_(false)
{
  setName(name);
}


AnalysisImplementation* AnalysisImplementation::clone() const
{
  return new AnalysisImplementation(*this);
}


PhysicalModel AnalysisImplementation::getPhysicalModel() const
{
  return physicalModel_;
}


void AnalysisImplementation::setPhysicalModel(const PhysicalModel & physicalModel)
{
  physicalModel_ = physicalModel;
}


bool AnalysisImplementation::isReliabilityAnalysis() const
{
  return isReliabilityAnalysis_;
}


void AnalysisImplementation::setIsReliabilityAnalysis(bool isReliabilityAnalysis)
{
  isReliabilityAnalysis_ = isReliabilityAnalysis;
}


void AnalysisImplementation::run()
{

}


bool AnalysisImplementation::analysisLaunched() const
{
  return false;
}


String AnalysisImplementation::dump() const
{
  return "";
}
}