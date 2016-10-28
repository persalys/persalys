//                                               -*- C++ -*-
/**
 *  @brief Abstract top-level class for otstudies
 *
 *  Copyright 2015-2016 EDF-Phimeca
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

#include "OTStudyImplementation.hxx"

namespace OTGUI {
class OTGUI_API OTStudy : public OT::TypedInterfaceObject<OTStudyImplementation>
{
  CLASSNAME;

public:
  typedef OT::Pointer<OTStudyImplementation> Implementation;

  // static methods
  static OT::Collection<OTStudy> GetInstances();
  static OT::Description GetFileNames();
  static bool HasInstanceNamed(const OT::String& otStudyName);
  static OT::String GetAvailableName();
  static void Add(const OTStudy& otstudy);
  static void Remove(const OTStudy& otstudy);
  static void Open(const OT::String& xmlFileName);
  static void SetInstanceObserver(Observer * observer);

  /** Default constructor */
  explicit OTStudy(const OT::String& name="Unnamed");
  /** Default constructor */
  OTStudy(const OTStudyImplementation& implementation);
  /** Constructor from implementation */
  OTStudy(const Implementation& p_implementation);
  /** Constructor from implementation pointer */
  OTStudy(OTStudyImplementation * p_implementation);

  /** Virtual constructor */
  virtual OTStudy * clone() const;

  /** Comparison operator */
  OT::Bool operator ==(const OTStudy & other) const;
  OT::Bool operator !=(const OTStudy & other) const;

  void addObserver(Observer * observer);

  OT::String getFileName() const;
  void setFileName(const OT::String& fileName);

  OT::Collection<DesignOfExperiment> getDataModels() const;
  DesignOfExperiment& getDataModelByName(const OT::String& dataModelName);
  OT::String getAvailableDataModelName() const;

  OT::Collection<PhysicalModel> getPhysicalModels() const;
  PhysicalModel& getPhysicalModelByName(const OT::String& physicalModelName);
  OT::String getAvailablePhysicalModelName(const OT::String& physicalModelRootName="PhysicalModel_") const;
  void add(const PhysicalModel& physicalModel);
  void remove(const PhysicalModel& physicalModel);

  OT::Collection<DesignOfExperiment> getDesignOfExperiments() const;
  DesignOfExperiment& getDesignOfExperimentByName(const OT::String& designOfExperimentName);
  OT::String getAvailableDesignOfExperimentName() const;
  void add(const DesignOfExperiment& designOfExperiment);
  void remove(const DesignOfExperiment& designOfExperiment);

  OT::Collection<Analysis> getAnalyses() const;
  Analysis& getAnalysisByName(const OT::String& analysisName);
  OT::String getAvailableAnalysisName(const OT::String& rootName) const;
  void add(const Analysis& analysis);
  void remove(const Analysis& analysis);

  OT::Collection<LimitState> getLimitStates() const;
  OT::String getAvailableLimitStateName() const;
  void add(const LimitState& limitState);
  void remove(const LimitState& limitState);

  OT::String getPythonScript();

  void save(const OT::String& xmlFileName);

  /** Method save() stores the object through the StorageManager */
  void save(OT::Advocate& adv) const;

  /** Method load() reloads the object from the StorageManager */
  void load(OT::Advocate& adv);

private:
  static OT::PersistentCollection<OTStudy > OTStudies_;
  static OT::Description OTStudiesFileNames_;
  static Observer * OTStudyObserver_;
};
}
#endif