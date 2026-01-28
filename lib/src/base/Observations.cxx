//                                               -*- C++ -*-
/**
 *  @brief Class to define observations
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
#include "persalys/Observations.hxx"

#include <openturns/PersistentObjectFactory.hxx>

using namespace OT;

namespace PERSALYS
{

CLASSNAMEINIT(Observations)

const static Factory<Observations> Factory_Observations;

/* Default constructor */
Observations::Observations()
  : DataModel()
{
}


/* Constructor with parameters */
Observations::Observations(const String& name, const PhysicalModel& physicalModel)
  : DataModel(name, physicalModel)
{
}


/* Constructor with parameters */
Observations::Observations(const String &name,
                           const PhysicalModel &physicalModel,
                           const String &fileName,
                           const Indices &inputColumns,
                           const Indices &outputColumns,
                           const Description &inputNames,
                           const Description &outputNames)
  : DataModel(name, physicalModel, ImportedDataset(fileName, inputColumns, outputColumns), inputNames, outputNames)
{
  checkColumnsAndNames(inputNames, outputColumns, outputNames);
  Observations::orderSamples();
}

/* Constructor with parameters */
Observations::Observations(const String &name,
                           const PhysicalModel &physicalModel,
                           const Sample &inSample,
                           const Sample &outSample)
  : DataModel(name, inSample, outSample)
{
  physicalModel_ = physicalModel;

  Indices outputColumns(outSample.getSize());
  checkColumnsAndNames(inSample.getDescription(), outputColumns, outSample.getDescription());

  Observations::orderSamples();
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

void Observations::checkColumnsAndNames(const Description &inputNames,
                                        const Indices &outputColumns,
                                        const Description &outputNames) const
{
  // check at least there at least one output and one input
  if (!outputColumns.getSize())
    throw InvalidArgumentException(HERE) << "Define observations for at least an output.";
  // check if names of physical model
  for (UnsignedInteger i = 0; i < inputNames.getSize(); ++i)
    if (!getPhysicalModel().getInputNames().contains(inputNames[i]))
      throw InvalidArgumentException(HERE) << "The physical model does not contain an input named " << inputNames[i];
  for (UnsignedInteger i = 0; i < outputNames.getSize(); ++i)
    if (!getPhysicalModel().getOutputNames().contains(outputNames[i]))
      throw InvalidArgumentException(HERE) << "The physical model does not contain an output named " << outputNames[i];
}

Description Observations::getInputNames() const
{
  return importedDataset_ ? importedDataset_->getSampleFromFile().getMarginal(importedDataset_->getInputColumns()).getDescription() : DataModel::getInputNames();
}

Description Observations::getOutputNames() const
{
  return importedDataset_ ? importedDataset_->getSampleFromFile().getMarginal(importedDataset_->getOutputColumns()).getDescription() : DataModel::getOutputNames();
}

void Observations::setColumns(const Indices &inputColumns,
                              const Description &inputNames,
                              const Indices &outputColumns,
                              const Description &outputNames)
{
  checkColumnsAndNames(inputNames, outputColumns, outputNames);
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

  Description inputNames  = getInputNames();
  Description outputNames = getOutputNames();

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
        if (inputNames[j] == modelInputNames[i])
        {
          orderedInd.add(j);
          break;
        }
      }
    }
    resetImportedDataset_ = false;
    setInputSample(inS.getMarginal(orderedInd));
    resetImportedDataset_ = true;
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
        if (outputNames[j] == modelOutputNames[i])
        {
          orderedInd.add(j);
          break;
        }
      }
    }
    resetImportedDataset_ = false;
    setOutputSample(outS.getMarginal(orderedInd));
    resetImportedDataset_ = true;
  }
}

String Observations::getPythonScript() const
{
  if (!physicalModel_)
    return getName() + " = persalys." + getClassName() + "()\n";

  OSS oss;

  oss << "name=" << "'" << getName() << "'\n";
  oss << "physicalModel=" << getPhysicalModel().getName() << "\n";

  if (importedDataset_)
  {
    oss << "fileName=" << "'" << importedDataset_->getFileName() << "'\n";
    oss << "inputColumns=" << Parameters::GetOTIndicesStr(importedDataset_->getInputColumns()) << "\n";
    oss << "outputColumns=" << Parameters::GetOTIndicesStr(importedDataset_->getOutputColumns()) << "\n";
    oss << "inputNames=" << Parameters::GetOTDescriptionStr(getInputNames()) << "\n";
    oss << "outputNames=" << Parameters::GetOTDescriptionStr(getOutputNames()) << "\n";
    oss << getName() << " = persalys." << getClassName() << "(name, physicalModel, fileName, inputColumns, outputColumns, inputNames, outputNames)\n";
  }
  else
  {
    oss << "inSample = ot.Sample(" << Parameters::GetOTSampleStr(getInputSample()) << ")\n";
    oss << "inSample.setDescription(" << Parameters::GetOTDescriptionStr(getInputNames()) << ")\n";
    oss << "outSample = ot.Sample(" << Parameters::GetOTSampleStr(getOutputSample()) << ")\n";
    oss << "outSample.setDescription(" << Parameters::GetOTDescriptionStr(getOutputNames()) << ")\n";
    oss << getName() << " = persalys." << getClassName() << "(name, physicalModel, inSample, outSample)\n";
  }

  return oss;
}

/* String converter */
String Observations::__repr__() const
{
  OSS oss;
  oss << "class=" << GetClassName()
      << " name=" << getName();
  if (physicalModel_)
    oss << " physicalModel=" << getPhysicalModel().getName();
  oss << " observedInputs=" << getInputNames()
      << " observedOutputs=" << getOutputNames();
  if (importedDataset_)
  {
    oss << " fileName=" << importedDataset_->getFileName()
        << " inputColumns=" << importedDataset_->getInputColumns()
        << " outputColumns=" << importedDataset_->getOutputColumns();
  }
        
  return oss;
}
}
