#include "otgui/PhysicalModelImplementation.hxx"

using namespace OT;

namespace OTGUI {

PhysicalModelImplementation::PhysicalModelImplementation(const std::string & name)
  : PersistentObject()
  , Observable()
  , name_(name)
  , inputs_(InputCollection(0))
  , outputs_(OutputCollection(0))
  , function_(NumericalMathFunction())
  , inputNames_(Description())
  , outputNames_(Description())
  , listFormula_(Description())
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
  , inputNames_(Description())
  , outputNames_(Description())
  , listFormula_(Description())
{
  // setInputs
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
  , inputNames_(other.inputNames_)
  , outputNames_(other.outputNames_)
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
  , inputNames_(other->inputNames_)
  , outputNames_(other->outputNames_)
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


// void PhysicalModelImplementation::setInput(int row, const Input & input)
// {
//   inputs_[row] = input;
//   checkInputs();
//   notify("inputChanged");
// }


bool PhysicalModelImplementation::updateInput(int row, const Input & input, bool updateProbaModel)
{
  inputs_[row] = input;
  if (updateProbaModel)
    notify("updateProbabilisticModel");
  return checkInputs();
}


// addition of an input required by the user from python
void PhysicalModelImplementation::addInput(Input input)
{
  newInput(input);
  notify("inputChanged");
}


// addition of an input required by the interface
void PhysicalModelImplementation::newInput(const Input & input)
{
  inputs_.add(input);
  checkInputs();
  notify("updateProbabilisticModel");
}


void PhysicalModelImplementation::removeInput(int row)
{
  inputs_.erase(inputs_.begin() + row);
  checkInputs();
  notify("updateProbabilisticModel");
}


bool PhysicalModelImplementation::checkInputs()
{
  inputNames_ = Description(inputs_.getSize());

  for (int i=0; i<inputs_.getSize(); ++i)
    inputNames_[i] = inputs_[i].getName();

  if (outputNames_.getSize())
  {
    try
    {
      function_ = NumericalMathFunction(inputNames_, outputNames_, listFormula_);
    }
    catch (std::exception & ex)
    {
      return false;
    }
  }

  return true;
}


Description PhysicalModelImplementation::getInputNames() const
{
  return inputNames_;
}


bool PhysicalModelImplementation::hasStochasticInputs()
{
  for (int i=0; i<inputs_.getSize(); ++i)
    if (inputs_[i].getDistribution().getImplementation()->getClassName() != "Dirac")
      return true;
  return false;
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


bool PhysicalModelImplementation::updateOutput(int row, const Output & output)
{
  outputs_[row] = output;
  return checkOutputs();
}


void PhysicalModelImplementation::addOutput(Output output)
{
  newOutput(output);
  notify("outputChanged");
}


void PhysicalModelImplementation::newOutput(const Output& output)
{
  outputs_.add(output);
  checkOutputs();  
}


void PhysicalModelImplementation::removeOutput(int row)
{
  outputs_.erase(outputs_.begin() + row);
  checkOutputs();
}


bool PhysicalModelImplementation::checkOutputs()
{
  outputNames_ = Description(outputs_.getSize());
  listFormula_ = Description(outputs_.getSize());

  for (int i=0; i<outputs_.getSize(); ++i)
  {
    outputNames_[i] = outputs_[i].getName();
    listFormula_[i] = outputs_[i].getFormula();
  }

  try
  {
    function_ = NumericalMathFunction(inputNames_, outputNames_, listFormula_);
  }
  catch (std::exception & ex)
  {
    return false;
  }

  return true;
}


ComposedDistribution PhysicalModelImplementation::getComposedDistribution() const
{
  ComposedDistribution::DistributionCollection marginales;
  for (int i=0; i<inputs_.getSize(); ++i)
    marginales.add(inputs_[i].getDistribution());
  return ComposedDistribution(marginales);
}


RandomVector PhysicalModelImplementation::getInputRandomVector()
{
//   TODO:  RandomVector(ComposedDistribution(marginales, getCopula()));
  return RandomVector(getComposedDistribution());
}


RandomVector PhysicalModelImplementation::getOutputRandomVector(const OutputCollection & outputs)
{
  return RandomVector(getFunction(outputs), getInputRandomVector());
}


NumericalMathFunction PhysicalModelImplementation::getFunction(const OutputCollection & outputs) const
{
  if (outputs == outputs_)
    return function_;

  Description outputNames(outputs.getSize());
  Description outputFormula(outputs.getSize());
  for (int i=0; i<outputs.getSize(); ++i)
  {
    outputNames[i] = outputs[i].getName();
    outputFormula[i] = outputs[i].getFormula();
  }
  return NumericalMathFunction(inputNames_, outputNames, outputFormula);
}


NumericalMathFunction PhysicalModelImplementation::getFunction() const
{
  return function_;
}


void PhysicalModelImplementation::setFunction(const NumericalMathFunction & function)
{
  function_ = function;
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