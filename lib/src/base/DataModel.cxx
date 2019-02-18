//                                               -*- C++ -*-
/**
 *  @brief Class to define data model
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
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
#include "otgui/DataModel.hxx"

#include "otgui/ImportedDesignOfExperiment.hxx"
#include "otgui/BaseTools.hxx"

#include <openturns/PersistentObjectFactory.hxx>

using namespace OT;

namespace OTGUI
{

CLASSNAMEINIT(DataModel)

static Factory<DataModel> Factory_DataModel;

/* Default constructor */
DataModel::DataModel(const String& name)
  : DesignOfExperimentImplementation()
{
  setName(name);
  hasPhysicalModel_ = false;
}


/* Constructor with parameters */
DataModel::DataModel(const String& name,
                     const String& fileName,
                     const Indices& inputColumns,
                     const Indices& outputColumns,
                     const Description& inputNames,
                     const Description& outputNames)
  : DesignOfExperimentImplementation()
{
  setName(name);
  hasPhysicalModel_ = false;

  setFileName(fileName);
  setColumns(inputColumns, outputColumns, inputNames, outputNames);
}


/* Constructor with parameters */
DataModel::DataModel(const String& name,
                     const Sample& inSample,
                     const Sample& outSample)
  : DesignOfExperimentImplementation()
  , inputNames_(inSample.getDescription())
  , outputNames_(outSample.getDescription())
{
  setName(name);
  hasPhysicalModel_ = false;

  setInputSample(inSample);
  setOutputSample(outSample);
  inputColumns_ = Indices(inSample.getDimension());
  inputColumns_.fill();
  if (outSample.getSize())
  {
    outputColumns_ = Indices(outSample.getDimension());
    outputColumns_.fill(inSample.getDimension());
  }
}


/* Virtual constructor */
DataModel* DataModel::clone() const
{
  return new DataModel(*this);
}


String DataModel::getFileName() const
{
  return fileName_;
}


void DataModel::setFileName(const String& fileName)
{
  if (fileName.empty())
    throw InvalidArgumentException(HERE) << "The file name can not be empty";

  // get sample from file
  sampleFromFile_ = getSampleFromFile(Tools::GetLocaleString(fileName));

  // save file path
  const String oldFileName = fileName_;
  fileName_ = fileName;

  // set columns and names
  bool validArg = false;
  // if reload file
  if (fileName_ == oldFileName)
  {
    try
    {
      // try to use the same indices
      setColumns(inputColumns_, outputColumns_, inputNames_, outputNames_);
      validArg = true;
    }
    catch (std::exception &)
    {
      // if the file content has changed
    }
  }
  // default values if needed
  if (!validArg)
  {
    const UnsignedInteger dim = sampleFromFile_.getDimension();
    Indices inputColumns(dim > 1 ? dim - 1 : 1);
    inputColumns.fill();
    Indices outputColumns(dim > 1 ? 1 : 0, dim - 1);
    setColumns(inputColumns, outputColumns);
  }
}


Indices DataModel::getInputColumns() const
{
  return inputColumns_;
}


Indices DataModel::getOutputColumns() const
{
  return outputColumns_;
}


void DataModel::setColumns(const Indices& inputColumns,
                           const Indices& outputColumns,
                           const Description& inputNames,
                           const Description& outputNames)
{
  // check indices
  if (!inputColumns.check(getSampleFromFile().getDimension()))
    throw InvalidArgumentException(HERE) << "Values in the inputs columns list are not compatible with the sample dimension contained in the file.";

  if (!outputColumns.check(getSampleFromFile().getDimension()))
    throw InvalidArgumentException(HERE) << "Values in the outputs columns list are not compatible with the sample dimension contained in the file.";

  Indices indices(inputColumns);
  indices.add(outputColumns);

  if (!indices.check(getSampleFromFile().getDimension()))
    throw InvalidArgumentException(HERE) << "A value can not be in the two columns lists at the same time.";

  // check names
  // - check input
  if (inputNames.getSize())
  {
    if (inputColumns.getSize() != inputNames.getSize())
      throw InvalidArgumentException(HERE) << "The dimension of the inputs names list has to be equal to the dimension of the inputs columns list.";
  }
  // - check output
  if (outputNames.getSize())
  {
    if (outputColumns.getSize() != outputNames.getSize())
      throw InvalidArgumentException(HERE) << "The dimension of the outputs names list has to be equal to the dimension of the outputs columns list.";
  }
  // - check unicity of the variables names
  if ((inputNames.getSize() + outputNames.getSize()) > 0)
  {
    std::set<String> variableNamesSet;
    for (UnsignedInteger i = 0; i < inputNames.getSize(); ++i)
      variableNamesSet.insert(inputNames[i]);
    for (UnsignedInteger i = 0; i < outputNames.getSize(); ++i)
      variableNamesSet.insert(outputNames[i]);

    if (variableNamesSet.size() != (inputNames.getSize() + outputNames.getSize()))
      throw InvalidArgumentException(HERE) << "Two variables can not have the same name.";
  }

  // set attributs
  inputColumns_ = inputColumns;
  outputColumns_ = outputColumns;
  inputNames_ = inputNames;
  outputNames_ = outputNames;

  // set samples
  Sample inS;
  if (inputColumns_.getSize())
  {
    inS = sampleFromFile_.getMarginal(inputColumns_);
    inS.setDescription(getInputNames());
  }
  setInputSample(inS);

  Sample outS;
  if (outputColumns_.getSize())
  {
    outS = sampleFromFile_.getMarginal(outputColumns_);
    outS.setDescription(getOutputNames());
  }
  setOutputSample(outS);

  notify("variablesChanged");
}


