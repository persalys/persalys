//                                               -*- C++ -*-
/**
 *  @brief Class to define the limit states
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
#include "otgui/LimitStateImplementation.hxx"

#include "openturns/Equal.hxx"
#include "openturns/PersistentObjectFactory.hxx"

using namespace OT;

namespace OTGUI {

CLASSNAMEINIT(LimitStateImplementation);

static Factory<LimitStateImplementation> RegisteredFactory;

/* Default constructor */
LimitStateImplementation::LimitStateImplementation()
  : PersistentObject()
  , Observable()
  , threshold_(0.)
{
}


/* Constructor with parameters */
LimitStateImplementation::LimitStateImplementation(const String& name,
                                                   const PhysicalModel& physicalModel,
                                                   const String& outputName,
                                                   const ComparisonOperator& comparisonOperator,
                                                   const double& threshold)
  : PersistentObject()
  , Observable()
  , physicalModel_(physicalModel)
  , threshold_(threshold)
{
  if (!physicalModel.getOutputs().getSize())
    throw InvalidArgumentException(HERE) << "The physical model does not contain output";

  setName(name);
  if (outputName.empty())
    setOutputName(physicalModel.getOutputs()[0].getName());
  else
    setOutputName(outputName);

  setOperator(comparisonOperator);
}


/* Virtual constructor */
LimitStateImplementation* LimitStateImplementation::clone() const
{
  return new LimitStateImplementation(*this);
}


PhysicalModel LimitStateImplementation::getPhysicalModel() const
{
  return physicalModel_;
}


String LimitStateImplementation::getOutputName() const
{
  return outputName_;
}


void LimitStateImplementation::setOutputName(const String& outputName)
{
  if (!physicalModel_.hasOutputNamed(outputName))
    throw InvalidArgumentException(HERE) << "The physical model does not contain an output named '" << outputName <<"'.";

  outputName_ = outputName;
  notify("outputNameChanged");
}


ComparisonOperator LimitStateImplementation::getOperator() const
{
  return operator_;
}


void LimitStateImplementation::setOperator(const ComparisonOperator& comparisonOperator)
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


void LimitStateImplementation::setThreshold(const double& threshold)
{
  threshold_ = threshold;
  notify("thresholdChanged");
}


bool LimitStateImplementation::isValid() const
{
  if (!physicalModel_.getImplementation().get()->hasOutputNamed(outputName_))
    return false;
  return true;
}


String LimitStateImplementation::getPythonScript() const
{
  String result;
  result += getName()+ " = otguibase.LimitState('" + getName() + "', " + getPhysicalModel().getName();
  result += ", '" + outputName_ + "', ot."+ operator_.getImplementation()->getClassName() + "(), ";
  OSS oss;
  oss << threshold_;
  result += oss.str() + ")\n";
  return result;
}


String LimitStateImplementation::__repr__() const
{
  OSS oss;
  oss << "class=" << GetClassName()
      << " name=" << getName()
      << " physicalModel=" << getPhysicalModel().getName()
      << " outputName=" << getOutputName()
      << " operator=" << getOperator()
      << " threshold=" << getThreshold();
  return oss;
}


/* Method save() stores the object through the StorageManager */
void LimitStateImplementation::save(Advocate& adv) const
{
  PersistentObject::save(adv);
  adv.saveAttribute("physicalModel_", physicalModel_);
  adv.saveAttribute("outputName_", outputName_);
  adv.saveAttribute("operator_", operator_);
  adv.saveAttribute("threshold_", threshold_);
}


/* Method load() reloads the object from the StorageManager */
void LimitStateImplementation::load(Advocate& adv)
{
  PersistentObject::load(adv);
  adv.loadAttribute("physicalModel_", physicalModel_);
  adv.loadAttribute("outputName_", outputName_);
  adv.loadAttribute("operator_", operator_);
  adv.loadAttribute("threshold_", threshold_);
}
}