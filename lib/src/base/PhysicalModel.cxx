#include "otgui/PhysicalModel.hxx"

#include "otgui/AnalyticalPhysicalModel.hxx"

using namespace OT;

namespace OTGUI {

CLASSNAMEINIT(PhysicalModel);

PhysicalModel::PhysicalModel(const std::string & name)
  : TypedInterfaceObject<PhysicalModelImplementation>(new AnalyticalPhysicalModel(name))
{
}


PhysicalModel::PhysicalModel(const std::string & name, const InputCollection & inputs, const OutputCollection & outputs)
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



PhysicalModel::PhysicalModel(const PhysicalModel & other)
  : TypedInterfaceObject<PhysicalModelImplementation>(other.getImplementation())
{
}


PhysicalModel* PhysicalModel::clone() const
{
  return new PhysicalModel(*this);
}


void PhysicalModel::addObserver(Observer * observer)
{
  getImplementation()->addObserver(observer);
}


void PhysicalModel::blockNotification(bool block, const std::string & notBlockedMessage)
{
  getImplementation()->blockNotification(block, notBlockedMessage);
}


std::string PhysicalModel::getName() const
{
  return getImplementation()->getName();
}


void PhysicalModel::setName(const std::string & name)
{
  getImplementation()->setName(name);
}


InputCollection PhysicalModel::getInputs() const
{
  return getImplementation()->getInputs();
}


Input & PhysicalModel::getInputByName(const std::string & inputName)
{
  return getImplementation()->getInputByName(inputName);
}


void PhysicalModel::setInputs(const InputCollection & inputs)
{
  getImplementation()->setInputs(inputs);
}


void PhysicalModel::setInputDescription(const std::string & inputName, const std::string & description)
{
  return getImplementation()->setInputDescription(inputName, description);
}


void PhysicalModel::setInputValue(const std::string & inputName, const double & value)
{
  return getImplementation()->setInputValue(inputName, value);
}


void PhysicalModel::setInputDistribution(const std::string & inputName, const Distribution & distribution)
{
  return getImplementation()->setInputDistribution(inputName, distribution);
}


void PhysicalModel::addInput(const Input & input)
{
  getImplementation()->addInput(input);
}


void PhysicalModel::removeInput(const std::string & inputName)
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


bool PhysicalModel::hasAnInputNamed(const std::string & inputName)
{
  return getImplementation()->hasAnInputNamed(inputName);
}


bool PhysicalModel::hasStochasticInputs()
{
  return getImplementation()->hasStochasticInputs();
}


Output & PhysicalModel::getOutputByName(const std::string & outputName)
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


void PhysicalModel::setOutputDescription(const std::string & outputName, const std::string & description)
{
  return getImplementation()->setOutputDescription(outputName, description);
}


void PhysicalModel::setOutputFormula(const std::string & outputName, const std::string & formula)
{
  return getImplementation()->setOutputFormula(outputName, formula);
}


void PhysicalModel::addOutput(const Output & output)
{
  getImplementation()->addOutput(output);
}


void PhysicalModel::removeOutput(const std::string & outputName)
{
  getImplementation()->removeOutput(outputName);
}


Description PhysicalModel::getOutputNames()
{
  return getImplementation()->getOutputNames();
}


bool PhysicalModel::hasAnOutputNamed(const std::string & outputName)
{
  return getImplementation()->hasAnOutputNamed(outputName);
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


NumericalMathFunction PhysicalModel::getFunction(const Description & outputNames)
{
  return getImplementation()->getFunction(outputNames);
}


NumericalMathFunction PhysicalModel::getFunction()
{
  return getImplementation()->getFunction();
}


Copula PhysicalModel::getCopula() const
{
  return getImplementation()->getCopula();
}


void PhysicalModel::setCopula(const Copula & copula)
{
  getImplementation()->setCopula(copula);
}


std::string PhysicalModel::dump() const
{
  return getImplementation()->dump();
}
}