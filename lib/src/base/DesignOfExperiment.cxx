//                                               -*- C++ -*-
/**
 *  @brief Class to define designs of experiments
 *
 *  Copyright 2015-2025 EDF-Phimeca
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

DesignOfExperiment::DesignOfExperiment(const String &name, const std::optional<PhysicalModel> &physicalModel, const std::optional<ImportedDataset> &importedDataset, const Description &inputNames, const Description &outputNames)
  : TypedInterfaceObject<DataModel>(new DataModel(name, physicalModel, importedDataset, inputNames, outputNames))
{
}

DesignOfExperiment::DesignOfExperiment(const String &name, const String &filename, const Indices &inputColumns, const Indices &outputColumns, const Description &inputNames, const Description &outputNames)
  : TypedInterfaceObject<DataModel>(new DataModel(name, filename, inputColumns, outputColumns, inputNames, outputNames))
{
}

DesignOfExperiment::DesignOfExperiment(const String &name, const Sample &inSample, const Sample &outSample)
  : TypedInterfaceObject<DataModel>(new DataModel(name, inSample, outSample))
{
}

/* Default constructor */
DesignOfExperiment::DesignOfExperiment(const DataModel& implementation)
  : TypedInterfaceObject<DataModel>(implementation.clone())
{
}


/* Constructor from implementation */
DesignOfExperiment::DesignOfExperiment(const Implementation& p_implementation)
  : TypedInterfaceObject<DataModel>(p_implementation)
{
  // Initialize any other class members here
  // At last, allocate memory space if needed, but go to destructor to free it
}


/* Constructor from implementation pointer */
DesignOfExperiment::DesignOfExperiment(DataModel* p_implementation)
  : TypedInterfaceObject<DataModel>(p_implementation)
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

Sample DesignOfExperiment::getMarginalWithoutNaN(const UnsignedInteger i) const
{
  return getImplementation()->getMarginalWithoutNaN(i);
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
  TypedInterfaceObject< DataModel >::setImplementationAsPersistentObject(obj);
  getImplementation()->notify("implementationModified");
}


Indices DesignOfExperiment::getEffectiveInputIndices() const
{
  return getImplementation()->getEffectiveInputIndices();
}

void DesignOfExperiment::setType(Type type)
{
  getImplementation()->setType(type);
}

DesignOfExperiment::Type DesignOfExperiment::getType() const
{
  return getImplementation()->getType();
}

}