//                                               -*- C++ -*-
/**
 *  @brief Abstract top-level class for all physical models 
 *
 *  Copyright 2015-2017 EDF-Phimeca
 *
 *  This library is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
#include "otgui/PhysicalModelImplementation.hxx"

#include <openturns/NonCenteredFiniteDifferenceGradient.hxx>
#include <openturns/CenteredFiniteDifferenceHessian.hxx>
#include <openturns/NormalCopula.hxx>
#include <openturns/TruncatedDistribution.hxx>
#include <openturns/PersistentObjectFactory.hxx>
#include <openturns/ParametricFunction.hxx>

using namespace OT;

namespace OTGUI {

static Factory<PersistentCollection<Input> > Factory_PersistentCollectionInput;
static Factory<PersistentCollection<Output> > Factory_PersistentCollectionOutput;

/* Constructor with parameters */
PhysicalModelImplementation::PhysicalModelImplementation(const String & name)
  : PersistentObject()
  , Observable()
{
  setName(name);
}


/* Constructor with parameters */
PhysicalModelImplementation::PhysicalModelImplementation(const String & name,
                                                         const InputCollection & inputs,
                                                         const OutputCollection & outputs)
  : PersistentObject()
  , Observable()
  , inputs_(inputs)
  , outputs_(outputs)
{
  setName(name);

  // set copula
  updateCopula();

  // set finite difference step
  updateFiniteDifferenceSteps();
}


/* Virtual constructor */
PhysicalModelImplementation* PhysicalModelImplementation::clone() const
{
  return new PhysicalModelImplementation(*this);
}


Bool PhysicalModelImplementation::operator==(const PhysicalModelImplementation& other) const
{
  if (this == &other)
    return true;
  return false;
}


Bool PhysicalModelImplementation::operator!=(const PhysicalModelImplementation& other) const
{
  return !operator==(other);
}


InputCollection PhysicalModelImplementation::getInputs() const
{
  return inputs_;
}


Input & PhysicalModelImplementation::getInputByName(const String & inputName)
{
  for (UnsignedInteger i=0; i<inputs_.getSize(); ++i)
    if (inputs_[i].getName() == inputName)
      return inputs_[i];
  throw InvalidArgumentException(HERE) << "The given input name " << inputName <<" does not correspond to an input of the physical model.\n";
}


Input PhysicalModelImplementation::getInputByName(const String & inputName) const
{
  for (UnsignedInteger i=0; i<inputs_.getSize(); ++i)
    if (inputs_[i].getName() == inputName)
      return inputs_[i];
  throw InvalidArgumentException(HERE) << "The given input name " << inputName <<" does not correspond to an input of the physical model.\n";
}


void PhysicalModelImplementation::setInputs(const InputCollection & inputs)
{
  std::set<String> inputNames;
  for (UnsignedInteger i=0; i<inputs.getSize(); ++i)
    inputNames.insert(inputs[i].getName());
  if (inputNames.size() != inputs.getSize())
    throw InvalidArgumentException(HERE) << "Two inputs can not have the same name."; 

  inputs_ = inputs;

  // reset outputs values
  for (UnsignedInteger i=0; i<getOutputs().getSize(); ++i)
    getOutputByName(getOutputs()[i].getName()).setHasBeenComputed(false);

  // update copula
  updateCopula();

  // update finite difference step
  updateFiniteDifferenceSteps();

  notify("inputNumberChanged");
}


void PhysicalModelImplementation::setInputName(const OT::String & inputName, const OT::String & newName)
{
  if (inputName == newName)
    return;

  std::set<String> inputNames;
  inputNames.insert(newName);
  for (UnsignedInteger i=0; i<inputs_.getSize(); ++i)
    if (inputs_[i].getName() != inputName)
      inputNames.insert(inputs_[i].getName());
  if (inputNames.size() != inputs_.getSize())
    throw InvalidArgumentException(HERE) << "The proposed name " << newName << " is not valid. Two inputs can not have the same name.";

  getInputByName(inputName).setName(newName);
  notify("inputNameChanged");
}


void PhysicalModelImplementation::setInputDescription(const String & inputName, const String & description)
{
  getInputByName(inputName).setDescription(description);
  notify("inputDescriptionChanged");
}


