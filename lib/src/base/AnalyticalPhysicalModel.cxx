#include "otgui/AnalyticalPhysicalModel.hxx"

using namespace OT;

namespace OTGUI {

CLASSNAMEINIT(AnalyticalPhysicalModel);

AnalyticalPhysicalModel::AnalyticalPhysicalModel(const std::string & name)
  : PhysicalModelImplementation(name)
  , function_(NumericalMathFunction())
{
}


AnalyticalPhysicalModel::AnalyticalPhysicalModel(const std::string & name,
                                                 const InputCollection & inputs,
                                                 const OutputCollection & outputs)
  : PhysicalModelImplementation(name, inputs, outputs)
{
  updateFunction();
}


AnalyticalPhysicalModel::AnalyticalPhysicalModel(const AnalyticalPhysicalModel & other)
 : PhysicalModelImplementation(other)
 , function_(other.function_)
{
}


AnalyticalPhysicalModel* AnalyticalPhysicalModel::clone() const
{
  return new AnalyticalPhysicalModel(*this);
}


void AnalyticalPhysicalModel::setInputs(const InputCollection & inputs)
{
  PhysicalModelImplementation::setInputs(inputs);
  try
  {
    if (getOutputNames().getSize())
      updateFunction();
  }
  catch(std::exception & ex)
  {
    throw InvalidArgumentException(HERE) << ex.what();
  }
}


void AnalyticalPhysicalModel::addInput(const Input & input)
{
  PhysicalModelImplementation::addInput(input);
  try
  {
    if (getOutputNames().getSize())
      updateFunction();
  }
  catch(std::exception & ex)
  {
    throw InvalidArgumentException(HERE) << ex.what();
  }
}


void AnalyticalPhysicalModel::removeInput(const std::string & inputName)
{
  PhysicalModelImplementation::removeInput(inputName);
  try
  {
    if (getOutputNames().getSize())
      updateFunction();
  }
  catch(std::exception & ex)
  {
    throw InvalidArgumentException(HERE) << ex.what();
  }
}


void AnalyticalPhysicalModel::setOutputs(const OutputCollection & outputs)
{
  formulas_ = Description();
  PhysicalModelImplementation::setOutputs(outputs);
  try
  {
    if (getInputNames().getSize())
      updateFunction();
  }
  catch(std::exception & ex)
  {
    throw InvalidArgumentException(HERE) << ex.what();
  }
}


void AnalyticalPhysicalModel::addOutput(const Output & output)
{
  PhysicalModelImplementation::addOutput(output);
  try  
  {
    formulas_.add(output.getFormula());
    updateFunction();
  }
  catch(std::exception & ex)
  {
    throw InvalidArgumentException(HERE) << ex.what();
  }
}

Description AnalyticalPhysicalModel::getFormulas()
{
  if (!formulas_.getSize())
  {
    Description formulas = Description(getOutputs().getSize());
    for (int i=0; i<getOutputs().getSize(); ++i)
      formulas[i] = getOutputs()[i].getFormula();
  }
  return formulas_;
}


void AnalyticalPhysicalModel::updateFunction()
{
  try
  {
    function_ = NumericalMathFunction(getInputNames(), getOutputNames(), getFormulas());
  }
  catch (std::exception & ex)
  {
    throw InvalidArgumentException(HERE) << ex.what();
  }
}


NumericalMathFunction AnalyticalPhysicalModel::getFunction(const Description & outputNames)
{
  if (outputNames == getOutputNames())
    return getFunction();

  Description outputFormula(outputNames.getSize());
  for (int i=0; i<outputNames.getSize(); ++i)
    outputFormula[i] = getOutputByName(outputNames[i]).getFormula();

  try
  {
    return NumericalMathFunction(getInputNames(), outputNames, outputFormula);
  }
  catch (std::exception & ex)
  {
    throw InvalidArgumentException(HERE) << ex.what();
  }
}


NumericalMathFunction AnalyticalPhysicalModel::getFunction()
{
  updateFunction();
  return function_;
}


std::string AnalyticalPhysicalModel::dump() const
{
  std::string result;

  for (int i=0; i<getInputs().getSize(); ++i)
    result += getInputs()[i].dump();

  for (int i=0; i<getOutputs().getSize(); ++i)
    result += getOutputs()[i].dump();

  result += getName()+ " = otguibase.AnalyticalPhysicalModel('" + getName() + "')\n";

  for (int i=0; i<getInputs().getSize(); ++i)
    result += getName()+ ".addInput(" + getInputs()[i].getName() + ")\n";

  for (int i=0; i<getOutputs().getSize(); ++i)
    result += getName()+ ".addOutput(" + getOutputs()[i].getName() + ")\n";

  return result;
}
}