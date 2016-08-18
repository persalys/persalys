//                                               -*- C++ -*-
/**
 *  @brief Abstract top-level class to define data model
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

using namespace OT;

namespace OTGUI {

CLASSNAMEINIT(DataModel);

/* Default constructor */
DataModel::DataModel()
  : TypedInterfaceObject<DataModelImplementation>(new DataModelImplementation())
{
}


/* Constructor with parameters */
DataModel::DataModel(const String & name)
  : TypedInterfaceObject<DataModelImplementation>(new DataModelImplementation(name))
{
}


DataModel::DataModel(const String& name, const NumericalSample& inSample, const NumericalSample& outSample)
  : TypedInterfaceObject<DataModelImplementation>(new DataModelImplementation(name, inSample, outSample))
{

}


DataModel::DataModel(const String& name, const String& fileName, const Indices& inputColumns, const Indices& outputColumns, const Description& inputNames, const Description& outputNames)
  : TypedInterfaceObject<DataModelImplementation>(new DataModelImplementation(name, fileName, inputColumns, outputColumns, inputNames, outputNames))
{

}


/* Default constructor */
DataModel::DataModel(const DataModelImplementation & implementation)
  : TypedInterfaceObject<DataModelImplementation>(implementation.clone())
{
}


/* Constructor from implementation */
DataModel::DataModel(const Implementation & p_implementation)
  : TypedInterfaceObject<DataModelImplementation>(p_implementation)
{
  // Initialize any other class members here
  // At last, allocate memory space if needed, but go to destructor to free it
}


/* Constructor from implementation pointer */
DataModel::DataModel(DataModelImplementation * p_implementation)
  : TypedInterfaceObject<DataModelImplementation>(p_implementation)
{
  // Initialize any other class members here
  // At last, allocate memory space if needed, but go to destructor to free it
}


void DataModel::addObserver(Observer * observer)
{
  getImplementation()->addObserver(observer);
}


NumericalSample DataModel::getInputSample() const
{
  return getImplementation()->getInputSample();
}


void DataModel::setInputSample(const NumericalSample& sample)
{
  getImplementation()->setInputSample(sample);
}


NumericalSample DataModel::getOutputSample() const
{
  return getImplementation()->getOutputSample();
}


void DataModel::setOutputSample(const NumericalSample& sample)
{
  getImplementation()->setOutputSample(sample);
}


DataModel::NumericalSampleCollection DataModel::getListXMax() const
{
  return getImplementation()->getListXMin();
}


DataModel::NumericalSampleCollection DataModel::getListXMin() const
{
  return getImplementation()->getListXMax();
}


NumericalSample DataModel::getSample() const
{
  return getImplementation()->getSample();
}


String DataModel::getFileName() const
{
  return getImplementation()->getFileName();
}


void DataModel::setFileName(const String & fileName)
{
  getImplementation()->setFileName(fileName);
}


Indices DataModel::getInputColumns() const
{
  return getImplementation()->getInputColumns();
}


void DataModel::setInputColumns(const Indices & inputColumns)
{
  getImplementation()->setInputColumns(inputColumns);
}


Indices DataModel::getOutputColumns() const
{
  return getImplementation()->getOutputColumns();
}


void DataModel::setColumns(const Indices & inputColumns, const Indices & outputColumns,
                           const Description & inputNames, const Description & outputNames)
{
  getImplementation()->setColumns(inputColumns, outputColumns, inputNames, outputNames);
}


Description DataModel::getInputNames() const
{
  return getImplementation()->getInputNames();
}


Description DataModel::getOutputNames() const
{
  return getImplementation()->getOutputNames();
}


String DataModel::getPythonScript() const
{
  return getImplementation()->getPythonScript();
}
}