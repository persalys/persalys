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
#include "otgui/OTStudy.hxx"

#include "otgui/ReliabilityAnalysis.hxx"

#include "PersistentObjectFactory.hxx"
#include "Study.hxx"
#include "XMLStorageManager.hxx"

using namespace OT;

namespace OTGUI {

static Factory<OTStudy> RegisteredFactory("OTStudy");

static Factory<PersistentCollection<PhysicalModel> > RegisteredFactory_CollPM("PersistentCollection<PhysicalModel>");
static Factory<PersistentCollection<DesignOfExperiment> > RegisteredFactory_CollDOE("PersistentCollection<DesignOfExperiment>");
static Factory<PersistentCollection<LimitState> > RegisteredFactory_CollLS("PersistentCollection<LimitState>");
static Factory<PersistentCollection<Analysis> > RegisteredFactory_CollAn("PersistentCollection<Analysis>");


Collection<OTStudy*> OTStudy::OTStudies_;
Description OTStudy::OTStudiesFileNames_;
Observer * OTStudy::OTStudyObserver_ = 0;


Collection<OTStudy*> OTStudy::GetOTStudies()
{
  return OTStudies_;
}


Description OTStudy::GetOTStudiesFileNames()
{
  return OTStudiesFileNames_;
}


OTStudy * OTStudy::GetOTStudyByName(const String & otStudyName)
{
  for (Collection<OTStudy*>::iterator it=OTStudies_.begin(); it!=OTStudies_.end(); ++it)
    if ((*it)->getName() == otStudyName)
      return (*it);
  return 0;
}


bool OTStudy::HasOTStudyNamed(const String & otStudyName)
{
  for (Collection<OTStudy*>::iterator it=OTStudies_.begin(); it!=OTStudies_.end(); ++it)
    if ((*it)->getName() == otStudyName)
      return true;
  return false;
}


String OTStudy::GetAvailableOTStudyName()
{
  int i = 0;
  String rootName = "OTStudy_";
  while (HasOTStudyNamed(rootName + (OSS()<<i).str()))
    ++i;
  return rootName + (OSS()<<i).str();
}


void OTStudy::AddOTStudy(OTStudy * otstudy)
{
  OTStudies_.add(otstudy);
  OTStudiesFileNames_.add(otstudy->getFileName());
  if (OTStudyObserver_)
    OTStudyObserver_->update(otstudy, "addStudy");
}


void OTStudy::OpenOTStudy(const String & xmlFileName)
{
  Study study;
  study.setStorageManager(XMLStorageManager(xmlFileName));
  study.load();
  OTStudy * openedStudy = new OTStudy;
  study.fillObject("otstudy", *openedStudy);
  openedStudy->setFileName(xmlFileName);
  AddOTStudy(openedStudy);
}


void OTStudy::SetInstanceObserver(Observer * observer)
{
  OTStudyObserver_ = observer;
}


/* Default constructor */
OTStudy::OTStudy(const String & name)
  : PersistentObject()
  , Observable()
{
  setName(name);
}


/* Virtual constructor */
OTStudy* OTStudy::clone() const
{
  return new OTStudy(*this);
}


/* Destructor */
OTStudy::~OTStudy()
{
  OTStudiesFileNames_.erase(std::remove(OTStudiesFileNames_.begin(), OTStudiesFileNames_.end(), this->getFileName()), OTStudiesFileNames_.end());
  OTStudies_.erase(std::remove(OTStudies_.begin(), OTStudies_.end(), this), OTStudies_.end());
}


String OTStudy::getFileName() const
{
  return fileName_;
}


void OTStudy::setFileName(const String& fileName)
{
  fileName_ = fileName;
}


Collection<PhysicalModel> OTStudy::getPhysicalModels() const
{
  return physicalModels_;
}


PhysicalModel & OTStudy::getPhysicalModelByName(const String & physicalModelName)
{
  for (UnsignedInteger i=0; i<physicalModels_.getSize(); ++i)
    if (physicalModels_[i].getName() == physicalModelName)
      return physicalModels_[i];
  throw InvalidArgumentException(HERE) << "The given name " << physicalModelName <<" does not correspond to a physical model of the study.\n"; 
}


bool OTStudy::hasPhysicalModelNamed(const String & physicalModelName)
{
  for (UnsignedInteger i=0; i<physicalModels_.getSize(); ++i)
    if (physicalModels_[i].getImplementation()->getName() == physicalModelName)
      return true;
  return false;
}


String OTStudy::getAvailablePhysicalModelName()
{
  int i = 0;
  String rootName = "physicalModel_";
  while (hasPhysicalModelNamed(rootName + (OSS()<<i).str()))
    ++i;
  return rootName + (OSS()<<i).str();
}


void OTStudy::addPhysicalModel(const PhysicalModel & physicalModel)
{
  if (hasPhysicalModelNamed(physicalModel.getName()))
    throw InvalidArgumentException(HERE) << "The study has already contained a physical model named " << physicalModel.getName();

  physicalModels_.add(physicalModel);
  notify("addPhysicalModel");
}


Collection<DesignOfExperiment> OTStudy::getDesignOfExperiments() const
{
  return designOfExperiments_;
}


bool OTStudy::hasDesignOfExperimentNamed(const String & designOfExperimentName)
{
  for (UnsignedInteger i=0; i<designOfExperiments_.getSize(); ++i)
    if (designOfExperiments_[i].getImplementation()->getName() == designOfExperimentName)
      return true;
  return false;
}


String OTStudy::getAvailableDesignOfExperimentName()
{
  int i = 0;
  String rootName = "design_";
  while (hasDesignOfExperimentNamed(rootName + (OSS()<<i).str()))
    ++i;
  return rootName + (OSS()<<i).str();
}


void OTStudy::addDesignOfExperiment(const DesignOfExperiment & designOfExperiment)
{
  if (hasDesignOfExperimentNamed(designOfExperiment.getName()))
    throw InvalidArgumentException(HERE) << "The study has already contained a design of experiment named " << designOfExperiment.getName();

  if (!hasPhysicalModelNamed(designOfExperiment.getPhysicalModel().getName()))
    throw InvalidArgumentException(HERE) << "The design of experiment has been created with a physical model not belonging to the study.";

  designOfExperiments_.add(designOfExperiment);
  notify("addDesignOfExperiment");
}


Collection<Analysis> OTStudy::getAnalyses() const
{
  return analyses_;
}

Analysis & OTStudy::getAnalysisByName(const String & analysisName)
{
  for (UnsignedInteger i=0; i<analyses_.getSize(); ++i)
    if (analyses_[i].getName() == analysisName)
      return analyses_[i];
  throw InvalidArgumentException(HERE) << "The given name " << analysisName <<" does not correspond to an analysis of the study.\n"; 
}


bool OTStudy::hasAnalysisNamed(const String & analysisName)
{
  for (UnsignedInteger i=0; i<analyses_.getSize(); ++i)
    if (analyses_[i].getImplementation()->getName() == analysisName)
      return true;
  return false;
}


String OTStudy::getAvailableAnalysisName(const String & rootName)
{
  int i = 0;
  while (hasAnalysisNamed(rootName + (OSS()<<i).str()))
    ++i;
  return rootName + (OSS()<<i).str();
}


void OTStudy::addAnalysis(const Analysis & analysis)
{
  if (hasAnalysisNamed(analysis.getName()))
    throw InvalidArgumentException(HERE) << "The study has already contained an analysis named " << analysis.getName();

  if (!hasPhysicalModelNamed(analysis.getPhysicalModel().getName()))
    throw InvalidArgumentException(HERE) << "The analysis has been created with a physical model not belonging to the study.";

  if (analysis.isReliabilityAnalysis())
    if (!hasLimitStateNamed(dynamic_cast<const ReliabilityAnalysis*>(&*analysis.getImplementation())->getLimitState().getName()))
      throw InvalidArgumentException(HERE) << "The analysis has been created with a limit state not belonging to the study.";

  analyses_.add(analysis);
  notify("addAnalysis");
}


void OTStudy::removeAnalysis(const Analysis & analysis)
{
  analyses_.erase(std::remove(analyses_.begin(), analyses_.end(), analysis), analyses_.end());
  analysis.getImplementation().get()->notify("analysisRemoved");
}


Collection<LimitState> OTStudy::getLimitStates() const
{
  return limitStates_;
}


bool OTStudy::hasLimitStateNamed(const String & limitStateName)
{
  for (UnsignedInteger i=0; i<limitStates_.getSize(); ++i)
    if (limitStates_[i].getImplementation()->getName() == limitStateName)
      return true;
  return false;
}


String OTStudy::getAvailableLimitStateName()
{
  int i = 0;
  String rootName = "limitState_";
  while (hasLimitStateNamed(rootName + (OSS()<<i).str()))
    ++i;
  return rootName + (OSS()<<i).str();
}


void OTStudy::addLimitState(const LimitState & limitState)
{
  if (hasLimitStateNamed(limitState.getName()))
    throw InvalidArgumentException(HERE) << "The study has already contained a limit state named " << limitState.getName();

  if (!hasPhysicalModelNamed(limitState.getPhysicalModel().getName()))
    throw InvalidArgumentException(HERE) << "The limit state has been created with a physical model not belonging to the study.";

  limitStates_.add(limitState);
  notify("addLimitState");
}


String OTStudy::dump()
{
  String result;
  result += getName() + " = otguibase.OTStudy('" + getName() + "')\n";
  result += "otguibase.OTStudy.AddOTStudy(" + getName() + ")\n";
  for (Collection<PhysicalModel>::iterator it=physicalModels_.begin(); it!= physicalModels_.end(); ++it)
  {
    result += (*it).dump();
    result += getName() + ".addPhysicalModel(" + (*it).getName() + ")\n";
  }
  for (Collection<DesignOfExperiment>::iterator it=designOfExperiments_.begin(); it!= designOfExperiments_.end(); ++it)
  {
    result += (*it).dump();
    result += getName() + ".addDesignOfExperiment(" + (*it).getName() + ")\n";
  }
  for (Collection<LimitState>::iterator it=limitStates_.begin(); it!= limitStates_.end(); ++it)
  {
    result += (*it).dump();
    result += getName() + ".addLimitState(" + (*it).getName() + ")\n";
  }
  for (Collection<Analysis>::iterator it=analyses_.begin(); it!= analyses_.end(); ++it)
  {
    result += (*it).dump();
    result += getName() + ".addAnalysis(" + (*it).getName() + ")\n";
  }
  return result;
}


void OTStudy::save(const String & xmlFileName)
{
  Study study;
  study.setStorageManager(XMLStorageManager(xmlFileName));
  study.add("otstudy", this->clone());
  study.save();
  setFileName(xmlFileName);
}


/* Method save() stores the object through the StorageManager */
void OTStudy::save(Advocate & adv) const
{
  PersistentObject::save(adv);
  adv.saveAttribute("physicalModels_", physicalModels_);
  adv.saveAttribute("designOfExperiments_", designOfExperiments_);
  adv.saveAttribute("limitStates_", limitStates_);
  adv.saveAttribute("analyses_", analyses_);
}


/* Method load() reloads the object from the StorageManager */
void OTStudy::load(Advocate & adv)
{
  PersistentObject::load(adv);
  adv.loadAttribute("physicalModels_", physicalModels_);
  adv.loadAttribute("designOfExperiments_", designOfExperiments_);
  adv.loadAttribute("limitStates_", limitStates_);
  adv.loadAttribute("analyses_", analyses_);
}
}