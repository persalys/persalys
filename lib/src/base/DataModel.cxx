//                                               -*- C++ -*-
/**
 *  @brief Class to define data models
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
#include <openturns/SpecFunc.hxx>
#include <openturns/Catalog.hxx>

using namespace OT;

namespace PERSALYS
{

CLASSNAMEINIT(DataModel)

// TODO : change in OT 1.27
const static Factory<DataModel> Factory_DesignOfExperimentImplementation = [] {
  Factory<DataModel> factory;
  const PersistentObjectFactory * poFactory = &Catalog::Get("DataModel");
  Catalog::Add("DesignOfExperimentImplementation", poFactory);
  return factory;
}();

DataModel::DataModel(const String& name,
  const std::optional<PhysicalModel> & physicalModel,
  const std::optional<ImportedDataset> & importedDataset,
  const Description& inputNames,
  const Description& outputNames)
: physicalModel_(physicalModel)
, importedDataset_(importedDataset)
{
PersistentObject::setName(name);

if (inputNames.getSize() || outputNames.getSize())
setNames(inputNames, outputNames);
else if (importedDataset_)
DataModel::update();
}

/* Constructor with parameters */
DataModel::DataModel(const String& name,
                     const String& fileName,
                     const Indices& inputColumns,
                     const Indices& outputColumns,
                     const Description& inputNames,
                     const Description& outputNames)
  : importedDataset_(ImportedDataset(fileName, inputColumns, outputColumns))
{
  PersistentObject::setName(name);
  if (inputNames.getSize() || outputNames.getSize())
    setNames(inputNames, outputNames);
  else
    DataModel::update();
}

/* Constructor with parameters */
DataModel::DataModel( const String& name,
                      const Sample& inSample,
                      const Sample& outSample)
{
  PersistentObject::setName(name);

DataModel::setInputSample(inSample);
DataModel::setOutputSample(outSample);
}

/* Virtual constructor */
DataModel* DataModel::clone() const
{
  return new DataModel(*this);
}

bool DataModel::hasPhysicalModel() const
{
  return physicalModel_.has_value();
}


PhysicalModel DataModel::getPhysicalModel() const
{
  if (!physicalModel_)
    throw InvalidValueException(HERE) << "No physical model is associated to the design of experiment " << getName();
  return physicalModel_.value();
}


void DataModel::initialize()
{
  setInputSample(Sample());
  setOutputSample(Sample());
}


void DataModel::setInputSample(const Sample& sample)
{
  if (sample.getSize() && hasPhysicalModel())
  {
    const Description sampleDescription = sample.getDescription();
    for (UnsignedInteger i = 0; i < sampleDescription.getSize(); ++i)
    {
      if (!physicalModel_->getInputNames().contains(sampleDescription[i]))
        throw InvalidArgumentException(HERE) << "The physical model does not contain an input named " << sampleDescription[i];
    }
  }
  DataSample::setInputSample(sample);
  if (resetImportedDataset_)
    importedDataset_.reset();
}


void DataModel::setOutputSample(const Sample& sample)
{
  if (sample.getSize() && hasPhysicalModel())
  {
    const Description sampleDescription = sample.getDescription();
    for (UnsignedInteger i = 0; i < sampleDescription.getSize(); ++i)
    {
      if (!physicalModel_->getOutputNames().contains(sampleDescription[i]))
        throw InvalidArgumentException(HERE) << "The physical model does not contain an output named " << sampleDescription[i];
    }
  }
  DataSample::setOutputSample(sample);
  if (resetImportedDataset_)
    importedDataset_.reset();
}


Indices DataModel::getEffectiveInputIndices() const
{
  Indices inputIndices;
  if (getInputSample().getSize())
  {
    const Point xmin(getInputSample().getMin());
    const Point xmax(getInputSample().getMax());
    for (UnsignedInteger i = 0; i < xmin.getDimension(); ++i)
      if (xmax[i] > xmin[i])
        inputIndices.add(i);
  }
  return inputIndices;
}

void DataModel::setType(Type type)
{
    type_ = type;
}

DataModel::Type DataModel::getType() const
{
    return type_;
}

void DataModel::removeAllObservers()
{
  notifyAndRemove("DataModelDefinitionItem");
  notifyAndRemove("DataModelDiagramItem");
  notifyAndRemove("Study");
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
  
  resetImportedDataset_ = false;
  setInputSample(inS);
  resetImportedDataset_ = true;

  auto outS = Sample(0, 0);
  if (importedDataset_->getOutputColumns().getSize())
    outS = importedDataset_->getSampleFromFile().getMarginal(importedDataset_->getOutputColumns());
  
  resetImportedDataset_ = false;
  setOutputSample(outS);
  resetImportedDataset_ = true;

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

    DataModel::setInputSample(inSample);
    DataModel::setOutputSample(outSample);
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

String DataModel::TypeToString(Type type)
{
    switch(type)
    {
        case MC:
            return "MC";
        case QMC:
            return "QMC";
        case LHS:
            return "LHS";
        case GRID:
            return "GRID";
        default:
            throw InvalidArgumentException(HERE) << "Invalid ImportedDesignOfExperiment type";
    }
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

  oss << getName() << ".setType(persalys.DataModel." << TypeToString(type_) << ")\n";

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
      << " outputNames=" << getOutputNames()
      << " type=" << TypeToString(type_);
  
  return oss;
}


/* Method save() stores the object through the StorageManager */
void DataModel::save(Advocate& adv) const
{
  DataSample::save(adv);
  if (physicalModel_)
  {
    adv.saveAttribute("hasPhysicalModel_", true);
    adv.saveAttribute("physicalModel_", physicalModel_.value());
  }
  else
  {
    adv.saveAttribute("hasPhysicalModel_", false);
  }
  adv.saveAttribute("type_", static_cast<UnsignedInteger>(type_));
  if (importedDataset_)
  {
    ImportedDataset importedDataset = *importedDataset_;
    adv.saveAttribute("importedDataset_", importedDataset);
  }
}


/* Method load() reloads the object from the StorageManager */
void DataModel::load(Advocate& adv)
{
  DataSample::load(adv);
  Bool hasPhysicalModel;
  adv.loadAttribute("hasPhysicalModel_", hasPhysicalModel);
  if (hasPhysicalModel)
  {
    PhysicalModel physicalModel;
    adv.loadAttribute("physicalModel_", physicalModel);
    physicalModel_ = physicalModel;
  }
  if (adv.hasAttribute("type_"))
  {
    UnsignedInteger typeInt;
    adv.loadAttribute("type_", typeInt);
    type_ = static_cast<Type>(typeInt);
  }
  if (adv.hasAttribute("importedDataset_"))
  {
    ImportedDataset importedDataset;
    adv.loadAttribute("importedDataset_", importedDataset);
    importedDataset_ = importedDataset;
  }
  else if (adv.hasAttribute("fileName_"))
    loadDataModelAttributes(adv);
}

void DataModel::loadDataModelAttributes(Advocate & adv)
{
  // load attributes corresponding to pre Persalys 20.0 DataModel class
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

} // END namespace PERSALYS