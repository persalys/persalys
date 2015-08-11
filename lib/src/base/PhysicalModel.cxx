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



void PhysicalModel::loadDataWithYACS(const std::string & fileName)
{
#ifdef OTGUI_HAVE_YACS
  getImplementation()->loadDataWithYACS(fileName);
#endif
}


std::string PhysicalModel::dump() const
{
  return getImplementation()->dump();
}


}