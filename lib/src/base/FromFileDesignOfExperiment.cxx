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

#include "openturns/PersistentObjectFactory.hxx"

using namespace OT;

namespace OTGUI {

CLASSNAMEINIT(FromFileDesignOfExperiment);

static Factory<FromFileDesignOfExperiment> RegisteredFactory;

/* Default constructor */
FromFileDesignOfExperiment::FromFileDesignOfExperiment()
  : DesignOfExperimentImplementation()
  , fileName_("")
{
}


/* Constructor with parameters */
FromFileDesignOfExperiment::FromFileDesignOfExperiment(const String & name, const PhysicalModel & physicalModel)
  : DesignOfExperimentImplementation(name, physicalModel)
  , fileName_("")
{
}


/* Constructor with parameters */
FromFileDesignOfExperiment::FromFileDesignOfExperiment(const String & name,
                                                       const PhysicalModel & physicalModel,
                                                       const String & fileName,
                                                       const OT::Indices & inputColumns)
  : DesignOfExperimentImplementation(name, physicalModel)
{
  setFileName(fileName);
  setInputColumns(inputColumns);
}


/* Virtual constructor */
FromFileDesignOfExperiment* FromFileDesignOfExperiment::clone() const
{
  return new FromFileDesignOfExperiment(*this);
}


void FromFileDesignOfExperiment::generateInputSample()
{
  if (!sampleFromFile_.getSize())
    sampleFromFile_ = ImportSample(fileName_);

  if (!inputColumns_.check(sampleFromFile_.getDimension()))
    throw InvalidArgumentException(HERE) << "In FromFileDesignOfExperiment::getInputSample: input columns is not compatible with the sample contained in the file";

  NumericalSample inS(sampleFromFile_.getMarginal(inputColumns_));
  inS.setDescription(physicalModel_.getInputNames());
  setInputSample(inS);
}


String FromFileDesignOfExperiment::getFileName() const
{
  return fileName_;
}


void FromFileDesignOfExperiment::setFileName(const String & fileName)
{
  fileName_ = fileName;
  NumericalSample importedSample(ImportSample(fileName_));
  if (importedSample == sampleFromFile_)
    return;
  sampleFromFile_ = importedSample;
  // reinitialization
  setInputSample(NumericalSample());
  setOutputSample(NumericalSample());
  inputColumns_ = Indices();
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

  if (!sampleFromFile_.getSize())
    sampleFromFile_ = ImportSample(fileName_);

  if (!inputColumns.check(sampleFromFile_.getDimension()))
    throw InvalidArgumentException(HERE) << "In FromFileDesignOfExperiment::setInputColumns: input columns is not compatible with the sample contained in the file";

  inputColumns_ = inputColumns;
  generateInputSample();
  setOutputSample(NumericalSample());
}


NumericalSample FromFileDesignOfExperiment::ImportSample(const String & fileName)
{
  std::vector< String > separatorsList(3);
  separatorsList[0] = " ";
  separatorsList[1] = ",";
  separatorsList[2] = ";";
  NumericalSample sampleFromFile;

  for (UnsignedInteger i = 0; i < separatorsList.size(); ++ i)
  {
    // import sample from the file
    sampleFromFile = NumericalSample::ImportFromTextFile(fileName, separatorsList[i]);
    if (sampleFromFile.getSize())
      break;
  }
  if (!sampleFromFile.getSize())
    throw InvalidArgumentException(HERE) << "In FromFileDesignOfExperiment: impossible to load sample";

  return sampleFromFile;
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