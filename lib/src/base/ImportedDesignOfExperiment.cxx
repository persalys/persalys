//                                               -*- C++ -*-
/**
 *  @brief Class to define designs of experiments
 *
 *  Copyright 2015-2018 EDF-Phimeca
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
#include "otgui/ImportedDesignOfExperiment.hxx"

#include <openturns/PersistentObjectFactory.hxx>

using namespace OT;

namespace OTGUI
{

CLASSNAMEINIT(ImportedDesignOfExperiment)

static Factory<ImportedDesignOfExperiment> Factory_ImportedDesignOfExperiment;

/* Default constructor */
ImportedDesignOfExperiment::ImportedDesignOfExperiment()
  : DesignOfExperimentEvaluation()
  , fileName_("")
  , inputColumns_()
  , sampleFromFile_()
{
}


/* Constructor with parameters */
ImportedDesignOfExperiment::ImportedDesignOfExperiment(const String& name, const PhysicalModel& physicalModel)
  : DesignOfExperimentEvaluation(name, physicalModel)
  , fileName_("")
  , inputColumns_()
  , sampleFromFile_()
{
}


/* Constructor with parameters */
ImportedDesignOfExperiment::ImportedDesignOfExperiment(const String& name,
    const PhysicalModel& physicalModel,
    const String& fileName,
    const Indices& inputColumns)
  : DesignOfExperimentEvaluation(name, physicalModel)
  , fileName_("")
  , inputColumns_()
  , sampleFromFile_()
{
  setFileName(fileName);
  setInputColumns(inputColumns);
}


/* Virtual constructor */
ImportedDesignOfExperiment* ImportedDesignOfExperiment::clone() const
{
  return new ImportedDesignOfExperiment(*this);
}


Sample ImportedDesignOfExperiment::generateInputSample(const UnsignedInteger /*nbSimu*/) const
{
  Sample inS(getSampleFromFile().getMarginal(inputColumns_));
  inS.setDescription(getPhysicalModel().getInputNames());
  return inS;
}


String ImportedDesignOfExperiment::getFileName() const
{
  return fileName_;
}


void ImportedDesignOfExperiment::setFileName(const String& fileName)
{
  if (fileName.empty())
    throw InvalidArgumentException(HERE) << "The file name can not be empty";

  // get sample from file
  sampleFromFile_ = getSampleFromFile(fileName);

  // save file path
  const String oldFileName = fileName_;
  // TODO convert to utf-8
  fileName_ = fileName;

  // set columns and names
  bool validArg = false;
  // if reload file
  if (fileName_ == oldFileName)
  {
    try
    {
      // try to use the same indices
      setInputColumns(inputColumns_);
      validArg = true;
    }
    catch (std::exception)
    {
      // if the number of input variables in the physical model have changed
      // of if the file content has changed
    }
  }
  // default values if needed
  if (!validArg)
  {
    Indices inputColumns(getPhysicalModel().getInputDimension());
    inputColumns.fill();
    setInputColumns(inputColumns);
  }
}


Indices ImportedDesignOfExperiment::getInputColumns() const
{
  return inputColumns_;
}


void ImportedDesignOfExperiment::setInputColumns(const Indices& inputColumns)
{
  if (inputColumns.getSize() != getPhysicalModel().getInputDimension())
  {
    OSS oss;
    oss << "The dimension of the list of the column numbers has to be equal to the number of inputs of the physical model: ";
    oss << getPhysicalModel().getInputDimension();
    throw InvalidArgumentException(HERE) << oss.str();
  }

  if (!inputColumns.check(getSampleFromFile().getDimension()))
    throw InvalidArgumentException(HERE) << "Values in the input columns list are not compatible with the sample dimension contained in the file.";

  inputColumns_ = inputColumns;

  // reset
  originalInputSample_.clear();
  initialize();
}


Sample ImportedDesignOfExperiment::getSampleFromFile() const
{
  // TODO reload file if empty
  // when we will be able to retrieve ImportedDesignOfExperiment object
  // in the Python console (use boost::locale::conv::from_utf ?)
  return sampleFromFile_;
}


