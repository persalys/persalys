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
#include "otgui/LimitState.hxx"

using namespace OT;

namespace OTGUI {

CLASSNAMEINIT(LimitState);

LimitState::LimitState(const String & name, const PhysicalModel & physicalModel,
                       const String & outputName,
                       const OT::ComparisonOperator & comparisonOperator, const double & threshold)
  : TypedInterfaceObject<LimitStateImplementation>(new LimitStateImplementation(name, physicalModel, outputName, comparisonOperator, threshold))
{
}


/* Default constructor */
LimitState::LimitState(const LimitStateImplementation & implementation)
  : TypedInterfaceObject<LimitStateImplementation>(implementation.clone())
{
}


/* Constructor from implementation */
LimitState::LimitState(const Implementation & p_implementation)
  : TypedInterfaceObject<LimitStateImplementation>(p_implementation)
{
  // Initialize any other class members here
  // At last, allocate memory space if needed, but go to destructor to free it
}

/* Constructor from implementation pointer */
LimitState::LimitState(LimitStateImplementation * p_implementation)
  : TypedInterfaceObject<LimitStateImplementation>(p_implementation)
{
  // Initialize any other class members here
  // At last, allocate memory space if needed, but go to destructor to free it
}


LimitState* LimitState::clone() const
{
  return new LimitState(*this);
}


void LimitState::addObserver(Observer * observer)
{
  getImplementation()->addObserver(observer);
}


void LimitState::blockNotification(bool block, const String & notBlockedMessage)
{
  getImplementation()->blockNotification(block, notBlockedMessage);
}


PhysicalModel LimitState::getPhysicalModel() const
{
  return getImplementation()->getPhysicalModel();
}


void LimitState::setPhysicalModel(const PhysicalModel & physicalModel)
{
  getImplementation()->setPhysicalModel(physicalModel);
}


String LimitState::getOutputName() const
{
  return getImplementation()->getOutputName();
}


void LimitState::setOutputName(const String & outputName)
{
  getImplementation()->setOutputName(outputName);
}


ComparisonOperator LimitState::getOperator() const
{
  return getImplementation()->getOperator();
}


void LimitState::setOperator(const ComparisonOperator & comparisonOperator)
{
  getImplementation()->setOperator(comparisonOperator);
}


double LimitState::getThreshold() const
{
  return getImplementation()->getThreshold();
}


void LimitState::setThreshold(const double & threshold)
{
  getImplementation()->setThreshold(threshold);
}


// build event
Event LimitState::getEvent()
{
  return getImplementation()->getEvent();
}


String LimitState::dump() const
{
  return getImplementation()->dump();
}


/* Method save() stores the object through the StorageManager */
void LimitState::save(Advocate & adv) const
{
  getImplementation()->save(adv);
}


/* Method load() reloads the object from the StorageManager */
void LimitState::load(Advocate & adv)
{
  getImplementation()->load(adv);
}
}