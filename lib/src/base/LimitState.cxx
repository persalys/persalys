//                                               -*- C++ -*-
/**
 *  @brief Class to define the limit states
 *
 *  Copyright 2015-2018 EDF-Phimeca
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
#include "otgui/LimitState.hxx"

using namespace OT;

namespace OTGUI
{

CLASSNAMEINIT(LimitState)

/* Default constructor */
LimitState::LimitState()
  : TypedInterfaceObject<LimitStateImplementation>(new LimitStateImplementation())
{
}


/* Constructor with parameters */
LimitState::LimitState(const String& name,
                       const PhysicalModel& physicalModel,
                       const String& outputName,
                       const ComparisonOperator& comparisonOperator,
                       const double& threshold)
  : TypedInterfaceObject<LimitStateImplementation>(new LimitStateImplementation(name, physicalModel, outputName, comparisonOperator, threshold))
{
}


/* Default constructor */
LimitState::LimitState(const LimitStateImplementation& implementation)
  : TypedInterfaceObject<LimitStateImplementation>(implementation.clone())
{
}


/* Constructor from implementation */
LimitState::LimitState(const Implementation& p_implementation)
  : TypedInterfaceObject<LimitStateImplementation>(p_implementation)
{
  // Initialize any other class members here
  // At last, allocate memory space if needed, but go to destructor to free it
}


/* Constructor from implementation pointer */
LimitState::LimitState(LimitStateImplementation* p_implementation)
  : TypedInterfaceObject<LimitStateImplementation>(p_implementation)
{
  // Initialize any other class members here
  // At last, allocate memory space if needed, but go to destructor to free it
}


Bool LimitState::operator==(const LimitState& other) const
{
  if (this == &other) return true;
  return *getImplementation() == *other.getImplementation();
}


Bool LimitState::operator!=(const LimitState& other) const
{
  return !operator==(other);
}


void LimitState::addObserver(Observer* observer)
{
  getImplementation()->addObserver(observer);
}


void LimitState::blockNotification(const String& blockedObserverType)
{
  getImplementation()->blockNotification(blockedObserverType);
}


PhysicalModel LimitState::getPhysicalModel() const
{
  return getImplementation()->getPhysicalModel();
}


String LimitState::getOutputName() const
{
  return getImplementation()->getOutputName();
}


void LimitState::setOutputName(const String& outputName)
{
  getImplementation()->setOutputName(outputName);
}


ComparisonOperator LimitState::getOperator() const
{
  return getImplementation()->getOperator();
}


void LimitState::setOperator(const ComparisonOperator& comparisonOperator)
{
  getImplementation()->setOperator(comparisonOperator);
}


double LimitState::getThreshold() const
{
  return getImplementation()->getThreshold();
}


void LimitState::setThreshold(const double& threshold)
{
  getImplementation()->setThreshold(threshold);
}


bool LimitState::isValid() const
{
  return getImplementation()->isValid();
}


String LimitState::getPythonScript() const
{
  return getImplementation()->getPythonScript();
}


/* Method save() stores the object through the StorageManager */
void LimitState::save(Advocate& adv) const
{
  getImplementation()->save(adv);
}


/* Method load() reloads the object from the StorageManager */
void LimitState::load(Advocate& adv)
{
  getImplementation()->load(adv);
}
}