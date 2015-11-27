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

  Analysis(const std::string & name, const PhysicalModel & physicalModel);
  Analysis(const AnalysisImplementation & implementation);
  Analysis(const Implementation & p_implementation);
  Analysis(AnalysisImplementation * p_implementation);
  Analysis(const Analysis & other);

  virtual ~Analysis();

  void addObserver(Observer * observer);

  std::string getName() const;
  void setName(const std::string & name);

  PhysicalModel getPhysicalModel() const;
  void setPhysicalModel(const PhysicalModel & physicalModel);

  bool isReliabilityAnalysis() const;

  void run();
  std::string dump() const;
  bool analysisLaunched() const;
};
}
#endif