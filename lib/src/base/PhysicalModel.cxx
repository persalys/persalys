#include "PhysicalModel.hxx"

namespace OTGUI {

PhysicalModel::PhysicalModel(const std::string & name)
  : OT::TypedInterfaceObject<PhysicalModelImplementation>(new PhysicalModelImplementation(name))
{
}


PhysicalModel::PhysicalModel(const std::string & name, const InputCollection & inputs, const OutputCollection & outputs)
  : OT::TypedInterfaceObject<PhysicalModelImplementation>(new PhysicalModelImplementation(name, inputs, outputs))
{
}


PhysicalModel::PhysicalModel(const PhysicalModelImplementation & implementation)
  : OT::TypedInterfaceObject<PhysicalModelImplementation>(implementation.clone())
{
  
}


/* Constructor from implementation */
PhysicalModel::PhysicalModel(const Implementation & p_implementation)
  : OT::TypedInterfaceObject<PhysicalModelImplementation>(p_implementation)
{
  // Initialize any other class members here
  // At last, allocate memory space if needed, but go to destructor to free it
}

/* Constructor from implementation pointer */
PhysicalModel::PhysicalModel(PhysicalModelImplementation * p_implementation)
  : OT::TypedInterfaceObject<PhysicalModelImplementation>(p_implementation)
{
  // Initialize any other class members here
  // At last, allocate memory space if needed, but go to destructor to free it
}



PhysicalModel::PhysicalModel(const PhysicalModel & other)
  : OT::TypedInterfaceObject<PhysicalModelImplementation>(other.getImplementation())
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


bool PhysicalModel::updateInputs(const InputCollection & inputs)
{
  return getImplementation()->updateInputs(inputs);
}


void PhysicalModel::addInput(Input input)
{
  getImplementation()->addInput(input);
}


OutputCollection PhysicalModel::getOutputs() const
{
  return getImplementation()->getOutputs();
}

void PhysicalModel::setOutputs(const OutputCollection & outputs)
{
  getImplementation()->setOutputs(outputs);
}


bool PhysicalModel::updateOutputs(const OutputCollection& outputs)
{
  return getImplementation()->updateOutputs(outputs);
}


void PhysicalModel::addOutput(Output output)
{
  getImplementation()->addOutput(output);
}


OT::NumericalMathFunction PhysicalModel::getFunction() const
{
  return getImplementation()->getFunction();
}

void PhysicalModel::setFunction(const OT::NumericalMathFunction & function)
{
  getImplementation()->setFunction(function);
}


bool PhysicalModel::checkInputs()
{
  return getImplementation()->checkInputs();
}


bool PhysicalModel::checkOutputs()
{
  return getImplementation()->checkOutputs();
}


std::string PhysicalModel::dump() const
{
  return getImplementation()->dump();
}


}