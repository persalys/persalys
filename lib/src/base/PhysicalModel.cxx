//                                               -*- C++ -*-
/**
 *  @brief Abstract top-level class for all physical models
 *
 *  Copyright 2015-2019 EDF-Phimeca
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
#include "persalys/PhysicalModel.hxx"

#include "persalys/SymbolicPhysicalModel.hxx"

using namespace OT;

namespace PERSALYS
{

CLASSNAMEINIT(PhysicalModel)

/* Default constructor */
PhysicalModel::PhysicalModel(const String & name)
  : TypedInterfaceObject<PhysicalModelImplementation>(new SymbolicPhysicalModel(name))
{
}


/* Default constructor */
PhysicalModel::PhysicalModel(const PhysicalModelImplementation & implementation)
  : TypedInterfaceObject<PhysicalModelImplementation>(implementation.clone())
{

}


/* Constructor from implementation */
PhysicalModel::PhysicalModel(const Implementation & p_implementation)
  : TypedInterfaceObject<PhysicalModelImplementation>(p_implementation)
{
  // Initialize any other class members here
  // At last, allocate memory space if needed, but go to destructor to free it
}

/* Constructor from implementation pointer */
PhysicalModel::PhysicalModel(PhysicalModelImplementation * p_implementation)
  : TypedInterfaceObject<PhysicalModelImplementation>(p_implementation)
{
  // Initialize any other class members here
  // At last, allocate memory space if needed, but go to destructor to free it
}


PhysicalModel* PhysicalModel::clone() const
{
  return new PhysicalModel(*this);
}


Bool PhysicalModel::operator==(const PhysicalModel& other) const
{
  if (this == &other) return true;
  return *getImplementation() == *other.getImplementation();
}


Bool PhysicalModel::operator!=(const PhysicalModel& other) const
{
  return !operator==(other);
}


void PhysicalModel::addObserver(Observer * observer)
{
  getImplementation()->addObserver(observer);
}


void PhysicalModel::blockNotification(const String & blockedObserverType)
{
  getImplementation()->blockNotification(blockedObserverType);
}


UnsignedInteger PhysicalModel::getInputDimension() const
{
  return getImplementation()->getInputDimension();
}


InputCollection PhysicalModel::getInputs() const
{
  return getImplementation()->getInputs();
}


Input & PhysicalModel::getInputByName(const String & inputName)
{
  return getImplementation()->getInputByName(inputName);
}


Input PhysicalModel::getInputByName(const String & inputName) const
{
  return getImplementation()->getInputByName(inputName);
}


void PhysicalModel::setInputs(const InputCollection & inputs)
{
  getImplementation()->setInputs(inputs);
}


void PhysicalModel::setInputName(const String & inputName, const String & newName)
{
  getImplementation()->setInputName(inputName, newName);
}


void PhysicalModel::setInputDescription(const String & inputName, const String & description)
{
  getImplementation()->setInputDescription(inputName, description);
}


void PhysicalModel::setInputValue(const String & inputName, const double & value)
{
  getImplementation()->setInputValue(inputName, value);
}


void PhysicalModel::setInputStochastic(const String & inputName, const bool & stoch)
{
  getImplementation()->setInputStochastic(inputName, stoch);
}


void PhysicalModel::setDistribution(const String & inputName, const Distribution & distribution)
{
  getImplementation()->setDistribution(inputName, distribution);
}


void PhysicalModel::setDistributionParametersType(const String & inputName, const UnsignedInteger & distributionParametersType)
{
  getImplementation()->setDistributionParametersType(inputName, distributionParametersType);
}


void PhysicalModel::setFiniteDifferenceStep(const String& inputName, const double& step)
{
  getImplementation()->setFiniteDifferenceStep(inputName, step);
}


void PhysicalModel::addInput(const Input & input)
{
  getImplementation()->addInput(input);
}


void PhysicalModel::removeInput(const String & inputName)
{
  getImplementation()->removeInput(inputName);
}


Description PhysicalModel::getInputNames() const
{
  return getImplementation()->getInputNames();
}


Description PhysicalModel::getStochasticInputNames() const
{
  return getImplementation()->getStochasticInputNames();
}


bool PhysicalModel::hasInputNamed(const String & inputName) const
{
  return getImplementation()->hasInputNamed(inputName);
}


bool PhysicalModel::hasStochasticInputs() const
{
  return getImplementation()->hasStochasticInputs();
}


Output & PhysicalModel::getOutputByName(const String & outputName)
{
  return getImplementation()->getOutputByName(outputName);
}


