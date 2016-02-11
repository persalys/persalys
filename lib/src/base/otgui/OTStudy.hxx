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
  static OT::Collection<OTStudy*> GetOTStudies();
  static OTStudy* GetOTStudyByName(const OT::String & otStudyName);
  static bool HasOTStudyNamed(const OT::String & otStudyName);
  static OT::String GetAvailableOTStudyName();
  static void SetInstanceObserver(Observer * observer);

  OTStudy(const OT::String & name);

  virtual OTStudy * clone() const;

  virtual ~OTStudy();

  OT::Collection<PhysicalModel> getPhysicalModels() const;
  PhysicalModel & getPhysicalModelByName(const OT::String & physicalModelName);
  bool hasPhysicalModelNamed(const OT::String & physicalModelName);
  OT::String getAvailablePhysicalModelName();
  void addPhysicalModel(const PhysicalModel & physicalModel);

  OT::Collection<DesignOfExperiment> getDesignOfExperiments() const;
  bool hasDesignOfExperimentNamed(const OT::String & designOfExperimentName);
  OT::String getAvailableDesignOfExperimentName();
  void addDesignOfExperiment(const DesignOfExperiment & designOfExperiment);

  OT::Collection<Analysis> getAnalyses() const;
  Analysis & getAnalysisByName(const OT::String & analysisName);
  bool hasAnalysisNamed(const OT::String & analysisName);
  OT::String getAvailableAnalysisName(const OT::String & rootName);
  void addAnalysis(const Analysis & analysis);

  OT::Collection<LimitState> getLimitStates() const;
  bool hasLimitStateNamed(const OT::String & limitStateName);
  OT::String getAvailableLimitStateName();
  void addLimitState(const LimitState & limitState);

  OT::String dump();

  /** Method save() stores the object through the StorageManager */
  void save(OT::Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  void load(OT::Advocate & adv);

private:
  OTStudy(const OTStudy & other);
  static OT::Collection<OTStudy*> OTStudies_;
  static Observer * OTStudyObserver_;
  OT::PersistentCollection<PhysicalModel> physicalModels_;
  OT::PersistentCollection<DesignOfExperiment> designOfExperiments_;
  OT::PersistentCollection<Analysis> analyses_;
  OT::PersistentCollection<LimitState> limitStates_;
};
}
#endif