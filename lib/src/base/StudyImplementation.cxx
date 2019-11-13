//                                               -*- C++ -*-
/**
 *  @brief Class defining the persalys studies
 *
 *  Copyright 2015-2019 EDF-Phimeca
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
#include "persalys/StudyImplementation.hxx"

#include "persalys/ReliabilityAnalysis.hxx"
#include "persalys/DesignOfExperimentAnalysis.hxx"
#include "persalys/DesignOfExperimentEvaluation.hxx"
#include "persalys/CalibrationAnalysis.hxx"

#include <openturns/PersistentObjectFactory.hxx>
#include <openturns/Study.hxx>
#include <openturns/XMLStorageManager.hxx>

using namespace OT;

namespace PERSALYS
{

CLASSNAMEINIT(StudyImplementation)

static Factory<StudyImplementation> Factory_StudyImplementation;

static Factory<PersistentCollection<PhysicalModel> > Factory_PersistentCollection_PhysicalModel;
static Factory<PersistentCollection<DesignOfExperiment> > Factory_PersistentCollection_DesignOfExperiment;
static Factory<PersistentCollection<LimitState> > Factory_PersistentCollection_LimitState;
static Factory<PersistentCollection<Analysis> > Factory_PersistentCollection_Analysis;


/* Default constructor */
StudyImplementation::StudyImplementation(const String& name)
  : PersistentObject()
  , Observable()
  , Observer("Study")
  , modified_(true)
{
  setName(name);
}


StudyImplementation::~StudyImplementation()
{
  analyses_.clear();
  limitStates_.clear();
  physicalModels_.clear();
  dataModels_.clear();
}


/* Virtual constructor */
StudyImplementation* StudyImplementation::clone() const
{
  return new StudyImplementation(*this);
}


void StudyImplementation::setName(const String & name)
{
  PersistentObject::setName(name);
  modified_ = true;
  notify("statusChanged");
}


void StudyImplementation::removeAllObservers()
{
  notifyAndRemove("StudyItem");
}


void StudyImplementation::update(Observable* /*source*/, const String& message)
{
  if (message == "informationMessageUpdated" ||
      message == "progressValueChanged")
    return;

  const bool oldModifiedValue = modified_;
  modified_ = true;

  // notification for StudyItem to update its Icon
  if (modified_ != oldModifiedValue)
    notify("statusChanged");
}


bool StudyImplementation::hasBeenModified() const
{
  return modified_;
}


/* Comparison operator */
Bool StudyImplementation::operator ==(const StudyImplementation& other) const
{
  if (this == &other)
    return true;
  return false;
}


/* Comparison operator */
Bool StudyImplementation::operator !=(const StudyImplementation& other) const
{
  return !operator==(other);
}


void StudyImplementation::clear()
{
  // remove all the analyses
  for (UnsignedInteger i = 0; i < analyses_.getSize(); ++i)
  {
    DesignOfExperimentEvaluation * doeEval = dynamic_cast<DesignOfExperimentEvaluation *>(analyses_[i].getImplementation().get());
    // we do not want to remove the design of experiments for now
    // some analyses can be dependant of the design of experiments: we need to remove them before the design of experiments
    if (!doeEval)
      analyses_[i].getImplementation()->removeAllObservers();
  }
  // remove all the DesignOfExperimentEvaluation
  for (UnsignedInteger i = 0; i < analyses_.getSize(); ++i)
  {
    DesignOfExperimentEvaluation * doeEval = dynamic_cast<DesignOfExperimentEvaluation *>(analyses_[i].getImplementation().get());
    if (doeEval)
      doeEval->removeAllObservers();
  }

  // remove all the limit states
  for (UnsignedInteger i = 0; i < limitStates_.getSize(); ++i)
    limitStates_[i].getImplementation()->removeAllObservers();

  // remove all the datamodels
  for (UnsignedInteger i = 0; i < dataModels_.getSize(); ++i)
    dataModels_[i].getImplementation()->removeAllObservers();

  // remove all the physical models
  for (UnsignedInteger i = 0; i < physicalModels_.getSize(); ++i)
    physicalModels_[i].getImplementation()->removeAllObservers();
}


