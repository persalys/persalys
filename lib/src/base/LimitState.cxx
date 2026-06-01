//                                               -*- C++ -*-
/**
 *  @brief Class to define the limit states
 *
 *  Copyright 2015-2026 EDF-Phimeca
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
#include "persalys/LimitState.hxx"

using namespace OT;

namespace PERSALYS
{

CLASSNAMEINIT(LimitState)

/* Default constructor */
LimitState::LimitState()
  : TypedInterfaceObject<LimitStateImplementation>(new LimitStateImplementation())
{
}


/* Constructor for regular limit state */
LimitState::LimitState(const String& name,
                       const PhysicalModel& physicalModel,
                       const String& outputName,
                       const ComparisonOperator& comparisonOperator,
                       const double& threshold)
  : TypedInterfaceObject<LimitStateImplementation>(new LimitStateImplementation(name, physicalModel, outputName, comparisonOperator, threshold))
{
}

/* Constructor for system limit state */
LimitState::LimitState(const String& name,
                       const PhysicalModel& physicalModel,
                       const Description& outputNames,
                       const Collection<ComparisonOperator>& operators,
                       const Type type,
                       const Point& thresholds)
  : TypedInterfaceObject<LimitStateImplementation>(new LimitStateImplementation(name, physicalModel, outputNames, operators, type, thresholds))
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


Description LimitState::getOutputNames() const
{
  return getImplementation()->getOutputNames();
}

void LimitState::setOutputNames(const Description& outputNames)
{
  getImplementation()->setOutputNames(outputNames);
}

void LimitState::setOutputName(UnsignedInteger index, const String& outputName)
{
  getImplementation()->setOutputName(index, outputName);
}

Collection<ComparisonOperator> LimitState::getOperators() const
{
  return getImplementation()->getOperators();
}

ComparisonOperator LimitState::getOperator(UnsignedInteger index) const
{
  return getImplementation()->getOperator(index);
}

void LimitState::setOperators(const Collection<ComparisonOperator>& operators)
{
  getImplementation()->setOperators(operators);
}

void LimitState::setOperator(UnsignedInteger index, const ComparisonOperator& comparisonOperator)
{
  getImplementation()->setOperator(index, comparisonOperator);
}

Point LimitState::getThresholds() const
{
  return getImplementation()->getThresholds();
}

double LimitState::getThreshold(UnsignedInteger index) const
{
  return getImplementation()->getThreshold(index);
}

void LimitState::setThresholds(const Point& thresholds)
{
  getImplementation()->setThresholds(thresholds);
}

void LimitState::setThreshold(UnsignedInteger index, const double& threshold)
{
  getImplementation()->setThreshold(index, threshold);
}

void LimitState::setType(const Type type)
{
  getImplementation()->setType(type);
}

LimitState::Type LimitState::getType() const
{
  return getImplementation()->getType();
}

bool LimitState::isSystemLimitState() const
{
  return getImplementation()->isSystemLimitState();
}

bool LimitState::isValid() const
{
  return getImplementation()->isValid();
}

void LimitState::addFailureEvent(const String& variableName, const ComparisonOperator& comparisonOperator, const double& threshold)
{
  getImplementation()->addFailureEvent(variableName, comparisonOperator, threshold);
}

void LimitState::removeFailureEvent(UnsignedInteger index)
{
  getImplementation()->removeFailureEvent(index);
}

RandomVector LimitState::getThresholdEvent(OT::Collection<OT::Function> &functions) const
{
  return getImplementation()->getThresholdEvent(functions);
}

RandomVector LimitState::getThresholdEvent() const
{
  return getImplementation()->getThresholdEvent();
}

RandomVector LimitState::getISThresholdEvent() const
{
  return getImplementation()->getISThresholdEvent();
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