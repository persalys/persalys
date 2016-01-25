#include "otgui/PhysicalModelImplementation.hxx"

#include "NormalCopula.hxx"
#include "TruncatedDistribution.hxx"

using namespace OT;

namespace OTGUI {

PhysicalModelImplementation::PhysicalModelImplementation(const std::string & name)
  : PersistentObject()
  , Observable()
  , name_(name)
  , inputs_(InputCollection(0))
  , outputs_(OutputCollection(0))
  , copula_(Copula())
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
  , copula_(Copula())
{
}


PhysicalModelImplementation* PhysicalModelImplementation::clone() const
{
  return new PhysicalModelImplementation(*this);
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


Input & PhysicalModelImplementation::getInputByName(const std::string & inputName)
{
  for (UnsignedInteger i=0; i<inputs_.getSize(); ++i)
    if (inputs_[i].getName() == inputName)
      return inputs_[i];
  throw InvalidArgumentException(HERE) << "The given input name " << inputName <<" does not correspond to an input of the physical model.\n";
}


void PhysicalModelImplementation::setInputs(const InputCollection & inputs)
{
  std::set<std::string> inputNames;
  for (UnsignedInteger i=0; i<inputs.getSize(); ++i)
    inputNames.insert(inputs[i].getName());
  if (inputNames.size() != inputs.getSize())
    throw InvalidArgumentException(HERE) << "Two inputs can not have the same name."; 

  inputs_ = inputs;
  notify("inputChanged");
  notify("updateProbabilisticModelWindow");
}


void PhysicalModelImplementation::setInputDescription(const std::string & inputName, const std::string & description)
{
  getInputByName(inputName).setDescription(description);
  notify("inputChanged");
  notify("updateProbabilisticModelWindow");
}


void PhysicalModelImplementation::setInputValue(const std::string & inputName, const double & value)
{
  getInputByName(inputName).setValue(value);
  notify("inputChanged");
  notify("updateProbabilisticModelWindow");
}


void PhysicalModelImplementation::setInputDistribution(const std::string & inputName, const Distribution & distribution)
{
  bool inputOldStateIsStochastic = getInputByName(inputName).isStochastic();

  getInputByName(inputName).setDistribution(distribution);

  // update copula if need
  if ((!inputOldStateIsStochastic && distribution.getImplementation()->getClassName()!="Dirac") ||
      (inputOldStateIsStochastic && distribution.getImplementation()->getClassName()=="Dirac"))
    updateCopula();

  notify("inputChanged");
  notify("updateProbabilisticModelWindow");
}


void PhysicalModelImplementation::addInput(const Input & input)
{
  if (hasInputNamed(input.getName()))
    throw InvalidArgumentException(HERE) << "The physical model has already an input named " << input.getName(); 

  inputs_.add(input);
  if (input.isStochastic())
    updateCopula();

  notify("inputChanged");
  notify("updateProbabilisticModelWindow");
}


void PhysicalModelImplementation::removeInput(const std::string & inputName)
{
  if (hasInputNamed(inputName))
  {
    for (UnsignedInteger i=0; i<inputs_.getSize(); ++i)
      if (inputs_[i].getName() == inputName)
      {
        bool inputIsStochastic = inputs_[i].isStochastic();
        inputs_.erase(inputs_.begin() + i);
        if (inputIsStochastic)
          updateCopula();

        notify("inputChanged");
        notify("updateProbabilisticModelWindow");
        break;
      }
  }
  else
    throw InvalidArgumentException(HERE) << "The given input name " << inputName <<" does not correspond to an input of the physical model.\n";
}


void PhysicalModelImplementation::updateCopula()
{
  Description stochasticInputNames = getStochasticInputNames();
  if (!stochasticInputNames.getSize())
  {
    copula_ = Copula();
    return;
  }

  CorrelationMatrix newSpearmanCorrelation(stochasticInputNames.getSize());
  if (newSpearmanCorrelation.getDimension() > 1)
  {
    Description oldStochasticInputNames(copula_.getDescription());
    UnsignedInteger size = oldStochasticInputNames.getSize();
    CorrelationMatrix oldSpearmanCorrelation(copula_.getSpearmanCorrelation());
    for (UnsignedInteger row=0; row<size; ++row)
      for (UnsignedInteger col=row+1; col<size; ++col)
        {
          Collection<String>::iterator it1;
          it1 = std::find(stochasticInputNames.begin(), stochasticInputNames.end(), oldStochasticInputNames[row]);
          Collection<String>::iterator it2;
          it2 = std::find(stochasticInputNames.begin(), stochasticInputNames.end(), oldStochasticInputNames[col]);
          if (it1 != stochasticInputNames.end() &&  it2 != stochasticInputNames.end())
          {
            UnsignedInteger newRow = it1 - stochasticInputNames.begin();
            UnsignedInteger newCol = it2 - stochasticInputNames.begin();
            newSpearmanCorrelation(newRow, newCol) = oldSpearmanCorrelation(row, col);
            newSpearmanCorrelation(newCol, newRow) = oldSpearmanCorrelation(row, col);
          }
        }
  }

  CorrelationMatrix correlationMatrix(NormalCopula::GetCorrelationFromSpearmanCorrelation(newSpearmanCorrelation));
  copula_ = NormalCopula(correlationMatrix);
  copula_.setDescription(stochasticInputNames);
}


Description PhysicalModelImplementation::getInputNames()
{
  Description inputNames = Description(inputs_.getSize());
  for (UnsignedInteger i=0; i<inputs_.getSize(); ++i)
    inputNames[i] = inputs_[i].getName();
  return inputNames;
}


Description PhysicalModelImplementation::getStochasticInputNames()
{
  Description stochasticInputNames = Description();
  for (UnsignedInteger i=0; i<inputs_.getSize(); ++i)
    if (inputs_[i].isStochastic())
      stochasticInputNames.add(inputs_[i].getName());
  return stochasticInputNames;
}


bool PhysicalModelImplementation::hasInputNamed(const std::string & inputName)
{
  Description inputNames = getInputNames();
  if (std::find(inputNames.begin(), inputNames.end(), inputName) != inputNames.end())
    return true;
  return false;
}


bool PhysicalModelImplementation::hasStochasticInputs()
{
  if (getStochasticInputNames().getSize())
    return true;
  return false;
}


Output & PhysicalModelImplementation::getOutputByName(const std::string & outputName)
{
  for (UnsignedInteger i=0; i<outputs_.getSize(); ++i)
    if (outputs_[i].getName() == outputName)
      return outputs_[i];
  throw InvalidArgumentException(HERE) << "The given output name " << outputName <<" does not correspond to an output of the physical model.\n";
}


OutputCollection PhysicalModelImplementation::getOutputs() const
{
  return outputs_;
}

void PhysicalModelImplementation::setOutputs(const OutputCollection & outputs)
{
  std::set<std::string> outputNames;
  for (UnsignedInteger i=0; i<outputs.getSize(); ++i)
    outputNames.insert(outputs[i].getName());
  if (outputNames.size() != outputs.getSize())
    throw InvalidArgumentException(HERE) << "Two inputs can not have the same name.";

  outputs_ = outputs;
  notify("outputChanged");
  notify("updateLimitStateWindow");
}


void PhysicalModelImplementation::setOutputDescription(const std::string & outputName, const std::string & description)
{
  getOutputByName(outputName).setDescription(description);
  notify("outputChanged");
  notify("updateLimitStateWindow");
}


void PhysicalModelImplementation::setOutputFormula(const std::string & outputName, const std::string & formula)
{
  getOutputByName(outputName).setFormula(formula);
  notify("outputChanged");
  notify("updateLimitStateWindow");
}


void PhysicalModelImplementation::setOutputValue(const std::string & outputName, const double & value)
{
  getOutputByName(outputName).setValue(value);
  notify("outputChanged");
}


void PhysicalModelImplementation::addOutput(const Output & output)
{
  if (hasOutputNamed(output.getName()))
    throw InvalidArgumentException(HERE) << "The physical model has already an output named " << output.getName(); 

  outputs_.add(output);
  notify("outputChanged");
  notify("updateLimitStateWindow");
}


void PhysicalModelImplementation::removeOutput(const std::string & outputName)
{
  if (hasOutputNamed(outputName))
  {
    for (UnsignedInteger i=0; i<outputs_.getSize(); ++i)
      if (outputs_[i].getName() == outputName)
      {
        outputs_.erase(outputs_.begin() + i);
        notify("outputChanged");
        notify("updateLimitStateWindow");
        break;
      }
  }
  else
    throw InvalidArgumentException(HERE) << "The given output name " << outputName <<" does not correspond to an output of the physical model\n";
}


Description PhysicalModelImplementation::getOutputNames()
{
  Description  outputNames = Description(outputs_.getSize());
  for (UnsignedInteger i=0; i<outputs_.getSize(); ++i)
    outputNames[i] = outputs_[i].getName();
  return outputNames;
}


bool PhysicalModelImplementation::hasOutputNamed(const std::string & outputName)
{
  Description outputNames = getOutputNames();
  if (std::find(outputNames.begin(), outputNames.end(), outputName) != outputNames.end())
    return true;
  return false;
}


ComposedDistribution PhysicalModelImplementation::getComposedDistribution() const
{
  ComposedDistribution::DistributionCollection marginales;
  for (UnsignedInteger i=0; i<inputs_.getSize(); ++i)
    if (inputs_[i].isStochastic())
      marginales.add(inputs_[i].getDistribution());
  return ComposedDistribution(marginales, getCopula());
}


RandomVector PhysicalModelImplementation::getInputRandomVector()
{
  return RandomVector(getComposedDistribution());
}


RandomVector PhysicalModelImplementation::getOutputRandomVector(const Description & outputNames)
{
  return RandomVector(getFunction(outputNames), getInputRandomVector());
}


NumericalMathFunction PhysicalModelImplementation::getFunction(const Description & outputNames)
{
  if (outputNames.getSize() == getOutputs().getSize())
    return getFunction();

  Indices indices;
  for (UnsignedInteger i=0; i<outputNames.getSize(); ++i)
    for (UnsignedInteger j=0; j<getOutputs().getSize(); ++j)
      if (getOutputs()[j].getName() == outputNames[i])
      {
        indices.add(j);
        break;
      }

  try
  {
    return getFunction().getMarginal(indices);
  }
  catch (std::exception & ex)
  {
    throw InvalidArgumentException(HERE) << ex.what();
  }
}


NumericalMathFunction PhysicalModelImplementation::getFunction()
{
  throw NotYetImplementedException(HERE) << "In PhysicalModelImplementation::getFunction()";
}


NumericalMathFunction PhysicalModelImplementation::getRestrictedFunction()
{
  if (getInputs().getSize() == getStochasticInputNames().getSize())
    return getFunction();

  Indices deterministicInputsIndices;
  NumericalPoint inputsValues(getInputs().getSize());
  for (UnsignedInteger i=0; i<getInputs().getSize(); ++i)
  {
    inputsValues[i] = getInputs()[i].getValue();
    if (!getInputs()[i].isStochastic())
      deterministicInputsIndices.add(i);
  }
  try
  {
    return NumericalMathFunction(getFunction(), deterministicInputsIndices, inputsValues);
  }
  catch (std::exception & ex)
  {
    throw InvalidArgumentException(HERE) << "In PhysicalModelImplementation::getRestrictedFunction(): " << ex.what();
  }
}


NumericalMathFunction PhysicalModelImplementation::getRestrictedFunction(const Description & outputNames)
{
  if (outputNames.getSize() == getOutputs().getSize())
    return getRestrictedFunction();

  Indices indices;
  for (UnsignedInteger i=0; i<outputNames.getSize(); ++i)
    for (UnsignedInteger j=0; j<getOutputs().getSize(); ++j)
      if (getOutputs()[j].getName() == outputNames[i])
      {
        indices.add(j);
        break;
      }

  try
  {
    return getRestrictedFunction().getMarginal(indices);
  }
  catch (std::exception & ex)
  {
    throw InvalidArgumentException(HERE) << "In PhysicalModelImplementation::getRestrictedFunction(const Description & outputNames): " << ex.what();
  }
}


Copula PhysicalModelImplementation::getCopula() const
{
  return copula_;
}


void PhysicalModelImplementation::setCopula(const Copula & copula)
{
  if (copula.getDimension() != getStochasticInputNames().getSize())
    throw InvalidArgumentException(HERE) << "The given copula must have a dimension equal to the number of stochastic inputs.\n";

  copula_ = copula;
  copula_.setDescription(getStochasticInputNames());
}


std::string PhysicalModelImplementation::dump() const
{
  throw NotYetImplementedException(HERE) << "In PhysicalModelImplementation::getDump()";
}


std::string PhysicalModelImplementation::dumpProbaModel() const
{
  std::string result;

  for (UnsignedInteger i=0; i<getInputs().getSize(); ++i)
  {
    if (getInputs()[i].isStochastic())
    {
      Distribution distribution = getInputs()[i].getDistribution();
      std::string distributionName = distribution.getImplementation()->getClassName();
      std::string inputName = getInputs()[i].getName();

      OSS oss;
      if (distributionName != "TruncatedDistribution")
      {
        oss << "dist_" << inputName << " = ot." << distributionName << "(";
        NumericalPointWithDescription parameters = distribution.getParametersCollection()[0];
        for (UnsignedInteger i=0; i<parameters.getSize(); ++i)
        {
          oss << parameters[i];
          if (i < parameters.getSize() - 1)
            oss << ", ";
        }
        oss << ")\n";
      }
      else
      {
        TruncatedDistribution truncatedDistribution = *dynamic_cast<TruncatedDistribution*>(&*distribution.getImplementation());
        Distribution distribution = truncatedDistribution.getDistribution();
        oss << "dist_" << inputName << " = ot." << distribution.getImplementation()->getClassName() << "(";
        NumericalPointWithDescription parameters = distribution.getParametersCollection()[0];
        for (UnsignedInteger i=0; i<parameters.getSize(); ++i)
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

      result += oss.str();
      result += getName()+ ".setInputDistribution('" + inputName + "', ";
      result += " dist_" + inputName + ")\n";
    }
  }
  return result;
}
}