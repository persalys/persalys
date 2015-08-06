// OTStudy.cxx

#include "OTStudy.hxx"

namespace OTGUI {

std::vector<OTStudy*> OTStudy::Instances_;


Observer * OTStudy::InstanceObserver_ = 0;


std::vector<OTStudy*> OTStudy::GetInstances()
{
  return Instances_;
}


OTStudy * OTStudy::FindInstance(std::string name)
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


OTStudy::OTStudy(std::string name)
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


void OTStudy::setName(std::string name)
{
  name_ = name;
}


std::vector<PhysicalModel> OTStudy::getPhysicalModels() const
{
  return physicalModels_;
}


void OTStudy::addPhysicalModel(const PhysicalModel & physicalModel)
{
  physicalModels_.push_back(physicalModel);
  notify("addPhysicalModel");
}


std::vector<Calculus*> OTStudy::getCalculuses() const
{
  return calculuses_;
}


void OTStudy::addCalculus(const Calculus & calculus)
{
  calculuses_.push_back(calculus.clone());
  notify("addParametricCalculus");
}


std::string OTStudy::dump()
{
  std::string result;
  result += getName()+ "= otguibase.OTStudy(\"" + getName() + "\")\n";
  for (std::vector<PhysicalModel>::iterator it=physicalModels_.begin(); it!= physicalModels_.end(); ++it )
  {
    result += (*it).dump();
    result += getName() + ".addPhysicalModel(" + (*it).getName() + ")\n";
  }
  for (std::vector<Calculus*>::iterator it=calculuses_.begin(); it!= calculuses_.end(); ++it )
  {
    result += (*it)->dump();
    result += getName() + ".addParametricCalculus(" + (*it)->getName() + ")\n";
  }
  return result;
}


}
