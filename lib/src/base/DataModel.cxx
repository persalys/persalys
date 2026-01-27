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
#include "persalys/DataModel.hxx"

#include "persalys/BaseTools.hxx"

#include <openturns/PersistentObjectFactory.hxx>

using namespace OT;

namespace PERSALYS
{

CLASSNAMEINIT(DataModel)

static const Factory<DataModel> Factory_DataModel;

/* Default constructor */
DataModel::DataModel(const String& name)
  : DesignOfExperimentImplementation()
{
  DesignOfExperimentImplementation::setName(name);
}


/* Constructor with parameters */
DataModel::DataModel(const String& name,
                     const String& fileName,
                     const Indices& inputColumns,
                     const Indices& outputColumns,
                     const Description& inputNames,
                     const Description& outputNames)
  : DesignOfExperimentImplementation()
  , importedDataset_(ImportedDataset(fileName, inputColumns, outputColumns))
{
  DesignOfExperimentImplementation::setName(name);
  if (inputNames.getSize() || outputNames.getSize())
    setNames(inputNames, outputNames);
  else
    DataModel::update();
}

/* Constructor with parameters */
DataModel::DataModel( const String& name,
                      const Sample& inSample,
                      const Sample& outSample)
: DesignOfExperimentImplementation()
{
DesignOfExperimentImplementation::setName(name);

DataModel::setInputSample(inSample);
DataModel::setOutputSample(outSample);
}

DataModel::DataModel(const String& name,
                     const PhysicalModel& physicalModel,
                     const std::optional<ImportedDataset> & importedDataset,
                     const Description& inputNames,
                     const Description& outputNames)
  : DesignOfExperimentImplementation(name, physicalModel)
  , importedDataset_(importedDataset)
{
  if (inputNames.getSize() || outputNames.getSize())
    setNames(inputNames, outputNames);
  else if (importedDataset_)
    DataModel::update();
}

/* Virtual constructor */
DataModel* DataModel::clone() const
{
  return new DataModel(*this);
}


void DataModel::removeAllObservers()
{
  notifyAndRemove("DataModelDefinitionItem");
  notifyAndRemove("DataModelDiagramItem");
  notifyAndRemove("Study");
}

void DataModel::setInputSample(const Sample& sample)
{
  DesignOfExperimentImplementation::setInputSample(sample);
  importedDataset_.reset();
}

void DataModel::setOutputSample(const Sample& sample)
{
  DesignOfExperimentImplementation::setOutputSample(sample);
  importedDataset_.reset();
}

/** @brief Update the DataSample samples based on the ImportedDataset*/
void DataModel::update()
{
  if (!importedDataset_)
    throw NotDefinedException(HERE) << "No ImportedDataset defined in the DataModel.";

  // set samples
  auto inS = Sample(0, 0);
  if (importedDataset_->getInputColumns().getSize())
    inS = importedDataset_->getSampleFromFile().getMarginal(importedDataset_->getInputColumns());
  
  DesignOfExperimentImplementation::setInputSample(inS);

  auto outS = Sample(0, 0);
  if (importedDataset_->getOutputColumns().getSize())
    outS = importedDataset_->getSampleFromFile().getMarginal(importedDataset_->getOutputColumns());
  
  DesignOfExperimentImplementation::setOutputSample(outS);

  notify("variablesChanged");
}

void DataModel::setColumns(const Indices &inputColumns,
                           const Description &inputNames,
                           const Indices &outputColumns,
                           const Description &outputNames)
{
  if (!importedDataset_)
    throw NotDefinedException(HERE) << "No ImportedDataset defined in the DataModel.";
  
  importedDataset_->setColumns(inputColumns, outputColumns);
  setNames(inputNames, outputNames);

  // set samples
  update();
}

/** @brief Set the modifies the sample of the ImportedDataset */
void DataModel::setSample(const Sample & sample)
{
  if (!importedDataset_)
    throw InternalException(HERE) << "No ImportedDataset defined in the DataModel.";
  
  importedDataset_->setSampleFromFile(sample);
  update();
}

Sample DataModel::getSampleFromFile() const
{
  if (!importedDataset_)
    return Sample();
  
  return importedDataset_->getSampleFromFile();
}

Indices DataModel::getInputColumns() const
{
  if (!importedDataset_)
    return Indices();
  
  return importedDataset_->getInputColumns();
}

Indices DataModel::getOutputColumns() const
{
  if (!importedDataset_)
    return Indices();
  
  return importedDataset_->getOutputColumns();
}

String DataModel::getFileName() const
{
  if (!importedDataset_)
    return String();
  
  return importedDataset_->getFileName();
}

void DataModel::setFileName(const String & fileName)
{
  if (!importedDataset_)
    importedDataset_.emplace();

  importedDataset_->setFileName(fileName);
  update();
}

void DataModel::setNames(const Description &inputNames, const Description &outputNames)
{
  if (importedDataset_)
  {
    importedDataset_->setNames(inputNames, outputNames);
    update();
  }
  else
  {
    Sample inSample = getInputSample();
    Sample outSample = getOutputSample();

    if (inputNames.getSize() == inSample.getDimension() && outputNames.getSize() == outSample.getDimension())
    {
      inSample.setDescription(inputNames);
      outSample.setDescription(outputNames);
    }
    else
    {
      throw InvalidArgumentException(HERE)  << "The dimension of the inputs/outputs names list (" 
                                            << inputNames.getSize() << "/" << outputNames.getSize() 
                                            << ") has to be equal to the dimension of the inputs/outputs samples (" 
                                            << inSample.getDimension() << "/" << outSample.getDimension() << ").";
    }

    DesignOfExperimentImplementation::setInputSample(inSample);
    DesignOfExperimentImplementation::setOutputSample(outSample);
  }
}


Description DataModel::getInputNames() const
{
  return getInputSample().getDescription();
}


Description DataModel::getOutputNames() const
{
  return getOutputSample().getDescription();
}

String DataModel::getPythonScript() const
{
  OSS oss;

  oss << "inputNames = " << Parameters::GetOTDescriptionStr(getInputNames()) << "\n";
  oss << "outputNames = " << Parameters::GetOTDescriptionStr(getOutputNames()) << "\n";
  if (importedDataset_)
  {
    oss << "inputColumns = " << Parameters::GetOTIndicesStr(importedDataset_->getInputColumns()) << "\n";
    oss << "outputColumns = " << Parameters::GetOTIndicesStr(importedDataset_->getOutputColumns()) << "\n";
  }

  if(physicalModel_)
  {
    oss << "importedDataset = ";
    if (importedDataset_)
      oss << "persalys.ImportedDataset('" << importedDataset_->getFileName() << "', inputColumns, outputColumns)\n";
    else
      oss << "None\n";
    oss << getName() << " = persalys." << getClassName() << "('" << getName() << "', " << getPhysicalModel().getName() << ", importedDataset, inputNames, outputNames)\n";
  }
  else if (importedDataset_)
  {
    oss << getName() << " = persalys." << getClassName() << "('" << getName() << "', '" << importedDataset_->getFileName() << "', inputColumns, outputColumns, inputNames, outputNames)\n";
  }
  else
  {
    oss << getName() << " = persalys." << getClassName() << "('" << getName() << ")\n";
    oss << "inputSample = ot.Sample(" << Parameters::GetOTSampleStr(getInputSample()) << ")\n";
    oss << "inputSample.setDescription(" << Parameters::GetOTDescriptionStr(getInputNames()) << ")\n";
    oss << "outputSample = ot.Sample(" << Parameters::GetOTSampleStr(getOutputSample()) << ")\n";
    oss << "outputSample.setDescription(" << Parameters::GetOTDescriptionStr(getOutputNames()) << ")\n";
    oss << getName() << ".setInputSample(inputSample)\n";
    oss << getName() << ".setOutputSample(outputSample)\n";
  }

  return oss;
}


/* String converter */
String DataModel::__repr__() const
{
  OSS oss;
  oss << "class=" << GetClassName()
      << " name=" << getName();
  if (importedDataset_)
    oss << " importedDataset=" << importedDataset_->__repr__();
  else
    oss << " importedDataset=None";
  
  if (physicalModel_)
    oss << " physicalModel=" << physicalModel_->__repr__();
  else
    oss << " physicalModel=" << false;
  
  oss << " inputNames=" << getInputNames()
      << " outputNames=" << getOutputNames();
  
  return oss;
}


/* Method save() stores the object through the StorageManager */
void DataModel::save(Advocate & adv) const
{
  DesignOfExperimentImplementation::save(adv);
  if (importedDataset_)
  {
    ImportedDataset importedDataset = *importedDataset_;
    adv.saveAttribute("importedDataset_", importedDataset);
  }
}


/* Method load() reloads the object from the StorageManager */
void DataModel::load(Advocate & adv)
{
  DesignOfExperimentImplementation::load(adv);
  if (adv.hasAttribute("importedDataset_"))
  {
    ImportedDataset importedDataset;
    adv.loadAttribute("importedDataset_", importedDataset);
    importedDataset_ = importedDataset;
  }
  else if (adv.hasAttribute("fileName_"))
  {
    // for compatilibility with files generated before Persalys 20.0
    String fileName;
    adv.loadAttribute("fileName_", fileName);
    Indices inputColumns;
    adv.loadAttribute("inputColumns_", inputColumns);
    Indices outputColumns;
    adv.loadAttribute("outputColumns_", outputColumns);
    try
    {
      importedDataset_ = ImportedDataset(fileName, inputColumns, outputColumns);
    }
    catch (const Exception &)
    {
      Sample sampleFromFile;
      adv.loadAttribute("sampleFromFile_", sampleFromFile);
      if (sampleFromFile.getSize())
      {
        importedDataset_ = ImportedDataset();
        importedDataset_->setSampleFromFile(sampleFromFile);
        try
        {
          importedDataset_->setColumns(inputColumns, outputColumns);
        }
        catch (const Exception &)
        {
          // do nothing
        }
      }
      
    }

    if (adv.hasAttribute("inputNames_") && adv.hasAttribute("outputNames_"))
    {
      Description inputNames;
      adv.loadAttribute("inputNames_", inputNames);
      Description outputNames;
      adv.loadAttribute("outputNames_", outputNames);
      setNames(inputNames, outputNames);
    }
  }
}
}
