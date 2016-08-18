//                                               -*- C++ -*-
/**
 *  @brief Class to define data model
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
#include "otgui/DataModelImplementation.hxx"

#include "otgui/AnalyticalPhysicalModel.hxx"

#include "openturns/PersistentObjectFactory.hxx"

using namespace OT;

namespace OTGUI {

CLASSNAMEINIT(DataModelImplementation);

static Factory<DataModelImplementation> RegisteredFactory;

/* Default constructor */
DataModelImplementation::DataModelImplementation(const OT::String & name)
  : PersistentObject()
  , DataSample()
  , ImportedSample()
  , Observable()
{
  setName(name);
}


/* Constructor with parameters */
DataModelImplementation::DataModelImplementation(const String & name,
                                                 const String & fileName,
                                                 const OT::Indices & inputColumns,
                                                 const OT::Indices & outputColumns,
                                                 const Description & inputNames,
                                                 const Description & outputNames)
  : PersistentObject()
  , DataSample()
  , ImportedSample()
  , Observable()
{
  setName(name);
  setFileName(fileName);
  setColumns(inputColumns, outputColumns, inputNames, outputNames);
}


DataModelImplementation::DataModelImplementation(const String & name,
                                                 const NumericalSample & inSample,
                                                 const NumericalSample & outSample)
  : PersistentObject()
  , DataSample(inSample, outSample)
  , ImportedSample()
  , Observable()
  , inputNames_(inSample.getDescription())
  , outputNames_(outSample.getDescription())
{
  setName(name);
}


/* Virtual constructor */
DataModelImplementation* DataModelImplementation::clone() const
{
  return new DataModelImplementation(*this);
}


void DataModelImplementation::setFileName(const String & fileName)
{
  ImportedSample::setFileName(fileName);
  setInputSample(NumericalSample());
  setOutputSample(NumericalSample());
}


void DataModelImplementation::setInputColumns(const Indices & inputColumns)
{
  setColumns(inputColumns, outputColumns_, inputNames_, outputNames_);
}


Indices DataModelImplementation::getOutputColumns() const
{
  return outputColumns_;
}


void DataModelImplementation::setColumns(const Indices & inputColumns,
                                         const Indices & outputColumns,
                                         const Description & inputNames,
                                         const Description & outputNames)
{
  if (!sampleFromFile_.getSize())
    sampleFromFile_ = ImportSample(getFileName());

  // check indices
  if (!inputColumns.getSize())
    throw InvalidArgumentException(HERE) << "In DataModelImplementation::setColumns: the model must have at least one input";

  OT::Indices indices(inputColumns);
  indices.add(outputColumns);

  if (!indices.check(sampleFromFile_.getDimension()))
    throw InvalidArgumentException(HERE) << "In DataModelImplementation::setColumns: input columns and ouput columns are not compatible";

  // check names
  // input
  OT::Description inNames(inputNames);
  if (inputNames.getSize())
  {
    if (inputColumns.getSize() != inputNames.getSize())
      throw InvalidArgumentException(HERE) << "In DataModelImplementation::setColumns: inputNames size not compatible with input columns size";

    std::set<String> inputNamesSet;
    for (UnsignedInteger i=0; i<inputNames.getSize(); ++i)
      inputNamesSet.insert(inputNames[i]);
    if (inputNamesSet.size() != inputNames.getSize())
      throw InvalidArgumentException(HERE) << "Two inputs can not have the same name.";
  }
  else
  {
    OT::Description desc(inputColumns.getSize());
    for (UnsignedInteger i=0; i<inputColumns.getSize(); ++i)
      desc[i] = sampleFromFile_.getDescription()[inputColumns[i]];
    inNames = desc;
  }

  // output
  OT::Description outNames(outputNames);
  if (outputNames.getSize())
  {
    if (outputColumns.getSize() != outputNames.getSize())
      throw InvalidArgumentException(HERE) << "In DataModelImplementation::setColumns: outputNames size not compatible with output columns size";

    std::set<String> outputNamesSet;
    for (UnsignedInteger i=0; i<outputNames.getSize(); ++i)
      outputNamesSet.insert(outputNames[i]);
    if (outputNamesSet.size() != outputNames.getSize())
      throw InvalidArgumentException(HERE) << "Two outputs can not have the same name.";
  }
  else
  {
    OT::Description desc(outputColumns.getSize());
    for (UnsignedInteger i=0; i<outputColumns.getSize(); ++i)
      desc[i] = sampleFromFile_.getDescription()[outputColumns[i]];
    outNames = desc;
  }
  // set attributs
  ImportedSample::setInputColumns(inputColumns);
  outputColumns_ = outputColumns;
  inputNames_ = inNames;
  outputNames_ = outNames;

  NumericalSample inS(sampleFromFile_.getMarginal(getInputColumns()));
  inS.setDescription(inputNames_);
  setInputSample(inS);
  NumericalSample outS(sampleFromFile_.getMarginal(outputColumns_));
  outS.setDescription(outputNames_);
  setOutputSample(outS);
}


Description DataModelImplementation::getInputNames() const
{
  return inputNames_;
}


Description DataModelImplementation::getOutputNames() const
{
  return outputNames_;
}


String DataModelImplementation::getPythonScript() const
{
  OSS oss;

  oss << "inputColumns = [";
  for (UnsignedInteger i = 0; i < getInputColumns().getSize(); ++ i)
  {
    oss << getInputColumns()[i];
    if (i < getInputColumns().getSize()-1)
      oss << ", ";
  }
  oss << "]\n";

  oss << "outputColumns = [";
  for (UnsignedInteger i = 0; i < outputColumns_.getSize(); ++ i)
  {
    oss << outputColumns_[i];
    if (i < outputColumns_.getSize()-1)
      oss << ", ";
  }
  oss << "]\n";

  oss << "inputNames = [";
  for (UnsignedInteger i = 0; i < inputNames_.getSize(); ++ i)
  {
    oss << "'" << inputNames_[i] << "'";
    if (i < inputNames_.getSize()-1)
      oss << ", ";
  }
  oss << "]\n";

  oss << "outputNames = [";
  for (UnsignedInteger i = 0; i < outputNames_.getSize(); ++ i)
  {
    oss << "'" << outputNames_[i] << "'";
    if (i < outputNames_.getSize()-1)
      oss << ", ";
  }
  oss << "]\n";

  oss << getName()+ " = otguibase.DataModel('" + getName() + "', ";
  oss << "'"+getFileName()+"', inputColumns, outputColumns, inputNames, outputNames)\n";

  return oss.str();
}


/* Method save() stores the object through the StorageManager */
void DataModelImplementation::save(Advocate & adv) const
{
  PersistentObject::save(adv);
  DataSample::save(adv);
  ImportedSample::save(adv);
  adv.saveAttribute("outputColumns_", outputColumns_);
  adv.saveAttribute("inputNames_", inputNames_);
  adv.saveAttribute("outputNames_", outputNames_);
}


/* Method load() reloads the object from the StorageManager */
void DataModelImplementation::load(Advocate & adv)
{
  PersistentObject::load(adv);
  DataSample::load(adv);
  ImportedSample::load(adv);
  adv.loadAttribute("outputColumns_", outputColumns_);
  adv.loadAttribute("inputNames_", inputNames_);
  adv.loadAttribute("outputNames_", outputNames_);
}
}