//                                               -*- C++ -*-
/**
 *  @brief Class defining the otgui studies
 *
 *  Copyright 2015-2016 EDF
 *
 *  This library is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
#ifndef OTGUI_OTSTUDY_HXX
#define OTGUI_OTSTUDY_HXX

#include "DesignOfExperiment.hxx"
#include "Analysis.hxx"
#include "LimitState.hxx"
#include "Observer.hxx"

namespace OTGUI {
class OTStudy : public OT::PersistentObject, public Observable
{
public:
  static std::vector<OTStudy*> GetOTStudies();
  static OTStudy* GetOTStudyByName(const OT::String & otStudyName);
  static bool HasOTStudyNamed(const OT::String & otStudyName);
  static OT::String GetAvailableOTStudyName();
  static void SetInstanceObserver(Observer * observer);

  OTStudy(const OT::String & name);

  virtual OTStudy * clone() const;

  virtual ~OTStudy();

  std::vector<PhysicalModel> getPhysicalModels() const;
  PhysicalModel & getPhysicalModelByName(const OT::String & physicalModelName);
  bool hasPhysicalModelNamed(const OT::String & physicalModelName);
  OT::String getAvailablePhysicalModelName();
  void addPhysicalModel(const PhysicalModel & physicalModel);

  std::vector<DesignOfExperiment> getDesignOfExperiments() const;
  bool hasDesignOfExperimentNamed(const OT::String & designOfExperimentName);
  OT::String getAvailableDesignOfExperimentName();
  void addDesignOfExperiment(const DesignOfExperiment & designOfExperiment);

  std::vector<Analysis> getAnalyses() const;
  Analysis & getAnalysisByName(const OT::String & analysisName);
  bool hasAnalysisNamed(const OT::String & analysisName);
  OT::String getAvailableAnalysisName(const OT::String & rootName);
  void addAnalysis(const Analysis & analysis);

  std::vector<LimitState> getLimitStates() const;
  bool hasLimitStateNamed(const OT::String & limitStateName);
  OT::String getAvailableLimitStateName();
  void addLimitState(const LimitState & limitState);

  OT::String dump();

private:
  OTStudy(const OTStudy & other);
  static std::vector<OTStudy*> OTStudies_;
  static Observer * OTStudyObserver_;
  std::vector<PhysicalModel> physicalModels_;
  std::vector<DesignOfExperiment> designOfExperiments_;
  std::vector<Analysis> analyses_;
  std::vector<LimitState> limitStates_;
};
}
#endif