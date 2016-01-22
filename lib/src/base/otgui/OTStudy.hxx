// OTStudy.hxx

#ifndef OTSTUDY_H
#define OTSTUDY_H

#include "DesignOfExperiment.hxx"
#include "Analysis.hxx"
#include "LimitState.hxx"
#include "Observer.hxx"

namespace OTGUI {
class OTStudy : public OT::PersistentObject, public Observable
{
public:
  static std::vector<OTStudy*> GetOTStudies();
  static OTStudy* GetOTStudyByName(const std::string & otStudyName);
  static bool HasOTStudyNamed(const std::string & otStudyName);
  static std::string GetAvailableOTStudyName();
  static void SetInstanceObserver(Observer * observer);

  OTStudy(const std::string & name);
  virtual OTStudy * clone() const;

  virtual ~OTStudy();

  std::string getName() const;
  void setName(const std::string & name);

  std::vector<PhysicalModel> getPhysicalModels() const;
  PhysicalModel & getPhysicalModelByName(const std::string & physicalModelName);
  bool hasPhysicalModelNamed(const std::string & physicalModelName);
  std::string getAvailablePhysicalModelName();
  void addPhysicalModel(const PhysicalModel & physicalModel);

  std::vector<DesignOfExperiment> getDesignOfExperiments() const;
  bool hasDesignOfExperimentNamed(const std::string & designOfExperimentName);
  std::string getAvailableDesignOfExperimentName();
  void addDesignOfExperiment(const DesignOfExperiment & designOfExperiment);

  std::vector<Analysis> getAnalyses() const;
  Analysis & getAnalysisByName(const std::string & analysisName);
  bool hasAnalysisNamed(const std::string & analysisName);
  std::string getAvailableAnalysisName(const std::string & rootName);
  void addAnalysis(const Analysis & analysis);

  std::vector<LimitState> getLimitStates() const;
  bool hasLimitStateNamed(const std::string & limitStateName);
  std::string getAvailableLimitStateName();
  void addLimitState(const LimitState & limitState);

  std::string dump();

private:
  OTStudy(const OTStudy & other);
  static std::vector<OTStudy*> OTStudies_;
  static Observer * OTStudyObserver_;
  std::string name_;
  std::vector<PhysicalModel> physicalModels_;
  std::vector<DesignOfExperiment> designOfExperiments_;
  std::vector<Analysis> analyses_;
  std::vector<LimitState> limitStates_;
};
}
#endif