void DataModel::setNames(const Description & inputNames, const Description & outputNames)
{
  // check names
  // - check input
  if (inputNames.getSize())
  {
    if (inputColumns_.getSize() != inputNames.getSize())
      throw InvalidArgumentException(HERE) << "The dimension of the inputs names list has to be equal to the dimension of the inputs columns list.";
  }
  // - check output
  if (outputNames.getSize())
  {
    if (outputColumns_.getSize() != outputNames.getSize())
      throw InvalidArgumentException(HERE) << "The dimension of the outputs names list has to be equal to the dimension of the outputs columns list.";
  }
  // - check unicity of the variables names
  if ((inputNames.getSize() + outputNames.getSize()) > 0)
  {
    std::set<String> variableNamesSet;
    for (UnsignedInteger i = 0; i < inputNames.getSize(); ++i)
      variableNamesSet.insert(inputNames[i]);
    for (UnsignedInteger i = 0; i < outputNames.getSize(); ++i)
      variableNamesSet.insert(outputNames[i]);

    if (variableNamesSet.size() != (inputNames.getSize() + outputNames.getSize()))
      throw InvalidArgumentException(HERE) << "Two variables can not have the same name.";
  }
  // set attributs
  inputNames_ = inputNames;
  outputNames_ = outputNames;

  // set samples
  Sample inS(getInputSample());
  if (inS.getSize())
    inS.setDescription(getInputNames());
  setInputSample(inS);

  Sample outS(getOutputSample());
  if (outS.getSize())
    outS.setDescription(getOutputNames());
  setOutputSample(outS);
}


Description DataModel::getInputNames()
{
  if (!inputNames_.getSize())
  {
    const Description sampleDescription(getSampleFromFile().getDescription());

    // set input names
    inputNames_ = Description(inputColumns_.getSize());
    for (UnsignedInteger i = 0; i < inputColumns_.getSize(); ++i)
      inputNames_[i] = sampleDescription[inputColumns_[i]];
  }
  return inputNames_;
}


Description DataModel::getOutputNames()
{
  if (!outputNames_.getSize())
  {
    const Description sampleDescription(getSampleFromFile().getDescription());

    // set output names
    outputNames_ = Description(outputColumns_.getSize());
    for (UnsignedInteger i = 0; i < outputColumns_.getSize(); ++i)
      outputNames_[i] = sampleDescription[outputColumns_[i]];
  }
  return outputNames_;
}


Sample DataModel::getSampleFromFile() const
{
  return sampleFromFile_;
}


Sample DataModel::getSampleFromFile(const String& fileName)
{
  Sample sampleFromFile(Tools::ImportSample(fileName));

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


String DataModel::getPythonScript() const
{
  OSS oss;

  if (getFileName().empty())
  {
    oss << "inputSample = ot.Sample(" << Parameters::GetOTSampleStr(getInputSample()) << ")\n";
    oss << "inputSample.setDescription(" << Parameters::GetOTDescriptionStr(inputNames_) << ")\n";
    oss << "outputSample = ot.Sample(" << Parameters::GetOTSampleStr(getOutputSample()) << ")\n";
    oss << "outputSample.setDescription(" << Parameters::GetOTDescriptionStr(outputNames_) << ")\n";
    oss << getName() << " = otguibase.DataModel('" << getName() << "', inputSample, outputSample)\n";
  }
  else
  {
    oss << "inputColumns = " << inputColumns_.__str__() << "\n";
    oss << "outputColumns = " << outputColumns_.__str__() << "\n";

    oss << "inputNames = " << Parameters::GetOTDescriptionStr(inputNames_) << "\n";
    oss << "outputNames = " << Parameters::GetOTDescriptionStr(outputNames_) << "\n";

    oss << getName() << " = otguibase.DataModel('" << getName() << "', ";
    oss << "'" << getFileName() << "', inputColumns, outputColumns, inputNames, outputNames)\n";
  }

  return oss;
}


/* String converter */
String DataModel::__repr__() const
{
  OSS oss;
  oss << "class=" << GetClassName()
      << " name=" << getName()
      << " fileName=" << getFileName()
      << " inputColumns=" << getInputColumns()
      << " outputColumns=" << getOutputColumns()
      << " inputNames=" << inputNames_
      << " outputNames=" << outputNames_;
  return oss;
}


/* Method save() stores the object through the StorageManager */
void DataModel::save(Advocate & adv) const
{
  DesignOfExperimentImplementation::save(adv);
  adv.saveAttribute("fileName_", fileName_);
  adv.saveAttribute("inputColumns_", inputColumns_);
  adv.saveAttribute("outputColumns_", outputColumns_);
  adv.saveAttribute("inputNames_", inputNames_);
  adv.saveAttribute("outputNames_", outputNames_);
}


/* Method load() reloads the object from the StorageManager */
void DataModel::load(Advocate & adv)
{
  DesignOfExperimentImplementation::load(adv);
  adv.loadAttribute("fileName_", fileName_);
  adv.loadAttribute("inputColumns_", inputColumns_);
  adv.loadAttribute("outputColumns_", outputColumns_);
  adv.loadAttribute("inputNames_", inputNames_);
  adv.loadAttribute("outputNames_", outputNames_);
}
}
