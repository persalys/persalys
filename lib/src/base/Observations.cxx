//                                               -*- C++ -*-
/**
 *  @brief Class to define observations
 *
 *  Copyright 2015-2024 EDF-Phimeca
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
#include "persalys/Observations.hxx"

#include <openturns/PersistentObjectFactory.hxx>

using namespace OT;

namespace PERSALYS
{

CLASSNAMEINIT(Observations)

static Factory<Observations> Factory_Observations;

/* Default constructor */
Observations::Observations()
  : DataModel()
{
  hasPhysicalModel_ = true;
}


/* Constructor with parameters */
Observations::Observations(const String& name, const PhysicalModel& physicalModel)
  : DataModel(name)
{
  physicalModel_ = physicalModel;
  hasPhysicalModel_ = true;
}


/* Constructor with parameters */
Observations::Observations(const String &name,
    const PhysicalModel &physicalModel,
    const String &fileName,
    const Indices &inputColumns,
    const Indices &outputColumns,
    const Description &inputNames,
    const Description &outputNames)
  : DataModel(name)
{
  physicalModel_ = physicalModel;
  hasPhysicalModel_ = true;

  setFileName(fileName);
  setColumns(inputColumns, inputNames, outputColumns, outputNames);
}


/* Constructor with parameters */
Observations::Observations(const String &name,
                           const PhysicalModel &physicalModel,
                           const Sample &inSample,
                           const Sample &outSample)
  : DataModel(name, inSample, outSample)
{
  physicalModel_ = physicalModel;
  hasPhysicalModel_ = true;

  // check dimension
  if (!outSample.getSize())
    throw InvalidArgumentException(HERE) << "Set at least an output to calibrate";
  if (!inSample.getSize()) // TODO next OT : can have only observed output
    throw InvalidArgumentException(HERE) << "Set at least an input to calibrate";
  // check if names of physical model
  for (UnsignedInteger i = 0; i < inSample.getDescription().getSize(); ++i)
    if (!getPhysicalModel().getInputNames().contains(inSample.getDescription()[i]))
      throw InvalidArgumentException(HERE) << "The physical model does not contain an input named " << inSample.getDescription()[i];
  for (UnsignedInteger i = 0; i < outSample.getDescription().getSize(); ++i)
    if (!getPhysicalModel().getOutputNames().contains(outSample.getDescription()[i]))
      throw InvalidArgumentException(HERE) << "The physical model does not contain an output named " << outSample.getDescription()[i];

  orderSamples();
}


/* Virtual constructor */
Observations* Observations::clone() const
{
  return new Observations(*this);
}


void Observations::removeAllObservers()
{
  notifyAndRemove("ObservationsItem");
  notifyAndRemove("Study");
}


void Observations::setDefaultColumns()
{
  setColumns(Indices(1, 0),
             Description(1, getPhysicalModel().getInputNames()[0]),
             Indices(1, 1),
             Description(1, getPhysicalModel().getOutputNames()[0]));
}



void Observations::setColumns(const Indices &inputColumns,
                              const Description &inputNames,
                              const Indices &outputColumns,
                              const Description &outputNames)
{
  // check at least there at least one output and one input
  if (!outputColumns.getSize())
    throw InvalidArgumentException(HERE) << "Define observations for at least an output.";
  // TODO : remove this check with new version of OT (possible to have no input observations in the future)
  if (!inputColumns.getSize())
    throw InvalidArgumentException(HERE) << "Define observations for at least an input.";
  // check if names of physical model
  for (UnsignedInteger i = 0; i < inputNames.getSize(); ++i)
    if (!getPhysicalModel().getInputNames().contains(inputNames[i]))
      throw InvalidArgumentException(HERE) << "The physical model does not contain an input named " << inputNames[i];
  for (UnsignedInteger i = 0; i < outputNames.getSize(); ++i)
    if (!getPhysicalModel().getOutputNames().contains(outputNames[i]))
      throw InvalidArgumentException(HERE) << "The physical model does not contain an output named " << outputNames[i];

  DataModel::setColumns(inputColumns, inputNames, outputColumns, outputNames);
}


void Observations::update()
{
  DataModel::update();
  // order sample according to the order of the model variable lists
  orderSamples();
}


void Observations::orderSamples()
{
  // order input sample
  Sample inS(getInputSample());
  if (inS.getSize())
  {
    const Description modelInputNames(getPhysicalModel().getInputNames());
    Indices orderedInd;
    for (UnsignedInteger i = 0; i < modelInputNames.getSize(); ++i)
    {
      for (UnsignedInteger j = 0; j < inS.getDimension(); ++j)
      {
        if (inputNames_[j] == modelInputNames[i])
        {
          orderedInd.add(j);
          break;
        }
      }
    }
    setInputSample(inS.getMarginal(orderedInd));
  }
  // order output sample
  Sample outS(getOutputSample());
  if (outS.getSize())
  {
    const Description modelOutputNames(getPhysicalModel().getOutputNames());
    Indices orderedInd;
    // - order sample
    for (UnsignedInteger i = 0; i < modelOutputNames.getSize(); ++i)
    {
      for (UnsignedInteger j = 0; j < outS.getDimension(); ++j)
      {
        if (outputNames_[j] == modelOutputNames[i])
        {
          orderedInd.add(j);
          break;
        }
      }
    }
    setOutputSample(outS.getMarginal(orderedInd));
  }
}


Sample Observations::importSample(const String &fileName)
{
  Sample sampleFromFile(DataImport::importSample(fileName));

  // check sampleFromFile dimension TODO: rm with OT1.14
  if (sampleFromFile.getDimension() < 2)
    throw InvalidArgumentException(HERE) << "The file must contain at least two columns to define observations for at least an input and an output.";

  return sampleFromFile;
}


/* String converter */
String Observations::__repr__() const
{
  OSS oss;
  oss << "class=" << GetClassName()
      << " name=" << getName()
      << " physicalModel=" << getPhysicalModel().getName()
      << " observedInputs=" << inputNames_
      << " observedOutputs=" << outputNames_;
  if (!getFileName().empty())
    oss << " fileName=" << getFileName()
        << " inputColumns=" << getInputColumns()
        << " outputColumns=" << getOutputColumns();
  return oss;
}
}
