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
#include "otgui/FromFileDesignOfExperiment.hxx"

#include "Box.hxx"
#include "TruncatedDistribution.hxx"
#include "TruncatedNormal.hxx"
#include "PersistentObjectFactory.hxx"

using namespace OT;

namespace OTGUI {

CLASSNAMEINIT(FromFileDesignOfExperiment);

static Factory<FromFileDesignOfExperiment> RegisteredFactory("FromFileDesignOfExperiment");

/* Default constructor */
FromFileDesignOfExperiment::FromFileDesignOfExperiment()
  : DesignOfExperimentImplementation()
{
}


/* Constructor with parameters */
FromFileDesignOfExperiment::FromFileDesignOfExperiment(const String & name, const PhysicalModel & physicalModel)
  : DesignOfExperimentImplementation(name, physicalModel)
{
}


/* Constructor with parameters */
FromFileDesignOfExperiment::FromFileDesignOfExperiment(const String & name,
                                                       const PhysicalModel & physicalModel,
                                                       const String & fileName,
                                                       const OT::Indices & inputColumns)
  : DesignOfExperimentImplementation(name, physicalModel)
  , fileName_(fileName)
{
  setInputColumns(inputColumns);
}


/* Virtual constructor */
FromFileDesignOfExperiment* FromFileDesignOfExperiment::clone() const
{
  return new FromFileDesignOfExperiment(*this);
}


void FromFileDesignOfExperiment::setFileName(const String & fileName)
{
  fileName_ = fileName;
}


String FromFileDesignOfExperiment::getFileName() const
{
  return fileName_;
}


Indices FromFileDesignOfExperiment::getInputColumns() const
{
  return inputColumns_;
}


void FromFileDesignOfExperiment::setInputColumns(const Indices & inputColumns)
{
  if (inputColumns.getSize() != physicalModel_.getInputs().getSize())
  {
    OSS oss;
    oss << "The dimension of the list of the column numbers has to be equal to the number of inputs of the physical model: ";
    oss << physicalModel_.getInputs().getSize();
    throw InvalidArgumentException(HERE) << oss.str();
  }
  if (inputSample_.getSize() && inputColumns_ != inputColumns)
  {
    inputSample_.clear();
    clearResult();
  }

  inputColumns_ = inputColumns;
}


NumericalSample FromFileDesignOfExperiment::getInputSample()
{
  if (!inputSample_.getSize())
  {
    inputSample_.clear();
    if (fileName_.size() > 0)
    {
      std::vector< String > separatorsList(3);
      separatorsList[0] = " ";
      separatorsList[1] = ",";
      separatorsList[2] = ";";
      NumericalSample sampleFromFile;
      for (UnsignedInteger i = 0; i < separatorsList.size(); ++ i)
      {
        // import sample from the file
        sampleFromFile = NumericalSample::ImportFromTextFile(fileName_, separatorsList[i]);
        if (sampleFromFile.getSize())
          break;
      }
      if (!sampleFromFile.getSize())
        throw InvalidArgumentException(HERE) << "In FromFileDesignOfExperiment: impossible to load sample";
      if (!inputColumns_.check(sampleFromFile.getDimension()))
        throw InvalidArgumentException(HERE) << "In FromFileDesignOfExperiment: impossible to load sample marginals";
      inputSample_ = sampleFromFile.getMarginal(inputColumns_);
      inputSample_.setDescription(physicalModel_.getInputNames());
    }
  }
  return inputSample_;
}


String FromFileDesignOfExperiment::getPythonScript() const
{
  OSS oss;

  oss << "inputColumns = ot.Indices([";
  for (UnsignedInteger i = 0; i < inputColumns_.getSize(); ++ i)
  {
    oss << inputColumns_[i];
    if (i < inputColumns_.getSize()-1)
      oss << ", ";
  }
  oss << "])\n";

  oss << getName()+ " = otguibase.FromFileDesignOfExperiment('" + getName() + "', "+getPhysicalModel().getName()+", ";
  oss << "'"+fileName_+"', inputColumns)\n";

  return oss.str();
}


/* Method save() stores the object through the StorageManager */
void FromFileDesignOfExperiment::save(Advocate & adv) const
{
  DesignOfExperimentImplementation::save(adv);
  adv.saveAttribute("fileName_", fileName_);
  adv.saveAttribute("inputColumns_", inputColumns_);
}


/* Method load() reloads the object from the StorageManager */
void FromFileDesignOfExperiment::load(Advocate & adv)
{
  DesignOfExperimentImplementation::load(adv);
  adv.loadAttribute("fileName_", fileName_);
  adv.loadAttribute("inputColumns_", inputColumns_);
}
}