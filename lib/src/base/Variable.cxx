//                                               -*- C++ -*-
/**
 *  @brief The class that implements the variable of the physical models
 *
 *  Copyright 2015-2021 EDF-Phimeca
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
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
#include "persalys/Variable.hxx"

#include <openturns/PersistentObjectFactory.hxx>

#include <regex>

using namespace OT;

namespace PERSALYS
{

CLASSNAMEINIT(Variable)

static Factory<PersistentCollection<Variable> > Factory_PersistentCollectionVariable;
static Factory<Variable> Factory_Variable;

/* Default constructor */
Variable::Variable()
  : PersistentObject()
  , value_(0.)
  , description_("")
{
}


/* Constructor with parameters */
Variable::Variable(const String & name, const double & value, const String & description, const String & unit)
  : PersistentObject()
  , value_(value)
  , description_(description)
  , unit_(unit)
{
  setName(name);
}


/* Constructor with parameters */
Variable::Variable(const String & name, const String & description)
  : PersistentObject()
  , value_(0.)
  , description_(description)
{
  setName(name);
}

/* Virtual constructor */
Variable* Variable::clone() const
{
  return new Variable(*this);
}


String Variable::__repr__() const
{
  return OSS() << "class=" << getClassName() << " name=" << getName() << " description=" << getDescription();
}


void Variable::setName(const String &name)
{
  if (name.empty() || name.find_first_not_of(" \t\n\v\f\r") == name.npos)
    throw InvalidArgumentException(HERE) << "The variable name cannot be empty";
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

String Variable::getUnit() const
{
  return unit_;
}

void Variable::setUnit(const String & unit)
{
  unit_ = unit;
}

String Variable::getPythonScript() const
{
  OSS oss;
  oss.setPrecision(12);

  String pythonName(getName());
  pythonName = std::regex_replace(pythonName, std::regex("[^0-9a-zA-Z_]"), "_");

  oss << pythonName << " = persalys.Variable('" << getName() << "', " << getValue() << ", '" << getEscapedDescription() << "')\n";

  return oss;
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