Sample ImportedDesignOfExperiment::getSampleFromFile(const String& fileName) const
{
  Sample sampleFromFile(ImportSample(fileName));

  // check sampleFromFile dimension
  const Description inputNames = getPhysicalModel().getInputNames();
  if (sampleFromFile.getDimension() < inputNames.getSize())
    throw InvalidArgumentException(HERE) << "The file contains a sample with a dimension lesser than the number of inputs of the physical model: "
                                          << inputNames.getSize();

  // check the sample description
  const Description sampleDescription(sampleFromFile.getDescription());
  Description descriptionToCheck;
  for (UnsignedInteger i = 0; i < sampleDescription.getSize(); ++i)
    if (!descriptionToCheck.contains(sampleDescription[i]) && !sampleDescription[i].empty())
      descriptionToCheck.add(sampleDescription[i]);

  // if empty name or at least two same names
  if (descriptionToCheck.getSize() != sampleDescription.getSize())
    sampleFromFile.setDescription(Description::BuildDefault(sampleDescription.getSize(), "data_"));

  return sampleFromFile;
}


Sample ImportedDesignOfExperiment::ImportSample(const String& fileName)
{
  std::vector< String > separatorsList(3);
  separatorsList[0] = " ";
  separatorsList[1] = ",";
  separatorsList[2] = ";";
  Sample sampleFromFile;

  for (UnsignedInteger i = 0; i < separatorsList.size(); ++ i)
  {
    // import sample from the file
    sampleFromFile = Sample::ImportFromTextFile(fileName, separatorsList[i]);
    if (sampleFromFile.getSize())
      break;
  }
  if (!sampleFromFile.getSize())
    throw InvalidArgumentException(HERE) << "The file " << fileName << " does not contain a sample.";

  return sampleFromFile;
}


Parameters ImportedDesignOfExperiment::getParameters() const
{
  Parameters param;

  param.add("Design type", "Imported");
  param.add("Outputs of interest", getInterestVariables().__str__());
  param.add("Sample size", getOriginalInputSample().getSize());
  param.add("File", getFileName());
  OSS columns;
  for (UnsignedInteger i = 0; i < getOriginalInputSample().getDimension(); ++i)
  {
    columns << getOriginalInputSample().getDescription()[i] << " : " << getInputColumns()[i];
    if (i < getOriginalInputSample().getDimension() - 1)
      columns << "\n";
  }
  param.add("Columns", columns);
  param.add("Block size", getBlockSize());

  return param;
}


String ImportedDesignOfExperiment::getPythonScript() const
{
  OSS oss;

  oss << "inputColumns = " << inputColumns_.__str__() << "\n";

  oss << getName() << " = otguibase.ImportedDesignOfExperiment('" << getName() << "', " << getPhysicalModel().getName() << ", ";
  oss << "'" << fileName_ << "', inputColumns)\n";

  oss << getName() << ".setBlockSize(" << getBlockSize() << ")\n";
  oss << "interestVariables = " << Parameters::GetOTDescriptionStr(getInterestVariables()) << "\n";
  oss << getName() << ".setInterestVariables(interestVariables)\n";

  return oss;
}


/* String converter */
String ImportedDesignOfExperiment::__repr__() const
{
  OSS oss;
  oss << "class=" << GetClassName()
      << " name=" << getName()
      << " physicalModel=" << getPhysicalModel().getName()
      << " fileName=" << getFileName()
      << " inputColumns=" << getInputColumns()
      << " blockSize=" << getBlockSize();
  return oss;
}


/* Method save() stores the object through the StorageManager */
void ImportedDesignOfExperiment::save(Advocate& adv) const
{
  DesignOfExperimentEvaluation::save(adv);
  adv.saveAttribute("fileName_", fileName_);
  adv.saveAttribute("inputColumns_", inputColumns_);
}


/* Method load() reloads the object from the StorageManager */
void ImportedDesignOfExperiment::load(Advocate& adv)
{
  DesignOfExperimentEvaluation::load(adv);
  adv.loadAttribute("fileName_", fileName_);
  adv.loadAttribute("inputColumns_", inputColumns_);
}
}
