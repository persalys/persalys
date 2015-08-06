#include "PhysicalModel.hxx"

namespace OTGUI {

PhysicalModel::PhysicalModel(const std::string & name)
  : p_implementation_(new PhysicalModelImplementation(name))
{
}


PhysicalModel::PhysicalModel(const std::string & name, const InputCollection & inputs, const OutputCollection & outputs)
  : p_implementation_(new PhysicalModelImplementation(name, inputs, outputs))
{
}


PhysicalModel::PhysicalModel(const PhysicalModel & other)
  : p_implementation_(other.p_implementation_)
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
  p_implementation_->addObserver(observer);
}


std::string PhysicalModel::getName() const
{
  return p_implementation_->getName();
}


void PhysicalModel::setName(const std::string & name)
{
  p_implementation_->setName(name);
}


InputCollection PhysicalModel::getInputs() const
{
  return p_implementation_->getInputs();
}

void PhysicalModel::setInputs(const InputCollection & inputs)
{
  p_implementation_->setInputs(inputs);
}


void PhysicalModel::addInput(Input input)
{
  p_implementation_->addInput(input);
}


OutputCollection PhysicalModel::getOutputs() const
{
  return p_implementation_->getOutputs();
}

void PhysicalModel::setOutputs(const OutputCollection & outputs)
{
  p_implementation_->setOutputs(outputs);
}


void PhysicalModel::addOutput(Output output)
{
  p_implementation_->addOutput(output);
}


OT::NumericalMathFunction PhysicalModel::getFunction() const
{
  return p_implementation_->getFunction();
}

void PhysicalModel::setFunction(const OT::NumericalMathFunction & function)
{
  p_implementation_->setFunction(function);
}


bool PhysicalModel::checkInputs()
{
  return p_implementation_->checkInputs();
}


bool PhysicalModel::checkOutputs()
{
  return p_implementation_->checkOutputs();
}



void PhysicalModel::loadDataWithYACS(const std::string & fileName)
{
#ifdef OTGUI_HAVE_YACS
  p_implementation_->loadDataWithYACS(fileName);
#endif
}


std::string PhysicalModel::dump() const
{
  return p_implementation_->dump();
}


}