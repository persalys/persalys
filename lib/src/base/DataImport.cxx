//                                               -*- C++ -*-
/**
 *  @brief Class to define data model
 *
 *  Copyright 2015-2021 EDF-Phimeca
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
#include "persalys/DataImport.hxx"

#include "persalys/BaseTools.hxx"

using namespace OT;

namespace PERSALYS
{

/* Default constructor */
DataImport::DataImport()
{

}


/* Constructor with parameters */
DataImport::DataImport(const String& fileName,
                     const Indices& inputColumns,
                     const Indices& outputColumns)
{
  setFileName(fileName);
  setColumns(inputColumns, outputColumns);
}


DataImport::~DataImport()
{
}


String DataImport::getFileName() const
{
  return fileName_;
}


void DataImport::setFileName(const String& fileName)
{
  if (fileName.empty())
    throw InvalidArgumentException(HERE) << "The file name cannot be empty";

  // get sample from file
  sampleFromFile_ = importSample(fileName);

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
    catch (std::exception &)
    {
      // if the file content has changed
    }
  }
  // default values if needed
  if (!validArg)
  {
    setDefaultColumns();
  }
}


Indices DataImport::getInputColumns() const
{
  return inputColumns_;
}


Indices DataImport::getOutputColumns() const
{
  return outputColumns_;
}


void DataImport::setColumns(const Indices &inputColumns,
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


Sample DataImport::importSample(const String& fileName)
{
  Sample sampleFromFile(Tools::ImportSample(Tools::GetLocaleString(fileName)));
  return sampleFromFile;
}


void DataImport::setDefaultColumns()
{
  const UnsignedInteger dim = getSampleFromFile().getDimension();
  Indices inputColumns(dim > 1 ? dim - 1 : 1);
  inputColumns.fill();
  Indices outputColumns(dim > 1 ? 1 : 0, dim - 1);
  setColumns(inputColumns, outputColumns);
}


void DataImport::check()
{
  // try to use the same indices
  setColumns(inputColumns_, outputColumns_);
}


Sample DataImport::getSampleFromFile() const
{
  return sampleFromFile_;
}


/* String converter */
String DataImport::__repr__() const
{
  OSS oss;
  oss << " fileName=" << getFileName()
      << " inputColumns=" << getInputColumns()
      << " outputColumns=" << getOutputColumns();
  return oss;
}


/* Method save() stores the object through the StorageManager */
void DataImport::save(Advocate & adv) const
{
  adv.saveAttribute("fileName_", fileName_);
  adv.saveAttribute("inputColumns_", inputColumns_);
  adv.saveAttribute("outputColumns_", outputColumns_);
}


/* Method load() reloads the object from the StorageManager */
void DataImport::load(Advocate & adv)
{
  adv.loadAttribute("fileName_", fileName_);
  adv.loadAttribute("inputColumns_", inputColumns_);
  adv.loadAttribute("outputColumns_", outputColumns_);
}
}
