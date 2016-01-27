// Analysis.hxx

#ifndef ANALYSIS_H
#define ANALYSIS_H

#include "AnalysisImplementation.hxx"

namespace OTGUI {
class Analysis : public OT::TypedInterfaceObject<AnalysisImplementation>
{
  CLASSNAME;

public:
  typedef OT::Pointer<AnalysisImplementation>       Implementation;

  Analysis(const OT::String & name, const PhysicalModel & physicalModel);
  Analysis(const AnalysisImplementation & implementation);
  Analysis(const Implementation & p_implementation);
  Analysis(AnalysisImplementation * p_implementation);

  void addObserver(Observer * observer);

  PhysicalModel getPhysicalModel() const;
  void setPhysicalModel(const PhysicalModel & physicalModel);

  bool isReliabilityAnalysis() const;

  void run();
  OT::String dump() const;
  bool analysisLaunched() const;
};
}
#endif