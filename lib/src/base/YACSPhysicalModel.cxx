#include "otgui/YACSPhysicalModel.hxx"

using namespace OT;

namespace OTGUI {

CLASSNAMEINIT(YACSPhysicalModel);

YACSPhysicalModel::YACSPhysicalModel(const std::string & name, const std::string & fileName)
  : PhysicalModelImplementation(name)
  , evaluation_(YACSEvaluation(fileName))
{
  try
  {
    setXMLFileName(fileName);
  }
  catch(std::exception)
  {
    throw InvalidArgumentException(HERE) << "Impossible to create an YACSPhysicalModel with the file " << fileName << "\n";
  }
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


void YACSPhysicalModel::removeInput(const std::string & inputName)
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


void YACSPhysicalModel::removeOutput(const std::string & outputName)
{
  throw NotYetImplementedException(HERE) << "Use setXMLFileName to modify an YACSPhysicalModel";
}


std::string YACSPhysicalModel::getXMLFileName() const
{
  return evaluation_.getXMLFileName();
}


void YACSPhysicalModel::setXMLFileName(const std::string & fileName)
{
  if (fileName.empty())
  {
    std::cerr<<"Impossible to create a model from an empty file "<<fileName<<std::endl;
    throw InvalidArgumentException(HERE) << "Impossible to create a model from an empty file";
  }
    
  try
  {
    evaluation_.setXMLFileName(fileName);
    updateData();
  }
  catch(std::exception)
  {
    std::cerr<<"Impossible to create a model from the file "<<fileName<<std::endl;
    throw;
  }
}


void YACSPhysicalModel::updateData()
{
  for (int i=0; i<evaluation_.getInputDimension(); ++i)
  {
    Input newInput = Input(evaluation_.getInputVariablesNames()[i], evaluation_.getInputValues()[i]);
    PhysicalModelImplementation::addInput(newInput);
  }
  notify("inputChanged");

  for (int i=0; i<evaluation_.getOutputDimension(); ++i)
  {
    Output newOutput = Output(evaluation_.getOutputVariablesNames()[i]);
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


std::string YACSPhysicalModel::dump() const
{
  std::string result;

  result += getName()+ " = otguibase.YACSPhysicalModel('" + getName() + "', '";
  result += getXMLFileName() + "')\n";

  result += dumpProbaModel();
  result += PhysicalModelImplementation::dumpCopula();

  return result;
}
}