String StudyImplementation::getFileName() const
{
  return fileName_;
}


void StudyImplementation::setFileName(const String& fileName)
{
  fileName_ = fileName;
}


// ----- DATA MODEL -----
Collection< DesignOfExperiment > StudyImplementation::getDataModels() const
{
  return dataModels_;
}


DesignOfExperiment& StudyImplementation::getDataModelByName(const String& dataModelName)
{
  for (UnsignedInteger i = 0; i < dataModels_.getSize(); ++i)
    if (dataModels_[i].getName() == dataModelName)
      return dataModels_[i];
  throw InvalidArgumentException(HERE) << "The given name " << dataModelName << " does not correspond to a data model of the study.\n";
}


bool StudyImplementation::hasDataModelNamed(const String& dataModelName) const
{
  for (UnsignedInteger i = 0; i < dataModels_.getSize(); ++i)
    if (dataModels_[i].getName() == dataModelName)
      return true;
  return false;
}


String StudyImplementation::getAvailableDataModelName(const String& modelRootName) const
{
  int i = 0;
  while (hasDataModelNamed(modelRootName + (OSS() << i).str()))
    ++i;
  return modelRootName + (OSS() << i).str();
}


void StudyImplementation::add(const DesignOfExperiment& designOfExperiment)
{
  if (hasDataModelNamed(designOfExperiment.getName()))
    throw InvalidArgumentException(HERE) << "The study already contains a data model named " << designOfExperiment.getName();

  dataModels_.add(designOfExperiment);

  designOfExperiment.getImplementation().get()->addObserver(this);
  if (Observer * obs = getObserver("StudyItem"))
    obs->appendItem(designOfExperiment);
  modified_ = true;
  notify("statusChanged");
}


void StudyImplementation::remove(const DesignOfExperiment& designOfExperiment)
{
  if (!dataModels_.contains(designOfExperiment))
    return;

  // remove analyses/observers depending on designOfExperiment
  clear(designOfExperiment);
  designOfExperiment.getImplementation().get()->removeAllObservers();

  dataModels_.erase(std::remove(dataModels_.begin(), dataModels_.end(), designOfExperiment), dataModels_.end());
}


void StudyImplementation::clear(const DesignOfExperiment& designOfExperiment)
{
  // remove all the analyses
  PersistentCollection<Analysis>::iterator iter = analyses_.begin();
  while (iter != analyses_.end())
  {
    DesignOfExperimentAnalysis * analysis_ptr1 = dynamic_cast<DesignOfExperimentAnalysis*>((*iter).getImplementation().get());
    CalibrationAnalysis * analysis_ptr2 = dynamic_cast<CalibrationAnalysis*>((*iter).getImplementation().get());

    if ((analysis_ptr1 && analysis_ptr1->getDesignOfExperiment() == designOfExperiment) ||
        (analysis_ptr2 && analysis_ptr2->getObservations() == designOfExperiment))
    {
      (*iter).getImplementation()->removeAllObservers();
      iter = analyses_.erase(iter);
    }
    else
    {
      ++iter;
    }
  }
}


// ----- PHYSICAL MODEL -----
Collection<PhysicalModel> StudyImplementation::getPhysicalModels() const
{
  return physicalModels_;
}


PhysicalModel& StudyImplementation::getPhysicalModelByName(const String& physicalModelName)
{
  for (UnsignedInteger i = 0; i < physicalModels_.getSize(); ++i)
    if (physicalModels_[i].getName() == physicalModelName)
      return physicalModels_[i];
  throw InvalidArgumentException(HERE) << "The given name " << physicalModelName << " does not correspond to a physical model of the study.\n";
}


