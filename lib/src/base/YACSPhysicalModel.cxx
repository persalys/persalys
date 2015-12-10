#include "otgui/YACSPhysicalModel.hxx"

#include "TruncatedDistribution.hxx"

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
    std::cerr<<"Impossible to create an YACSPhysicalModel with the file "<<fileName<<std::endl;
    throw;
  }
}


YACSPhysicalModel::YACSPhysicalModel(const YACSPhysicalModel & other)
 : PhysicalModelImplementation(other)
 , evaluation_(other.evaluation_)
{
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

  for (int i=0; i<getInputs().getSize(); ++i)
  {
    Distribution distribution = getInputs()[i].getDistribution();
    std::string inputName = getInputs()[i].getName();
    std::string distributionName = distribution.getImplementation()->getClassName();

    if (distributionName != "Dirac")
    {
      if (distributionName != "TruncatedDistribution")
      {
        OSS oss;
        oss << "dist_" << inputName << " = ot." << distributionName << "(";
        NumericalPointWithDescription parameters = distribution.getParametersCollection()[0];
        for (unsigned int i = 0; i < parameters.getSize(); ++ i)
        {
          oss << parameters[i];
          if (i < parameters.getSize() - 1)
            oss << ", ";
        }
        oss << ")\n";
      }
//       TODO TruncatedNormal
      else
      {
        OSS oss;
        TruncatedDistribution truncatedDistribution = *dynamic_cast<TruncatedDistribution*>(&*distribution.getImplementation());
        Distribution distribution = truncatedDistribution.getDistribution();
        oss << "dist_" << inputName << " = ot." << distribution.getImplementation()->getClassName() << "(";
        NumericalPointWithDescription parameters = distribution.getParametersCollection()[0];
        for (unsigned int i = 0; i < parameters.getSize(); ++ i)
        {
          oss << parameters[i];
          if (i < parameters.getSize() - 1)
            oss << ", ";
        }
        oss << ")\n";
        oss << "dist_" << inputName << " = ot." << distributionName << "(";
        oss << "dist_" << inputName << ", ";

        if (!(truncatedDistribution.getFiniteLowerBound() && truncatedDistribution.getFiniteUpperBound())) // one side truncation ?
        {
          if (truncatedDistribution.getFiniteLowerBound())    //lower bound truncation
            oss << truncatedDistribution.getLowerBound() << ")\n";
          else
            oss << truncatedDistribution.getUpperBound() << ", ot.TruncatedDistribution.UPPER)\n";
        }
        else  // both sides truncation
          oss << truncatedDistribution.getUpperBound() << ", " << truncatedDistribution.getUpperBound() <<")\n";
      }

      result += getName()+ ".setInputDistribution(" + inputName + ", ";
      result += " dist_" + inputName + ")\n";
    }
  }

  return result;
}
}