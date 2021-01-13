//                                               -*- C++ -*-
/**
 *  @brief Class defining the persalys studies
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
#include "persalys/Study.hxx"

#include "persalys/BaseTools.hxx"

#include <openturns/Study.hxx>
#include <openturns/XMLH5StorageManager.hxx>
#include <openturns/PersistentObjectFactory.hxx>
#include <openturns/WeibullMin.hxx>
#include <openturns/ThresholdEventImplementation.hxx>

#include <boost/filesystem.hpp>
#include <locale>
#include <codecvt>

using namespace OT;

// define old OT classes to be able to open old xml filesystem

namespace OT
{

// -- removed Copula class

class Copula : public Distribution
{
  CLASSNAME
};
CLASSNAMEINIT(Copula)
TEMPLATE_CLASSNAMEINIT(PersistentCollection<Copula>)
static const Factory<PersistentCollection<Copula> > Factory_PersistentCollection_Copula;


// -- renamed Weibull class

class Weibull : public WeibullMin
{
  CLASSNAME
};
CLASSNAMEINIT(Weibull)
static Factory<Weibull> Factory_Weibull;


// -- renamed EventRandomVector class

class EventRandomVector : public ThresholdEventImplementation
{
  CLASSNAME
};
CLASSNAMEINIT(EventRandomVector)
static Factory<EventRandomVector> Factory_EventRandomVector;

} // namespace OT


namespace PERSALYS
{

CLASSNAMEINIT(Study)

PersistentCollection<Study > Study::studies_;
Observer * Study::studyObserver_ = 0;


Collection<PERSALYS::Study> Study::GetInstances()
{
  return studies_;
}


Description Study::GetFileNames()
{
  Description studiesFileNames;
  for (UnsignedInteger i = 0; i < studies_.getSize(); ++ i)
    if (!studies_[i].getFileName().empty())
      studiesFileNames.add(studies_[i].getFileName());

  return studiesFileNames;
}


bool Study::HasInstanceNamed(const String & studyName)
{
  for (PersistentCollection<Study >::iterator it = studies_.begin(); it != studies_.end(); ++it)
    if ((*it).getImplementation().get()->getName() == studyName)
      return true;
  return false;
}


String Study::GetAvailableName(const String& rootName)
{
  int i = 0;
  while (HasInstanceNamed(rootName + (OSS() << i).str()))
    ++i;
  return rootName + (OSS() << i).str();
}


void Study::Add(const Study& study)
{
  if (studyObserver_)
  {
    if (studies_.contains(study))
      throw InvalidArgumentException(HERE) << "The study already exists\n";
    studies_.add(study);
    studyObserver_->update(study.getImplementation().get(), "addStudy");
  }
}


void Study::Remove(const Study& study)
{
  if (!studies_.contains(study))
    return;

  study.getImplementation().get()->clear();

  if (studyObserver_)
  {
    study.getImplementation().get()->removeAllObservers();

    for (UnsignedInteger i = 0; i < studies_.getSize(); ++ i)
    {
      if (studies_[i] == study)
      {
        studies_.erase(studies_.begin() + i);
        break;
      }
    }
  }
}


Study Study::Open(const String & xmlFileName)
{
  // open study
  OT::Study study;
  boost::filesystem::path::imbue(std::locale(std::locale(), new std::codecvt_utf8_utf16<wchar_t>()));
  boost::filesystem::path h5FileName(xmlFileName);
  h5FileName.replace_extension(".h5");
  if(boost::filesystem::exists(h5FileName))
    study.setStorageManager(XMLH5StorageManager(xmlFileName));
  else
    study.setStorageManager(XMLStorageManager(xmlFileName));

  study.load();
  Study openedStudy;
  study.fillObject("aStudy", openedStudy);
  openedStudy.getImplementation()->setFileName(xmlFileName);

  return openedStudy;
}


void Study::SetInstanceObserver(Observer * observer)
{
  studyObserver_ = observer;
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


String Study::getAvailableDataModelName(const String& modelRootName) const
{
  return getImplementation()->getAvailableDataModelName(modelRootName);
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


String Study::getAvailableLimitStateName(const String & rootName) const
{
  return getImplementation()->getAvailableLimitStateName(rootName);
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
