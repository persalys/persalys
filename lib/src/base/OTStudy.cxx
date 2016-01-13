// OTStudy.cxx

#include "otgui/OTStudy.hxx"

#include "otgui/ReliabilityAnalysis.hxx"

using namespace OT;

namespace OTGUI {

std::vector<OTStudy*> OTStudy::Instances_;


Observer * OTStudy::InstanceObserver_ = 0;


std::vector<OTStudy*> OTStudy::GetInstances()
{
  return Instances_;
}


OTStudy * OTStudy::FindInstance(const std::string & name)
{
  for (std::vector<OTStudy*>::iterator it=Instances_.begin(); it!=Instances_.end(); ++it)
    if (name == (*it)->getName())
      return (*it);
  return 0;
}


void OTStudy::SetInstanceObserver(Observer * observer)
{
  InstanceObserver_ = observer;
}


OTStudy::OTStudy(const std::string & name)
 : PersistentObject()
 , Observable()
 , name_(name)
{
  Instances_.push_back(this);
  if (InstanceObserver_)
    InstanceObserver_->update(this, "addStudy");
}


OTStudy::OTStudy(const OTStudy & other)
  : PersistentObject()
  , Observable()
  , name_(other.name_+"_copy")
{
  setObserver(other.getObserver());
  Instances_.push_back(this);
  if (InstanceObserver_)
    InstanceObserver_->update(this, "addStudy");

  for (int i=0; i<other.physicalModels_.size(); ++i)
    addPhysicalModel(*other.physicalModels_[i].getImplementation()->clone());
  // TODO copy of designOfExperiments_/analyses_/limitStates_
}


OTStudy* OTStudy::clone() const
{
  return new OTStudy(*this);
}


OTStudy::~OTStudy()
{
  Instances_.erase(std::remove(Instances_.begin(), Instances_.end(), this), Instances_.end());
}


std::string OTStudy::getName() const
{
  return name_;
}


void OTStudy::setName(const std::string & name)
{
  name_ = name;
}


std::vector<PhysicalModel> OTStudy::getPhysicalModels() const
{
  return physicalModels_;
}


PhysicalModel & OTStudy::getPhysicalModelByName(const std::string& physicalModelName)
{
  for (UnsignedInteger i=0; i<physicalModels_.size(); ++i)
    if (physicalModels_[i].getName() == physicalModelName)
      return physicalModels_[i];
  throw InvalidArgumentException(HERE) << "The given name " << physicalModelName <<" does not correspond to a physical model of the study.\n"; 
}


bool OTStudy::hasPhysicalModelNamed(const std::string & physicalModelName)
{
  for (UnsignedInteger i=0; i<physicalModels_.size(); ++i)
    if (physicalModels_[i].getImplementation()->getName() == physicalModelName)
      return true;
  return false;
}


void OTStudy::addPhysicalModel(const PhysicalModel & physicalModel)
{
  if (hasPhysicalModelNamed(physicalModel.getName()))
    throw InvalidArgumentException(HERE) << "The study has already contained a physical model named " << physicalModel.getName();

  physicalModels_.push_back(physicalModel);
  notify("addPhysicalModel");
}


std::vector<DesignOfExperiment> OTStudy::getDesignOfExperiments() const
{
  return designOfExperiments_;
}


bool OTStudy::hasDesignOfExperimentNamed(const std::string & designOfExperimentName)
{
  for (UnsignedInteger i=0; i<designOfExperiments_.size(); ++i)
    if (designOfExperiments_[i].getImplementation()->getName() == designOfExperimentName)
      return true;
  return false;
}


void OTStudy::addDesignOfExperiment(const DesignOfExperiment & designOfExperiment)
{
  if (hasDesignOfExperimentNamed(designOfExperiment.getName()))
    throw InvalidArgumentException(HERE) << "The study has already contained a design of experiment named " << designOfExperiment.getName();

  if (!hasPhysicalModelNamed(designOfExperiment.getPhysicalModel().getName()))
    throw InvalidArgumentException(HERE) << "The design of experiment has been created with a physical model not belonging to the study.";

  designOfExperiments_.push_back(designOfExperiment);
  notify("addDesignOfExperiment");
}


std::vector<Analysis> OTStudy::getAnalyses() const
{
  return analyses_;
}

Analysis & OTStudy::getAnalysisByName(const std::string & analysisName)
{
  for (UnsignedInteger i=0; i<analyses_.size(); ++i)
    if (analyses_[i].getName() == analysisName)
      return analyses_[i];
  throw InvalidArgumentException(HERE) << "The given name " << analysisName <<" does not correspond to an analysis of the study.\n"; 
}


bool OTStudy::hasAnalysisNamed(const std::string & analysisName)
{
  for (UnsignedInteger i=0; i<analyses_.size(); ++i)
    if (analyses_[i].getImplementation()->getName() == analysisName)
      return true;
  return false;
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

  analyses_.push_back(analysis);
  notify("add"+analysis.getImplementation()->getClassName());
}


std::vector<LimitState> OTStudy::getLimitStates() const
{
  return limitStates_;
}


bool OTStudy::hasLimitStateNamed(const std::string & limitStateName)
{
  for (UnsignedInteger i=0; i<limitStates_.size(); ++i)
    if (limitStates_[i].getImplementation()->getName() == limitStateName)
      return true;
  return false;
}


void OTStudy::addLimitState(const LimitState & limitState)
{
  if (hasLimitStateNamed(limitState.getName()))
    throw InvalidArgumentException(HERE) << "The study has already contained a limit state named " << limitState.getName();

  if (!hasPhysicalModelNamed(limitState.getPhysicalModel().getName()))
    throw InvalidArgumentException(HERE) << "The limit state has been created with a physical model not belonging to the study.";

  limitStates_.push_back(limitState);
  notify("addLimitState");
}


std::string OTStudy::dump()
{
  std::string result;
  result += getName()+ " = otguibase.OTStudy('" + getName() + "')\n";
  for (std::vector<PhysicalModel>::iterator it=physicalModels_.begin(); it!= physicalModels_.end(); ++it)
  {
    result += (*it).dump();
    result += getName() + ".addPhysicalModel(" + (*it).getName() + ")\n";
  }
  for (std::vector<DesignOfExperiment>::iterator it=designOfExperiments_.begin(); it!= designOfExperiments_.end(); ++it)
  {
    result += (*it).dump();
    result += getName() + ".addDesignOfExperiment(" + (*it).getName() + ")\n";
  }
  for (std::vector<LimitState>::iterator it=limitStates_.begin(); it!= limitStates_.end(); ++it)
  {
    result += (*it).dump();
    result += getName() + ".addLimitState(" + (*it).getName() + ")\n";
  }
  for (std::vector<Analysis>::iterator it=analyses_.begin(); it!= analyses_.end(); ++it)
  {
    result += (*it).dump();
    result += getName() + ".addAnalysis(" + (*it).getName() + ")\n";
  }
  return result;
}


}
