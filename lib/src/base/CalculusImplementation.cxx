// CalculusImplementation.cxx

#include "otgui/CalculusImplementation.hxx"


namespace OTGUI {

CalculusImplementation::CalculusImplementation(const std::string & name, const PhysicalModel & physicalModel)
  : PersistentObject()
  , Observable()
  , name_(name)
  , physicalModel_(physicalModel)
{
}


CalculusImplementation::CalculusImplementation(const CalculusImplementation & other)
  : PersistentObject()
  , Observable()
  , name_(other.name_)
  , physicalModel_(other.physicalModel_)
{
}


CalculusImplementation * CalculusImplementation::clone() const
{
  return new CalculusImplementation(*this);
}


CalculusImplementation::~CalculusImplementation()
{
}


std::string CalculusImplementation::getName() const
{
  return name_;
}


void CalculusImplementation::setName(const std::string & name)
{
  name_ = name;
}


PhysicalModel CalculusImplementation::getPhysicalModel() const
{
  return physicalModel_;
}


void CalculusImplementation::setPhysicalModel(const PhysicalModel & physicalModel)
{
  physicalModel_ = physicalModel;
}


void CalculusImplementation::run()
{

}


bool CalculusImplementation::calculusLaunched() const
{
  return false;
}


std::string CalculusImplementation::dump() const
{
  return "";
}

}