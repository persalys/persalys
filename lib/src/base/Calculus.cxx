// Calculus.cxx

#include "Calculus.hxx"


namespace OTGUI {

Calculus::Calculus(const std::string & name, const PhysicalModel & physicalModel)
  : PersistentObject()
  , Observable()
  , name_(name)
  , physicalModel_(physicalModel)
{
}


Calculus::Calculus(const Calculus & other)
  : PersistentObject()
  , Observable()
  , name_(other.name_)
  , physicalModel_(other.physicalModel_)
{
}


Calculus::~Calculus()
{
}


std::string Calculus::getName() const
{
  return name_;
}


void Calculus::setName(const std::string & name)
{
  name_ = name;
}


PhysicalModel Calculus::getPhysicalModel() const
{
  return physicalModel_;
}


void Calculus::setPhysicalModel(const PhysicalModel & physicalModel)
{
  physicalModel_ = physicalModel;
}


}