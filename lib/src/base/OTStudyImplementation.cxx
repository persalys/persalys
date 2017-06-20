//                                               -*- C++ -*-
/**
 *  @brief Class defining the otgui studies
 *
 *  Copyright 2015-2017 EDF-Phimeca
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
#include "otgui/OTStudyImplementation.hxx"

#include "otgui/ReliabilityAnalysis.hxx"

#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/Study.hxx"
#include "openturns/XMLStorageManager.hxx"

using namespace OT;

namespace OTGUI {

CLASSNAMEINIT(OTStudyImplementation);

static Factory<OTStudyImplementation> RegisteredFactory;

static Factory<PersistentCollection<DesignOfExperiment> > RegisteredFactory_CollDM;
static Factory<PersistentCollection<PhysicalModel> > RegisteredFactory_CollPM;
static Factory<PersistentCollection<DesignOfExperiment> > RegisteredFactory_CollDOE;
static Factory<PersistentCollection<LimitState> > RegisteredFactory_CollLS;
static Factory<PersistentCollection<Analysis> > RegisteredFactory_CollAn;


/* Default constructor */
OTStudyImplementation::OTStudyImplementation(const String& name)
  : PersistentObject()
  , Observable()
{
  setName(name);
}


OTStudyImplementation::~OTStudyImplementation()
{
  analyses_.clear();
  limitStates_.clear();
  designOfExperiments_.clear();
  physicalModels_.clear();
  dataModels_.clear();
}


/* Virtual constructor */
OTStudyImplementation* OTStudyImplementation::clone() const
{
  return new OTStudyImplementation(*this);
}


/* Comparison operator */
Bool OTStudyImplementation::operator ==(const OTStudyImplementation& other) const
{
  if (this == &other)
    return true;
  return false;
}


/* Comparison operator */
Bool OTStudyImplementation::operator !=(const OTStudyImplementation& other) const
{
  return !operator==(other);
}


void OTStudyImplementation::clear()
{
  // remove all the analyses
  for (UnsignedInteger i=0; i<analyses_.getSize(); ++i)
  {
    analyses_[i].getImplementation().get()->notify("analysisRemoved");
  }

  // remove all the limit states
  for (UnsignedInteger i=0; i<limitStates_.getSize(); ++i)
    limitStates_[i].getImplementation().get()->notify("limitStateRemoved");

  // remove all the design Of Experiments
  for (UnsignedInteger i=0; i<designOfExperiments_.getSize(); ++i)
  {
    designOfExperiments_[i].getImplementation().get()->notifyAndRemove("analysisRemoved", "Analysis");
    designOfExperiments_[i].getImplementation().get()->notify("designOfExperimentRemoved");
  }

  // remove all the physical models
  for (UnsignedInteger i=0; i<physicalModels_.getSize(); ++i)
  {
    physicalModels_[i].getImplementation().get()->notifyAndRemove("probabilisticModelRemoved", "ProbabilisticModel");
    physicalModels_[i].getImplementation().get()->notifyAndRemove("physicalModelRemoved", "PhysicalModelDefinition");
    physicalModels_[i].getImplementation().get()->notifyAndRemove("physicalModelRemoved", "PhysicalModelDiagram");
  }

  // remove all the datamodels
  for (UnsignedInteger i=0; i<dataModels_.getSize(); ++i)
  {
    dataModels_[i].getImplementation().get()->notifyAndRemove("designOfExperimentRemoved", "DataModelDefinition");
    dataModels_[i].getImplementation().get()->notifyAndRemove("designOfExperimentRemoved", "DataModelDiagram");
  }
}


String OTStudyImplementation::getFileName() const
{
  return fileName_;
}


void OTStudyImplementation::setFileName(const String& fileName)
{
  fileName_ = fileName;
  notify("fileNameChanged");
}


// ----- DATA MODEL -----
Collection< DesignOfExperiment > OTStudyImplementation::getDataModels() const
{
  return dataModels_;
}


DesignOfExperiment& OTStudyImplementation::getDataModelByName(const String& dataModelName)
{
  for (UnsignedInteger i=0; i<dataModels_.getSize(); ++i)
    if (dataModels_[i].getName() == dataModelName)
      return dataModels_[i];
  throw InvalidArgumentException(HERE) << "The given name " << dataModelName <<" does not correspond to a data model of the study.\n"; 
}


bool OTStudyImplementation::hasDataModelNamed(const String& dataModelName) const
{
  for (UnsignedInteger i=0; i<dataModels_.getSize(); ++i)
    if (dataModels_[i].getName() == dataModelName)
      return true;
  return false;
}


