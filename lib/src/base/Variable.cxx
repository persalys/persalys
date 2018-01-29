//                                               -*- C++ -*-
/**
 *  @brief The class that implements the variable of the physical models
 *
 *  Copyright 2015-2017 EDF-Phimeca
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
 *  You should have received a copy of the GNU Lesser General Public
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
#include "otgui/Variable.hxx"

using namespace OT;

namespace OTGUI
{

CLASSNAMEINIT(Variable)

/* Default constructor */
Variable::Variable()
  : PersistentObject()
{
}


/* Constructor with parameters */
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


void Variable::setName(const String &name)
{
  if (name.empty()|| name.find_first_not_of(" \t\n\v\f\r") == name.npos)
    throw InvalidArgumentException(HERE) << "The variable name can not be empty";
  PersistentObject::setName(name);
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

String Variable::getEscapedDescription() const
{
  String description(getDescription());
  size_t start_pos = description.find("'");
  while (start_pos != std::string::npos)
  {
    description.replace(start_pos, 1, "\\'");
    start_pos = description.find("'", start_pos + 2);
  }
  return description;
}


void Variable::setDescription(const String & description)
{
  description_ = description;
}


/* Method save() stores the object through the StorageManager */
void Variable::save(Advocate & adv) const
{
  PersistentObject::save(adv);
  adv.saveAttribute("value_", value_);
  adv.saveAttribute("description_", description_);
}


/* Method load() reloads the object from the StorageManager */
void Variable::load(Advocate & adv)
{
  PersistentObject::load(adv);
  adv.loadAttribute("value_", value_);
  adv.loadAttribute("description_", description_);
}
}
