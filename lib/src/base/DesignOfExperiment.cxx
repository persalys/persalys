//                                               -*- C++ -*-
/**
 *  @brief Class to define designs of experiments
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
#include "otgui/DesignOfExperiment.hxx"

using namespace OT;

namespace OTGUI {

CLASSNAMEINIT(DesignOfExperiment);

/* Default constructor */
DesignOfExperiment::DesignOfExperiment()
  : TypedInterfaceObject<DesignOfExperimentImplementation>(new DesignOfExperimentImplementation())
{
}


/* Constructor with parameters */
DesignOfExperiment::DesignOfExperiment(const String & name, const PhysicalModel & physicalModel)
  : TypedInterfaceObject<DesignOfExperimentImplementation>(new DesignOfExperimentImplementation(name, physicalModel))
{
}


/* Constructor with parameters */
DesignOfExperiment::DesignOfExperiment(const String & name, const PhysicalModel & physicalModel,
                                       const NumericalPoint & lowerBounds,
                                       const NumericalPoint & upperBounds, const Indices & nbValues,
                                       const NumericalPoint & values)
  : TypedInterfaceObject<DesignOfExperimentImplementation>(new DesignOfExperimentImplementation(name, physicalModel, lowerBounds, upperBounds, nbValues))
{
}


/* Constructor with parameters */
DesignOfExperiment::DesignOfExperiment(const String & name, const PhysicalModel & physicalModel,
                                       const String & fileName, Indices columns)
  : TypedInterfaceObject<DesignOfExperimentImplementation>(new DesignOfExperimentImplementation(name, physicalModel, fileName, columns))
{
}

// TODO
// DesignOfExperiment::DesignOfExperiment(const String & name, const PhysicalModel & physicalModel,
//                                        const Experiment & experiment)
//   : TypedInterfaceObject<DesignOfExperimentImplementation>(new DesignOfExperimentImplementation(name, physicalModel, experiment))
// {
// }


/* Default constructor */
DesignOfExperiment::DesignOfExperiment(const DesignOfExperimentImplementation & implementation)
  : TypedInterfaceObject<DesignOfExperimentImplementation>(implementation.clone())
{
}


/* Constructor from implementation */
DesignOfExperiment::DesignOfExperiment(const Implementation & p_implementation)
  : TypedInterfaceObject<DesignOfExperimentImplementation>(p_implementation)
{
  // Initialize any other class members here
  // At last, allocate memory space if needed, but go to destructor to free it
}


/* Constructor from implementation pointer */
DesignOfExperiment::DesignOfExperiment(DesignOfExperimentImplementation * p_implementation)
  : TypedInterfaceObject<DesignOfExperimentImplementation>(p_implementation)
{
  // Initialize any other class members here
  // At last, allocate memory space if needed, but go to destructor to free it
}


void DesignOfExperiment::addObserver(Observer * observer)
{
  getImplementation()->addObserver(observer);
}


PhysicalModel DesignOfExperiment::getPhysicalModel() const
{
  return getImplementation()->getPhysicalModel();
}


void DesignOfExperiment::setPhysicalModel(const PhysicalModel & physicalModel)
{
  getImplementation()->setPhysicalModel(physicalModel);
}


DesignOfExperimentImplementation::Type DesignOfExperiment::getTypeDesignOfExperiment() const
{
  return getImplementation()->getTypeDesignOfExperiment();
}


int DesignOfExperiment::getNumberOfExperiments() const
{
  return getImplementation()->getNumberOfExperiments();
}


NumericalPoint DesignOfExperiment::getValues() const
{
  return getImplementation()->getValues();
}


void DesignOfExperiment::setValues(const NumericalPoint & values)
{
  getImplementation()->setValues(values);
}


NumericalPoint DesignOfExperiment::getLowerBounds() const
{
  return getImplementation()->getLowerBounds();
}


void DesignOfExperiment::setLowerBounds(const NumericalPoint & lowerBounds)
{
  getImplementation()->setLowerBounds(lowerBounds);
}


NumericalPoint DesignOfExperiment::getUpperBounds() const
{
  return getImplementation()->getUpperBounds();
}


void DesignOfExperiment::setUpperBounds(const NumericalPoint & upperBounds)
{
  getImplementation()->setUpperBounds(upperBounds);
}


Indices DesignOfExperiment::getLevels() const
{
  return getImplementation()->getLevels();
}


void DesignOfExperiment::setLevels(const Indices & nbValues)
{
  getImplementation()->setLevels(nbValues);
}


NumericalPoint DesignOfExperiment::getDeltas() const
{
  return getImplementation()->getDeltas();
}


void DesignOfExperiment::setDeltas(const NumericalPoint & deltas)
{
  getImplementation()->setDeltas(deltas);
}


String DesignOfExperiment::getFileName() const
{
  return getImplementation()->getFileName();
}


void DesignOfExperiment::setFileName(const String & fileName)
{
  getImplementation()->setFileName(fileName);
}


Indices DesignOfExperiment::getColumns() const
{
  return getImplementation()->getColumns();
}


void DesignOfExperiment::setColumns(Indices columns)
{
  getImplementation()->setColumns(columns);
}

// TODO
// Experiment DesignOfExperiment::getExperiment() const
// {
//   return getImplementation()->getExperiment();
// }
// 
// 
// void DesignOfExperiment::setExperiment(const Experiment & experiment)
// {
//   getImplementation()->setExperiment(experiment);
// }


Description DesignOfExperiment::getVariableInputsNames() const
{
  return getImplementation()->getVariableInputsNames();
}


NumericalSample DesignOfExperiment::getInputSample()
{
  return getImplementation()->getInputSample();
}


void DesignOfExperiment::setInputSample(const NumericalSample & sample)
{
  getImplementation()->setInputSample(sample);
}


SimulationAnalysisResult DesignOfExperiment::getResult() const
{
  return getImplementation()->getResult();
}


void DesignOfExperiment::clearResult()
{
  getImplementation()->clearResult();
}


void DesignOfExperiment::updateParameters()
{
  getImplementation()->updateParameters();
}


void DesignOfExperiment::eval()
{
  getImplementation()->eval();
}


String DesignOfExperiment::dump() const
{
  return getImplementation()->dump();
}


/* Method save() stores the object through the StorageManager */
void DesignOfExperiment::save(Advocate & adv) const
{
  getImplementation()->save(adv);
}


/* Method load() reloads the object from the StorageManager */
void DesignOfExperiment::load(Advocate & adv)
{
  getImplementation()->load(adv);
}
}