String OTStudyImplementation::getAvailableDataModelName() const
{
  int i = 0;
  String rootName = "dataModel_";
  while (hasDataModelNamed(rootName + (OSS()<<i).str()))
    ++i;
  return rootName + (OSS()<<i).str();
}


void OTStudyImplementation::clear(const DesignOfExperiment& designOfExperiment)
{
  // remove all the analyses
  PersistentCollection<Analysis>::iterator iterAnalysis = analyses_.begin();
  while (iterAnalysis != analyses_.end())
  {
    if ((*iterAnalysis).getModelName() == designOfExperiment.getName())
    {
      (*iterAnalysis).getImplementation().get()->notify("analysisRemoved");
      iterAnalysis = analyses_.erase(iterAnalysis);
    }
    else
    {
      ++iterAnalysis;
    }
  }
}


// ----- PHYSICAL MODEL -----
Collection<PhysicalModel> OTStudyImplementation::getPhysicalModels() const
{
  return physicalModels_;
}


PhysicalModel& OTStudyImplementation::getPhysicalModelByName(const String& physicalModelName)
{
  for (UnsignedInteger i=0; i<physicalModels_.getSize(); ++i)
    if (physicalModels_[i].getName() == physicalModelName)
      return physicalModels_[i];
  throw InvalidArgumentException(HERE) << "The given name " << physicalModelName <<" does not correspond to a physical model of the study.\n"; 
}


bool OTStudyImplementation::hasPhysicalModelNamed(const String& physicalModelName) const
{
  for (UnsignedInteger i=0; i<physicalModels_.getSize(); ++i)
    if (physicalModels_[i].getName() == physicalModelName)
      return true;
  return false;
}


String OTStudyImplementation::getAvailablePhysicalModelName(const OT::String& physicalModelRootName) const
{
  int i = 0;
  while (hasPhysicalModelNamed(physicalModelRootName + (OSS()<<i).str()))
    ++i;
  return physicalModelRootName + (OSS()<<i).str();
}


void OTStudyImplementation::add(const PhysicalModel& physicalModel)
{
  if (hasPhysicalModelNamed(physicalModel.getName()))
    throw InvalidArgumentException(HERE) << "The study already contains a physical model named " << physicalModel.getName();

  physicalModels_.add(physicalModel);
  notify("addPhysicalModel");
}


void OTStudyImplementation::clear(const PhysicalModel& physicalModel)
{
  // remove all the limit states
  PersistentCollection<LimitState>::iterator iter = limitStates_.begin();
  while (iter != limitStates_.end())
  {
    if ((*iter).getPhysicalModel().getName() == physicalModel.getName())
    {
      clear(*iter);
      physicalModel.getImplementation().get()->removeObserver((*iter).getImplementation().get()->getObserver("LimitState"));
      (*iter).getImplementation().get()->notify("limitStateRemoved");
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
    if ((*iterAnalysis).getModelName() == physicalModel.getName())
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
      clear(*iterDOE);
      (*iterDOE).getImplementation().get()->notifyAndRemove("analysisRemoved", "Analysis");
      (*iterDOE).getImplementation().get()->notify("designOfExperimentRemoved");
      iterDOE = designOfExperiments_.erase(iterDOE);
    }
    else
    {
      ++iterDOE;
    }
  }
}


void OTStudyImplementation::remove(const PhysicalModel& physicalModel)
{
  clear(physicalModel);

  physicalModel.getImplementation().get()->notifyAndRemove("probabilisticModelRemoved", "ProbabilisticModel");
  physicalModel.getImplementation().get()->notifyAndRemove("physicalModelRemoved", "PhysicalModelDefinition");
  physicalModel.getImplementation().get()->notifyAndRemove("physicalModelRemoved", "PhysicalModelDiagram");

  for (UnsignedInteger i=0; i<physicalModels_.getSize(); ++i)
    if (physicalModels_[i].getName() == physicalModel.getName())
    {
      physicalModels_.erase(physicalModels_.begin() + i);
      break;
    }
}


// ----- DESIGN OF EXPERIMENT -----
Collection<DesignOfExperiment> OTStudyImplementation::getDesignOfExperiments() const
{
  return designOfExperiments_;
}


DesignOfExperiment& OTStudyImplementation::getDesignOfExperimentByName(const String& designOfExperimentName)
{
  for (UnsignedInteger i=0; i<designOfExperiments_.getSize(); ++i)
    if (designOfExperiments_[i].getName() == designOfExperimentName)
      return designOfExperiments_[i];
  throw InvalidArgumentException(HERE) << "The given name " << designOfExperimentName <<" does not correspond to an design of experiments of the study.\n"; 
}


