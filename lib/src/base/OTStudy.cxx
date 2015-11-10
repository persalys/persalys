// OTStudy.cxx

#include "otgui/OTStudy.hxx"

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
  {
    if (name == (*it)->getName())
    {
      return (*it);
    }
  }
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
  , name_(other.name_)
{
  setObserver(other.getObserver());
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


Description OTStudy::getPhysicalModelsNames() const
{
  Description physicalModelsNames(physicalModels_.size());
  for (int i=0; i<physicalModels_.size(); ++i)
    physicalModelsNames[i] = physicalModels_[i].getImplementation()->getName();
  return physicalModelsNames;
}


void OTStudy::addPhysicalModel(const PhysicalModel & physicalModel)
{
  physicalModels_.push_back(physicalModel);
  notify("addPhysicalModel");
}


std::vector<Analysis> OTStudy::getAnalyses() const
{
  return analyses_;
}


Description OTStudy::getAnalysesNames() const
{
  Description analysesNames(analyses_.size());
  for (int i=0; i<analyses_.size(); ++i)
    analysesNames[i] = analyses_[i].getImplementation()->getName();
  return analysesNames;
}


void OTStudy::addAnalysis(const Analysis & analysis)
{
  analyses_.push_back(analysis);
  notify("add"+analysis.getImplementation()->getClassName());
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
  for (std::vector<Analysis>::iterator it=analyses_.begin(); it!= analyses_.end(); ++it)
  {
    result += (*it).dump();
    result += getName() + ".addAnalysis(" + (*it).getName() + ")\n";
  }
  return result;
}


}
