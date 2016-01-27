// AnalysisImplementation.hxx

#ifndef ANALYSISIMPLEMENTATION_H
#define ANALYSISIMPLEMENTATION_H

#include "PhysicalModel.hxx"

namespace OTGUI {
class AnalysisImplementation : public OT::PersistentObject, public Observable
{
public:
  AnalysisImplementation(const OT::String & name, const PhysicalModel & physicalModel);

  virtual AnalysisImplementation * clone() const;

  PhysicalModel getPhysicalModel() const;
  void setPhysicalModel(const PhysicalModel & physicalModel);

  virtual void run();
  virtual OT::String dump() const;
  virtual bool analysisLaunched() const;

  bool isReliabilityAnalysis() const;

protected:
  void setIsReliabilityAnalysis(bool isReliabilityAnalysis);

private:
  PhysicalModel physicalModel_;
  bool isReliabilityAnalysis_;
};
}
#endif