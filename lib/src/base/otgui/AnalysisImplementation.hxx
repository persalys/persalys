// AnalysisImplementation.hxx

#ifndef ANALYSISIMPLEMENTATION_H
#define ANALYSISIMPLEMENTATION_H

#include "PhysicalModel.hxx"

namespace OTGUI {
class AnalysisImplementation : public OT::PersistentObject, public Observable
{
public:
  AnalysisImplementation(const std::string & name, const PhysicalModel & physicalModel);
  AnalysisImplementation(const AnalysisImplementation & other);
  virtual AnalysisImplementation * clone() const;

  virtual ~AnalysisImplementation();

  std::string getName() const;
  void setName(const std::string & name);

  PhysicalModel getPhysicalModel() const;
  void setPhysicalModel(const PhysicalModel & physicalModel);

  virtual void run();
  virtual std::string dump() const;
  virtual bool analysisLaunched() const;

  bool isReliabilityAnalysis() const;

protected:
  void setIsReliabilityAnalysis(bool isReliabilityAnalysis);

private:
  std::string name_;
  PhysicalModel physicalModel_;
  bool isReliabilityAnalysis_;
};
}
#endif