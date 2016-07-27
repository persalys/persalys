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
#include "otgui/DataModel.hxx"

#include "otgui/AnalyticalPhysicalModel.hxx"

#include "openturns/PersistentObjectFactory.hxx"

using namespace OT;

namespace OTGUI {

CLASSNAMEINIT(DataModel);

static Factory<DataModel> RegisteredFactory;

/* Default constructor */
DataModel::DataModel(const OT::String & name)
  : FromFileDesignOfExperiment()
{
  setName(name);
}


/* Constructor with parameters */
DataModel::DataModel(const String & name,
                     const String & fileName,
                     const OT::Indices & inputColumns,
                     const OT::Indices & outputColumns,
                     const Description & inputNames, const Description & outputNames)
  : FromFileDesignOfExperiment(name, AnalyticalPhysicalModel(name))
{
  setFileName(fileName);
  setColumns(inputColumns, outputColumns, inputNames, outputNames);
}


/* Virtual constructor */
DataModel* DataModel::clone() const
{
  return new DataModel(*this);
}


void DataModel::setFileName(const String & fileName)
{
  if (fileName != fileName_)
  {
    fileName_ = fileName;
    sampleFromFile_ = ImportSample(fileName_);
  }
}


void DataModel::setInputColumns(const Indices & inputColumns)
{
  // overload setInputColumns of FromFileDesignOfExperiment
  try
  {
    setColumns(inputColumns, outputColumns_, inputNames_, outputNames_);
  }
  catch (std::exception & ex)
  {
    throw InvalidArgumentException(HERE) << "In DataModel::setInputColumns: input columns not compatible with other parameters. See setColumns.";
  }
}


Indices DataModel::getOutputColumns() const
{
  return outputColumns_;
}


void DataModel::setColumns(const Indices & inputColumns, const Indices & outputColumns,
                           const Description & inputNames, const Description & outputNames)
{
  if (!sampleFromFile_.getSize())
    sampleFromFile_ = ImportSample(fileName_);

  // check indices
  if (!inputColumns.getSize())
    throw InvalidArgumentException(HERE) << "In DataModel::setColumns: the model must have at least one input";

  OT::Indices indices(inputColumns);
  indices.add(outputColumns);

  if (!indices.check(sampleFromFile_.getDimension()))
    throw InvalidArgumentException(HERE) << "In DataModel::setColumns: input columns and ouput columns are not compatible";

  if (inputSample_.getSize() && inputColumns_ != inputColumns)
    inputSample_.clear();
  if (getResult().getOutputSample().getSize() && outputColumns_ != outputColumns)
    clearResult();

  // check names
  // input
  OT::Description inNames(inputNames);
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
      throw InvalidArgumentException(HERE) << "In DataModel::setColumns: outputNames size not compatible with output columns size";

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
  inputColumns_ = inputColumns;
  outputColumns_ = outputColumns;
  inputNames_ = inNames;
  outputNames_ = outNames;

  if (!inputSample_.getSize())
  {
    inputSample_ = sampleFromFile_.getMarginal(inputColumns_);
    inputSample_.setDescription(inputNames_);
  }

  if (!getResult().getOutputSample().getSize() && outputColumns_.getSize())
  {
    NumericalSample outputSample = sampleFromFile_.getMarginal(outputColumns_);
    outputSample.setDescription(outputNames_);
    result_ = SimulationAnalysisResult(getInputSample(), outputSample);
  }
}


NumericalSample DataModel::getInputSample()
{
  if (!inputSample_.getSize())
  {
    if (fileName_.size() > 0)
    {
      if (!sampleFromFile_.getSize())
        sampleFromFile_ = ImportSample(fileName_);
      NumericalSample sampleFromFile = ImportSample(fileName_);
      if (!inputColumns_.check(sampleFromFile_.getDimension()))
        throw InvalidArgumentException(HERE) << "In DataModel: impossible to load sample marginals";
      inputSample_ = sampleFromFile_.getMarginal(inputColumns_);
      inputSample_.setDescription(inputNames_);
    }
  }
  return inputSample_;
}


Description DataModel::getInputNames() const
{
  return inputNames_;
}


NumericalSample DataModel::getOuputSample()
{
  return result_.getOutputSample();
}


Description DataModel::getOutputNames() const
{
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
  oss << "'"+fileName_+"', inputColumns, outputColumns, inputNames, outputNames)\n";

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