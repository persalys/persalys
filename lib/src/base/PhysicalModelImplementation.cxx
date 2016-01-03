#include "otgui/PhysicalModelImplementation.hxx"

#include "NormalCopula.hxx"

using namespace OT;

namespace OTGUI {

PhysicalModelImplementation::PhysicalModelImplementation(const std::string & name)
  : PersistentObject()
  , Observable()
  , name_(name)
  , inputs_(InputCollection(0))
  , outputs_(OutputCollection(0))
  , inputNames_(Description())
  , stochasticInputNames_(Description())
  , outputNames_(Description())
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
  , inputNames_(Description())
  , stochasticInputNames_(Description())
  , outputNames_(Description())
  , copula_(Copula())
{
}


PhysicalModelImplementation::PhysicalModelImplementation(const PhysicalModelImplementation & other)
  : PersistentObject()
  , Observable()
  , name_(other.name_)
  , inputs_(other.inputs_)
  , outputs_(other.outputs_)
  , inputNames_(other.inputNames_)
  , stochasticInputNames_(other.stochasticInputNames_)
  , outputNames_(other.outputNames_)
  , copula_(other.copula_)
{
}


PhysicalModelImplementation::PhysicalModelImplementation(const PhysicalModelImplementation * other)
  : PersistentObject()
  , Observable()
  , name_(other->name_)
  , inputs_(other->inputs_)
  , outputs_(other->outputs_)
  , inputNames_(other->inputNames_)
  , stochasticInputNames_(other->stochasticInputNames_)
  , outputNames_(other->outputNames_)
  , copula_(other->copula_)
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
  for (int i=0; i<inputs_.getSize(); ++i)
    if (inputs_[i].getName() == inputName)
      return inputs_[i];
  throw InvalidArgumentException(HERE) << "The given input name " << inputName <<" does not correspond to an input of the physical model.\n";
}


void PhysicalModelImplementation::setInputs(const InputCollection & inputs)
{
  std::set<std::string> inputNames;
  for (int i=0; i<inputs.getSize(); ++i)
    inputNames.insert(inputs[i].getName());
  if (inputNames.size() != inputs.getSize())
    throw InvalidArgumentException(HERE) << "Two inputs can not have the same name."; 

  inputNames_ = Description();
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
  if (!inputOldStateIsStochastic && distribution.getImplementation()->getClassName()!="Dirac")
  {
    stochasticInputNames_ = Description();
    updateCopula();
  }
  else if (inputOldStateIsStochastic && distribution.getImplementation()->getClassName()=="Dirac")
  {
    stochasticInputNames_ = Description();
    updateCopula();
  }

  notify("inputChanged");
  notify("updateProbabilisticModelWindow");
}


void PhysicalModelImplementation::addInput(const Input & input)
{
  if (hasAnInputNamed(input.getName()))
    throw InvalidArgumentException(HERE) << "The physical model has already an input named " << input.getName(); 

  inputs_.add(input);
  inputNames_.add(input.getName());

  if (input.isStochastic())
  {
    stochasticInputNames_.add(input.getName());
    updateCopula();
  }
  notify("inputChanged");
  notify("updateProbabilisticModelWindow");
}


