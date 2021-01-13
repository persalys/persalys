//                                               -*- C++ -*-
/**
 *  @brief Abstract top-level class for studies
 *
 *  Copyright 2015-2021 EDF-Phimeca
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
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
#ifndef PERSALYS_STUDY_HXX
#define PERSALYS_STUDY_HXX

#include "StudyImplementation.hxx"

namespace PERSALYS
{
class PERSALYS_BASE_API Study : public OT::TypedInterfaceObject<StudyImplementation>
{
  CLASSNAME

public:
  typedef OT::Pointer<StudyImplementation> Implementation;

  // static methods
  static OT::Collection<PERSALYS::Study> GetInstances();
  static OT::Description GetFileNames();
  static bool HasInstanceNamed(const OT::String& studyName);
  static OT::String GetAvailableName(const OT::String& rootName);
  static void Add(const Study& study);
  static void Remove(const Study& study);
  static Study Open(const OT::String& xmlFileName);
  static void SetInstanceObserver(Observer * observer);

  /** Default constructor */
  explicit Study(const OT::String& name = "Unnamed");
  /** Default constructor */
  Study(const StudyImplementation& implementation);
  /** Constructor from implementation */
  Study(const Implementation& p_implementation);
  /** Constructor from implementation pointer */
  Study(StudyImplementation * p_implementation);

  /** Virtual constructor */
  virtual Study * clone() const;

  /** Comparison operator */
  OT::Bool operator ==(const Study & other) const;
  OT::Bool operator !=(const Study & other) const;

  void addObserver(Observer * observer);

  OT::String getFileName() const;

  OT::Collection<DesignOfExperiment> getDataModels() const;
  DesignOfExperiment& getDataModelByName(const OT::String& dataModelName);
  OT::String getAvailableDataModelName(const OT::String& modelRootName) const;
  void add(const DesignOfExperiment& designOfExperiment);
  void remove(const DesignOfExperiment& designOfExperiment);

  OT::Collection<PhysicalModel> getPhysicalModels() const;
  PhysicalModel& getPhysicalModelByName(const OT::String& physicalModelName);
  OT::String getAvailablePhysicalModelName(const OT::String& physicalModelRootName) const;
  void add(const PhysicalModel& physicalModel);
  void remove(const PhysicalModel& physicalModel);

  OT::Collection<Analysis> getAnalyses() const;
  Analysis& getAnalysisByName(const OT::String& analysisName);
  OT::String getAvailableAnalysisName(const OT::String& rootName) const;
  void add(const Analysis& analysis);
  void remove(const Analysis& analysis);

  OT::Collection<LimitState> getLimitStates() const;
  OT::String getAvailableLimitStateName(const OT::String & rootName) const;
  void add(const LimitState& limitState);
  void remove(const LimitState& limitState);

  OT::String getPythonScript();

  void save(const OT::String& xmlFileName);

  /** Method save() stores the object through the StorageManager */
  void save(OT::Advocate& adv) const;

  /** Method load() reloads the object from the StorageManager */
  void load(OT::Advocate& adv);

private:
  static OT::PersistentCollection<Study > studies_;
  static Observer * studyObserver_;
};
}
#endif
