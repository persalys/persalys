//                                               -*- C++ -*-
/**
 *  @brief Class defining the otgui studies
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

#include "DesignOfExperiment.hxx"
#include "DataModel.hxx"
#include "Analysis.hxx"
#include "LimitState.hxx"
#include "Observer.hxx"

namespace OTGUI {
class OTGUI_API OTStudy : public OT::PersistentObject, public Observable
{
public:
  typedef OT::Collection<OTStudy*> OTStudyCollection;

  static OTStudyCollection GetInstances();
  static OT::Description GetFileNames();
  static OTStudy * GetInstanceByName(const OT::String & otStudyName);
  static bool HasInstanceNamed(const OT::String & otStudyName);
  static OT::String GetAvailableName();
  static void Add(OTStudy * otstudy);
  static void Remove(OTStudy * otstudy);
  static void Open(const OT::String & xmlFileName);
  static void SetInstanceObserver(Observer * observer);

  /** Default constructor */
  OTStudy(const OT::String & name="Unnamed");

  /** Virtual constructor */
  virtual OTStudy * clone() const;

  OT::String getFileName() const;
  void setFileName(const OT::String & fileName);

  OT::Collection<DataModel> getDataModels() const;
  DataModel & getDataModelByName(const OT::String & dataModelName);
  bool hasDataModelNamed(const OT::String & dataModelName) const;
  OT::String getAvailableDataModelName() const;
  void add(const DataModel & dataModel);
  void clear(const DataModel & dataModel);
  void remove(DataModel & dataModel);

  OT::Collection<PhysicalModel> getPhysicalModels() const;
  PhysicalModel & getPhysicalModelByName(const OT::String & physicalModelName);
  bool hasPhysicalModelNamed(const OT::String & physicalModelName) const;
  OT::String getAvailablePhysicalModelName() const;
  void add(const PhysicalModel & physicalModel);
  void clear(const PhysicalModel & physicalModel);
  void remove(const PhysicalModel & physicalModel);

  OT::Collection<DesignOfExperiment> getDesignOfExperiments() const;
  DesignOfExperiment & getDesignOfExperimentByName(const OT::String & designOfExperimentName);
  bool hasDesignOfExperimentNamed(const OT::String & designOfExperimentName) const;
  OT::String getAvailableDesignOfExperimentName() const;
  void add(const DesignOfExperiment & designOfExperiment);
  void remove(const DesignOfExperiment & designOfExperiment);

  OT::Collection<Analysis> getAnalyses() const;
  Analysis & getAnalysisByName(const OT::String & analysisName);
  bool hasAnalysisNamed(const OT::String & analysisName) const;
  OT::String getAvailableAnalysisName(const OT::String & rootName) const;
  void add(const Analysis & analysis);
  void remove(const Analysis & analysis);

  OT::Collection<LimitState> getLimitStates() const;
  bool hasLimitStateNamed(const OT::String & limitStateName) const;
  OT::String getAvailableLimitStateName() const;
  void add(const LimitState & limitState);
  void clear(const LimitState & limitState);
  void remove(const LimitState & limitState);

  OT::String getPythonScript();

  void save(const OT::String & xmlFileName);

  /** Method save() stores the object through the StorageManager */
  void save(OT::Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  void load(OT::Advocate & adv);

private:
  static OT::Collection<OT::Pointer<OTStudy> > OTStudies_;
  static OT::Description OTStudiesFileNames_;
  static Observer * OTStudyObserver_;
  OT::String fileName_;
  OT::PersistentCollection<DataModel> dataModels_;
  OT::PersistentCollection<PhysicalModel> physicalModels_;
  OT::PersistentCollection<DesignOfExperiment> designOfExperiments_;
  OT::PersistentCollection<Analysis> analyses_;
  OT::PersistentCollection<LimitState> limitStates_;
};
}
#endif