bool OTStudyImplementation::hasDesignOfExperimentNamed(const String& designOfExperimentName) const
{
  for (UnsignedInteger i=0; i<designOfExperiments_.getSize(); ++i)
    if (designOfExperiments_[i].getName() == designOfExperimentName)
      return true;
  return false;
}


String OTStudyImplementation::getAvailableDesignOfExperimentName() const
{
  int i = 0;
  String rootName = "design_";
  while (hasDesignOfExperimentNamed(rootName + (OSS()<<i).str()))
    ++i;
  return rootName + (OSS()<<i).str();
}


void OTStudyImplementation::add(const DesignOfExperiment& designOfExperiment)
{
  if (designOfExperiment.hasPhysicalModel())
  {
    if (hasDesignOfExperimentNamed(designOfExperiment.getName()))
      throw InvalidArgumentException(HERE) << "The study already contains a design of experiment named " << designOfExperiment.getName();

    if (!hasPhysicalModelNamed(designOfExperiment.getPhysicalModel().getName()))
      throw InvalidArgumentException(HERE) << "The design of experiment has been created with a physical model not belonging to the study.";

    designOfExperiments_.add(designOfExperiment);
    notify("addDesignOfExperiment");
  }
  else
  {
    if (hasDataModelNamed(designOfExperiment.getName()))
      throw InvalidArgumentException(HERE) << "The study already contains a data model named " << designOfExperiment.getName();

    dataModels_.add(designOfExperiment);
    notify("addDataModel");
  }
}


void OTStudyImplementation::remove(const DesignOfExperiment& designOfExperiment)
{
  clear(designOfExperiment);

  if (designOfExperiment.hasPhysicalModel()) // design of experiment
  {
    for (UnsignedInteger i=0; i<designOfExperiments_.getSize(); ++i)
      if (designOfExperiments_[i].getName() == designOfExperiment.getName())
      {
        designOfExperiments_.erase(designOfExperiments_.begin() + i);
        break;
      }
    designOfExperiment.getImplementation().get()->notifyAndRemove("analysisRemoved", "Analysis");
  }
  else // datamodel
  {
    for (UnsignedInteger i=0; i<dataModels_.getSize(); ++i)
    {
      if (dataModels_[i].getName() == designOfExperiment.getName())
      {
        dataModels_.erase(dataModels_.begin() + i);
        break;
      }
    }
    designOfExperiment.getImplementation().get()->notifyAndRemove("designOfExperimentRemoved", "DataModelDefinition");
  }
  designOfExperiment.getImplementation().get()->notify("designOfExperimentRemoved");
}


// ----- ANALYSIS -----
Collection<Analysis> OTStudyImplementation::getAnalyses() const
{
  return analyses_;
}


Analysis& OTStudyImplementation::getAnalysisByName(const String& analysisName)
{
  for (UnsignedInteger i=0; i<analyses_.getSize(); ++i)
    if (analyses_[i].getName() == analysisName)
      return analyses_[i];
  throw InvalidArgumentException(HERE) << "The given name " << analysisName <<" does not correspond to an analysis of the study.\n"; 
}


bool OTStudyImplementation::hasAnalysisNamed(const String& analysisName) const
{
  for (UnsignedInteger i=0; i<analyses_.getSize(); ++i)
    if (analyses_[i].getName() == analysisName)
      return true;
  return false;
}


String OTStudyImplementation::getAvailableAnalysisName(const String& rootName) const
{
  int i = 0;
  while (hasAnalysisNamed(rootName + (OSS()<<i).str()))
    ++i;
  return rootName + (OSS()<<i).str();
}


void OTStudyImplementation::add(const Analysis& analysis)
{
  if (hasAnalysisNamed(analysis.getName()))
    throw InvalidArgumentException(HERE) << "The study already contains an analysis named " << analysis.getName();

  if (dynamic_cast<const PhysicalModelAnalysis*>(&*analysis.getImplementation()))
    if (!hasPhysicalModelNamed(analysis.getModelName()))
      throw InvalidArgumentException(HERE) << "The analysis has been created with a physical model not belonging to the study.";

  if (analysis.isReliabilityAnalysis())
    if (!hasLimitStateNamed(dynamic_cast<const ReliabilityAnalysis*>(&*analysis.getImplementation())->getLimitState().getName()))
      throw InvalidArgumentException(HERE) << "The analysis has been created with a limit state not belonging to the study.";

  analyses_.add(analysis);
  notify("addAnalysis");
}


