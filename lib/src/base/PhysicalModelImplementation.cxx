//                                               -*- C++ -*-
/**
 *  @brief Abstract top-level class for all physical models 
 *
 *  Copyright 2015-2016 EDF
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

#include "NormalCopula.hxx"
#include "TruncatedDistribution.hxx"
#include "PersistentObjectFactory.hxx"

using namespace OT;

namespace OTGUI {

static Factory<PersistentCollection<Input> > RegisteredFactory_CollIn("PersistentCollection<Input>");
static Factory<PersistentCollection<Output> > RegisteredFactory_CollOut("PersistentCollection<Output>");

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
  updateCopula();
}


/* Virtual constructor */
PhysicalModelImplementation* PhysicalModelImplementation::clone() const
{
  return new PhysicalModelImplementation(*this);
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


void PhysicalModelImplementation::setInputs(const InputCollection & inputs)
{
  std::set<String> inputNames;
  for (UnsignedInteger i=0; i<inputs.getSize(); ++i)
    inputNames.insert(inputs[i].getName());
  if (inputNames.size() != inputs.getSize())
    throw InvalidArgumentException(HERE) << "Two inputs can not have the same name."; 

  inputs_ = inputs;
  for (UnsignedInteger i=0; i<getOutputs().getSize(); ++i)
    getOutputByName(getOutputs()[i].getName()).setHasBeenComputed(false);
  notify("inputAdded");
  notify("modelInputsChanged");
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
  notify("modelInputsChanged");
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
  notify("modelInputsChanged");
}


void PhysicalModelImplementation::setInputDistribution(const String & inputName, const Distribution & distribution)
{
  bool inputOldStateIsStochastic = getInputByName(inputName).isStochastic();

  getInputByName(inputName).setDistribution(distribution);

  // update copula if need
  if ((!inputOldStateIsStochastic && distribution.getImplementation()->getClassName()!="Dirac") ||
      (inputOldStateIsStochastic && distribution.getImplementation()->getClassName()=="Dirac"))
    updateCopula();

  notify("inputDistributionChanged");
}


void PhysicalModelImplementation::addInput(const Input & input)
{
  if (hasInputNamed(input.getName()))
    throw InvalidArgumentException(HERE) << "The physical model has already an input named " << input.getName(); 

  inputs_.add(input);
  if (input.isStochastic())
    updateCopula();
  for (UnsignedInteger i=0; i<getOutputs().getSize(); ++i)
    getOutputByName(getOutputs()[i].getName()).setHasBeenComputed(false);

  notify("inputAdded");
  notify("modelInputsChanged");
}


void PhysicalModelImplementation::removeInput(const String & inputName)
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
        for (UnsignedInteger i=0; i<getOutputs().getSize(); ++i)
          getOutputByName(getOutputs()[i].getName()).setHasBeenComputed(false);

        notify("inputRemoved");
        notify("modelInputsChanged");
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
  setCopula(NormalCopula(correlationMatrix));
}


Description PhysicalModelImplementation::getInputNames()
{
  Description inputNames(inputs_.getSize());
  for (UnsignedInteger i=0; i<inputs_.getSize(); ++i)
    inputNames[i] = inputs_[i].getName();
  return inputNames;
}


Description PhysicalModelImplementation::getStochasticInputNames()
{
  Description stochasticInputNames;
  for (UnsignedInteger i=0; i<inputs_.getSize(); ++i)
    if (inputs_[i].isStochastic())
      stochasticInputNames.add(inputs_[i].getName());
  return stochasticInputNames;
}


bool PhysicalModelImplementation::hasInputNamed(const String & inputName)
{
  for (UnsignedInteger i=0; i<inputs_.getSize(); ++i)
    if (inputs_[i].getName() == inputName)
      return true;
  return false;
}


bool PhysicalModelImplementation::hasStochasticInputs()
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
    throw InvalidArgumentException(HERE) << "Two inputs can not have the same name.";

  outputs_ = outputs;
  notify("outputAdded");
  notify("modelOutputsChanged");
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
  notify("modelOutputsChanged");
}


