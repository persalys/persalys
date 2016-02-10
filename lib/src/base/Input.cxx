//                                               -*- C++ -*-
/**
 *  @brief Class to define inputs for the physical models 
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
#include "otgui/Input.hxx"

#include "TruncatedDistribution.hxx"

using namespace OT;

namespace OTGUI {

CLASSNAMEINIT(Input);

Input::Input(const String & name, const double & value, const String & description,
             const Distribution & distribution)
 : Variable(name, value, description)
 , distribution_(distribution)
{
}


Input* Input::clone() const
{
  return new Input(*this);
}


String Input::__repr__() const
{
  OSS oss;
  oss << "class="<<GetClassName() <<" var="<<getName()<<" value="<<getValue()<<" desc="<< getDescription();
  return oss;
}


Distribution Input::getDistribution() const
{
  return distribution_;
}


void Input::setDistribution(const Distribution & distribution)
{
  distribution_ = distribution;
}


bool Input::isStochastic() const
{
  if (distribution_.getImplementation()->getClassName() != "Dirac")
    return true;
  return false;
}


String Input::dumpDistribution() const
{
  OSS oss;

  String distributionName = distribution_.getImplementation()->getClassName();
  if (distributionName != "TruncatedDistribution")
  {
    oss << "dist_" << getName() << " = ot." << distributionName << "(";
    NumericalPointWithDescription parameters = distribution_.getParametersCollection()[0];
    for (unsigned int i = 0; i < parameters.getSize(); ++ i)
    {
      oss << parameters[i];
      if (i < parameters.getSize() - 1)
        oss << ", ";
    }
    oss << ")\n";
  }
  else
  {
    TruncatedDistribution truncatedDistribution = *dynamic_cast<TruncatedDistribution*>(&*distribution_.getImplementation());
    Distribution distribution = truncatedDistribution.getDistribution();
    oss << "dist_" << getName() << " = ot." << distribution.getImplementation()->getClassName() << "(";
    NumericalPointWithDescription parameters = distribution.getParametersCollection()[0];
    for (unsigned int i = 0; i < parameters.getSize(); ++ i)
    {
      oss << parameters[i];
      if (i < parameters.getSize() - 1)
        oss << ", ";
    }
    oss << ")\n";
    oss << "dist_" << getName() << " = ot." << distributionName << "(";
    oss << "dist_" << getName() << ", ";

    if (!(truncatedDistribution.getFiniteLowerBound() && truncatedDistribution.getFiniteUpperBound())) // one side truncation ?
    {
      if (truncatedDistribution.getFiniteLowerBound())    //lower bound truncation
        oss << truncatedDistribution.getLowerBound() << ")\n";
      else
        oss << truncatedDistribution.getUpperBound() << ", ot.TruncatedDistribution.UPPER)\n";
    }
    else  // both sides truncation
      oss << truncatedDistribution.getUpperBound() << ", " << truncatedDistribution.getUpperBound() <<")\n";
  }
  return oss;
}


String Input::dump() const
{
  OSS oss;

  if (!isStochastic())
    oss << getName() << " = otguibase.Input('" << getName() << "', " <<getValue() << ", '" << getDescription() << "')\n";
  else
  {
    oss << dumpDistribution();
    oss << getName() << " = otguibase.Input('" << getName() << "', " <<getValue() << ", '" << getDescription();
    oss << "', dist_" << getName() << ")\n";
  }

  return oss;
}
}