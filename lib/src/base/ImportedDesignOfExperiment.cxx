//                                               -*- C++ -*-
/**
 *  @brief Class to define designs of experiments
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
#include "persalys/ImportedDesignOfExperiment.hxx"

#include "persalys/BaseTools.hxx"

#include <openturns/PersistentObjectFactory.hxx>

using namespace OT;

namespace PERSALYS
{

CLASSNAMEINIT(ImportedDesignOfExperiment)

const static Factory<ImportedDesignOfExperiment> Factory_ImportedDesignOfExperiment;

/* Default constructor */
ImportedDesignOfExperiment::ImportedDesignOfExperiment()
  : DesignOfExperimentEvaluation()
{
}

/* Constructor with parameters */
ImportedDesignOfExperiment::ImportedDesignOfExperiment(const String& name, const PhysicalModel& physicalModel)
  : DesignOfExperimentEvaluation(name, physicalModel)
{
}

/* Constructor with parameters */
ImportedDesignOfExperiment::ImportedDesignOfExperiment(const String& name,
    const PhysicalModel& physicalModel,
    const String& fileName,
    const Indices& inputColumns,
    const Indices& outputColumns,
    Type type)
  : DesignOfExperimentEvaluation(name, physicalModel)
  , type_(type)
  , importedDataset_(fileName, inputColumns, outputColumns)
{
  setColumns(inputColumns, outputColumns);
}


/* Virtual constructor */
ImportedDesignOfExperiment* ImportedDesignOfExperiment::clone() const
{
  return new ImportedDesignOfExperiment(*this);
}

Sample ImportedDesignOfExperiment::generateInputSample(const UnsignedInteger /*nbSimu*/) const
{
  if (!importedDataset_.getSampleFromFile().getSize())
    return Sample();
  Sample inS(importedDataset_.getSampleFromFile().getMarginal(importedDataset_.getInputColumns()));
  inS.setDescription(getPhysicalModel().getInputNames());
  return inS;
}

void ImportedDesignOfExperiment::launch()
{
  DesignOfExperimentEvaluation::launch();
  result_.designOfExperiment_.setType(type_);
}


void ImportedDesignOfExperiment::setColumns(const Indices &inputColumns, const Indices &outputColumns)
{
  // check columns
  if (inputColumns.getSize() != getPhysicalModel().getInputDimension())
    throw InvalidArgumentException(HERE) << "The dimension of the list of the column numbers has to be equal to the number of inputs of the physical model: " << getPhysicalModel().getInputDimension();

  importedDataset_.setColumns(inputColumns, outputColumns);

  saveImportedSampleToResult();
}

void ImportedDesignOfExperiment::saveImportedSampleToResult()
{
  // if outputColumns, consider the DoE already evaluated, set result
  if(importedDataset_.getOutputColumns().getSize())
  {
    Sample inS = importedDataset_.getSampleFromFile().getMarginal(importedDataset_.getInputColumns());
    inS.setDescription(getPhysicalModel().getInputNames());
    Sample outS = importedDataset_.getSampleFromFile().getMarginal(importedDataset_.getOutputColumns());
    outS.setDescription(getPhysicalModel().getSelectedOutputsNames());
    result_.designOfExperiment_.setInputSample(inS);
    result_.designOfExperiment_.setOutputSample(outS);
    originalInputSample_ = inS;
  }
  else
  {
    // ready for evaluation, clear samples
    originalInputSample_.clear();
    initialize();
  }
}

Parameters ImportedDesignOfExperiment::getParameters() const
{
  Parameters param;

  param.add("Design type", "Imported");
  param.add("Outputs of interest", getInterestVariables().__str__());
  param.add("Sample size", getOriginalInputSample().getSize());
  param.add("File", importedDataset_.getFileName());
  OSS columns;
  if (getOriginalInputSample().getSize())
  {
    for (UnsignedInteger i = 0; i < getOriginalInputSample().getDimension(); ++i)
    {
      columns << getOriginalInputSample().getDescription()[i] << " : " << importedDataset_.getInputColumns()[i];
      if (i < getOriginalInputSample().getDimension() - 1)
        columns << "\n";
    }
  }
  param.add("Columns", columns);
  param.add("Block size", getBlockSize());
  
  switch(type_)
  {
    case Type::UK:
      param.add("Type", "Generic");
      break;
    case Type::MC:
      param.add("Type", "Monte Carlo");
      break;
    case Type::QMC:
      param.add("Type", "Quasi Monte Carlo");
      break;
    case Type::RLHS:
      param.add("Type", "Latin Hypercube Sampling");
      break;
    case Type::OLHS:
      param.add("Type", "Optimized LHS");
      break;
    case Type::GRID:
      param.add("Type", "Grid");
      break;
    case Type::MORRIS:
      param.add("Type", "Morris");
  }

  return param;
}

