//                                               -*- C++ -*-
/**
 *  @brief Class to define data model
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
#include "persalys/ImportedDataset.hxx"

#include "persalys/BaseTools.hxx"

#include <openturns/PersistentObjectFactory.hxx>

using namespace OT;

namespace PERSALYS
{

CLASSNAMEINIT(ImportedDataset)

static const Factory<ImportedDataset> Factory_ImportedDataset;

/* Constructor with parameters */
ImportedDataset::ImportedDataset(const String& fileName,
                       const Indices& inputColumns,
                       const Indices& outputColumns)
: PersistentObject()
{
  ImportedDataset::setFileName(fileName);
  ImportedDataset::setColumns(inputColumns, outputColumns);
}

/* Virtual constructor */
ImportedDataset* ImportedDataset::clone() const
{
  return new ImportedDataset(*this);
}

String ImportedDataset::getFileName() const
{
  return fileName_;
}

bool ImportedDataset::setFileName(const String& fileName, const Tools::DataOrder order)
{
  if (fileName.empty())
    throw InvalidArgumentException(HERE) << "The file name cannot be empty";

  dataOrder_ = order;
  Sample oldSample = sampleFromFile_;

  // get sample from file
  if (order == Tools::DataOrder::Unknown)
    // order unkown => mesh
    sampleFromFile_ = Tools::ImportMesh(fileName);
  else
    sampleFromFile_ = Tools::ImportSample(fileName, order);
  
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
      // try to use the same indices (and names for DataModel)
      check();
      validArg = true;
    }
    catch (const std::exception &)
    {
      // if the file content has changed
    }
  }
  // default values if needed
  if (!validArg)
  {
    setDefaultColumns();
  }
  else if (oldSample.getSize())
  {
    setNames(oldSample.getMarginal(inputColumns_).getDescription(),
             oldSample.getMarginal(outputColumns_).getDescription());
  }

  return validArg;
}


Indices ImportedDataset::getInputColumns() const
{
  return inputColumns_;
}


Indices ImportedDataset::getOutputColumns() const
{
  return outputColumns_;
}


void ImportedDataset::setColumns(const Indices &inputColumns,
                            const Indices &outputColumns)
{
  // check indices
  if (!inputColumns.check(getSampleFromFile().getDimension()))
    throw InvalidArgumentException(HERE) << "Values in the inputs columns list are not compatible with the sample dimension contained in the file.";

  if (!outputColumns.check(getSampleFromFile().getDimension()))
    throw InvalidArgumentException(HERE) << "Values in the outputs columns list are not compatible with the sample dimension contained in the file.";

  Indices indices(inputColumns);
  indices.add(outputColumns);

  if (!indices.check(getSampleFromFile().getDimension()))
    throw InvalidArgumentException(HERE) << "A value cannot be in the two columns lists at the same time.";

  // set attributs
  inputColumns_ = inputColumns;
  outputColumns_ = outputColumns;
}

void ImportedDataset::setDefaultColumns()
{
  const UnsignedInteger dim = getSampleFromFile().getDimension();
  Indices inputColumns(dim > 1 ? dim - 1 : 1);
  inputColumns.fill();
  Indices outputColumns(dim > 1 ? 1 : 0, dim - 1);
  setColumns(inputColumns, outputColumns);
}

void ImportedDataset::setNames(const Description & inputNames, const Description & outputNames)
{

  if (inputColumns_.getSize() != inputNames.getSize())
    throw InvalidArgumentException(HERE)  << "The dimension of the inputs names list (" << inputNames.getSize() 
                                          << ") has to be equal to the dimension of the inputs columns list (" 
                                          << inputColumns_.getSize() << ").";

  if (outputColumns_.getSize() != outputNames.getSize())
    throw InvalidArgumentException(HERE)  << "The dimension of the outputs names list (" << outputNames.getSize() 
                                          << ") has to be equal to the dimension of the outputs columns list (" 
                                          << outputColumns_.getSize() << ").";

  // - check unicity of the variables names
  std::set<String> variableNamesSet;
  for (UnsignedInteger i = 0; i < inputNames.getSize(); ++i)
    variableNamesSet.insert(inputNames[i]);
  for (UnsignedInteger i = 0; i < outputNames.getSize(); ++i)
    variableNamesSet.insert(outputNames[i]);

  if (variableNamesSet.size() != (inputNames.getSize() + outputNames.getSize()))
    throw InvalidArgumentException(HERE) << "Two variables cannot have the same name.";

  Description sampleDescription{sampleFromFile_.getDescription()};
  for (UnsignedInteger i = 0 ; i < inputColumns_.getSize(); ++i)
    sampleDescription[inputColumns_[i]] = inputNames[i];
  for (UnsignedInteger i = 0 ; i < outputColumns_.getSize(); ++i)
    sampleDescription[outputColumns_[i]] = outputNames[i];

  
  sampleFromFile_.setDescription(sampleDescription);
}

void ImportedDataset::check()
{
  // try to use the same indices
  setColumns(inputColumns_, outputColumns_);
}


Sample ImportedDataset::getSampleFromFile() const
{
  return sampleFromFile_;
}

void ImportedDataset::setSampleFromFile(const Sample & sample)
{
  sampleFromFile_ = sample;
  fileName_.clear();
}

/* String converter */
String ImportedDataset::__repr__() const
{
  OSS oss;
  oss << " fileName=" << getFileName()
      << " inputColumns=" << getInputColumns()
      << " outputColumns=" << getOutputColumns();
  return oss;
}


/* Method save() stores the object through the StorageManager */
void ImportedDataset::save(Advocate & adv) const
{
  PersistentObject::save(adv);
  adv.saveAttribute("fileName_", fileName_);
  adv.saveAttribute("inputColumns_", inputColumns_);
  adv.saveAttribute("outputColumns_", outputColumns_);
  adv.saveAttribute("dataOrder_", static_cast<UnsignedInteger>(dataOrder_));
  adv.saveAttribute("sampleFromFile_", sampleFromFile_);
}


/* Method load() reloads the object from the StorageManager */
void ImportedDataset::load(Advocate & adv)
{
  PersistentObject::load(adv);
  adv.loadAttribute("fileName_", fileName_);
  adv.loadAttribute("inputColumns_", inputColumns_);
  adv.loadAttribute("outputColumns_", outputColumns_);
  UnsignedInteger dataOrder_as_uint;
  adv.loadAttribute("dataOrder_", dataOrder_as_uint);
  dataOrder_ = static_cast<Tools::DataOrder>(dataOrder_as_uint);
  try
  {
    setFileName(fileName_, dataOrder_);
  }
  catch (const Exception &)
  {
    // in case of problem (file not found, changed content, ...)
    // load sample directly
    adv.loadAttribute("sampleFromFile_", sampleFromFile_);
    fileName_.clear();
  }
}

} // end namespace PERSALYS