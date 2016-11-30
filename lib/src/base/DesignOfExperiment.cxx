//                                               -*- C++ -*-
/**
 *  @brief Class to define designs of experiments
 *
 *  Copyright 2015-2016 EDF-Phimeca
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
#include "otgui/DesignOfExperiment.hxx"

#include "otgui/FixedDesignOfExperiment.hxx"

using namespace OT;

namespace OTGUI {

CLASSNAMEINIT(DesignOfExperiment);

/* Default constructor */
DesignOfExperiment::DesignOfExperiment()
  : TypedInterfaceObject<DesignOfExperimentImplementation>(new FixedDesignOfExperiment())
{
}


/* Constructor with parameters */
DesignOfExperiment::DesignOfExperiment(const String& name, const PhysicalModel& physicalModel)
  : TypedInterfaceObject<DesignOfExperimentImplementation>(new FixedDesignOfExperiment(name, physicalModel))
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


Description DesignOfExperiment::getVariableInputNames() const
{
  return getImplementation()->getVariableInputNames();
}


NumericalSample DesignOfExperiment::getInputSample() const
{
  return getImplementation()->getInputSample();
}


void DesignOfExperiment::setInputSample(const NumericalSample& sample)
{
  getImplementation()->setInputSample(sample);
}


NumericalSample DesignOfExperiment::getOutputSample() const
{
  return getImplementation()->getOutputSample();
}


void DesignOfExperiment::setOutputSample(const NumericalSample& sample)
{
  getImplementation()->setOutputSample(sample);
}


NumericalSample DesignOfExperiment::getSample() const
{
  return getImplementation()->getSample();
}


void DesignOfExperiment::run()
{
  getImplementation()->run();
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
}