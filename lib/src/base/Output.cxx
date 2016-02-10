//                                               -*- C++ -*-
/**
 *  @brief Class to define outputs of the physical models
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
#include "otgui/Output.hxx"

using namespace OT;

namespace OTGUI {

CLASSNAMEINIT(Output);

Output::Output(const String & name, const double & value, const String & description,
               const String & formula)
  : Variable(name, value, description)
  , formula_(formula)
  , hasBeenComputed_(false)
{
}


Output* Output::clone() const
{
  return new Output(*this);
}


String Output::getFormula() const
{
  return formula_;
}


void Output::setFormula(const String & formula)
{
  formula_ = formula;
}


bool Output::hasBeenComputed() const
{
  return hasBeenComputed_;
}


void Output::setHasBeenComputed(const bool hasBeenComputed)
{
  hasBeenComputed_ = hasBeenComputed;
}


String Output::dump() const
{
  OSS oss;
  oss << getName() << " = otguibase.Output('" << getName() << "', " << getValue() << ", '" << getDescription();
  oss << "', '" << formula_ << "')\n";

  return oss;
}
}