Output PhysicalModel::getOutputByName(const String & outputName) const
{
  return getImplementation()->getOutputByName(outputName);
}


UnsignedInteger PhysicalModel::getOutputDimension() const
{
  return getImplementation()->getOutputDimension();
}


OutputCollection PhysicalModel::getOutputs() const
{
  return getImplementation()->getOutputs();
}


void PhysicalModel::setOutputName(const String & outputName, const String & newName)
{
  getImplementation()->setOutputName(outputName, newName);
}


void PhysicalModel::setOutputs(const OutputCollection & outputs)
{
  getImplementation()->setOutputs(outputs);
}


void PhysicalModel::setOutputDescription(const String & outputName, const String & description)
{
  getImplementation()->setOutputDescription(outputName, description);
}


void PhysicalModel::setOutputValue(const String & outputName, const double & value)
{
  getImplementation()->setOutputValue(outputName, value);
}


void PhysicalModel::selectOutput(const String& outputName, const bool selected)
{
  getImplementation()->selectOutput(outputName, selected);
}


void PhysicalModel::addOutput(const Output & output)
{
  getImplementation()->addOutput(output);
}


void PhysicalModel::removeOutput(const String & outputName)
{
  getImplementation()->removeOutput(outputName);
}


Description PhysicalModel::getOutputNames() const
{
  return getImplementation()->getOutputNames();
}


bool PhysicalModel::hasOutputNamed(const String & outputName) const
{
  return getImplementation()->hasOutputNamed(outputName);
}


Description PhysicalModel::getSelectedOutputsNames() const
{
  return getImplementation()->getSelectedOutputsNames();
}


Distribution PhysicalModel::getDistribution() const
{
  return getImplementation()->getDistribution();
}


RandomVector PhysicalModel::getInputRandomVector() const
{
  return getImplementation()->getInputRandomVector();
}


RandomVector PhysicalModel::getOutputRandomVector(const Description & outputNames) const
{
  return getImplementation()->getOutputRandomVector(outputNames);
}


Function PhysicalModel::getFunction() const
{
  return getImplementation()->getFunction();
}


Function PhysicalModel::getFunction(const Description & outputNames) const
{
  return getImplementation()->getFunction(outputNames);
}


Function PhysicalModel::getFunction(const String & outputName) const
{
  return getImplementation()->getFunction(outputName);
}


Function PhysicalModel::getRestrictedFunction() const
{
  return getImplementation()->getRestrictedFunction();
}


Function PhysicalModel::getRestrictedFunction(const Description & outputNames) const
{
  return getImplementation()->getRestrictedFunction(outputNames);
}


PointToFieldFunction PhysicalModel::getPointToFieldFunction() const
{
  return getImplementation()->getPointToFieldFunction();
}


PointToFieldFunction PhysicalModel::getPointToFieldFunction(const Description & outputNames) const
{
  return getImplementation()->getPointToFieldFunction(outputNames);
}


PointToFieldFunction PhysicalModel::getRestrictedPointToFieldFunction(const Description & outputNames) const
{
  return getImplementation()->getRestrictedPointToFieldFunction(outputNames);
}


Copula PhysicalModel::getCopula() const
{
  return getImplementation()->getCopula();
}


Collection<Copula> PhysicalModel::getCopulaCollection() const
{
  return getImplementation()->getCopulaCollection();
}


void PhysicalModel::setCopula(const Description &inputNames, const Copula &copula)
{
  getImplementation()->setCopula(inputNames, copula);
}


Bool PhysicalModel::hasMesh() const
{
  return getImplementation()->hasMesh();
}


MeshModel PhysicalModel::getMeshModel() const
{
  return getImplementation()->getMeshModel();
}


void PhysicalModel::setMeshModel(const MeshModel& meshModel)
{
  getImplementation()->setMeshModel(meshModel);
}


Bool PhysicalModel::isParallel() const
{
  return getImplementation()->isParallel();
}


void PhysicalModel::setParallel(const Bool flag)
{
  getImplementation()->setParallel(flag);
}


bool PhysicalModel::isValid() const
{
  return getImplementation()->isValid();
}


String PhysicalModel::getHtmlDescription(const bool deterministic) const
{
  return getImplementation()->getHtmlDescription(deterministic);
}


String PhysicalModel::getPythonScript() const
{
  return getImplementation()->getPythonScript();
}


/* Method save() stores the object through the StorageManager */
void PhysicalModel::save(Advocate& adv) const
{
  getImplementation()->save(adv);
}


/* Method load() reloads the object from the StorageManager */
void PhysicalModel::load(Advocate& adv)
{
  getImplementation()->load(adv);
}
}
