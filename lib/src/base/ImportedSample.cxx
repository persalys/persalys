//                                               -*- C++ -*-
/**
 *  @brief Class for imported sample
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
#include "otgui/ImportedSample.hxx"

using namespace OT;

namespace OTGUI {

/* Default constructor */
ImportedSample::ImportedSample()
{
}


/* Constructor with parameters */
ImportedSample::ImportedSample(const String & fileName, const Indices & inputColumns)
{
  setFileName(fileName);
  setInputColumns(inputColumns);
}


ImportedSample::~ImportedSample()
{

}


String ImportedSample::getFileName() const
{
  return fileName_;
}


void ImportedSample::setFileName(const String & fileName)
{
  if (fileName_ == fileName)
    return;

  fileName_ = fileName;
  sampleFromFile_ = ImportSample(fileName_);
}


Indices ImportedSample::getInputColumns() const
{
  return inputColumns_;
}


void ImportedSample::setInputColumns(const Indices & inputColumns)
{
  if (inputColumns == inputColumns_)
    return;

  if (!inputColumns.getSize())
    throw InvalidArgumentException(HERE) << "In ImportedSample::setInputColumns: the model must have at least one input";

  if (!sampleFromFile_.getSize())
    sampleFromFile_ = ImportSample(fileName_);

  if (!inputColumns.check(sampleFromFile_.getDimension()))
    throw InvalidArgumentException(HERE) << "In ImportedSample::setInputColumns: input columns is not compatible wit hthe sample contained in the file";

  inputColumns_ = inputColumns;
}


NumericalSample ImportedSample::ImportSample(const String & fileName)
{
  std::vector< String > separatorsList(3);
  separatorsList[0] = " ";
  separatorsList[1] = ",";
  separatorsList[2] = ";";
  NumericalSample sampleFromFile;
  for (UnsignedInteger i = 0; i < separatorsList.size(); ++ i)
  {
    // import sample from the file
    sampleFromFile = NumericalSample::ImportFromTextFile(fileName, separatorsList[i]);
    if (sampleFromFile.getSize())
      break;
  }
  if (!sampleFromFile.getSize())
    throw InvalidArgumentException(HERE) << "In ImportedSample: impossible to load sample";

  return sampleFromFile;
}


/* Method save() stores the object through the StorageManager */
void ImportedSample::save(Advocate & adv) const
{
  adv.saveAttribute("fileName_", fileName_);
  adv.saveAttribute("inputColumns_", inputColumns_);
}


/* Method load() reloads the object from the StorageManager */
void ImportedSample::load(Advocate & adv)
{
  adv.loadAttribute("fileName_", fileName_);
  adv.loadAttribute("inputColumns_", inputColumns_);
}
}