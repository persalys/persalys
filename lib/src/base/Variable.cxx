#include "otgui/Variable.hxx"

using namespace OT;

namespace OTGUI {

CLASSNAMEINIT(Variable);

Variable::Variable(const std::string & name, const double & value, const std::string & description)
  : PersistentObject()
  , value_(value)
  , description_(description)
{
  setName(name);
}


String Variable::__repr__() const
{
  return OSS() << "class=" << getClassName() << " name=" << getName();
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