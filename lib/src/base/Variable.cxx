#include "Variable.hxx"

namespace OTGUI {

Variable::Variable()
  : PersistentObject()
  , name_("")
  , value_(0)
  , description_("")
{
}


Variable::Variable(std::string name, double value, std::string description)
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


std::string Variable::getName() const
{
  return name_;
}


void Variable::setName(std::string name)
{
  name_ = name;
}


double Variable::getValue() const
{
  return value_;
}


void Variable::setValue(double value)
{
  value_ = value;
}


std::string Variable::getDescription() const
{
  return description_;
}


void Variable::setDescription(std::string description)
{
  description_ = description;
}




}