#include "otgui/PhysicalModel.hxx"

using namespace OT;

namespace OTGUI {

PhysicalModel::PhysicalModel(const std::string & name)
  : TypedInterfaceObject<PhysicalModelImplementation>(new PhysicalModelImplementation(name))
{
}


PhysicalModel::PhysicalModel(const std::string & name, const InputCollection & inputs, const OutputCollection & outputs)
  : TypedInterfaceObject<PhysicalModelImplementation>(new PhysicalModelImplementation(name, inputs, outputs))
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


PhysicalModel::~PhysicalModel()
{
}


void PhysicalModel::addObserver(Observer * observer)
{
  getImplementation()->addObserver(observer);
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


void PhysicalModel::setInputs(const InputCollection & inputs)
{
  getImplementation()->setInputs(inputs);
}


bool PhysicalModel::updateInput(int row, const Input & input, bool updateProbaModel)
{
  return getImplementation()->updateInput(row, input, updateProbaModel);
}


void PhysicalModel::addInput(Input input)
{
  getImplementation()->addInput(input);
}


void PhysicalModel::newInput(const Input & input)
{
  getImplementation()->newInput(input);
}


void PhysicalModel::removeInput(int row)
{
  getImplementation()->removeInput(row);
}


bool PhysicalModel::checkInputs()
{
  return getImplementation()->checkInputs();
}


Description PhysicalModel::getInputNames() const
{
  return getImplementation()->getInputNames();
}


bool PhysicalModel::hasStochasticInputs()
{
  return getImplementation()->hasStochasticInputs();
}


OutputCollection PhysicalModel::getOutputs() const
{
  return getImplementation()->getOutputs();
}


void PhysicalModel::setOutputs(const OutputCollection & outputs)
{
  getImplementation()->setOutputs(outputs);
}


bool PhysicalModel::updateOutput(int row, const Output & output)
{
  return getImplementation()->updateOutput(row, output);
}


void PhysicalModel::addOutput(Output output)
{
  getImplementation()->addOutput(output);
}


void PhysicalModel::newOutput(const Output & output)
{
  getImplementation()->newOutput(output);
}


void PhysicalModel::removeOutput(int row)
{
  getImplementation()->removeOutput(row);
}


bool PhysicalModel::checkOutputs()
{
  return getImplementation()->checkOutputs();
}


ComposedDistribution PhysicalModel::getComposedDistribution() const
{
  return getImplementation()->getComposedDistribution();
}


RandomVector PhysicalModel::getInputRandomVector()
{
  return getImplementation()->getInputRandomVector();
}


RandomVector PhysicalModel::getOutputRandomVector(const OutputCollection & outputs)
{
  return getImplementation()->getOutputRandomVector(outputs);
}


NumericalMathFunction PhysicalModel::getFunction(const OutputCollection & outputs) const
{
  return getImplementation()->getFunction(outputs);
}


NumericalMathFunction PhysicalModel::getFunction() const
{
  return getImplementation()->getFunction();
}

void PhysicalModel::setFunction(const NumericalMathFunction & function)
{
  getImplementation()->setFunction(function);
}


std::string PhysicalModel::dump() const
{
  return getImplementation()->dump();
}


}