void PhysicalModelImplementation::removeInput(const std::string & inputName)
{
  if (hasAnInputNamed(inputName))
  {
    for (int i=0; i<inputs_.getSize(); ++i)
      if (inputs_[i].getName() == inputName)
      {
        inputs_.erase(inputs_.begin() + i);
        inputNames_.erase(inputNames_.begin() + i);
        if (inputs_[i].isStochastic())
        {
          stochasticInputNames_ = Description();
          updateCopula();
        }
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
  if (!getStochasticInputNames().getSize())
  {
    copula_ = Copula();
    return;
  }

  CorrelationMatrix newSpearmanCorrelation(getStochasticInputNames().getSize());
  if (newSpearmanCorrelation.getDimension() > 1)
  {
    Description oldStochasticInputNames(copula_.getDescription());
    int size = oldStochasticInputNames.getSize();
    CorrelationMatrix oldSpearmanCorrelation(copula_.getSpearmanCorrelation());
    for (int row=0; row<size; ++row)
      for (int col=row+1; col<size; ++col)
        {
          Collection<String>::iterator it1;
          it1 = std::find(stochasticInputNames_.begin(), stochasticInputNames_.end(), oldStochasticInputNames[row]);
          Collection<String>::iterator it2;
          it2 = std::find(stochasticInputNames_.begin(), stochasticInputNames_.end(), oldStochasticInputNames[col]);
          if (it1 != stochasticInputNames_.end() &&  it2 != stochasticInputNames_.end())
          {
            int newRow = it1 -stochasticInputNames_.begin();
            int newCol = it2 - stochasticInputNames_.begin();
            newSpearmanCorrelation(newRow, newCol) = oldSpearmanCorrelation(row, col);
            newSpearmanCorrelation(newCol, newRow) = newSpearmanCorrelation(newRow, newCol);
          }
        }
  }

  CorrelationMatrix correlationMatrix(NormalCopula::GetCorrelationFromSpearmanCorrelation(newSpearmanCorrelation));
  copula_ = NormalCopula(correlationMatrix);
  copula_.setDescription(stochasticInputNames_);
}


Description PhysicalModelImplementation::getInputNames()
{
  if (!inputNames_.getSize())
  {
    inputNames_ = Description(inputs_.getSize());
    for (int i=0; i<inputs_.getSize(); ++i)
      inputNames_[i] = inputs_[i].getName();
  }
  return inputNames_;
}


Description PhysicalModelImplementation::getStochasticInputNames()
{
  if (!stochasticInputNames_.getSize())
  {
    stochasticInputNames_ = Description();
    for (int i=0; i<inputs_.getSize(); ++i)
      if (inputs_[i].isStochastic())
        stochasticInputNames_.add(inputs_[i].getName());
  }
  return stochasticInputNames_;
}


bool PhysicalModelImplementation::hasAnInputNamed(const std::string & inputName)
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
  for (int i=0; i<outputs_.getSize(); ++i)
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
  for (int i=0; i<outputs.getSize(); ++i)
    outputNames.insert(outputs[i].getName());
  if (outputNames.size() != outputs.getSize())
    throw InvalidArgumentException(HERE) << "Two inputs can not have the same name.";

  outputNames_ = Description();
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


void PhysicalModelImplementation::addOutput(const Output & output)
{
  if (hasAnOutputNamed(output.getName()))
    throw InvalidArgumentException(HERE) << "The physical model has already an output named " << output.getName(); 

  outputs_.add(output);
  outputNames_.add(output.getName());
  notify("outputChanged");
  notify("updateLimitStateWindow");
}


void PhysicalModelImplementation::removeOutput(const std::string & outputName)
{
  if (hasAnOutputNamed(outputName))
  {
    for (int i=0; i<outputs_.getSize(); ++i)
      if (outputs_[i].getName() == outputName)
      {
        outputs_.erase(outputs_.begin() + i);
        outputNames_.erase(outputNames_.begin() + i);
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
  if (!outputNames_.getSize())
  {
    outputNames_ = Description(outputs_.getSize());
    for (int i=0; i<outputs_.getSize(); ++i)
      outputNames_[i] = outputs_[i].getName();
  }
  return outputNames_;
}


bool PhysicalModelImplementation::hasAnOutputNamed(const std::string & outputName)
{
  Description outputNames = getOutputNames();
  if (std::find(outputNames.begin(), outputNames.end(), outputName) != outputNames.end())
    return true;
  return false;
}


ComposedDistribution PhysicalModelImplementation::getComposedDistribution() const
{
  ComposedDistribution::DistributionCollection marginales;
  for (int i=0; i<inputs_.getSize(); ++i)
    marginales.add(inputs_[i].getDistribution());
  return ComposedDistribution(marginales, getCopula());
  // TODO  return ComposedDistribution(marginales, getCopula());
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
  throw NotYetImplementedException(HERE) << "In PhysicalModelImplementation::getFunction(const Description &)";
}


NumericalMathFunction PhysicalModelImplementation::getFunction()
{
  throw NotYetImplementedException(HERE) << "In PhysicalModelImplementation::getFunction()";
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
}