void PhysicalModelImplementation::setOutputDescription(const String & outputName, const String & description)
{
  getOutputByName(outputName).setDescription(description);
  notify("outputDescriptionChanged");
}


void PhysicalModelImplementation::setOutputFormula(const String & outputName, const String & formula)
{
  getOutputByName(outputName).setFormula(formula);
  getOutputByName(outputName).setHasBeenComputed(false);
  notify("outputFormulaChanged");
}


void PhysicalModelImplementation::setOutputValue(const String & outputName, const double & value)
{
  getOutputByName(outputName).setValue(value);
  getOutputByName(outputName).setHasBeenComputed(true);
  notify("outputValueChanged");
}


void PhysicalModelImplementation::addOutput(const Output & output)
{
  if (hasOutputNamed(output.getName()))
    throw InvalidArgumentException(HERE) << "The physical model has already an output named " << output.getName(); 

  outputs_.add(output);
  notify("outputAdded");
  notify("modelOutputsChanged");
}


void PhysicalModelImplementation::removeOutput(const String & outputName)
{
  if (hasOutputNamed(outputName))
  {
    for (UnsignedInteger i=0; i<outputs_.getSize(); ++i)
      if (outputs_[i].getName() == outputName)
      {
        outputs_.erase(outputs_.begin() + i);
        notify("outputRemoved");
        notify("modelOutputsChanged");
        break;
      }
  }
  else
    throw InvalidArgumentException(HERE) << "The given output name " << outputName <<" does not correspond to an output of the physical model\n";
}


Description PhysicalModelImplementation::getOutputNames()
{
  Description outputNames(outputs_.getSize());
  for (UnsignedInteger i=0; i<outputs_.getSize(); ++i)
    outputNames[i] = outputs_[i].getName();
  return outputNames;
}


bool PhysicalModelImplementation::hasOutputNamed(const String & outputName)
{
  for (UnsignedInteger i=0; i<outputs_.getSize(); ++i)
    if (outputs_[i].getName() == outputName)
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
  return RandomVector(getRestrictedFunction(outputNames), getInputRandomVector());
}


NumericalMathFunction PhysicalModelImplementation::getFunction()
{
  throw NotYetImplementedException(HERE) << "In PhysicalModelImplementation::getFunction()";
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


NumericalMathFunction PhysicalModelImplementation::getFunction(const String & outputName)
{
  Description outputNameDescription(1);
  outputNameDescription[0] = outputName;
  return getFunction(outputNameDescription);
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


String PhysicalModelImplementation::dump() const
{
  throw NotYetImplementedException(HERE) << "In PhysicalModelImplementation::getDump()";
}


String PhysicalModelImplementation::dumpProbaModel() const
{
  String result;

  for (UnsignedInteger i=0; i<getInputs().getSize(); ++i)
  {
    if (getInputs()[i].isStochastic())
    {
      Distribution distribution = getInputs()[i].getDistribution();
      String distributionName = distribution.getImplementation()->getClassName();
      String inputName = getInputs()[i].getName();

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
      result += getName() + ".setInputDistribution('" + inputName + "', ";
      result += " dist_" + inputName + ")\n";
    }
  }
  return result;
}


String PhysicalModelImplementation::dumpCopula() const
{
  String result;

  CorrelationMatrix correlationMatrix(getCopula().getCorrelation());

  OSS oss;
  oss << "correlationMatrix = ot.CorrelationMatrix(" << correlationMatrix.getNbRows() << ")\n";

  bool dumpCopula = false;
  for (UnsignedInteger i=0; i<correlationMatrix.getNbRows(); ++i)
    for (UnsignedInteger j=i+1; j<correlationMatrix.getNbRows(); ++j)
      if (correlationMatrix(i, j) != 0.0)
      {
        dumpCopula = true;
        oss << "correlationMatrix[" << i << ", " << j << "] = " << correlationMatrix(i, j) << "\n";
      }

  // if there is no correlation: write nothing
  if (!dumpCopula)
    return result;

  // else:
  result += oss.str();
  result += "copula = ot.NormalCopula(ot.NormalCopula.GetCorrelationFromSpearmanCorrelation(correlationMatrix))\n";
  result += getName() + ".setCopula(copula)\n";

  return result;
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
}