void ImportedDesignOfExperiment::setType(Type type)
{
  type_ = type;
}

ImportedDesignOfExperiment::Type ImportedDesignOfExperiment::getType() const
{
  return type_;
}

const ImportedDataset& ImportedDesignOfExperiment::getImportedDataset() const
{
  return importedDataset_;
}

void ImportedDesignOfExperiment::setEvaluations(Sample & /*outputSample*/)
{
  throw NotDefinedException(HERE) << "Please import a DOE with output columns to set evaluations.";
}

void ImportedDesignOfExperiment::setFileName(const String &fileName)
{
  importedDataset_.setFileName(fileName);
  saveImportedSampleToResult();
}

String ImportedDesignOfExperiment::getPythonScript() const
{
  OSS oss;

  oss << "inputColumns = " << Parameters::GetOTIndicesStr(importedDataset_.getInputColumns()) << "\n";

  oss << getName() << " = persalys.ImportedDesignOfExperiment('" << getName() << "', " << getPhysicalModel().getName() << ", ";
  oss << "'" << importedDataset_.getFileName() << "', inputColumns)\n";

  oss << getName() << ".setBlockSize(" << getBlockSize() << ")\n";
  oss << "interestVariables = " << Parameters::GetOTDescriptionStr(getInterestVariables()) << "\n";
  oss << getName() << ".setInterestVariables(interestVariables)\n";
  oss << getName() << ".setType(persalys.DataModel." << DataModel::TypeToString(type_) << ")\n";

  return oss;
}

/* String converter */
String ImportedDesignOfExperiment::__repr__() const
{
  OSS oss;
  oss << "class=" << GetClassName()
      << " name=" << getName()
      << " physicalModel=" << getPhysicalModel().getName()
      << " fileName=" << importedDataset_.getFileName()
      << " inputColumns=" << importedDataset_.getInputColumns()
      << " blockSize=" << getBlockSize()
      << " type=" << DataModel::TypeToString(type_);

  return oss;
}


/* Method save() stores the object through the StorageManager */
void ImportedDesignOfExperiment::save(Advocate& adv) const
{
  DesignOfExperimentEvaluation::save(adv);
  adv.saveAttribute("type_", static_cast<UnsignedInteger>(type_));
  adv.saveAttribute("importedDataset_", importedDataset_);
}


/* Method load() reloads the object from the StorageManager */
void ImportedDesignOfExperiment::load(Advocate& adv)
{
  DesignOfExperimentEvaluation::load(adv);
  UnsignedInteger type_as_uint;
  if (adv.hasAttribute("type_"))
  {
    adv.loadAttribute("type_", type_as_uint);
    type_ = static_cast<Type>(type_as_uint);
  }
  else
  {
    // for backward compatibility, if type_ attribute does not exist, set to MC
    type_ = Type::MC;
  }

  if (adv.hasAttribute("importedDataset_"))
    adv.loadAttribute("importedDataset_", importedDataset_);
  else
    loadOldFormat(adv);
}

void ImportedDesignOfExperiment::loadOldFormat(Advocate& adv)
{
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
    importedDataset_ = ImportedDataset();
    Sample sampleFromFile;
    adv.loadAttribute("sampleFromFile_", sampleFromFile);
    if (sampleFromFile.getSize())
    {
      importedDataset_.setSampleFromFile(sampleFromFile);
      try
      {
        importedDataset_.setColumns(inputColumns, outputColumns);
      }
      catch(const Exception &)
      {
        // do nothing
      }
    }
    else
    {
      UnsignedInteger maxColumn = 0;
      for (UnsignedInteger i = 0; i < inputColumns.getSize(); ++i)
        if (inputColumns[i] > maxColumn)
          maxColumn = inputColumns[i];
      for (UnsignedInteger i = 0; i < outputColumns.getSize(); ++i)
        if (outputColumns[i] > maxColumn)
          maxColumn = outputColumns[i];
      importedDataset_.setSampleFromFile(Sample(0, maxColumn + 1));
      importedDataset_.setColumns(inputColumns, outputColumns);
    }
  }
}

} // end namespace PERSALYS
