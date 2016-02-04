#include "otgui/YACSPhysicalModel.hxx"

using namespace OT;

namespace OTGUI {

CLASSNAMEINIT(YACSPhysicalModel);

YACSPhysicalModel::YACSPhysicalModel(const String & name, const String & fileName)
  : PhysicalModelImplementation(name)
  , evaluation_(YACSEvaluation(fileName))
{
  setXMLFileName(fileName);
}


YACSPhysicalModel* YACSPhysicalModel::clone() const
{
  return new YACSPhysicalModel(*this);
}


void YACSPhysicalModel::setInputs(const InputCollection & inputs)
{
  throw NotYetImplementedException(HERE) << "Use setXMLFileName to modify an YACSPhysicalModel";
}


void YACSPhysicalModel::addInput(const Input & input)
{
  throw NotYetImplementedException(HERE) << "Use setXMLFileName to modify an YACSPhysicalModel";
}


void YACSPhysicalModel::removeInput(const String & inputName)
{
  throw NotYetImplementedException(HERE) << "Use setXMLFileName to modify an YACSPhysicalModel";
}


void YACSPhysicalModel::setOutputs(const OutputCollection & outputs)
{
  throw NotYetImplementedException(HERE) << "Use setXMLFileName to modify an YACSPhysicalModel";
}


void YACSPhysicalModel::addOutput(const Output & output)
{
  throw NotYetImplementedException(HERE) << "Use setXMLFileName to modify an YACSPhysicalModel";
}


void YACSPhysicalModel::removeOutput(const String & outputName)
{
  throw NotYetImplementedException(HERE) << "Use setXMLFileName to modify an YACSPhysicalModel";
}


String YACSPhysicalModel::getXMLFileName() const
{
  return evaluation_.getXMLFileName();
}


void YACSPhysicalModel::setXMLFileName(const String & fileName)
{
  if (fileName.empty())
    throw InvalidArgumentException(HERE) << "Impossible to create a model from an empty file";

  try
  {
    evaluation_.setXMLFileName(fileName);
  }
  catch (std::exception & ex)
  {
    throw InvalidArgumentException(HERE) << "Impossible to create an YACSPhysicalModel with the file "
                                         << fileName << "\n" << ex.what();
  }
  updateData();
}


void YACSPhysicalModel::updateData()
{
  for (UnsignedInteger i=0; i<evaluation_.getInputDimension(); ++i)
  {
    Input newInput(evaluation_.getInputVariablesNames()[i], evaluation_.getInputValues()[i]);
    PhysicalModelImplementation::addInput(newInput);
  }
  notify("inputChanged");

  for (UnsignedInteger i=0; i<evaluation_.getOutputDimension(); ++i)
  {
    Output newOutput(evaluation_.getOutputVariablesNames()[i]);
    PhysicalModelImplementation::addOutput(newOutput);
  }
  notify("outputChanged");
}


NumericalMathFunction YACSPhysicalModel::getFunction(const Description & outputNames)
{
  if (outputNames == getOutputNames())
    return getFunction();

  YACSEvaluation anEvaluation(evaluation_);
  anEvaluation.setOutputVariablesNames(outputNames);
  return NumericalMathFunction(anEvaluation);
}


NumericalMathFunction YACSPhysicalModel::getFunction()
{
  return NumericalMathFunction(evaluation_);
}


String YACSPhysicalModel::dump() const
{
  String result;

  result += getName()+ " = otguibase.YACSPhysicalModel('" + getName() + "', '";
  result += getXMLFileName() + "')\n";

  result += dumpProbaModel();
  result += PhysicalModelImplementation::dumpCopula();

  return result;
}
}