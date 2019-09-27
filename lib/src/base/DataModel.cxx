//                                               -*- C++ -*-
/**
 *  @brief Class to define data model
 *
 *  Copyright 2015-2019 EDF-Phimeca
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
#include "persalys/DataModel.hxx"

#include "persalys/BaseTools.hxx"

#include <openturns/PersistentObjectFactory.hxx>

using namespace OT;

namespace PERSALYS
{

CLASSNAMEINIT(DataModel)

static Factory<DataModel> Factory_DataModel;

/* Default constructor */
DataModel::DataModel(const String& name)
  : DesignOfExperimentImplementation()
  , DataImport()
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
  , DataImport(fileName, inputColumns, outputColumns)
{
  setName(name);
  hasPhysicalModel_ = false;

  setNames(inputNames, outputNames);
  update();
}


/* Constructor with parameters */
DataModel::DataModel(const String& name,
                     const Sample& inSample,
                     const Sample& outSample)
  : DesignOfExperimentImplementation()
  , DataImport()
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


/* Constructor with parameters */
DataModel::DataModel(const String& name, const DesignOfExperiment & doe)
  : DataModel(name, doe.getInputSample(), doe.getOutputSample())
{
}


/* Virtual constructor */
DataModel* DataModel::clone() const
{
  return new DataModel(*this);
}


void DataModel::check()
{
  // try to use the same indices and names
  setColumns(inputColumns_, inputNames_, outputColumns_, outputNames_);
}


void DataModel::update()
{
  // set samples
  Sample inS;
  if (inputColumns_.getSize())
  {
    inS = sampleFromFile_.getMarginal(inputColumns_);
    if (getInputNames().getSize() == inS.getDimension())
      inS.setDescription(getInputNames());
  }
  setInputSample(inS);

  Sample outS;
  if (outputColumns_.getSize())
  {
    outS = sampleFromFile_.getMarginal(outputColumns_);
    if (getOutputNames().getSize() == outS.getDimension())
      outS.setDescription(getOutputNames());
  }
  setOutputSample(outS);

  notify("variablesChanged");
}


void DataModel::setDefaultColumns()
{
  // first reset variable names
  inputNames_.clear();
  outputNames_.clear();
  DataImport::setDefaultColumns();

  notify("variablesChanged");
}


void DataModel::setColumns(const Indices &inputColumns,
                           const Description &inputNames,
                           const Indices &outputColumns,
                           const Description &outputNames)
{
  DataImport::setColumns(inputColumns, outputColumns);
  setNames(inputNames, outputNames);
  // set samples
  update();
}


void DataModel::setNames(const Description &inputNames, const Description &outputNames)
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
      throw InvalidArgumentException(HERE) << "Two variables cannot have the same name.";
  }
  // set attributs
  inputNames_ = inputNames;
  outputNames_ = outputNames;
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


String DataModel::getPythonScript() const
{
  OSS oss;

  if (getFileName().empty())
  {
    oss << "inputSample = ot.Sample(" << Parameters::GetOTSampleStr(getInputSample()) << ")\n";
    oss << "inputSample.setDescription(" << Parameters::GetOTDescriptionStr(inputNames_) << ")\n";
    oss << "outputSample = ot.Sample(" << Parameters::GetOTSampleStr(getOutputSample()) << ")\n";
    oss << "outputSample.setDescription(" << Parameters::GetOTDescriptionStr(outputNames_) << ")\n";
    if (!hasPhysicalModel())
      oss << getName() << " = persalys."<< getClassName() <<"('" << getName() << "', inputSample, outputSample)\n";
    else
      oss << getName() << " = persalys."<< getClassName() <<"('" << getName() << "', " << getPhysicalModel().getName() << ", inputSample, outputSample)\n";
  }
  else
  {
    oss << "inputColumns = " << inputColumns_.__str__() << "\n";
    oss << "outputColumns = " << outputColumns_.__str__() << "\n";

    oss << "inputNames = " << Parameters::GetOTDescriptionStr(inputNames_) << "\n";
    oss << "outputNames = " << Parameters::GetOTDescriptionStr(outputNames_) << "\n";

    if (!hasPhysicalModel())
      oss << getName() << " = persalys."<< getClassName() <<"('" << getName() << "', ";
    else
      oss << getName() << " = persalys."<< getClassName() <<"('" << getName() << "', " << getPhysicalModel().getName() <<", ";
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
  DataImport::save(adv);
  adv.saveAttribute("inputNames_", inputNames_);
  adv.saveAttribute("outputNames_", outputNames_);
}


/* Method load() reloads the object from the StorageManager */
void DataModel::load(Advocate & adv)
{
  DesignOfExperimentImplementation::load(adv);
  DataImport::load(adv);
  adv.loadAttribute("inputNames_", inputNames_);
  adv.loadAttribute("outputNames_", outputNames_);
}
}
