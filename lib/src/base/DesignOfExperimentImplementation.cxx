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
#include "persalys/DesignOfExperimentImplementation.hxx"

#include "persalys/BaseTools.hxx"

#include <openturns/PersistentObjectFactory.hxx>
#include <openturns/SpecFunc.hxx>

using namespace OT;

namespace PERSALYS
{

CLASSNAMEINIT(DesignOfExperimentImplementation)

const static Factory<DesignOfExperimentImplementation> Factory_DesignOfExperimentImplementation;

/* Constructor with parameters */
DesignOfExperimentImplementation::DesignOfExperimentImplementation(const String& name, const PhysicalModel& physicalModel)
  : physicalModel_(physicalModel)
{
  DesignOfExperimentImplementation::setName(name);
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

bool DesignOfExperimentImplementation::hasPhysicalModel() const
{
  return physicalModel_.has_value();
}


PhysicalModel DesignOfExperimentImplementation::getPhysicalModel() const
{
  if (!physicalModel_)
    throw InvalidValueException(HERE) << "No physical model is associated to the design of experiment " << getName();
  return physicalModel_.value();
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
      if (!physicalModel_->getInputNames().contains(sampleDescription[i]))
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
      if (!physicalModel_->getOutputNames().contains(sampleDescription[i]))
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

void DesignOfExperimentImplementation::setType(Type type)
{
    type_ = type;
}

DesignOfExperimentImplementation::Type DesignOfExperimentImplementation::getType() const
{
    return type_;
}

String DesignOfExperimentImplementation::TypeToString(Type type)
{
    switch(type)
    {
        case MC:
            return "MC";
        case QMC:
            return "QMC";
        case LHS:
            return "LHS";
        case GRID:
            return "GRID";
        default:
            throw InvalidArgumentException(HERE) << "Invalid ImportedDesignOfExperiment type";
    }
}

String DesignOfExperimentImplementation::getPythonScript() const
{
  OSS oss;

  if (physicalModel_)
    oss << getName() << " = persalys." << getClassName() << "('" << getName() << "', " << physicalModel_->getName() << ")\n";
  else
  {
    oss << getName() << " = persalys." << getClassName() << "()\n";
    if (getName().size())
      oss << getName() << ".setName('" << getName() << "')\n";
  }

  oss << "inputSample = " << Parameters::GetOTSampleStr(getInputSample()) << "\n";
  oss << "outputSample = " << Parameters::GetOTSampleStr(getOutputSample()) << "\n";
  oss << getName() << ".setInputSample(inputSample)\n";
  oss << getName() << ".setOutputSample(outputSample)\n";
  oss << getName() << ".setType(persalys.DesignOfExperimentImplementation." << TypeToString(type_) << ")\n";

  return oss;
}

String DesignOfExperimentImplementation::__repr__() const
{
  OSS oss;
  oss << "class=" << GetClassName()
      << " name=" << getName()
      << " physicalModel=";
  if (physicalModel_)
    oss << getPhysicalModel().getName();
  else
    oss << "None";
  
  oss << " inputSample=" << getInputSample()
      << " outputSample=" << getOutputSample()
      << " type=" << TypeToString(type_);
  return oss;
}


/* Method save() stores the object through the StorageManager */
void DesignOfExperimentImplementation::save(Advocate& adv) const
{
  DataSample::save(adv);
  if (physicalModel_)
  {
    adv.saveAttribute("hasPhysicalModel_", true);
    adv.saveAttribute("physicalModel_", physicalModel_.value());
  }
  else
  {
    adv.saveAttribute("hasPhysicalModel_", false);
  }
  adv.saveAttribute("type_", static_cast<UnsignedInteger>(type_));
}


/* Method load() reloads the object from the StorageManager */
void DesignOfExperimentImplementation::load(Advocate& adv)
{
  DataSample::load(adv);
  Bool hasPhysicalModel;
  adv.loadAttribute("hasPhysicalModel_", hasPhysicalModel);
  if (hasPhysicalModel)
  {
    PhysicalModel physicalModel;
    adv.loadAttribute("physicalModel_", physicalModel);
    physicalModel_ = physicalModel;
  }
  if (adv.hasAttribute("type_"))
  {
    UnsignedInteger typeInt;
    adv.loadAttribute("type_", typeInt);
    type_ = static_cast<Type>(typeInt);
  }
}

} // END namespace PERSALYS