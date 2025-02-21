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

static Factory<ImportedDesignOfExperiment> Factory_ImportedDesignOfExperiment;

/* Default constructor */
ImportedDesignOfExperiment::ImportedDesignOfExperiment()
  : DesignOfExperimentEvaluation()
  , DataImport()
{
}


/* Constructor with parameters */
ImportedDesignOfExperiment::ImportedDesignOfExperiment(const String& name, const PhysicalModel& physicalModel)
  : DesignOfExperimentEvaluation(name, physicalModel)
  , DataImport()
{
}


/* Constructor with parameters */
ImportedDesignOfExperiment::ImportedDesignOfExperiment(const String& name,
    const PhysicalModel& physicalModel,
    const String& fileName,
    const Indices& inputColumns,
    const Indices& outputColumns)
  : DesignOfExperimentEvaluation(name, physicalModel)
  , DataImport(fileName, inputColumns, outputColumns)
{
  // If outputcolumns, set result
  if(outputColumns.getSize())
    setColumns(inputColumns, outputColumns);
}


/* Virtual constructor */
ImportedDesignOfExperiment* ImportedDesignOfExperiment::clone() const
{
  return new ImportedDesignOfExperiment(*this);
}


void ImportedDesignOfExperiment::check()
{
  // try to use the same indices
  setColumns(inputColumns_, outputColumns_);
}


Sample ImportedDesignOfExperiment::generateInputSample(const UnsignedInteger /*nbSimu*/) const
{
  if (!getSampleFromFile().getSize())
    return Sample();
  Sample inS(getSampleFromFile().getMarginal(inputColumns_));
  inS.setDescription(getPhysicalModel().getInputNames());
  return inS;
}


void ImportedDesignOfExperiment::setColumns(const Indices &inputColumns,
    const Indices &outputColumns)
{
  // check columns
  if (inputColumns.getSize() != getPhysicalModel().getInputDimension())
    throw InvalidArgumentException(HERE) << "The dimension of the list of the column numbers has to be equal to the number of inputs of the physical model: " << getPhysicalModel().getInputDimension();

  DataImport::setColumns(inputColumns, outputColumns);

  // if outputColumns, consider the DoE already evaluated, set result
  if(outputColumns.getSize())
  {
    Sample inS = sampleFromFile_.getMarginal(inputColumns);
    inS.setDescription(getPhysicalModel().getInputNames());
    Sample outS = sampleFromFile_.getMarginal(outputColumns);
    outS.setDescription(getPhysicalModel().getSelectedOutputsNames());
    result_.designOfExperiment_.setInputSample(inS);
    result_.designOfExperiment_.setOutputSample(outS);
  }
  else
  {
    // reset
    originalInputSample_.clear();
    initialize();
  }
}


void ImportedDesignOfExperiment::setDefaultColumns()
{
  Indices inputColumns(getPhysicalModel().getInputDimension());
  inputColumns.fill();
  setColumns(inputColumns);
}


Parameters ImportedDesignOfExperiment::getParameters() const
{
  Parameters param;

  param.add("Design type", "Imported");
  param.add("Outputs of interest", getInterestVariables().__str__());
  param.add("Sample size", getOriginalInputSample().getSize());
  param.add("File", getFileName());
  OSS columns;
  if (getOriginalInputSample().getSize())
  {
    for (UnsignedInteger i = 0; i < getOriginalInputSample().getDimension(); ++i)
    {
      columns << getOriginalInputSample().getDescription()[i] << " : " << getInputColumns()[i];
      if (i < getOriginalInputSample().getDimension() - 1)
        columns << "\n";
    }
  }
  param.add("Columns", columns);
  param.add("Block size", getBlockSize());

  return param;
}


String ImportedDesignOfExperiment::getPythonScript() const
{
  OSS oss;

  oss << "inputColumns = " << Parameters::GetOTIndicesStr(inputColumns_) << "\n";

  oss << getName() << " = persalys.ImportedDesignOfExperiment('" << getName() << "', " << getPhysicalModel().getName() << ", ";
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
  DataImport::save(adv);
}


/* Method load() reloads the object from the StorageManager */
void ImportedDesignOfExperiment::load(Advocate& adv)
{
  DesignOfExperimentEvaluation::load(adv);
  DataImport::load(adv);
}
}
