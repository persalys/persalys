//                                               -*- C++ -*-
/**
 *  @brief The class that implements the variable of the physical models
 *
 *  Copyright 2015-2016 EDF
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