void PhysicalModelImplementation::setInputValue(const String & inputName, const double & value)
{
  getInputByName(inputName).setValue(value);
  for (UnsignedInteger i=0; i<getOutputs().getSize(); ++i)
    getOutputByName(getOutputs()[i].getName()).setHasBeenComputed(false);
  notify("inputValueChanged");
}


void PhysicalModelImplementation::setDistribution(const String & inputName, const Distribution & distribution)
{
  bool inputOldStateIsStochastic = getInputByName(inputName).isStochastic();

  getInputByName(inputName).setDistribution(distribution);

  // update copula if need
  if ((!inputOldStateIsStochastic && distribution.getImplementation()->getClassName() != "Dirac") ||
      (inputOldStateIsStochastic && distribution.getImplementation()->getClassName() == "Dirac"))
    updateCopula();

  notify("inputDistributionChanged");
}


void PhysicalModelImplementation::setDistributionParametersType(const OT::String & inputName, const OT::UnsignedInteger & distributionParametersType)
{
  getInputByName(inputName).setDistributionParametersType(distributionParametersType);
}


void PhysicalModelImplementation::addInput(const Input & input)
{
  if (hasInputNamed(input.getName()))
    throw InvalidArgumentException(HERE) << "The physical model already contains an input named " << input.getName(); 

  inputs_.add(input);

  // update copula
  if (input.isStochastic())
    updateCopula();

  // reset outputs values
  for (UnsignedInteger i=0; i<getOutputs().getSize(); ++i)
    getOutputByName(getOutputs()[i].getName()).setHasBeenComputed(false);

  // update finite difference step
  updateFiniteDifferenceSteps();

  notify("inputNumberChanged");
}


void PhysicalModelImplementation::setFiniteDifferenceStep(const String& inputName, const double& step)
{
  getInputByName(inputName).setFiniteDifferenceStep(step);

  // update finite difference step
  updateFiniteDifferenceSteps();

  notify("inputStepChanged");
}


void PhysicalModelImplementation::removeInput(const String & inputName)
{
  if (hasInputNamed(inputName))
  {
    for (UnsignedInteger i=0; i<inputs_.getSize(); ++i)
    {
      if (inputs_[i].getName() == inputName)
      {
        bool inputIsStochastic = inputs_[i].isStochastic();
        inputs_.erase(inputs_.begin() + i);
        if (inputIsStochastic)
          updateCopula();
        for (UnsignedInteger j=0; j<getOutputs().getSize(); ++j)
          getOutputByName(getOutputs()[j].getName()).setHasBeenComputed(false);

        notify("inputNumberChanged");
        break;
      }
    }
    // update finite difference step
    updateFiniteDifferenceSteps();
  }
  else
    throw InvalidArgumentException(HERE) << "The given input name " << inputName <<" does not correspond to an input of the physical model.\n";
}


void PhysicalModelImplementation::clearInputs()
{
  inputs_.clear();
  notify("inputNumberChanged");
}


void PhysicalModelImplementation::updateCopula()
{
  Description stochasticInputNames = getStochasticInputNames();
  if (!stochasticInputNames.getSize())
  {
    copula_ = Copula();
    notify("copulaChanged");
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
            const UnsignedInteger newRow = it1 - stochasticInputNames.begin();
            const UnsignedInteger newCol = it2 - stochasticInputNames.begin();
            newSpearmanCorrelation(newRow, newCol) = oldSpearmanCorrelation(row, col);
            newSpearmanCorrelation(newCol, newRow) = oldSpearmanCorrelation(row, col);
          }
        }
  }

  CorrelationMatrix correlationMatrix(NormalCopula::GetCorrelationFromSpearmanCorrelation(newSpearmanCorrelation));
  setCopula(NormalCopula(correlationMatrix));
}


Description PhysicalModelImplementation::getInputNames() const
{
  Description inputNames(inputs_.getSize());
  for (UnsignedInteger i=0; i<inputs_.getSize(); ++i)
    inputNames[i] = inputs_[i].getName();
  return inputNames;
}


void PhysicalModelImplementation::updateFiniteDifferenceSteps() const
{
  finiteDifferenceSteps_ = Point(getInputs().getSize());
  for (UnsignedInteger i=0; i<getInputs().getSize(); ++i)
    finiteDifferenceSteps_[i] = getInputs()[i].getFiniteDifferenceStep();
}


