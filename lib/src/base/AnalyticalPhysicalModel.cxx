#include "otgui/AnalyticalPhysicalModel.hxx"

using namespace OT;

namespace OTGUI {

CLASSNAMEINIT(AnalyticalPhysicalModel);

AnalyticalPhysicalModel::AnalyticalPhysicalModel(const String & name)
  : PhysicalModelImplementation(name)
{
}


AnalyticalPhysicalModel::AnalyticalPhysicalModel(const String & name,
                                                 const InputCollection & inputs,
                                                 const OutputCollection & outputs)
  : PhysicalModelImplementation(name, inputs, outputs)
{
}


AnalyticalPhysicalModel::AnalyticalPhysicalModel(const AnalyticalPhysicalModel & other)
  : PhysicalModelImplementation(other)
{
}


AnalyticalPhysicalModel* AnalyticalPhysicalModel::clone() const
{
  return new AnalyticalPhysicalModel(*this);
}


Description AnalyticalPhysicalModel::getFormulas()
{
  Description formulas(getOutputs().getSize());
  for (UnsignedInteger i=0; i<getOutputs().getSize(); ++i)
    formulas[i] = getOutputs()[i].getFormula();
  return formulas;
}


NumericalMathFunction AnalyticalPhysicalModel::getFunction()
{
  return NumericalMathFunction(getInputNames(), getOutputNames(), getFormulas());
}


String AnalyticalPhysicalModel::dump() const
{
  String result;

  for (UnsignedInteger i=0; i<getInputs().getSize(); ++i)
    result += getInputs()[i].dump();

  for (UnsignedInteger i=0; i<getOutputs().getSize(); ++i)
    result += getOutputs()[i].dump();

  result += getName()+ " = otguibase.AnalyticalPhysicalModel('" + getName() + "')\n";

  for (UnsignedInteger i=0; i<getInputs().getSize(); ++i)
    result += getName()+ ".addInput(" + getInputs()[i].getName() + ")\n";

  for (UnsignedInteger i=0; i<getOutputs().getSize(); ++i)
    result += getName()+ ".addOutput(" + getOutputs()[i].getName() + ")\n";

  result += PhysicalModelImplementation::dumpCopula();
  return result;
}
}