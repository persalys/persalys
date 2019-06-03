//                                               -*- C++ -*-
/**
 *  @brief Class to define designs of experiments
 *
 *  Copyright 2015-2019 EDF-Phimeca
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
#include "persalys/DesignOfExperiment.hxx"

using namespace OT;

namespace PERSALYS
{

CLASSNAMEINIT(DesignOfExperiment)

/* Default constructor */
DesignOfExperiment::DesignOfExperiment()
  : TypedInterfaceObject<DesignOfExperimentImplementation>(new DesignOfExperimentImplementation())
{
}


/* Constructor with parameters */
DesignOfExperiment::DesignOfExperiment(const String& name, const PhysicalModel& physicalModel)
  : TypedInterfaceObject<DesignOfExperimentImplementation>(new DesignOfExperimentImplementation(name, physicalModel))
{
}


/* Default constructor */
DesignOfExperiment::DesignOfExperiment(const DesignOfExperimentImplementation& implementation)
  : TypedInterfaceObject<DesignOfExperimentImplementation>(implementation.clone())
{
}


/* Constructor from implementation */
DesignOfExperiment::DesignOfExperiment(const Implementation& p_implementation)
  : TypedInterfaceObject<DesignOfExperimentImplementation>(p_implementation)
{
  // Initialize any other class members here
  // At last, allocate memory space if needed, but go to destructor to free it
}


/* Constructor from implementation pointer */
DesignOfExperiment::DesignOfExperiment(DesignOfExperimentImplementation* p_implementation)
  : TypedInterfaceObject<DesignOfExperimentImplementation>(p_implementation)
{
  // Initialize any other class members here
  // At last, allocate memory space if needed, but go to destructor to free it
}


Bool DesignOfExperiment::operator==(const DesignOfExperiment& other) const
{
  if (this == &other) return true;
  return *getImplementation() == *other.getImplementation();
}


Bool DesignOfExperiment::operator!=(const DesignOfExperiment& other) const
{
  return !operator==(other);
}


void DesignOfExperiment::addObserver(Observer* observer)
{
  getImplementation()->addObserver(observer);
}


bool DesignOfExperiment::hasPhysicalModel() const
{
  return getImplementation()->hasPhysicalModel();
}


PhysicalModel DesignOfExperiment::getPhysicalModel() const
{
  return getImplementation()->getPhysicalModel();
}


Sample DesignOfExperiment::getInputSample() const
{
  return getImplementation()->getInputSample();
}


Sample DesignOfExperiment::getOutputSample() const
{
  return getImplementation()->getOutputSample();
}


Sample DesignOfExperiment::getSample() const
{
  return getImplementation()->getSample();
}


void DesignOfExperiment::setInputSample(const Sample& sample)
{
  getImplementation()->setInputSample(sample);
}


void DesignOfExperiment::setOutputSample(const Sample& sample)
{
  getImplementation()->setOutputSample(sample);
}


bool DesignOfExperiment::isValid() const
{
  return getImplementation()->isValid();
}


String DesignOfExperiment::getPythonScript() const
{
  return getImplementation()->getPythonScript();
}


/* Method save() stores the object through the StorageManager */
void DesignOfExperiment::save(Advocate& adv) const
{
  getImplementation()->save(adv);
}


/* Method load() reloads the object from the StorageManager */
void DesignOfExperiment::load(Advocate& adv)
{
  getImplementation()->load(adv);
}


void DesignOfExperiment::setImplementationAsPersistentObject(const ImplementationAsPersistentObject& obj)
{
  TypedInterfaceObject< DesignOfExperimentImplementation >::setImplementationAsPersistentObject(obj);
  getImplementation()->notify("implementationModified");
}
}