Point PhysicalModelImplementation::getFiniteDifferenceSteps() const
{
  if (finiteDifferenceSteps_.getSize() != inputs_.getSize())
    updateFiniteDifferenceSteps();

  return finiteDifferenceSteps_;
}


Description PhysicalModelImplementation::getStochasticInputNames() const
{
  Description stochasticInputNames;
  for (UnsignedInteger i=0; i<inputs_.getSize(); ++i)
    if (inputs_[i].isStochastic())
      stochasticInputNames.add(inputs_[i].getName());
  return stochasticInputNames;
}


bool PhysicalModelImplementation::hasInputNamed(const String & inputName) const
{
  for (UnsignedInteger i=0; i<inputs_.getSize(); ++i)
    if (inputs_[i].getName() == inputName)
      return true;
  return false;
}


bool PhysicalModelImplementation::hasStochasticInputs() const
{
  if (getStochasticInputNames().getSize())
    return true;
  return false;
}


Output & PhysicalModelImplementation::getOutputByName(const String & outputName)
{
  for (UnsignedInteger i=0; i<outputs_.getSize(); ++i)
    if (outputs_[i].getName() == outputName)
      return outputs_[i];
  throw InvalidArgumentException(HERE) << "The given output name " << outputName <<" does not correspond to an output of the physical model.\n";
}


Output PhysicalModelImplementation::getOutputByName(const String & outputName) const
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
  std::set<String> outputNames;
  for (UnsignedInteger i=0; i<outputs.getSize(); ++i)
    outputNames.insert(outputs[i].getName());
  if (outputNames.size() != outputs.getSize())
    throw InvalidArgumentException(HERE) << "Two outputs can not have the same name.";

  outputs_ = outputs;
  notify("outputNumberChanged");
}


void PhysicalModelImplementation::setOutputName(const OT::String & outputName, const OT::String & newName)
{
  if (outputName == newName)
    return;

  std::set<String> outputNames;
  outputNames.insert(newName);
  for (UnsignedInteger i=0; i<outputs_.getSize(); ++i)
    if (outputs_[i].getName() != outputName)
      outputNames.insert(outputs_[i].getName());
  if (outputNames.size() != outputs_.getSize())
    throw InvalidArgumentException(HERE) << "The proposed name " << newName << " is not valid. Two outputs can not have the same name.";

  getOutputByName(outputName).setName(newName);
  notify("outputNameChanged");
}


void PhysicalModelImplementation::setOutputDescription(const String & outputName, const String & description)
{
  getOutputByName(outputName).setDescription(description);
  notify("outputDescriptionChanged");
}


void PhysicalModelImplementation::setOutputValue(const String & outputName, const double & value)
{
  getOutputByName(outputName).setValue(value);
  getOutputByName(outputName).setHasBeenComputed(true);
  notify("outputValueChanged");
}


void PhysicalModelImplementation::selectOutput(const String& outputName, const bool selected)
{
  getOutputByName(outputName).setIsSelected(selected);
  notify("outputSelectionChanged");
}


void PhysicalModelImplementation::addOutput(const Output & output)
{
  if (hasOutputNamed(output.getName()))
    throw InvalidArgumentException(HERE) << "The physical model already contains an output named " << output.getName(); 

  outputs_.add(output);
  notify("outputNumberChanged");
}


void PhysicalModelImplementation::removeOutput(const String & outputName)
{
  if (hasOutputNamed(outputName))
  {
    for (UnsignedInteger i=0; i<outputs_.getSize(); ++i)
      if (outputs_[i].getName() == outputName)
      {
        outputs_.erase(outputs_.begin() + i);
        notify("outputNumberChanged");
        break;
      }
  }
  else
    throw InvalidArgumentException(HERE) << "The given output name " << outputName <<" does not correspond to an output of the physical model\n";
}


void PhysicalModelImplementation::clearOutputs()
{
  outputs_.clear();
  notify("outputNumberChanged");
}


Description PhysicalModelImplementation::getOutputNames() const
{
  Description outputNames(outputs_.getSize());
  for (UnsignedInteger i=0; i<outputs_.getSize(); ++i)
    outputNames[i] = outputs_[i].getName();
  return outputNames;
}


