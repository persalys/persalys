#include "otgui/Variable.hxx"

using namespace OT;

namespace OTGUI {

CLASSNAMEINIT(Variable);

Variable::Variable(const String & name, const double & value, const String & description)
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


String Variable::getDescription() const
{
  return description_;
}


void Variable::setDescription(const String & description)
{
  description_ = description;
}
}