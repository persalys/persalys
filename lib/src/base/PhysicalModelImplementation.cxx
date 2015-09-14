#include "PhysicalModelImplementation.hxx"


namespace OTGUI {

PhysicalModelImplementation::PhysicalModelImplementation(const std::string & name)
  : PersistentObject()
  , Observable()
  , name_(name)
  , inputs_(InputCollection(0))
  , outputs_(OutputCollection(0))
  , function_(OT::NumericalMathFunction())
  , listInput_(OT::Description())
  , listOutput_(OT::Description())
  , listFormula_(OT::Description())
{
}


PhysicalModelImplementation::PhysicalModelImplementation(const std::string & name,
                                                         const InputCollection & inputs,
                                                         const OutputCollection & outputs)
  : PersistentObject()
  , Observable()
  , name_(name)
  , inputs_(inputs)
  , outputs_(outputs)
  , listInput_(OT::Description())
  , listOutput_(OT::Description())
  , listFormula_(OT::Description())
{
  checkInputs();
  checkOutputs();
}


PhysicalModelImplementation::PhysicalModelImplementation(const PhysicalModelImplementation & other)
  : PersistentObject()
  , Observable()
  , name_(other.name_)
  , inputs_(other.inputs_)
  , outputs_(other.outputs_)
  , function_(other.function_)
  , listInput_(other.listInput_)
  , listOutput_(other.listOutput_)
  , listFormula_(other.listFormula_)
{
  checkInputs();
  checkOutputs();
}


PhysicalModelImplementation::PhysicalModelImplementation(const PhysicalModelImplementation * other)
  : PersistentObject()
  , Observable()
  , name_(other->name_)
  , inputs_(other->inputs_)
  , outputs_(other->outputs_)
  , function_(other->function_)
  , listInput_(other->listInput_)
  , listOutput_(other->listOutput_)
  , listFormula_(other->listFormula_)
{
  checkInputs();
  checkOutputs();
}


PhysicalModelImplementation* PhysicalModelImplementation::clone() const
{
  return new PhysicalModelImplementation(*this);
}


PhysicalModelImplementation::~PhysicalModelImplementation()
{
}


std::string PhysicalModelImplementation::getName() const
{
  return name_;
}


void PhysicalModelImplementation::setName(const std::string & name)
{
  name_ = name;
}


InputCollection PhysicalModelImplementation::getInputs() const
{
  return inputs_;
}

void PhysicalModelImplementation::setInputs(const InputCollection & inputs)
{
  inputs_ = inputs;
  checkInputs();
  notify("inputChanged");
}


void PhysicalModelImplementation::addInput(Input input)
{
  inputs_.add(input);
  checkInputs();
  notify("inputChanged");
}


OutputCollection PhysicalModelImplementation::getOutputs() const
{
  return outputs_;
}

void PhysicalModelImplementation::setOutputs(const OutputCollection & outputs)
{
  outputs_ = outputs;
  checkOutputs();
  notify("outputChanged");
}


void PhysicalModelImplementation::addOutput(Output output)
{
  outputs_.add(output);
  checkOutputs();
  notify("outputChanged");
}


OT::NumericalMathFunction PhysicalModelImplementation::getFunction() const
{
  return function_;
}

void PhysicalModelImplementation::setFunction(const OT::NumericalMathFunction & function)
{
  function_ = function;
}


bool PhysicalModelImplementation::checkInputs()
{
  listInput_ = OT::Description(inputs_.getSize());

  for (int i=0; i<inputs_.getSize(); ++i)
  {
    listInput_[i] = inputs_[i].getName();
  }

  if (listOutput_.getSize())
  {
    try
    {
      function_ = OT::NumericalMathFunction(listInput_, listOutput_, listFormula_);
    }
    catch (std::exception & ex)
    {
      return false;
    }
  }

  return true;
}


bool PhysicalModelImplementation::checkOutputs()
{
  listOutput_ = OT::Description(outputs_.getSize());
  listFormula_ = OT::Description(outputs_.getSize());

  for (int i=0; i<outputs_.getSize(); ++i)
  {
    listOutput_[i] = outputs_[i].getName();
    listFormula_[i] = outputs_[i].getFormula();
  }

  try
  {
    function_ = OT::NumericalMathFunction(listInput_, listOutput_, listFormula_);
  }
  catch (std::exception & ex)
  {
    return false;
  }

  return true;
}


std::string PhysicalModelImplementation::dump() const
{
  std::string result;

  for (int i=0; i<inputs_.getSize(); ++i)
    result += inputs_[i].dump();

  for (int i=0; i<outputs_.getSize(); ++i)
    result += outputs_[i].dump();

  result += getName()+ " = otguibase.PhysicalModel('" + getName() + "')\n";

  for (int i=0; i<inputs_.getSize(); ++i)
    result += getName()+ ".addInput(" + inputs_[i].getName() + ")\n";

  for (int i=0; i<outputs_.getSize(); ++i)
    result += getName()+ ".addOutput(" + outputs_[i].getName() + ")\n";

  return result;
}


}