bool PhysicalModelImplementation::hasOutputNamed(const String & outputName) const
{
  for (UnsignedInteger i=0; i<outputs_.getSize(); ++i)
    if (outputs_[i].getName() == outputName)
      return true;
  return false;
}


Description PhysicalModelImplementation::getSelectedOutputsNames() const
{
  Description selectedOutputsNames;
  for (UnsignedInteger i=0; i<getOutputs().getSize(); ++i)
    if (getOutputs()[i].isSelected())
      selectedOutputsNames.add(getOutputs()[i].getName());
  return selectedOutputsNames;
}


ComposedDistribution PhysicalModelImplementation::getComposedDistribution() const
{
  ComposedDistribution::DistributionCollection marginales;
  for (UnsignedInteger i=0; i<inputs_.getSize(); ++i)
    if (inputs_[i].isStochastic())
      marginales.add(inputs_[i].getDistribution());
  if (marginales.getSize())
    return ComposedDistribution(marginales, getCopula());
  return ComposedDistribution(); // we can not build a ComposedDistribution with an empty collection
}


RandomVector PhysicalModelImplementation::getInputRandomVector() const
{
  if (!hasStochasticInputs())
    throw PhysicalModelNotValidException(HERE) << "Can not use getInputRandomVector on a physical model which has no stochastic inputs.";
  return RandomVector(getComposedDistribution());
}


RandomVector PhysicalModelImplementation::getOutputRandomVector(const Description & outputNames) const
{
  return RandomVector(getRestrictedFunction(outputNames), getInputRandomVector());
}

Function PhysicalModelImplementation::generateFunction(const Description & outputNames) const
{
  throw NotYetImplementedException(HERE) << "In PhysicalModelImplementation::generateFunction(outputNames)";
}


Function PhysicalModelImplementation::getFunction() const
{
  return getFunction(getOutputNames());
}


Function PhysicalModelImplementation::getFunction(const Description & outputNames) const
{
  if (!getInputs().getSize())
    throw PhysicalModelNotValidException(HERE) << "The physical model has no inputs.";
  if (!getOutputs().getSize())
    throw PhysicalModelNotValidException(HERE) << "The physical model has no outputs.";

  Function function(generateFunction(outputNames));

  if (function.getUseDefaultGradientImplementation())
  {
    // use finite difference gradient
    NonCenteredFiniteDifferenceGradient gradient(getFiniteDifferenceSteps(), function.getEvaluation());
    function.setGradient(gradient);
  }
  if (function.getUseDefaultHessianImplementation())
  {
    // use finite difference hessian
    CenteredFiniteDifferenceHessian hessian(getFiniteDifferenceSteps(), function.getEvaluation());
    function.setHessian(hessian);
  }

  if (function.getOutputDimension() == outputNames.getSize())
    return function;

  // search interest outputs indices
  Indices indices;
  for (UnsignedInteger i=0; i<outputNames.getSize(); ++i)
    for (UnsignedInteger j=0; j<getOutputs().getSize(); ++j)
      if (getOutputs()[j].getName() == outputNames[i])
      {
        indices.add(j);
        break;
      }

  return function.getMarginal(indices);
}


Function PhysicalModelImplementation::getFunction(const String & outputName) const
{
  return getFunction(Description(1, outputName));
}


Function PhysicalModelImplementation::getRestrictedFunction() const
{
  if (getInputs().getSize() == getStochasticInputNames().getSize())
    return getFunction();

  return getRestrictedFunction(getOutputNames());
}


Function PhysicalModelImplementation::getRestrictedFunction(const Description& outputNames) const
{
  // search deterministic inputs
  Indices deterministicInputsIndices;
  Point inputsValues;
  for (UnsignedInteger i=0; i<getInputs().getSize(); ++i)
  {
    if (!getInputs()[i].isStochastic())
    {
      deterministicInputsIndices.add(i);
      inputsValues.add(getInputs()[i].getValue());
    }
  }

  const Function function(getFunction(outputNames));

  // if there is no deterministic inputs
  if (!deterministicInputsIndices.getSize())
    return function;

  // if there are deterministic inputs
  const ParametricFunction restricted(function, deterministicInputsIndices, inputsValues);
  return restricted;

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
  notify("copulaChanged");
}


bool PhysicalModelImplementation::isValid() const
{
  return getSelectedOutputsNames().getSize() && getInputs().getSize();
}


