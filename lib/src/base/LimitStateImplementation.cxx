//                                               -*- C++ -*-
/**
 *  @brief Class to define the limit states
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
#include "otgui/LimitStateImplementation.hxx"

#include "Equal.hxx"

using namespace OT;

namespace OTGUI {

LimitStateImplementation::LimitStateImplementation(const String & name, const PhysicalModel & physicalModel,
                       const String & outputName, const ComparisonOperator & comparisonOperator,
                       const double & threshold)
  : PersistentObject()
  , Observable()
  , physicalModel_(physicalModel)
  , threshold_(threshold)
{
  setName(name);
  setOutputName(outputName);
  setOperator(comparisonOperator);
}


LimitStateImplementation* LimitStateImplementation::clone() const
{
  return new LimitStateImplementation(*this);
}


PhysicalModel LimitStateImplementation::getPhysicalModel() const
{
  return physicalModel_;
}


void LimitStateImplementation::setPhysicalModel(const PhysicalModel & physicalModel)
{
  physicalModel_ = physicalModel;
}


String LimitStateImplementation::getOutputName() const
{
  return outputName_;
}


void LimitStateImplementation::setOutputName(const String & outputName)
{
  if (!physicalModel_.hasOutputNamed(outputName))
    throw InvalidArgumentException(HERE) << "The physical model does not contain an output named " << outputName;

  outputName_ = outputName;
  notify("outputNameChanged");
}


ComparisonOperator LimitStateImplementation::getOperator() const
{
  return operator_;
}


void LimitStateImplementation::setOperator(const ComparisonOperator & comparisonOperator)
{
  if (comparisonOperator.getImplementation()->getClassName() == "Equal")
    throw InvalidArgumentException(HERE) << "The operator Equal is not valid to define a limit state.\n";

  operator_ = comparisonOperator;
  notify("operatorChanged");
}


double LimitStateImplementation::getThreshold() const
{
  return threshold_;
}


void LimitStateImplementation::setThreshold(const double & threshold)
{
  threshold_ = threshold;
  notify("thresholdChanged");
}


// build event
Event LimitStateImplementation::getEvent()
{
  Description outputDescription(1);
  outputDescription[0] = outputName_;
  Event event(getPhysicalModel().getOutputRandomVector(outputDescription), operator_, threshold_);
  Description outputName(1);
  outputName[0] = outputName_;
  event.setDescription(outputName);
  return event;
}


String LimitStateImplementation::dump() const
{
  String result;
  result += getName()+ " = otguibase.LimitState('" + getName() + "', " + getPhysicalModel().getName();
  result += ", '" + outputName_ + "', ot."+ operator_.getImplementation()->getClassName() + "(), ";
  OSS oss;
  oss << threshold_;
  result += oss.str() + ")\n";
  return result;
}
}