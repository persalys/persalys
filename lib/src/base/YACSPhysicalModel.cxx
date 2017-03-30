//                                               -*- C++ -*-
/**
 *  @brief YACSPhysicalModel implements models defined by an xml file
 *
 *  Copyright 2015-2017 EDF-Phimeca
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
#include "otgui/YACSPhysicalModel.hxx"

#include <openturns/PersistentObjectFactory.hxx>

using namespace OT;

namespace OTGUI {

CLASSNAMEINIT(YACSPhysicalModel);

static Factory<YACSPhysicalModel> Factory_YACSPhysicalModel;

/* Default constructor */
YACSPhysicalModel::YACSPhysicalModel(const String & name)
  : PhysicalModelImplementation(name)
  , evaluation_(YACSEvaluation())
{
}


/* Constructor with parameters */
YACSPhysicalModel::YACSPhysicalModel(const String & name, const String & fileName)
  : PhysicalModelImplementation(name)
  , evaluation_(YACSEvaluation(fileName))
{
  setXMLFileName(fileName);
}


/* Virtual constructor */
YACSPhysicalModel* YACSPhysicalModel::clone() const
{
  return new YACSPhysicalModel(*this);
}


void YACSPhysicalModel::setInputs(const InputCollection & inputs)
{
  throw NotYetImplementedException(HERE) << "Use setXMLFileName to modify a YACS model";
}


void YACSPhysicalModel::addInput(const Input & input)
{
  throw NotYetImplementedException(HERE) << "Use setXMLFileName to modify a YACS model";
}


void YACSPhysicalModel::removeInput(const String & inputName)
{
  throw NotYetImplementedException(HERE) << "Use setXMLFileName to modify a YACS model";
}


void YACSPhysicalModel::setOutputs(const OutputCollection & outputs)
{
  throw NotYetImplementedException(HERE) << "Use setXMLFileName to modify a YACS model";
}


void YACSPhysicalModel::addOutput(const Output & output)
{
  throw NotYetImplementedException(HERE) << "Use setXMLFileName to modify a YACS model";
}


void YACSPhysicalModel::removeOutput(const String & outputName)
{
  throw NotYetImplementedException(HERE) << "Use setXMLFileName to modify a YACS model";
}


/* Accessor to the parallelize status */
bool YACSPhysicalModel::getParallelizeStatus() const
{
  return evaluation_.getParallelizeStatus();
}


/* Accessor to the parallelize status */
void YACSPhysicalModel::setParallelizeStatus(const bool & status)
{
  evaluation_.setParallelizeStatus(status);
  notify("parallelizeStatusChanged");
}


/* Accessor to the fitting machines */
Description YACSPhysicalModel::getFittingMachines() const
{
  return evaluation_.getFittingMachines();
}


/* Accessor to the wanted machine */
String YACSPhysicalModel::getWantedMachine() const
{
  return evaluation_.getWantedMachine();
}


/* Accessor to the wanted machine */
void YACSPhysicalModel::setWantedMachine(const String & machine)
{
  evaluation_.setWantedMachine(machine);
  notify("wantedMachineChanged");
}


String YACSPhysicalModel::getXMLFileName() const
{
  return xmlFileName_;
}


void YACSPhysicalModel::setXMLFileName(const String & fileName)
{
  if (fileName.empty())
    throw InvalidArgumentException(HERE) << "Impossible to create a model from an empty file";

  try
  {
    evaluation_.setXMLFileName(fileName);
    evaluation_.loadData();
    xmlFileName_ = fileName;
  }
  catch (std::exception & ex)
  {
    throw InvalidArgumentException(HERE) << "Impossible to load the xml file.\n" << ex.what();
  }
  updateData();
}


void YACSPhysicalModel::updateData()
{
  for (UnsignedInteger i=0; i<evaluation_.getInputDimension(); ++i)
  {
    Input newInput(evaluation_.getInputVariablesNames()[i], evaluation_.getInputValues()[i]);
    PhysicalModelImplementation::addInput(newInput);
  }
  notify("inputChanged");

  for (UnsignedInteger i=0; i<evaluation_.getOutputDimension(); ++i)
  {
    Output newOutput(evaluation_.getOutputVariablesNames()[i]);
    PhysicalModelImplementation::addOutput(newOutput);
  }
  notify("outputChanged");
}


NumericalMathFunction YACSPhysicalModel::generateFunction(const Description& outputNames) const
{
  YACSEvaluation anEvaluation(evaluation_);
  anEvaluation.setOutputVariablesNames(outputNames);

  return NumericalMathFunction(anEvaluation);
}


NumericalMathFunction YACSPhysicalModel::generateFunction() const
{
  return NumericalMathFunction(evaluation_);
}


String YACSPhysicalModel::getPythonScript() const
{
  String result;

  result += getName()+ " = otguibase.YACSPhysicalModel('" + getName() + "', '";
  result += getXMLFileName() + "')\n";

  result += getProbaModelPythonScript();
  result += PhysicalModelImplementation::getCopulaPythonScript();

  return result;
}


/** String converter */
String YACSPhysicalModel::__repr__() const
{
  OSS oss;
  oss << PhysicalModelImplementation::__repr__()
      << " xmlFileName=" << getXMLFileName()
      << " evaluation=" << evaluation_;
  return oss;
}


/* Method save() stores the object through the StorageManager */
void YACSPhysicalModel::save(Advocate & adv) const
{
  PhysicalModelImplementation::save(adv);
  adv.saveAttribute("xmlFileName_", xmlFileName_);
  adv.saveAttribute("evaluation_", evaluation_);
}


/* Method load() reloads the object from the StorageManager */
void YACSPhysicalModel::load(Advocate & adv)
{
  PhysicalModelImplementation::load(adv);
  adv.loadAttribute("xmlFileName_", xmlFileName_);
  adv.loadAttribute("evaluation_", evaluation_);
}
}