String PhysicalModelImplementation::getPythonScript() const
{
  throw NotYetImplementedException(HERE) << "In PhysicalModelImplementation::getPythonScript()";
}


String PhysicalModelImplementation::getProbaModelPythonScript() const
{
  String result;

  for (UnsignedInteger i=0; i<getInputs().getSize(); ++i)
  {
    if (getInputs()[i].isStochastic())
    {
      Distribution distribution = getInputs()[i].getDistribution();
      String distributionName = distribution.getImplementation()->getClassName();
      String inputName = getInputs()[i].getName();
      String inputPythonName(inputName);
      std::replace(inputPythonName.begin(), inputPythonName.end(), '.', '_');

      OSS oss;
      oss.setPrecision(12);

      if (distributionName != "TruncatedDistribution")
      {
        oss << "dist_" << inputPythonName << " = ot." << distributionName << "(";
        PointWithDescription parameters = distribution.getParametersCollection()[0];
        for (UnsignedInteger j=0; j<parameters.getSize(); ++ j)
        {
          oss << parameters[j];
          if (j < parameters.getSize() - 1)
            oss << ", ";
        }
        oss << ")\n";
      }
      else
      {
        TruncatedDistribution truncatedDistribution = *dynamic_cast<TruncatedDistribution*>(distribution.getImplementation().get());
        distribution = truncatedDistribution.getDistribution();
        oss << "dist_" << inputPythonName << " = ot." << distribution.getImplementation()->getClassName() << "(";
        PointWithDescription parameters = distribution.getParametersCollection()[0];
        for (UnsignedInteger j=0; j < parameters.getSize(); ++ j)
        {
          oss << parameters[j];
          if (j < parameters.getSize() - 1)
            oss << ", ";
        }
        oss << ")\n";
        oss << "dist_" << inputPythonName << " = ot." << distributionName << "(";
        oss << "dist_" << inputPythonName << ", ";

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
      result += getName() + ".setDistribution('" + inputName + "', ";
      result += " dist_" + inputPythonName + ")\n";
    }
  }
  return result;
}


String PhysicalModelImplementation::getCopulaPythonScript() const
{
  String result;

  CorrelationMatrix correlationMatrix(getCopula().getCorrelation());

  OSS oss;
  oss.setPrecision(12);

  oss << "correlationMatrix = ot.CorrelationMatrix(" << correlationMatrix.getNbRows() << ")\n";

  bool hasCorrelation = false;
  for (UnsignedInteger i=0; i<correlationMatrix.getNbRows(); ++i)
    for (UnsignedInteger j=i+1; j<correlationMatrix.getNbRows(); ++j)
      if (correlationMatrix(i, j) != 0.0)
      {
        hasCorrelation = true;
        oss << "correlationMatrix[" << i << ", " << j << "] = " << correlationMatrix(i, j) << "\n";
      }

  // if there is no correlation: write nothing
  if (!hasCorrelation)
    return result;

  // else:
  result += oss.str();
  result += "copula = ot.NormalCopula(ot.NormalCopula.GetCorrelationFromSpearmanCorrelation(correlationMatrix))\n";
  result += getName() + ".setCopula(copula)\n";

  return result;
}


/* String converter */
String PhysicalModelImplementation::__repr__() const
{
  OSS oss;
  oss << "class=" << getClassName()
      << " inputs=" << getInputs()
      << " outputs=" << getOutputs()
      << " copula=" << getCopula();
  return oss;
}


/* Method save() stores the object through the StorageManager */
void PhysicalModelImplementation::save(Advocate & adv) const
{
  PersistentObject::save(adv);
  adv.saveAttribute("inputs_", inputs_);
  adv.saveAttribute("outputs_", outputs_);
  adv.saveAttribute("copula_", copula_);
}


/* Method load() reloads the object from the StorageManager */
void PhysicalModelImplementation::load(Advocate & adv)
{
  PersistentObject::load(adv);
  adv.loadAttribute("inputs_", inputs_);
  adv.loadAttribute("outputs_", outputs_);
  adv.loadAttribute("copula_", copula_);
}


/* Nothing to do at this level. Overloaded in child classes if needed.*/
void PhysicalModelImplementation::acceptLaunchParameters(LaunchParametersVisitor* visitor)
{
}

}
