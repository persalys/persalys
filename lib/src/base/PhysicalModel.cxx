//                                               -*- C++ -*-
/**
 *  @brief Abstract top-level class for all physical models 
 *
 *  Copyright 2015-2016 EDF
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
#include "otgui/PhysicalModel.hxx"

#include "otgui/AnalyticalPhysicalModel.hxx"

using namespace OT;

namespace OTGUI {

CLASSNAMEINIT(PhysicalModel);

PhysicalModel::PhysicalModel(const String & name)
  : TypedInterfaceObject<PhysicalModelImplementation>(new AnalyticalPhysicalModel(name))
{
}


PhysicalModel::PhysicalModel(const String & name, const InputCollection & inputs, const OutputCollection & outputs)
  : TypedInterfaceObject<PhysicalModelImplementation>(new AnalyticalPhysicalModel(name, inputs, outputs))
{
}


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


void PhysicalModel::addObserver(Observer * observer)
{
  getImplementation()->addObserver(observer);
}


void PhysicalModel::blockNotification(bool block, const String & notBlockedMessage)
{
  getImplementation()->blockNotification(block, notBlockedMessage);
}


InputCollection PhysicalModel::getInputs() const
{
  return getImplementation()->getInputs();
}


Input & PhysicalModel::getInputByName(const String & inputName)
{
  return getImplementation()->getInputByName(inputName);
}


void PhysicalModel::setInputs(const InputCollection & inputs)
{
  getImplementation()->setInputs(inputs);
}


void PhysicalModel::setInputDescription(const String & inputName, const String & description)
{
  return getImplementation()->setInputDescription(inputName, description);
}


void PhysicalModel::setInputValue(const String & inputName, const double & value)
{
  return getImplementation()->setInputValue(inputName, value);
}


void PhysicalModel::setInputDistribution(const String & inputName, const Distribution & distribution)
{
  return getImplementation()->setInputDistribution(inputName, distribution);
}


void PhysicalModel::addInput(const Input & input)
{
  getImplementation()->addInput(input);
}


void PhysicalModel::removeInput(const String & inputName)
{
  getImplementation()->removeInput(inputName);
}


Description PhysicalModel::getInputNames()
{
  return getImplementation()->getInputNames();
}


Description PhysicalModel::getStochasticInputNames()
{
  return getImplementation()->getStochasticInputNames();
}


bool PhysicalModel::hasInputNamed(const String & inputName)
{
  return getImplementation()->hasInputNamed(inputName);
}


bool PhysicalModel::hasStochasticInputs()
{
  return getImplementation()->hasStochasticInputs();
}


Output & PhysicalModel::getOutputByName(const String & outputName)
{
  return getImplementation()->getOutputByName(outputName);
}


OutputCollection PhysicalModel::getOutputs() const
{
  return getImplementation()->getOutputs();
}


void PhysicalModel::setOutputs(const OutputCollection & outputs)
{
  getImplementation()->setOutputs(outputs);
}


void PhysicalModel::setOutputDescription(const String & outputName, const String & description)
{
  return getImplementation()->setOutputDescription(outputName, description);
}


void PhysicalModel::setOutputFormula(const String & outputName, const String & formula)
{
  return getImplementation()->setOutputFormula(outputName, formula);
}


void PhysicalModel::setOutputValue(const String & outputName, const double & value)
{
  return getImplementation()->setOutputValue(outputName, value);
}


void PhysicalModel::addOutput(const Output & output)
{
  getImplementation()->addOutput(output);
}


void PhysicalModel::removeOutput(const String & outputName)
{
  getImplementation()->removeOutput(outputName);
}


Description PhysicalModel::getOutputNames()
{
  return getImplementation()->getOutputNames();
}


bool PhysicalModel::hasOutputNamed(const String & outputName)
{
  return getImplementation()->hasOutputNamed(outputName);
}


ComposedDistribution PhysicalModel::getComposedDistribution() const
{
  return getImplementation()->getComposedDistribution();
}


RandomVector PhysicalModel::getInputRandomVector()
{
  return getImplementation()->getInputRandomVector();
}


RandomVector PhysicalModel::getOutputRandomVector(const Description & outputNames)
{
  return getImplementation()->getOutputRandomVector(outputNames);
}


NumericalMathFunction PhysicalModel::getFunction()
{
  return getImplementation()->getFunction();
}


NumericalMathFunction PhysicalModel::getFunction(const Description & outputNames)
{
  return getImplementation()->getFunction(outputNames);
}


NumericalMathFunction PhysicalModel::getFunction(const String & outputName)
{
  return getImplementation()->getFunction(outputName);
}


NumericalMathFunction PhysicalModel::getRestrictedFunction()
{
  return getImplementation()->getRestrictedFunction();
}


NumericalMathFunction PhysicalModel::getRestrictedFunction(const Description & outputNames)
{
  return getImplementation()->getRestrictedFunction(outputNames);
}


Copula PhysicalModel::getCopula() const
{
  return getImplementation()->getCopula();
}


void PhysicalModel::setCopula(const Copula & copula)
{
  getImplementation()->setCopula(copula);
}


String PhysicalModel::dump() const
{
  return getImplementation()->dump();
}
}