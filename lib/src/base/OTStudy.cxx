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


Collection<OTStudy*> OTStudy::GetInstances()
{
  return OTStudies_;
}


Description OTStudy::GetFileNames()
{
  return OTStudiesFileNames_;
}


OTStudy * OTStudy::GetInstanceByName(const String & otStudyName)
{
  for (Collection<OTStudy*>::iterator it=OTStudies_.begin(); it!=OTStudies_.end(); ++it)
    if ((*it)->getName() == otStudyName)
      return (*it);
  return 0;
}


bool OTStudy::HasInstanceNamed(const String & otStudyName)
{
  for (Collection<OTStudy*>::iterator it=OTStudies_.begin(); it!=OTStudies_.end(); ++it)
    if ((*it)->getName() == otStudyName)
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


void OTStudy::Add(OTStudy * otstudy)
{
  OTStudies_.add(otstudy);
  OTStudiesFileNames_.add(otstudy->getFileName());
  if (OTStudyObserver_)
    OTStudyObserver_->update(otstudy, "addStudy");
}


void OTStudy::Remove(OTStudy * otstudy)
{
  PersistentCollection<PhysicalModel>::iterator iter = otstudy->physicalModels_.begin();
  while (iter != otstudy->physicalModels_.end())
  {
    otstudy->clear(*iter);
    (*iter).getImplementation().get()->notify("physicalModelRemoved");
    iter = otstudy->physicalModels_.erase(iter);
  }
  OTStudiesFileNames_.erase(std::remove(OTStudiesFileNames_.begin(), OTStudiesFileNames_.end(), otstudy->getFileName()), OTStudiesFileNames_.end());
  OTStudies_.erase(std::remove(OTStudies_.begin(), OTStudies_.end(), otstudy), OTStudies_.end());
  otstudy->notify("otStudyRemoved");
}


void OTStudy::Open(const String & xmlFileName)
{
  Study study;
  study.setStorageManager(XMLStorageManager(xmlFileName));
  study.load();
  OTStudy * openedStudy = new OTStudy;
  study.fillObject("otstudy", *openedStudy);
  openedStudy->setFileName(xmlFileName);
  Add(openedStudy);
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


String OTStudy::getFileName() const
{
  return fileName_;
}


void OTStudy::setFileName(const String& fileName)
{
  fileName_ = fileName;
  notify("fileNameChanged");
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


bool OTStudy::hasPhysicalModelNamed(const String & physicalModelName) const
{
  for (UnsignedInteger i=0; i<physicalModels_.getSize(); ++i)
    if (physicalModels_[i].getImplementation()->getName() == physicalModelName)
      return true;
  return false;
}


String OTStudy::getAvailablePhysicalModelName() const
{
  int i = 0;
  String rootName = "physicalModel_";
  while (hasPhysicalModelNamed(rootName + (OSS()<<i).str()))
    ++i;
  return rootName + (OSS()<<i).str();
}


void OTStudy::add(const PhysicalModel & physicalModel)
{
  if (hasPhysicalModelNamed(physicalModel.getName()))
    throw InvalidArgumentException(HERE) << "The study has already contained a physical model named " << physicalModel.getName();

  physicalModels_.add(physicalModel);
  notify("addPhysicalModel");
}


void OTStudy::clear(const PhysicalModel & physicalModel)
{
  // remove all the limit states
  PersistentCollection<LimitState>::iterator iter = limitStates_.begin();
  while (iter != limitStates_.end())
  {
    if ((*iter).getPhysicalModel().getName() == physicalModel.getName())
    {
      clear(*iter);
      (*iter).getImplementation().get()->notify("limitStateRemoved");

      if ((*iter).getImplementation().get()->getObservers().size())
        physicalModel.getImplementation().get()->removeObserver((*iter).getImplementation().get()->getObservers()[0]);
      iter = limitStates_.erase(iter);
    }
    else
    {
      ++iter;
    }
  }

  // remove all the analyses
  PersistentCollection<Analysis>::iterator iterAnalysis = analyses_.begin();
  while (iterAnalysis != analyses_.end())
  {
    if ((*iterAnalysis).getPhysicalModel().getName() == physicalModel.getName())
    {
      (*iterAnalysis).getImplementation().get()->notify("analysisRemoved");
      iterAnalysis = analyses_.erase(iterAnalysis);
    }
    else
    {
      ++iterAnalysis;
    }
  }

  // remove all the design Of Experiments
  PersistentCollection<DesignOfExperiment>::iterator iterDOE = designOfExperiments_.begin();
  while (iterDOE != designOfExperiments_.end())
  {
    if ((*iterDOE).getPhysicalModel().getName() == physicalModel.getName())
    {
      (*iterDOE).getImplementation().get()->notify("designOfExperimentRemoved");
      iterDOE = designOfExperiments_.erase(iterDOE);
    }
    else
    {
      ++iterDOE;
    }
  }
}


void OTStudy::remove(const PhysicalModel & physicalModel)
{
  clear(physicalModel);

  for (UnsignedInteger i=0; i<physicalModels_.getSize(); ++i)
    if (physicalModels_[i].getName() == physicalModel.getName())
    {
      physicalModels_.erase(physicalModels_.begin() + i);
      break;
    }

  physicalModel.getImplementation().get()->notify("physicalModelRemoved");
}


Collection<DesignOfExperiment> OTStudy::getDesignOfExperiments() const
{
  return designOfExperiments_;
}


bool OTStudy::hasDesignOfExperimentNamed(const String & designOfExperimentName) const
{
  for (UnsignedInteger i=0; i<designOfExperiments_.getSize(); ++i)
    if (designOfExperiments_[i].getImplementation()->getName() == designOfExperimentName)
      return true;
  return false;
}


String OTStudy::getAvailableDesignOfExperimentName() const
{
  int i = 0;
  String rootName = "design_";
  while (hasDesignOfExperimentNamed(rootName + (OSS()<<i).str()))
    ++i;
  return rootName + (OSS()<<i).str();
}


void OTStudy::add(const DesignOfExperiment & designOfExperiment)
{
  if (hasDesignOfExperimentNamed(designOfExperiment.getName()))
    throw InvalidArgumentException(HERE) << "The study has already contained a design of experiment named " << designOfExperiment.getName();

  if (!hasPhysicalModelNamed(designOfExperiment.getPhysicalModel().getName()))
    throw InvalidArgumentException(HERE) << "The design of experiment has been created with a physical model not belonging to the study.";

  designOfExperiments_.add(designOfExperiment);
  notify("addDesignOfExperiment");
}


void OTStudy::remove(const DesignOfExperiment & designOfExperiment)
{
  for (UnsignedInteger i=0; i<designOfExperiments_.getSize(); ++i)
    if (designOfExperiments_[i].getName() == designOfExperiment.getName())
    {
      designOfExperiments_.erase(designOfExperiments_.begin() + i);
      break;
    }

  designOfExperiment.getImplementation().get()->notify("designOfExperimentRemoved");
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


bool OTStudy::hasAnalysisNamed(const String & analysisName) const
{
  for (UnsignedInteger i=0; i<analyses_.getSize(); ++i)
    if (analyses_[i].getImplementation()->getName() == analysisName)
      return true;
  return false;
}


String OTStudy::getAvailableAnalysisName(const String & rootName) const
{
  int i = 0;
  while (hasAnalysisNamed(rootName + (OSS()<<i).str()))
    ++i;
  return rootName + (OSS()<<i).str();
}


void OTStudy::add(const Analysis & analysis)
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


void OTStudy::remove(const Analysis & analysis)
{
  for (UnsignedInteger i=0; i<analyses_.getSize(); ++i)
    if (analyses_[i].getName() == analysis.getName())
    {
      analyses_.erase(analyses_.begin() + i);
      break;
    }

  analysis.getImplementation().get()->notify("analysisRemoved");
}


Collection<LimitState> OTStudy::getLimitStates() const
{
  return limitStates_;
}


bool OTStudy::hasLimitStateNamed(const String & limitStateName) const
{
  for (UnsignedInteger i=0; i<limitStates_.getSize(); ++i)
    if (limitStates_[i].getImplementation()->getName() == limitStateName)
      return true;
  return false;
}


String OTStudy::getAvailableLimitStateName() const
{
  int i = 0;
  String rootName = "limitState_";
  while (hasLimitStateNamed(rootName + (OSS()<<i).str()))
    ++i;
  return rootName + (OSS()<<i).str();
}


void OTStudy::add(const LimitState & limitState)
{
  if (hasLimitStateNamed(limitState.getName()))
    throw InvalidArgumentException(HERE) << "The study has already contained a limit state named " << limitState.getName();

  if (!hasPhysicalModelNamed(limitState.getPhysicalModel().getName()))
    throw InvalidArgumentException(HERE) << "The limit state has been created with a physical model not belonging to the study.";

  limitStates_.add(limitState);
  notify("addLimitState");
}


void OTStudy::clear(const LimitState & limitState)
{
  // remove the analyses based on the limitState
  PersistentCollection<Analysis>::iterator iter = analyses_.begin();
  while (iter != analyses_.end())
  {
    if ((*iter).isReliabilityAnalysis())
    {
      if (dynamic_cast<ReliabilityAnalysis*>(&*(*iter).getImplementation())->getLimitState().getName() == limitState.getName())
      {
        (*iter).getImplementation().get()->notify("analysisRemoved");
        iter = analyses_.erase(iter);
      }
      else
        ++iter;
    }
    else
    {
      ++iter;
    }
  }
}


void OTStudy::remove(const LimitState & limitState)
{
  // remove the analyses based on the limitState
  clear(limitState);

  // remove the limitState
  for (UnsignedInteger i=0; i<limitStates_.getSize(); ++i)
    if (limitStates_[i].getName() == limitState.getName())
    {
      if (limitStates_[i].getImplementation().get()->getObservers().size())
          limitStates_[i].getPhysicalModel().getImplementation().get()->removeObserver(limitStates_[i].getImplementation().get()->getObservers()[0]);

      limitStates_.erase(limitStates_.begin() + i);
      break;
    }

  limitState.getImplementation().get()->notify("limitStateRemoved");
}


String OTStudy::getPythonScript()
{
  String result;

  result += "#!/usr/bin/env python\n# coding: utf-8\n\nfrom __future__ import print_function\nimport openturns as ot\nimport otguibase\n\n";

  result += getName() + " = otguibase.OTStudy('" + getName() + "')\n";
  result += "otguibase.OTStudy.Add(" + getName() + ")\n";

  for (Collection<PhysicalModel>::iterator it=physicalModels_.begin(); it!= physicalModels_.end(); ++it)
  {
    result += (*it).getPythonScript();
    result += getName() + ".add(" + (*it).getName() + ")\n";
  }
  for (Collection<DesignOfExperiment>::iterator it=designOfExperiments_.begin(); it!= designOfExperiments_.end(); ++it)
  {
    result += (*it).getPythonScript();
    result += getName() + ".add(" + (*it).getName() + ")\n";
  }
  for (Collection<LimitState>::iterator it=limitStates_.begin(); it!= limitStates_.end(); ++it)
  {
    result += (*it).getPythonScript();
    result += getName() + ".add(" + (*it).getName() + ")\n";
  }
  for (Collection<Analysis>::iterator it=analyses_.begin(); it!= analyses_.end(); ++it)
  {
    result += (*it).getPythonScript();
    result += getName() + ".add(" + (*it).getName() + ")\n";
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
