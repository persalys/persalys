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
#include "otgui/OTStudy.hxx"

#include "openturns/Study.hxx"
#include "openturns/XMLStorageManager.hxx"

using namespace OT;

namespace OTGUI {

CLASSNAMEINIT(OTStudy);


PersistentCollection<OTStudy > OTStudy::OTStudies_;
Description OTStudy::OTStudiesFileNames_;
Observer * OTStudy::OTStudyObserver_ = 0;


Collection<OTStudy> OTStudy::GetInstances()
{
  return OTStudies_;
}


Description OTStudy::GetFileNames()
{
  return OTStudiesFileNames_;
}


bool OTStudy::HasInstanceNamed(const String & otStudyName)
{
  for (PersistentCollection<OTStudy >::iterator it=OTStudies_.begin(); it!=OTStudies_.end(); ++it)
    if ((*it).getImplementation().get()->getName() == otStudyName)
      return true;
  return false;
}


String OTStudy::GetAvailableName()
{
  int i = 0;
  String rootName = "OTStudy_";
  while (HasInstanceNamed(rootName + (OSS()<<i).str()))
    ++i;
  return rootName + (OSS()<<i).str();
}


void OTStudy::Add(const OTStudy& otstudy)
{
  if (OTStudyObserver_)
  {
    if (OTStudies_.contains(otstudy))
      throw InvalidArgumentException(HERE) << "The OTStudy already exists\n";
    OTStudies_.add(otstudy);
    OTStudiesFileNames_.add(otstudy.getFileName());
    OTStudyObserver_->update(otstudy.getImplementation().get(), "addStudy");
  }
}


void OTStudy::Remove(const OTStudy& otstudy)
{
  otstudy.getImplementation().get()->clear();

  if (OTStudyObserver_)
  {
    otstudy.getImplementation().get()->notifyAndRemove("otStudyRemoved", "OTStudy");

    for (UnsignedInteger i=0; i < OTStudies_.getSize(); ++ i)
    {
      if (OTStudies_[i] == otstudy)
      {
        OTStudiesFileNames_.erase(OTStudiesFileNames_.begin() + i);
        OTStudies_.erase(OTStudies_.begin() + i);
        break;
      }
    }
  }
}


void OTStudy::Open(const String & xmlFileName)
{
  Study study;
  study.setStorageManager(XMLStorageManager(xmlFileName));
  study.load();
  OTStudy openedStudy;
  study.fillObject("otstudy", openedStudy);
  openedStudy.setFileName(xmlFileName);
  Add(openedStudy);
}


void OTStudy::SetInstanceObserver(Observer * observer)
{
  OTStudyObserver_ = observer;
}


/** Default constructor */
OTStudy::OTStudy(const String& name)
  : TypedInterfaceObject<OTStudyImplementation>(new OTStudyImplementation(name))
{
};


/** Default constructor */
OTStudy::OTStudy(const OTStudyImplementation& implementation)
  : TypedInterfaceObject<OTStudyImplementation>(implementation.clone())
{
};


/** Constructor from implementation */
OTStudy::OTStudy(const Implementation& p_implementation)
  : TypedInterfaceObject<OTStudyImplementation>(p_implementation)
{
}


/** Constructor from implementation pointer */
OTStudy::OTStudy(OTStudyImplementation * p_implementation)
: TypedInterfaceObject<OTStudyImplementation>(p_implementation)
{
}


/* Virtual constructor */
OTStudy* OTStudy::clone() const
{
  return new OTStudy(*this);
}


/* Comparison operator */
Bool OTStudy::operator ==(const OTStudy & other) const
{
  if (this == &other) return true;
  return *getImplementation() == *other.getImplementation();
}


/* Comparison operator */
Bool OTStudy::operator !=(const OTStudy & other) const
{
  return !operator==(other);
}


void OTStudy::addObserver(Observer * observer)
{
  getImplementation()->addObserver(observer);
}


String OTStudy::getFileName() const
{
  return getImplementation()->getFileName();
}


void OTStudy::setFileName(const String& fileName)
{
  getImplementation()->setFileName(fileName);
}


// ----- DATA MODEL -----
Collection< DesignOfExperiment > OTStudy::getDataModels() const
{
  return getImplementation()->getDataModels();
}


DesignOfExperiment& OTStudy::getDataModelByName(const String& dataModelName)
{
  return getImplementation()->getDataModelByName(dataModelName);
}


String OTStudy::getAvailableDataModelName() const
{
  return getImplementation()->getAvailableDataModelName();
}


// ----- PHYSICAL MODEL -----
Collection<PhysicalModel> OTStudy::getPhysicalModels() const
{
  return getImplementation()->getPhysicalModels();
}


PhysicalModel& OTStudy::getPhysicalModelByName(const String& physicalModelName)
{
  return getImplementation()->getPhysicalModelByName(physicalModelName);
}


String OTStudy::getAvailablePhysicalModelName(const String& physicalModelRootName) const
{
  return getImplementation()->getAvailablePhysicalModelName();
}


void OTStudy::add(const PhysicalModel& physicalModel)
{
  getImplementation()->add(physicalModel);
}


void OTStudy::remove(const PhysicalModel& physicalModel)
{
  getImplementation()->remove(physicalModel);
}


// ----- DESIGN OF EXPERIMENT -----
Collection<DesignOfExperiment> OTStudy::getDesignOfExperiments() const
{
  return getImplementation()->getDesignOfExperiments();
}


DesignOfExperiment& OTStudy::getDesignOfExperimentByName(const String& designOfExperimentName)
{
  return getImplementation()->getDesignOfExperimentByName(designOfExperimentName);
}


String OTStudy::getAvailableDesignOfExperimentName() const
{
  return getImplementation()->getAvailableDesignOfExperimentName();
}


void OTStudy::add(const DesignOfExperiment& designOfExperiment)
{
  getImplementation()->add(designOfExperiment);
}


void OTStudy::remove(const DesignOfExperiment& designOfExperiment)
{
  getImplementation()->remove(designOfExperiment);
}


// ----- ANALYSIS -----
Collection<Analysis> OTStudy::getAnalyses() const
{
  return getImplementation()->getAnalyses();
}


Analysis& OTStudy::getAnalysisByName(const String& analysisName)
{
  return getImplementation()->getAnalysisByName(analysisName);
}


String OTStudy::getAvailableAnalysisName(const String& rootName) const
{
  return getImplementation()->getAvailableAnalysisName(rootName);
}


void OTStudy::add(const Analysis& analysis)
{
  getImplementation()->add(analysis);
}


void OTStudy::remove(const Analysis& analysis)
{
  getImplementation()->remove(analysis);
}


// ----- LIMIT STATE -----
Collection<LimitState> OTStudy::getLimitStates() const
{
  return getImplementation()->getLimitStates();
}


String OTStudy::getAvailableLimitStateName() const
{
  return getImplementation()->getAvailableLimitStateName();
}


void OTStudy::add(const LimitState& limitState)
{
  getImplementation()->add(limitState);
}


void OTStudy::remove(const LimitState& limitState)
{
  getImplementation()->remove(limitState);
}


String OTStudy::getPythonScript()
{
  return getImplementation()->getPythonScript();
}


void OTStudy::save(const String& xmlFileName)
{
  getImplementation()->save(xmlFileName);
}


/* Method save() stores the object through the StorageManager */
void OTStudy::save(Advocate& adv) const
{
  getImplementation()->save(adv);
}


/* Method load() reloads the object from the StorageManager */
void OTStudy::load(Advocate& adv)
{
  getImplementation()->load(adv);
}
}