bool StudyImplementation::hasPhysicalModelNamed(const String& physicalModelName) const
{
  for (UnsignedInteger i = 0; i < physicalModels_.getSize(); ++i)
    if (physicalModels_[i].getName() == physicalModelName)
      return true;
  return false;
}


String StudyImplementation::getAvailablePhysicalModelName(const String& physicalModelRootName) const
{
  int i = 0;
  while (hasPhysicalModelNamed(physicalModelRootName + (OSS() << i).str()))
    ++i;
  return physicalModelRootName + (OSS() << i).str();
}


void StudyImplementation::add(const PhysicalModel& physicalModel)
{
  if (physicalModels_.contains(physicalModel))
    throw InvalidArgumentException(HERE) << "The study already contains this physical model";

  physicalModels_.add(physicalModel);

  physicalModel.getImplementation().get()->addObserver(this);
  if (Observer * obs = getObserver("StudyItem"))
    obs->appendItem(physicalModel);
  modified_ = true;
  notify("statusChanged");
}


void StudyImplementation::clear(const PhysicalModel& physicalModel)
{
  // remove all the limit states
  PersistentCollection<LimitState>::iterator iter = limitStates_.begin();
  while (iter != limitStates_.end())
  {
    if ((*iter).getPhysicalModel() == physicalModel)
    {
      clear(*iter);
      (*iter).getImplementation()->removeAllObservers();;
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
    PhysicalModelAnalysis * analysis_ptr = dynamic_cast<PhysicalModelAnalysis*>((*iterAnalysis).getImplementation().get());
    if (analysis_ptr)
    {
      if (analysis_ptr->getPhysicalModel() == physicalModel)
      {
        clear(*analysis_ptr);
        analysis_ptr->removeAllObservers();
        iterAnalysis = analyses_.erase(iterAnalysis);
      }
      else
      {
        ++iterAnalysis;
      }
    }
    else
    {
      ++iterAnalysis;
    }
  }
}


void StudyImplementation::remove(const PhysicalModel& physicalModel)
{
  if (!physicalModels_.contains(physicalModel))
    return;

  // remove all does/analyses/observers depending on physicalModel
  clear(physicalModel);
  physicalModel.getImplementation().get()->removeAllObservers();

  physicalModels_.erase(std::remove(physicalModels_.begin(), physicalModels_.end(), physicalModel), physicalModels_.end());
}


// ----- ANALYSIS -----
Collection<Analysis> StudyImplementation::getAnalyses() const
{
  return analyses_;
}


Analysis& StudyImplementation::getAnalysisByName(const String& analysisName)
{
  for (UnsignedInteger i = 0; i < analyses_.getSize(); ++i)
    if (analyses_[i].getName() == analysisName)
      return analyses_[i];
  throw InvalidArgumentException(HERE) << "The given name " << analysisName << " does not correspond to an analysis of the study.\n";
}


bool StudyImplementation::hasAnalysisNamed(const String& analysisName) const
{
  for (UnsignedInteger i = 0; i < analyses_.getSize(); ++i)
    if (analyses_[i].getName() == analysisName)
      return true;
  return false;
}


String StudyImplementation::getAvailableAnalysisName(const String& rootName) const
{
  int i = 0;
  while (hasAnalysisNamed(rootName + (OSS() << i).str()))
    ++i;
  return rootName + (OSS() << i).str();
}


void StudyImplementation::add(const Analysis& analysis)
{
  // check
  if (analyses_.contains(analysis))
    throw InvalidArgumentException(HERE) << "The study already contains this analysis ";

  const PhysicalModelAnalysis * pm_analysis_ptr = dynamic_cast<const PhysicalModelAnalysis*>(analysis.getImplementation().get());
  if (pm_analysis_ptr)
  {
    if (!physicalModels_.contains(pm_analysis_ptr->getPhysicalModel()))
      throw InvalidArgumentException(HERE) << "The analysis has been created with a physical model not belonging to the study.";
    if (analysis.isReliabilityAnalysis() &&
        !limitStates_.contains(dynamic_cast<const ReliabilityAnalysis*>(pm_analysis_ptr)->getLimitState()))
      throw InvalidArgumentException(HERE) << "The analysis has been created with a limit state not belonging to the study.";
  }

  const DesignOfExperimentAnalysis * dm_analysis_ptr = dynamic_cast<const DesignOfExperimentAnalysis*>(analysis.getImplementation().get());
  if (dm_analysis_ptr)
  {
    if (dm_analysis_ptr->getDesignOfExperiment().hasPhysicalModel())
    {
      bool doeFound = false;
      for (UnsignedInteger i = 0; i < analyses_.getSize(); ++i)
      {
        DesignOfExperimentEvaluation * doe_ptr = dynamic_cast<DesignOfExperimentEvaluation*>(analyses_[i].getImplementation().get());
        if (doe_ptr)
        {
          if (doe_ptr->getResult().getDesignOfExperiment() == dm_analysis_ptr->getDesignOfExperiment())
          {
            doeFound = true;
            break;
          }
        }
      }
      if (!doeFound)
        throw InvalidArgumentException(HERE) << "The analysis has been created with a design of experiments not belonging to the study.";
    }
    else
    {
      if (!dataModels_.contains(dm_analysis_ptr->getDesignOfExperiment()))
        throw InvalidArgumentException(HERE) << "The analysis has been created with a data model not belonging to the study.";
    }
  }

  // add analysis
  analyses_.add(analysis);

  analysis.getImplementation().get()->addObserver(this);
  if (Observer * obs = getObserver("StudyItem"))
    obs->appendItem(analysis);
  modified_ = true;
  notify("statusChanged");
}


void StudyImplementation::remove(const Analysis& analysis)
{
  if (!analyses_.contains(analysis))
    return;

  clear(analysis);
  analysis.getImplementation().get()->removeAllObservers();

  analyses_.erase(std::remove(analyses_.begin(), analyses_.end(), analysis), analyses_.end());
}


void StudyImplementation::clear(const Analysis& analysis)
{
  DesignOfExperimentEvaluation * doeEval = dynamic_cast<DesignOfExperimentEvaluation *>(analysis.getImplementation().get());
  if (!doeEval)
    return;

  // remove all the analyses
  PersistentCollection<Analysis>::iterator iterAnalysis = analyses_.begin();
  while (iterAnalysis != analyses_.end())
  {
    DesignOfExperimentAnalysis * analysis_ptr = dynamic_cast<DesignOfExperimentAnalysis*>((*iterAnalysis).getImplementation().get());
    if (analysis_ptr)
    {
      if (analysis_ptr->getDesignOfExperiment() == doeEval->getResult().getDesignOfExperiment())
      {
        analysis_ptr->removeAllObservers();
        iterAnalysis = analyses_.erase(iterAnalysis);
      }
      else
      {
        ++iterAnalysis;
      }
    }
    else
    {
      ++iterAnalysis;
    }
  }
}


// ----- LIMIT STATE -----
Collection<LimitState> StudyImplementation::getLimitStates() const
{
  return limitStates_;
}


bool StudyImplementation::hasLimitStateNamed(const String& limitStateName) const
{
  for (UnsignedInteger i = 0; i < limitStates_.getSize(); ++i)
    if (limitStates_[i].getName() == limitStateName)
      return true;
  return false;
}


String StudyImplementation::getAvailableLimitStateName(const String & rootName) const
{
  int i = 0;
  while (hasLimitStateNamed(rootName + (OSS() << i).str()))
    ++i;
  return rootName + (OSS() << i).str();
}


void StudyImplementation::add(const LimitState& limitState)
{
  if (limitStates_.contains(limitState))
    throw InvalidArgumentException(HERE) << "The study already contains this limit state";;

  if (!physicalModels_.contains(limitState.getPhysicalModel()))
    throw InvalidArgumentException(HERE) << "The limit state has been created with a physical model not belonging to the study.";

  limitStates_.add(limitState);

  limitState.getImplementation().get()->addObserver(this);
  if (Observer * obs = getObserver("StudyItem"))
    obs->appendItem(limitState);
  modified_ = true;
  notify("statusChanged");
}


void StudyImplementation::clear(const LimitState& limitState)
{
  // remove the analyses depending on the limitState
  PersistentCollection<Analysis>::iterator iter = analyses_.begin();
  while (iter != analyses_.end())
  {
    if ((*iter).isReliabilityAnalysis())
    {
      if (dynamic_cast<ReliabilityAnalysis*>((*iter).getImplementation().get())->getLimitState() == limitState)
      {
        (*iter).getImplementation()->removeAllObservers();
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


void StudyImplementation::remove(const LimitState& limitState)
{
  if (!limitStates_.contains(limitState))
    return;

  // remove the analyses/observers depending on the limitState
  clear(limitState);
  limitState.getImplementation().get()->removeAllObservers();

  limitStates_.erase(std::remove(limitStates_.begin(), limitStates_.end(), limitState), limitStates_.end());
}


String StudyImplementation::getPythonScript()
{
  String result;

  result += "#!/usr/bin/env python\n# coding: utf-8\n\nfrom __future__ import print_function\nimport openturns as ot\nimport persalys\n\n";

  result += getName() + " = persalys.Study('" + getName() + "')\n";
  result += "persalys.Study.Add(" + getName() + ")\n";

  for (Collection<PhysicalModel>::iterator it = physicalModels_.begin(); it != physicalModels_.end(); ++it)
  {
    if ((*it).getImplementation()->getClassName() != "MetaModel")
    {
      result += (*it).getPythonScript();
      result += getName() + ".add(" + (*it).getName() + ")\n";
    }
  }
  for (Collection<DesignOfExperiment>::iterator it = dataModels_.begin(); it != dataModels_.end(); ++it)
  {
    result += (*it).getPythonScript();
    result += getName() + ".add(" + (*it).getName() + ")\n";
  }
  for (Collection<LimitState>::iterator it = limitStates_.begin(); it != limitStates_.end(); ++it)
  {
    result += (*it).getPythonScript();
    result += getName() + ".add(" + (*it).getName() + ")\n";
  }
  for (Collection<Analysis>::iterator it = analyses_.begin(); it != analyses_.end(); ++it)
  {
    result += (*it).getPythonScript();
    result += getName() + ".add(" + (*it).getName() + ")\n";
  }
  return result;
}


void StudyImplementation::save(const String& xmlFileName)
{
  OT::Study study;
  study.setStorageManager(XMLStorageManager(xmlFileName));
  study.add("aStudy", this->clone());
  study.save();

  setFileName(xmlFileName);
  modified_ = false;
  notify("statusChanged");
}


/* Method save() stores the object through the StorageManager */
void StudyImplementation::save(Advocate& adv) const
{
  PersistentObject::save(adv);
  adv.saveAttribute("dataModels_", dataModels_);
  adv.saveAttribute("physicalModels_", physicalModels_);
  adv.saveAttribute("limitStates_", limitStates_);
  adv.saveAttribute("analyses_", analyses_);
}


/* Method load() reloads the object from the StorageManager */
void StudyImplementation::load(Advocate& adv)
{
  PersistentObject::load(adv);
  adv.loadAttribute("dataModels_", dataModels_);
  adv.loadAttribute("physicalModels_", physicalModels_);
  adv.loadAttribute("limitStates_", limitStates_);
  adv.loadAttribute("analyses_", analyses_);

  modified_ = false;
  notify("statusChanged");
}
}
