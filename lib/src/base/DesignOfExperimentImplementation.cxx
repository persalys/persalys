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
#include "otgui/DesignOfExperimentImplementation.hxx"

using namespace OT;

namespace OTGUI {

/* Default constructor */
DesignOfExperimentImplementation::DesignOfExperimentImplementation()
  : PersistentObject()
  , Observable()
  , DataSample()
{
}


/* Constructor with parameters */
DesignOfExperimentImplementation::DesignOfExperimentImplementation(const String & name, const PhysicalModel & physicalModel)
  : PersistentObject()
  , Observable()
  , DataSample()
  , physicalModel_(physicalModel)
{
  setName(name);
}


/* Virtual constructor */
DesignOfExperimentImplementation* DesignOfExperimentImplementation::clone() const
{
  return new DesignOfExperimentImplementation(*this);
}


PhysicalModel DesignOfExperimentImplementation::getPhysicalModel() const
{
  return physicalModel_;
}


void DesignOfExperimentImplementation::setPhysicalModel(const PhysicalModel & physicalModel)
{
  physicalModel_ = physicalModel;
}


Description DesignOfExperimentImplementation::getVariableInputNames() const
{
  return physicalModel_.getInputNames();
}


NumericalSample DesignOfExperimentImplementation::getInputSample()
{
  throw NotYetImplementedException(HERE) << "In DesignOfExperimentImplementation::getInputSample()";
}


void DesignOfExperimentImplementation::clearResult()
{
  setOutputSample(NumericalSample());
}


void DesignOfExperimentImplementation::run()
{
  setOutputSample(physicalModel_.getFunction()(getInputSample()));
  notify("analysisFinished");
}


String DesignOfExperimentImplementation::getPythonScript() const
{
  throw NotYetImplementedException(HERE) << "In DesignOfExperimentImplementation::getPythonScript()";
}


/* Method save() stores the object through the StorageManager */
void DesignOfExperimentImplementation::save(Advocate & adv) const
{
  PersistentObject::save(adv);
  DataSample::save(adv);
  adv.saveAttribute("physicalModel_", physicalModel_);
}


/* Method load() reloads the object from the StorageManager */
void DesignOfExperimentImplementation::load(Advocate & adv)
{
  PersistentObject::load(adv);
  DataSample::load(adv);
  adv.loadAttribute("physicalModel_", physicalModel_);
}
}