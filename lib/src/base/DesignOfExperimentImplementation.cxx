//                                               -*- C++ -*-
/**
 *  @brief Class to define designs of experiments
 *
 *  Copyright 2015-2023 EDF-Phimeca
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
#include "persalys/DesignOfExperimentImplementation.hxx"

#include "persalys/BaseTools.hxx"

#include <openturns/PersistentObjectFactory.hxx>
#include <openturns/SpecFunc.hxx>

using namespace OT;

namespace PERSALYS
{

CLASSNAMEINIT(DesignOfExperimentImplementation)

static Factory<DesignOfExperimentImplementation> Factory_DesignOfExperimentImplementation;

/* Default constructor */
DesignOfExperimentImplementation::DesignOfExperimentImplementation()
  : DataSample()
  , Observable()
  , hasPhysicalModel_(false)
  , physicalModel_()
{
}


/* Constructor with parameters */
DesignOfExperimentImplementation::DesignOfExperimentImplementation(const String& name, const PhysicalModel& physicalModel)
  : DataSample()
  , Observable()
  , hasPhysicalModel_(true)
  , physicalModel_(physicalModel)
{
  setName(name);
}


/* Virtual constructor */
DesignOfExperimentImplementation* DesignOfExperimentImplementation::clone() const
{
  return new DesignOfExperimentImplementation(*this);
}


void DesignOfExperimentImplementation::setName(const String & name)
{
  PersistentObject::setName(name);
  notify("nameChanged");
}


Bool DesignOfExperimentImplementation::operator==(const DesignOfExperimentImplementation& other) const
{
  if (this == &other)
    return true;
  return false;
}


Bool DesignOfExperimentImplementation::operator!=(const DesignOfExperimentImplementation& other) const
{
  return !operator==(other);
}


bool DesignOfExperimentImplementation::hasPhysicalModel() const
{
  return hasPhysicalModel_;
}


PhysicalModel DesignOfExperimentImplementation::getPhysicalModel() const
{
  return physicalModel_;
}


void DesignOfExperimentImplementation::initialize()
{
  setInputSample(Sample());
  setOutputSample(Sample());
}


void DesignOfExperimentImplementation::setInputSample(const Sample& sample)
{
  if (sample.getSize() && hasPhysicalModel())
  {
    const Description sampleDescription = sample.getDescription();
    for (UnsignedInteger i = 0; i < sampleDescription.getSize(); ++i)
    {
      if (!physicalModel_.getInputNames().contains(sampleDescription[i]))
        throw InvalidArgumentException(HERE) << "The physical model does not contain an input named " << sampleDescription[i];
    }
  }
  DataSample::setInputSample(sample);
}


void DesignOfExperimentImplementation::setOutputSample(const Sample& sample)
{
  if (sample.getSize() && hasPhysicalModel())
  {
    const Description sampleDescription = sample.getDescription();
    for (UnsignedInteger i = 0; i < sampleDescription.getSize(); ++i)
    {
      if (!physicalModel_.getOutputNames().contains(sampleDescription[i]))
        throw InvalidArgumentException(HERE) << "The physical model does not contain an output named " << sampleDescription[i];
    }
  }
  DataSample::setOutputSample(sample);
}


Indices DesignOfExperimentImplementation::getEffectiveInputIndices() const
{
  Indices inputIndices;
  if (getInputSample().getSize())
  {
    const Point xmin(getInputSample().getMin());
    const Point xmax(getInputSample().getMax());
    for (UnsignedInteger i = 0; i < xmin.getDimension(); ++i)
      if (xmax[i] > xmin[i])
        inputIndices.add(i);
  }
  return inputIndices;
}

String DesignOfExperimentImplementation::getPythonScript() const
{
  OSS oss;

  oss << getName() << " = persalys.DesignOfExperimentImplementation('" << getName() << "', " << getPhysicalModel().getName() << ")\n";

  oss << "inputSample = " << Parameters::GetOTSampleStr(getInputSample());

  oss << getName() << ".setInputSample(inputSample)\n";

  return oss;
}


/* Method save() stores the object through the StorageManager */
void DesignOfExperimentImplementation::save(Advocate& adv) const
{
  DataSample::save(adv);
  adv.saveAttribute("hasPhysicalModel_", hasPhysicalModel_);
  adv.saveAttribute("physicalModel_", physicalModel_);
}


/* Method load() reloads the object from the StorageManager */
void DesignOfExperimentImplementation::load(Advocate& adv)
{
  DataSample::load(adv);
  adv.loadAttribute("hasPhysicalModel_", hasPhysicalModel_);
  adv.loadAttribute("physicalModel_", physicalModel_);
}
}
