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
#include "persalys/FixedDesignOfExperiment.hxx"

#include "persalys/BaseTools.hxx"

#include <openturns/PersistentObjectFactory.hxx>

using namespace OT;

namespace PERSALYS
{

CLASSNAMEINIT(FixedDesignOfExperiment)

const static Factory<FixedDesignOfExperiment> Factory_FixedDesignOfExperiment;

/* Default constructor */
FixedDesignOfExperiment::FixedDesignOfExperiment()
  : DesignOfExperimentEvaluation()
{
}


/* Constructor with parameters */
FixedDesignOfExperiment::FixedDesignOfExperiment(const String& name, const PhysicalModel& physicalModel)
  : DesignOfExperimentEvaluation(name, physicalModel)
{
}


FixedDesignOfExperiment::FixedDesignOfExperiment(const String& name,
    const PhysicalModel& physicalModel,
    const Sample& inputSample)
  : DesignOfExperimentEvaluation(name, physicalModel)
{
  setOriginalInputSample(inputSample);
}


/* Virtual constructor */
FixedDesignOfExperiment* FixedDesignOfExperiment::clone() const
{
  return new FixedDesignOfExperiment(*this);
}


void FixedDesignOfExperiment::setOriginalInputSample(const Sample& sample, Type type)
{
  Sample newsample(sample);
  if (newsample.getSize())
  {
    if (getPhysicalModel().getInputDimension() != sample.getDimension())
      throw InvalidArgumentException(HERE) << "The sample dimension (" << sample.getDimension()
                                           << ") must be equal to the number of inputs in the physical model " << getPhysicalModel().getInputDimension();

    newsample.setDescription(getPhysicalModel().getInputNames());
  }
  originalInputSample_ = newsample;
  initialize();
  type_ = type;
}


Sample FixedDesignOfExperiment::generateInputSample(const UnsignedInteger /*nbSimu*/) const
{
  return originalInputSample_;
}

void FixedDesignOfExperiment::launch()
{
  DesignOfExperimentEvaluation::launch();
  result_.designOfExperiment_.setType(type_);
}


String FixedDesignOfExperiment::getPythonScript() const
{
  OSS oss;

  oss << getName() << " = persalys.FixedDesignOfExperiment('" << getName() << "', " << getPhysicalModel().getName() << ")\n";
  oss << "inputSample = " << Parameters::GetOTSampleStr(getOriginalInputSample());
  oss << getName() << ".setOriginalInputSample(inputSample)\n";
  oss << getName() << ".setBlockSize(" << getBlockSize() << ")\n";
  oss << "interestVariables = " << Parameters::GetOTDescriptionStr(getInterestVariables()) << "\n";
  oss << getName() << ".setInterestVariables(interestVariables)\n";

  if (getResult().getDesignOfExperiment().getOutputSample().getSize())
    oss << getName() << ".run()\n";

  return oss;
}


/* String converter */
String FixedDesignOfExperiment::__repr__() const
{
  OSS oss;
  oss << "class=" << GetClassName()
      << " name=" << getName()
      << " physicalModel=" << getPhysicalModel().getName();
  return oss;
}
}