void OTStudyImplementation::remove(const Analysis& analysis)
{
  for (UnsignedInteger i=0; i<analyses_.getSize(); ++i)
    if (analyses_[i].getName() == analysis.getName())
    {
      analyses_.erase(analyses_.begin() + i);
      break;
    }
  analysis.getImplementation().get()->notify("analysisRemoved");
}


// ----- LIMIT STATE -----
Collection<LimitState> OTStudyImplementation::getLimitStates() const
{
  return limitStates_;
}


bool OTStudyImplementation::hasLimitStateNamed(const String& limitStateName) const
{
  for (UnsignedInteger i=0; i<limitStates_.getSize(); ++i)
    if (limitStates_[i].getName() == limitStateName)
      return true;
  return false;
}


String OTStudyImplementation::getAvailableLimitStateName() const
{
  int i = 0;
  String rootName = "limitState_";
  while (hasLimitStateNamed(rootName + (OSS()<<i).str()))
    ++i;
  return rootName + (OSS()<<i).str();
}


void OTStudyImplementation::add(const LimitState& limitState)
{
  if (hasLimitStateNamed(limitState.getName()))
    throw InvalidArgumentException(HERE) << "The study already contains a limit state named " << limitState.getName();

  if (!hasPhysicalModelNamed(limitState.getPhysicalModel().getName()))
    throw InvalidArgumentException(HERE) << "The limit state has been created with a physical model not belonging to the study.";

  limitStates_.add(limitState);
  notify("addLimitState");
}


void OTStudyImplementation::clear(const LimitState& limitState)
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


void OTStudyImplementation::remove(const LimitState& limitState)
{
  // remove the analyses based on the limitState
  clear(limitState);

  // remove the limitState
  for (UnsignedInteger i=0; i<limitStates_.getSize(); ++i)
    if (limitStates_[i].getName() == limitState.getName())
    {
      limitStates_[i].getPhysicalModel().getImplementation().get()->removeObserver(limitStates_[i].getImplementation().get()->getObserver("LimitState"));

      limitStates_.erase(limitStates_.begin() + i);
      break;
    }
  limitState.getImplementation().get()->notify("limitStateRemoved");
}


String OTStudyImplementation::getPythonScript()
{
  String result;

  result += "#!/usr/bin/env python\n# coding: utf-8\n\nfrom __future__ import print_function\nimport openturns as ot\nimport otguibase\n\n";

  result += getName() + " = otguibase.OTStudy('" + getName() + "')\n";
  result += "otguibase.OTStudy.Add(" + getName() + ")\n";

  for (Collection<DesignOfExperiment>::iterator it=dataModels_.begin(); it!= dataModels_.end(); ++it)
  {
    result += (*it).getPythonScript();
    result += getName() + ".add(" + (*it).getName() + ")\n";
  }

  for (Collection<PhysicalModel>::iterator it=physicalModels_.begin(); it!= physicalModels_.end(); ++it)
  {
    if ((*it).getImplementation()->getClassName() != "MetaModel")
    {
      result += (*it).getPythonScript();
      result += getName() + ".add(" + (*it).getName() + ")\n";
    }
  }
  for (Collection<DesignOfExperiment>::iterator it=designOfExperiments_.begin(); it!= designOfExperiments_.end(); ++it)
  {
    result += (*it).getPythonScript();
    result += getName() + ".add(" + (*it).getName() + ")\n";
    if ((*it).getOutputSample().getSize())
      result += (*it).getName() + ".run()\n";
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


void OTStudyImplementation::save(const String& xmlFileName)
{
  Study study;
  study.setStorageManager(XMLStorageManager(xmlFileName));
  study.add("otstudy", this->clone());
  study.save();
  setFileName(xmlFileName);
}


/* Method save() stores the object through the StorageManager */
void OTStudyImplementation::save(Advocate& adv) const
{
  PersistentObject::save(adv);
  adv.saveAttribute("dataModels_", dataModels_);
  adv.saveAttribute("physicalModels_", physicalModels_);
  adv.saveAttribute("designOfExperiments_", designOfExperiments_);
  adv.saveAttribute("limitStates_", limitStates_);
  adv.saveAttribute("analyses_", analyses_);
}


/* Method load() reloads the object from the StorageManager */
void OTStudyImplementation::load(Advocate& adv)
{
  PersistentObject::load(adv);
  adv.loadAttribute("dataModels_", dataModels_);
  adv.loadAttribute("physicalModels_", physicalModels_);
  adv.loadAttribute("designOfExperiments_", designOfExperiments_);
  adv.loadAttribute("limitStates_", limitStates_);
  adv.loadAttribute("analyses_", analyses_);
}
}