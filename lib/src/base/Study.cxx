//                                               -*- C++ -*-
/**
 *  @brief Class defining the otgui studies
 *
 *  Copyright 2015-2018 EDF-Phimeca
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
#include "otgui/Study.hxx"

#include "otgui/BaseTools.hxx"

#include <openturns/Study.hxx>
#include <openturns/XMLStorageManager.hxx>

#include <boost/filesystem.hpp>

using namespace OT;

namespace OTGUI
{

CLASSNAMEINIT(Study)

PersistentCollection<Study > Study::OTStudies_;
Observer * Study::OTStudyObserver_ = 0;


Collection<OTGUI::Study> Study::GetInstances()
{
  return OTStudies_;
}


Description Study::GetFileNames()
{
  Description otStudiesFileNames;
  for (UnsignedInteger i = 0; i < OTStudies_.getSize(); ++ i)
    if (!OTStudies_[i].getFileName().empty())
      otStudiesFileNames.add(OTStudies_[i].getFileName());

  return otStudiesFileNames;
}


bool Study::HasInstanceNamed(const String & otStudyName)
{
  for (PersistentCollection<Study >::iterator it = OTStudies_.begin(); it != OTStudies_.end(); ++it)
    if ((*it).getImplementation().get()->getName() == otStudyName)
      return true;
  return false;
}


String Study::GetAvailableName()
{
  int i = 0;
  String rootName = "Study_";
  while (HasInstanceNamed(rootName + (OSS() << i).str()))
    ++i;
  return rootName + (OSS() << i).str();
}


void Study::Add(const Study& otstudy)
{
  if (OTStudyObserver_)
  {
    if (OTStudies_.contains(otstudy))
      throw InvalidArgumentException(HERE) << "The study already exists\n";
    OTStudies_.add(otstudy);
    OTStudyObserver_->update(otstudy.getImplementation().get(), "addStudy");
  }
}


void Study::Remove(const Study& otstudy)
{
  if (!OTStudies_.contains(otstudy))
    return;

  otstudy.getImplementation().get()->clear();

  if (OTStudyObserver_)
  {
    otstudy.getImplementation().get()->notifyAndRemove("otStudyRemoved", "Study");

    for (UnsignedInteger i = 0; i < OTStudies_.getSize(); ++ i)
    {
      if (OTStudies_[i] == otstudy)
      {
        OTStudies_.erase(OTStudies_.begin() + i);
        break;
      }
    }
  }
}


Study Study::Open(const String & xmlFileName)
{
  // open study
  OT::Study study;
  study.setStorageManager(XMLStorageManager(xmlFileName));
  study.load();
  Study openedStudy;
  study.fillObject("aStudy", openedStudy);
  openedStudy.getImplementation()->setFileName(xmlFileName);

  return openedStudy;
}


void Study::SetInstanceObserver(Observer * observer)
{
  OTStudyObserver_ = observer;
}


/** Default constructor */
Study::Study(const String& name)
  : TypedInterfaceObject<StudyImplementation>(new StudyImplementation(name))
{
}


/** Default constructor */
Study::Study(const StudyImplementation& implementation)
  : TypedInterfaceObject<StudyImplementation>(implementation.clone())
{
}


/** Constructor from implementation */
Study::Study(const Implementation& p_implementation)
  : TypedInterfaceObject<StudyImplementation>(p_implementation)
{
}


/** Constructor from implementation pointer */
Study::Study(StudyImplementation * p_implementation)
  : TypedInterfaceObject<StudyImplementation>(p_implementation)
{
}


/* Virtual constructor */
Study* Study::clone() const
{
  return new Study(*this);
}


/* Comparison operator */
Bool Study::operator ==(const Study & other) const
{
  if (this == &other) return true;
  return *getImplementation() == *other.getImplementation();
}


/* Comparison operator */
Bool Study::operator !=(const Study & other) const
{
  return !operator==(other);
}


void Study::addObserver(Observer * observer)
{
  getImplementation()->addObserver(observer);
}


String Study::getFileName() const
{
  return getImplementation()->getFileName();
}


// ----- DATA MODEL -----
Collection< DesignOfExperiment > Study::getDataModels() const
{
  return getImplementation()->getDataModels();
}


DesignOfExperiment& Study::getDataModelByName(const String& dataModelName)
{
  return getImplementation()->getDataModelByName(dataModelName);
}


String Study::getAvailableDataModelName() const
{
  return getImplementation()->getAvailableDataModelName();
}


void Study::add(const DesignOfExperiment& designOfExperiment)
{
  getImplementation()->add(designOfExperiment);
}


void Study::remove(const DesignOfExperiment& designOfExperiment)
{
  getImplementation()->remove(designOfExperiment);
}


// ----- PHYSICAL MODEL -----
Collection<PhysicalModel> Study::getPhysicalModels() const
{
  return getImplementation()->getPhysicalModels();
}


PhysicalModel& Study::getPhysicalModelByName(const String& physicalModelName)
{
  return getImplementation()->getPhysicalModelByName(physicalModelName);
}


String Study::getAvailablePhysicalModelName(const String& physicalModelRootName) const
{
  return getImplementation()->getAvailablePhysicalModelName(physicalModelRootName);
}


void Study::add(const PhysicalModel& physicalModel)
{
  getImplementation()->add(physicalModel);
}


void Study::remove(const PhysicalModel& physicalModel)
{
  getImplementation()->remove(physicalModel);
}


// ----- ANALYSIS -----
Collection<Analysis> Study::getAnalyses() const
{
  return getImplementation()->getAnalyses();
}


Analysis& Study::getAnalysisByName(const String& analysisName)
{
  return getImplementation()->getAnalysisByName(analysisName);
}


String Study::getAvailableAnalysisName(const String& rootName) const
{
  return getImplementation()->getAvailableAnalysisName(rootName);
}


void Study::add(const Analysis& analysis)
{
  getImplementation()->add(analysis);
}


void Study::remove(const Analysis& analysis)
{
  getImplementation()->remove(analysis);
}


// ----- LIMIT STATE -----
Collection<LimitState> Study::getLimitStates() const
{
  return getImplementation()->getLimitStates();
}


String Study::getAvailableLimitStateName() const
{
  return getImplementation()->getAvailableLimitStateName();
}


void Study::add(const LimitState& limitState)
{
  getImplementation()->add(limitState);
}


void Study::remove(const LimitState& limitState)
{
  getImplementation()->remove(limitState);
}


String Study::getPythonScript()
{
  return getImplementation()->getPythonScript();
}


void Study::save(const String& xmlFileName)
{
  getImplementation()->save(xmlFileName);
}


/* Method save() stores the object through the StorageManager */
void Study::save(Advocate& adv) const
{
  getImplementation()->save(adv);
}


/* Method load() reloads the object from the StorageManager */
void Study::load(Advocate& adv)
{
  getImplementation()->load(adv);
}
}
