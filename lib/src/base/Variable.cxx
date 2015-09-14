#include "Variable.hxx"

using namespace OT;

namespace OTGUI {
CLASSNAMEINIT(Variable);

Variable::Variable()
  : PersistentObject()
  , name_("")
  , value_(0)
  , description_("")
{
}


Variable::Variable(const std::string & name, const double & value, const std::string & description)
  : PersistentObject()
  , name_(name)
  , value_(value)
  , description_(description)
{
}


Variable::Variable(const Variable & other)
  : PersistentObject()
  , name_(other.name_)
  , value_(other.value_)
  , description_(other.description_)
{

}


Variable::~Variable()
{
}

String Variable::__repr__() const
{
  return OT::OSS() << "class=" << getClassName() << " name=" << getName();
}

std::string Variable::getName() const
{
  return name_;
}


void Variable::setName(const std::string & name)
{
  name_ = name;
}


double Variable::getValue() const
{
  return value_;
}


void Variable::setValue(const double & value)
{
  value_ = value;
}


std::string Variable::getDescription() const
{
  return description_;
}


void Variable::setDescription(const std::string & description)
{
  description_ = description;
}




}