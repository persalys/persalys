#include "otgui/PythonPhysicalModel.hxx"
#include "otgui/PythonEvaluation.hxx"

using namespace OT;

namespace OTGUI {

CLASSNAMEINIT(PythonPhysicalModel);

PythonPhysicalModel::PythonPhysicalModel(const String & name)
  : PhysicalModelImplementation(name)
{
  setCode("def _exec(X):\n    Y0 = X[0]\n    return [Y0]");
  updateFunction();
}


PythonPhysicalModel::PythonPhysicalModel(const String & name,
                                         const InputCollection & inputs,
                                         const OutputCollection & outputs,
                                         const String & code)
  : PhysicalModelImplementation(name, inputs, outputs)
{
  updateFunction();
}


PythonPhysicalModel* PythonPhysicalModel::clone() const
{
  return new PythonPhysicalModel(*this);
}


void PythonPhysicalModel::setCode(const String & code)
{
  code_ = code;
  notify("codeChanged");
}


String PythonPhysicalModel::getCode() const
{
  return code_;
}


void PythonPhysicalModel::updateFunction()
{
  try
  {
    function_ = NumericalMathFunction();
    function_.setEvaluation(new PythonEvaluation(getInputs().getSize(), getOutputs().getSize(), getCode()));
  }
  catch (std::exception & ex)
  {
    throw InvalidArgumentException(HERE) << ex.what();
  }
}


NumericalMathFunction PythonPhysicalModel::getFunction(const Description & outputNames)
{
  if (outputNames == getOutputNames())
    return getFunction();

  Description outputFormula(outputNames.getSize());
  for (UnsignedInteger i=0; i<outputNames.getSize(); ++i)
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


NumericalMathFunction PythonPhysicalModel::getFunction()
{
  updateFunction();
  return function_;
}


String PythonPhysicalModel::dump() const
{
  String result;

  for (UnsignedInteger i=0; i<getInputs().getSize(); ++i)
    result += getInputs()[i].dump();

  for (UnsignedInteger i=0; i<getOutputs().getSize(); ++i)
    result += getOutputs()[i].dump();

  result += getName() + " = otguibase.PythonPhysicalModel('" + getName() + "')\n";

  for (UnsignedInteger i=0; i<getInputs().getSize(); ++i)
    result += getName() + ".addInput(" + getInputs()[i].getName() + ")\n";

  for (UnsignedInteger i=0; i<getOutputs().getSize(); ++i)
    result += getName() + ".addOutput(" + getOutputs()[i].getName() + ")\n";

  result += getName() + ".setCode('";

  std::stringstream ss(getCode());
  String to;

  while (std::getline(ss, to, '\n'))
  {
    result += to + "\\n";
  }
  result += "')\n";

  result += PhysicalModelImplementation::dumpCopula();

  return result;
}
}