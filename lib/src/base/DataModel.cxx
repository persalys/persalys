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
#include "otgui/DataModel.hxx"

#include "openturns/PersistentObjectFactory.hxx"

using namespace OT;

namespace OTGUI {

CLASSNAMEINIT(DataModel);

static Factory<DataModel> RegisteredFactory;

/* Default constructor */
DataModel::DataModel(const String & name)
  : FromFileDesignOfExperiment()
{
  setName(name);
  hasPhysicalModel_ = false;
}


/* Constructor with parameters */
DataModel::DataModel(const String & name,
                     const String & fileName,
                     const Indices & inputColumns,
                     const Indices & outputColumns,
                     const Description & inputNames,
                     const Description & outputNames)
  : FromFileDesignOfExperiment()
{
  setName(name);
  hasPhysicalModel_ = false;
  setFileName(fileName);
  setColumns(inputColumns, outputColumns, inputNames, outputNames);
}


DataModel::DataModel(const String & name,
                     const NumericalSample & inSample,
                     const NumericalSample & outSample)
  : FromFileDesignOfExperiment()
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


void DataModel::setFileName(const String& fileName)
{
  FromFileDesignOfExperiment::setFileName(fileName);
  // reinitialization
  if (!inputColumns_.getSize())
  {
    outputColumns_ = Indices();
    inputNames_ = Description();
    outputNames_ = Description();
  }
}


void DataModel::setInputColumns(const Indices & inputColumns)
{
  setColumns(inputColumns, outputColumns_, inputNames_, outputNames_);
}


Indices DataModel::getOutputColumns() const
{
  return outputColumns_;
}


void DataModel::setColumns(const Indices & inputColumns,
                           const Indices & outputColumns,
                           const Description & inputNames,
                           const Description & outputNames)
{
  if (!sampleFromFile_.getSize())
    sampleFromFile_ = ImportSample(getFileName());

  // check indices
  if (!inputColumns.getSize())
    throw InvalidArgumentException(HERE) << "In DataModel::setColumns: the model must have at least one input";

  Indices indices(inputColumns);
  indices.add(outputColumns);

  if (!indices.check(sampleFromFile_.getDimension()))
    throw InvalidArgumentException(HERE) << "In DataModel::setColumns: input columns and ouput columns are not compatible";

  // check names
  // input
  Description inNames(inputNames);
  if (inputNames.getSize())
  {
    if (inputColumns.getSize() != inputNames.getSize())
      throw InvalidArgumentException(HERE) << "In DataModel::setColumns: inputNames size not compatible with input columns size";

    std::set<String> inputNamesSet;
    for (UnsignedInteger i=0; i<inputNames.getSize(); ++i)
      inputNamesSet.insert(inputNames[i]);
    if (inputNamesSet.size() != inputNames.getSize())
      throw InvalidArgumentException(HERE) << "Two inputs can not have the same name.";
  }

  // output
  Description outNames(outputNames);
  if (outputNames.getSize())
  {
    if (outputColumns.getSize() != outputNames.getSize())
      throw InvalidArgumentException(HERE) << "In DataModel::setColumns: outputNames size not compatible with output columns size";

    std::set<String> outputNamesSet;
    for (UnsignedInteger i=0; i<outputNames.getSize(); ++i)
      outputNamesSet.insert(outputNames[i]);
    if (outputNamesSet.size() != outputNames.getSize())
      throw InvalidArgumentException(HERE) << "Two outputs can not have the same name.";
  }

  // set attributs
  inputColumns_ = inputColumns;
  outputColumns_ = outputColumns;
  inputNames_ = inNames;
  outputNames_ = outNames;

  // set samples
  NumericalSample inS(sampleFromFile_.getMarginal(inputColumns_));
  inS.setDescription(getInputNames());
  setInputSample(inS);

  NumericalSample outS;
  if (outputColumns_.getSize())
  {
    outS = sampleFromFile_.getMarginal(outputColumns_);
    outS.setDescription(getOutputNames());
  }
  setOutputSample(outS);
}


Description DataModel::getInputNames()
{
  if (!inputNames_.getSize())
  {
    if (!sampleFromFile_.getSize())
      sampleFromFile_ = ImportSample(getFileName());

    inputNames_ = Description(inputColumns_.getSize());
    for (UnsignedInteger i=0; i<inputColumns_.getSize(); ++i)
      inputNames_[i] = sampleFromFile_.getDescription()[inputColumns_[i]];
  }
  return inputNames_;
}


Description DataModel::getOutputNames()
{
  if (!outputNames_.getSize())
  {
    if (!sampleFromFile_.getSize())
      sampleFromFile_ = ImportSample(getFileName());

    outputNames_ = Description(outputColumns_.getSize());
    for (UnsignedInteger i=0; i<outputColumns_.getSize(); ++i)
      outputNames_[i] = sampleFromFile_.getDescription()[outputColumns_[i]];
  }
  return outputNames_;
}


String DataModel::getPythonScript() const
{
  OSS oss;

  oss << "inputColumns = [";
  for (UnsignedInteger i = 0; i < inputColumns_.getSize(); ++ i)
  {
    oss << inputColumns_[i];
    if (i < inputColumns_.getSize()-1)
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
void DataModel::save(Advocate & adv) const
{
  FromFileDesignOfExperiment::save(adv);
  adv.saveAttribute("outputColumns_", outputColumns_);
  adv.saveAttribute("inputNames_", inputNames_);
  adv.saveAttribute("outputNames_", outputNames_);
}


/* Method load() reloads the object from the StorageManager */
void DataModel::load(Advocate & adv)
{
  FromFileDesignOfExperiment::load(adv);
  adv.loadAttribute("outputColumns_", outputColumns_);
  adv.loadAttribute("inputNames_", inputNames_);
  adv.loadAttribute("outputNames_", outputNames_);
}
}