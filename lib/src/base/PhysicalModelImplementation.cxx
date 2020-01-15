//                                               -*- C++ -*-
/**
 *  @brief Abstract top-level class for all physical models
 *
 *  Copyright 2015-2020 EDF-Phimeca
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
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
#include "persalys/PhysicalModelImplementation.hxx"

#include "persalys/BaseTools.hxx"

#include <openturns/NonCenteredFiniteDifferenceGradient.hxx>
#include <openturns/CenteredFiniteDifferenceHessian.hxx>
#include <openturns/NormalCopula.hxx>
#include <openturns/TruncatedDistribution.hxx>
#include <openturns/PersistentObjectFactory.hxx>
#include <openturns/ParametricFunction.hxx>
#include <openturns/IndependentCopula.hxx>
#include <openturns/CompositeRandomVector.hxx>
#include <openturns/ParametricPointToFieldFunction.hxx>

using namespace OT;

namespace PERSALYS
{

static Factory<PersistentCollection<Input> > Factory_PersistentCollectionInput;
static Factory<PersistentCollection<Output> > Factory_PersistentCollectionOutput;

/* Constructor with parameters */
PhysicalModelImplementation::PhysicalModelImplementation(const String & name)
  : PersistentObject()
  , Observable()
  , hasMesh_(false)
  , inputs_()
  , outputs_()
  , composedCopula_()
  , finiteDifferenceSteps_()
  , isParallel_(false)
{
  setName(name);
  // by default a ComposedCopula contain an IndependentCopula with the description ("X0","X1")
  composedCopula_.setDescription(Description(2, ""));
}


/* Constructor with parameters */
PhysicalModelImplementation::PhysicalModelImplementation(const String & name,
    const InputCollection & inputs,
    const OutputCollection & outputs)
  : PersistentObject()
  , Observable()
  , hasMesh_(false)
  , inputs_()
  , outputs_()
  , composedCopula_()
  , finiteDifferenceSteps_()
  , isParallel_(false)
{
  setName(name);

  setInputs(inputs);
  setOutputs(outputs);
}


/* Virtual constructor */
PhysicalModelImplementation* PhysicalModelImplementation::clone() const
{
  return new PhysicalModelImplementation(*this);
}


void PhysicalModelImplementation::setName(const String & name)
{
  PersistentObject::setName(name);
  notify("nameChanged");
}


void PhysicalModelImplementation::removeAllObservers()
{
  notifyAndRemove("ProbabilisticModelItem");
  notifyAndRemove("PhysicalModelDefinitionItem");
  notifyAndRemove("MeshItem");
  notifyAndRemove("PhysicalModelDiagramItem");
  notifyAndRemove("Study");
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


UnsignedInteger PhysicalModelImplementation::getInputDimension() const
{
  return inputs_.getSize();
}


InputCollection PhysicalModelImplementation::getInputs() const
{
  return inputs_;
}


Input & PhysicalModelImplementation::getInputByName(const String & inputName)
{
  for (UnsignedInteger i = 0; i < inputs_.getSize(); ++i)
    if (inputs_[i].getName() == inputName)
      return inputs_[i];
  throw InvalidArgumentException(HERE) << "The given input name " << inputName << " does not correspond to an input of the physical model.\n";
}


Input PhysicalModelImplementation::getInputByName(const String & inputName) const
{
  for (UnsignedInteger i = 0; i < inputs_.getSize(); ++i)
    if (inputs_[i].getName() == inputName)
      return inputs_[i];
  throw InvalidArgumentException(HERE) << "The given input name " << inputName << " does not correspond to an input of the physical model.\n";
}


void PhysicalModelImplementation::setInputs(const InputCollection & inputs)
{
  std::set<String> inputNames;
  for (UnsignedInteger i = 0; i < inputs.getSize(); ++i)
    inputNames.insert(inputs[i].getName());
  if (inputNames.size() != inputs.getSize())
    throw InvalidArgumentException(HERE) << "Two inputs cannot have the same name.";

  inputs_ = inputs;

  // update composedCopula_
  composedCopula_ = ComposedCopula();
  // by default a ComposedCopula contain an IndependentCopula with the description ("X0","X1")
  composedCopula_.setDescription(Description(2, ""));

  inputsChanged();
}


void PhysicalModelImplementation::setInputName(const String & inputName, const String & newName)
{
  if (inputName == newName)
    return;

  if (hasInputNamed(newName))
    throw InvalidArgumentException(HERE) << "The physical model already contains an input named " << newName;

  Input& input = getInputByName(inputName);
  input.setName(newName);

  // update composedCopula_ description
  if (input.isStochastic())
  {
    Collection<Distribution> coll(composedCopula_.getCopulaCollection());
    for (UnsignedInteger i = 0; i < coll.getSize(); ++i)
    {
      Description copulaDescription(coll[i].getDescription());
      for (UnsignedInteger j = 0; j < copulaDescription.getSize(); ++j)
      {
        if (copulaDescription[j] == inputName)
        {
          copulaDescription[j] = newName;
          Distribution copula(coll[i]);
          copula.setDescription(copulaDescription);
          coll[i] = copula;
        }
      }
    }
    composedCopula_.setCopulaCollection(coll);
    notify("copulaChanged");
  }

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
  for (UnsignedInteger i = 0; i < getOutputDimension(); ++i)
    getOutputByName(getOutputs()[i].getName()).setHasBeenComputed(false);
  notify("inputValueChanged");
}


void PhysicalModelImplementation::setInputStochastic(const String & inputName, const bool & stoch)
{
  getInputByName(inputName).setStochastic(stoch);
  // update copula
  updateCopula();
  notify("inputDistributionChanged");
}


void PhysicalModelImplementation::setDistribution(const String & inputName, const Distribution & distribution)
{
  if (distribution.getDimension() > 1)
    throw InvalidArgumentException(HERE) << "A distribution of dimension 1 is requested";

  getInputByName(inputName).setDistribution(distribution);

  // update copula
  updateCopula();

  notify("inputDistributionChanged");
}


void PhysicalModelImplementation::setDistributionParametersType(const String & inputName, const UnsignedInteger & distributionParametersType)
{
  getInputByName(inputName).setDistributionParametersType(distributionParametersType);
}


void PhysicalModelImplementation::addInput(const Input & input)
{
  if (hasInputNamed(input.getName()))
    throw InvalidArgumentException(HERE) << "The physical model already contains an input named " << input.getName();

  inputs_.add(input);

  inputsChanged();
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
  for (UnsignedInteger i = 0; i < inputs_.getSize(); ++i)
  {
    if (inputs_[i].getName() == inputName)
    {
      inputs_.erase(inputs_.begin() + i);
      inputsChanged();
      return;
    }
  }

  throw InvalidArgumentException(HERE) << "The given input name " << inputName << " does not correspond to an input of the physical model.\n";
}


void PhysicalModelImplementation::clearInputs()
{
  inputs_.clear();
  inputsChanged();
}


void PhysicalModelImplementation::inputsChanged()
{
  // reset outputs values
  for (UnsignedInteger i = 0; i < getOutputDimension(); ++i)
    getOutputByName(getOutputs()[i].getName()).setHasBeenComputed(false);

  // update copula
  updateCopula();

  // update finite difference step
  updateFiniteDifferenceSteps();

  notify("inputNumberChanged");
}


void PhysicalModelImplementation::updateCopula()
{
  const Description stochasticInput(getStochasticInputNames());

  Collection<Distribution> coll(composedCopula_.getCopulaCollection());
  std::set<UnsignedInteger> indicesToRemove;

  // remove IndependentCopula
  // get dependent inputs
  Description dependentVar;
  for (UnsignedInteger i = 0; i < coll.getSize(); ++i)
  {
    if (coll[i].getImplementation()->getClassName() == "IndependentCopula")
    {
      indicesToRemove.insert(i);
    }
    else
    {
      bool copulaOk = true;
      for (UnsignedInteger j = 0; j < coll[i].getDescription().getSize(); ++j)
      {
        if (!stochasticInput.contains(coll[i].getDescription()[j]))
        {
          indicesToRemove.insert(i);
          copulaOk = false;
          break;
        }
      }
      if (copulaOk)
        dependentVar.add(coll[i].getDescription());
    }
  }

  // new collection of copulas
  Collection<Distribution> newColl;
  for (UnsignedInteger i = 0; i < coll.getSize(); ++i)
  {
    if (indicesToRemove.find(i) == indicesToRemove.end())
      newColl.add(coll[i]);
  }

  // get independent inputs
  Description independentVar;
  if (dependentVar.getSize() != stochasticInput.getSize())
  {
    for (UnsignedInteger i = 0; i < stochasticInput.getSize(); ++i)
    {
      if (!dependentVar.contains(stochasticInput[i]))
        independentVar.add(stochasticInput[i]);
    }
  }
  // add independent copula
  if (independentVar.getSize())
  {
    IndependentCopula indpCop(independentVar.getSize());
    indpCop.setDescription(independentVar);
    newColl.add(indpCop);
  }

  // update composedCopula_
  if (newColl.getSize())
  {
    composedCopula_.setCopulaCollection(newColl);
  }
  else
  {
    composedCopula_ = ComposedCopula();
    // by default a ComposedCopula contain an IndependentCopula with the description ("X0","X1")
    composedCopula_.setDescription(Description(2, ""));
  }

  notify("copulaChanged");
}


Description PhysicalModelImplementation::getInputNames() const
{
  Description inputNames(inputs_.getSize());
  for (UnsignedInteger i = 0; i < inputs_.getSize(); ++i)
    inputNames[i] = inputs_[i].getName();
  return inputNames;
}


void PhysicalModelImplementation::updateFiniteDifferenceSteps() const
{
  finiteDifferenceSteps_ = Point(getInputDimension());
  for (UnsignedInteger i = 0; i < getInputDimension(); ++i)
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
  for (UnsignedInteger i = 0; i < inputs_.getSize(); ++i)
    if (inputs_[i].isStochastic())
      stochasticInputNames.add(inputs_[i].getName());
  return stochasticInputNames;
}


bool PhysicalModelImplementation::hasInputNamed(const String & inputName) const
{
  for (UnsignedInteger i = 0; i < inputs_.getSize(); ++i)
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
  for (UnsignedInteger i = 0; i < outputs_.getSize(); ++i)
    if (outputs_[i].getName() == outputName)
      return outputs_[i];
  throw InvalidArgumentException(HERE) << "The given output name " << outputName << " does not correspond to an output of the physical model.\n";
}


Output PhysicalModelImplementation::getOutputByName(const String & outputName) const
{
  for (UnsignedInteger i = 0; i < outputs_.getSize(); ++i)
    if (outputs_[i].getName() == outputName)
      return outputs_[i];
  throw InvalidArgumentException(HERE) << "The given output name " << outputName << " does not correspond to an output of the physical model.\n";
}


UnsignedInteger PhysicalModelImplementation::getOutputDimension() const
{
  return outputs_.getSize();
}


OutputCollection PhysicalModelImplementation::getOutputs() const
{
  return outputs_;
}


void PhysicalModelImplementation::setOutputs(const OutputCollection & outputs)
{
  std::set<String> outputNames;
  for (UnsignedInteger i = 0; i < outputs.getSize(); ++i)
    outputNames.insert(outputs[i].getName());
  if (outputNames.size() != outputs.getSize())
    throw InvalidArgumentException(HERE) << "Two outputs cannot have the same name.";

  outputs_ = outputs;
  notify("outputNumberChanged");
}


void PhysicalModelImplementation::setOutputName(const String & outputName, const String & newName)
{
  if (outputName == newName)
    return;

  if (hasOutputNamed(newName))
    throw InvalidArgumentException(HERE) << "The physical model already contains an output named " << newName;

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
    for (UnsignedInteger i = 0; i < outputs_.getSize(); ++i)
      if (outputs_[i].getName() == outputName)
      {
        outputs_.erase(outputs_.begin() + i);
        notify("outputNumberChanged");
        break;
      }
  }
  else
    throw InvalidArgumentException(HERE) << "The given output name " << outputName << " does not correspond to an output of the physical model\n";
}


void PhysicalModelImplementation::clearOutputs()
{
  outputs_.clear();
  notify("outputNumberChanged");
}


Description PhysicalModelImplementation::getOutputNames() const
{
  Description outputNames(outputs_.getSize());
  for (UnsignedInteger i = 0; i < outputs_.getSize(); ++i)
    outputNames[i] = outputs_[i].getName();
  return outputNames;
}


bool PhysicalModelImplementation::hasOutputNamed(const String & outputName) const
{
  for (UnsignedInteger i = 0; i < outputs_.getSize(); ++i)
    if (outputs_[i].getName() == outputName)
      return true;
  return false;
}


Description PhysicalModelImplementation::getSelectedOutputsNames() const
{
  Description selectedOutputsNames;
  for (UnsignedInteger i = 0; i < getOutputDimension(); ++i)
    if (getOutputs()[i].isSelected())
      selectedOutputsNames.add(getOutputs()[i].getName());
  return selectedOutputsNames;
}


Distribution PhysicalModelImplementation::getDistribution() const
{
  Description copulaDescription(composedCopula_.getDescription());
  Indices copulaMarginals;

  ComposedDistribution::DistributionCollection marginals;
  for (UnsignedInteger i = 0; i < inputs_.getSize(); ++i)
  {
    if (inputs_[i].isStochastic())
    {
      marginals.add(inputs_[i].getDistribution());
      const Description::const_iterator it = std::find(copulaDescription.begin(), copulaDescription.end(), inputs_[i].getName());
      copulaMarginals.add(it - copulaDescription.begin());
    }
  }

  // we cannot build a ComposedDistribution with an empty collection
  if (!marginals.getSize())
    return Distribution();

  return ComposedDistribution(marginals, composedCopula_.getMarginal(copulaMarginals));
}


RandomVector PhysicalModelImplementation::getInputRandomVector() const
{
  if (!hasStochasticInputs())
    throw PhysicalModelNotValidException(HERE) << "Can not use getInputRandomVector on a physical model which has no stochastic inputs.";
  return RandomVector(getDistribution());
}


RandomVector PhysicalModelImplementation::getOutputRandomVector(const Description & outputNames) const
{
  return CompositeRandomVector(getRestrictedFunction(outputNames), getInputRandomVector());
}


Function PhysicalModelImplementation::generateFunction(const Description & /*outputNames*/) const
{
  throw NotYetImplementedException(HERE) << "In PhysicalModelImplementation::generateFunction(outputNames)";
}


Function PhysicalModelImplementation::getFunction() const
{
  return getFunction(getOutputNames());
}


Function PhysicalModelImplementation::getFunction(const Description & outputNames) const
{
  if (!getInputDimension())
    throw PhysicalModelNotValidException(HERE) << "The physical model has no inputs.";
  if (!getOutputDimension())
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
  for (UnsignedInteger i = 0; i < outputNames.getSize(); ++i)
    for (UnsignedInteger j = 0; j < getOutputDimension(); ++j)
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
  if (getInputDimension() == getStochasticInputNames().getSize())
    return getFunction();

  return getRestrictedFunction(getOutputNames());
}


Function PhysicalModelImplementation::getRestrictedFunction(const Description& outputNames) const
{
  // search deterministic inputs
  Indices deterministicInputsIndices;
  Point inputsValues;
  for (UnsignedInteger i = 0; i < getInputDimension(); ++i)
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


PointToFieldFunction PhysicalModelImplementation::generatePointToFieldFunction(const Description & /*outputNames*/) const
{
  throw NotYetImplementedException(HERE) << "In PhysicalModelImplementation::generatePointToFieldFunction(Description)";
}


PointToFieldFunction PhysicalModelImplementation::getPointToFieldFunction() const
{
  return getPointToFieldFunction(getOutputNames());
}


PointToFieldFunction PhysicalModelImplementation::getPointToFieldFunction(const Description& outputNames) const
{
  if (!getInputDimension())
    throw PhysicalModelNotValidException(HERE) << "The physical model has no inputs.";
  if (!getOutputDimension())
    throw PhysicalModelNotValidException(HERE) << "The physical model has no outputs.";

  PointToFieldFunction function(generatePointToFieldFunction(outputNames));

  if (function.getOutputDimension() != outputNames.getSize())
    throw PhysicalModelNotValidException(HERE) << "Error when building the function.";

  return function;
}


PointToFieldFunction PhysicalModelImplementation::getRestrictedPointToFieldFunction(const Description& outputNames) const
{
  // search deterministic inputs
  Indices deterministicInputsIndices;
  Point inputsValues;
  for (UnsignedInteger i = 0; i < getInputDimension(); ++i)
  {
    if (!getInputs()[i].isStochastic())
    {
      deterministicInputsIndices.add(i);
      inputsValues.add(getInputs()[i].getValue());
    }
  }

  const PointToFieldFunction function(getPointToFieldFunction(outputNames));

  // if there is no deterministic inputs
  if (!deterministicInputsIndices.getSize())
    return function;

  // if there are deterministic inputs
  const ParametricPointToFieldFunction restricted(function, deterministicInputsIndices, inputsValues);
  return restricted;
}


Distribution PhysicalModelImplementation::getCopula() const
{
  Description copulaDescription(composedCopula_.getDescription());
  Indices copulaMarginals;

  for (UnsignedInteger i = 0; i < inputs_.getSize(); ++i)
  {
    if (inputs_[i].isStochastic())
    {
      const Description::const_iterator it = std::find(copulaDescription.begin(), copulaDescription.end(), inputs_[i].getName());
      copulaMarginals.add(it - copulaDescription.begin());
    }
  }
  return copulaMarginals.getSize() > 0 ? composedCopula_.getMarginal(copulaMarginals) : composedCopula_;
}


Collection<Distribution> PhysicalModelImplementation::getCopulaCollection() const
{
  return composedCopula_.getCopulaCollection();
}


void PhysicalModelImplementation::setCopula(const Description &inputNames, const Distribution &copula)
{
  // - check
  // inputNames and copula must have the same dimension
  if (copula.getDimension() != inputNames.getSize())
    throw InvalidArgumentException(HERE) << "The copula must have a dimension equal to the inputs list.\n";

  // error if the input is not in the model or if it is deterministic
  for (UnsignedInteger i = 0; i < inputNames.getSize(); ++i)
  {
    if (!getInputByName(inputNames[i]).isStochastic())
      throw InvalidArgumentException(HERE) << "The input " << inputNames[i] << " is not stochastic";
  }

  if (!copula.isCopula())
    throw InvalidArgumentException(HERE) << "The provided distribution is not a copula";
 
  Collection<Distribution> coll(composedCopula_.getCopulaCollection());
  std::set<UnsignedInteger> indicesToRemove;

  // remove copula if its description contains an input name of the list
  for (UnsignedInteger i = 0; i < coll.getSize(); ++i)
  {
    if (coll[i].getImplementation()->getClassName() != "IndependentCopula")
    {
      for (UnsignedInteger j = 0; j < inputNames.getSize(); ++j)
      {
        if (coll[i].getDescription().contains(inputNames[j]))
        {
          indicesToRemove.insert(i);
          break;
        }
      }
    }
  }

  // new collection of copulas
  Collection<Distribution> newColl;
  if ((indicesToRemove.size() == 1 && copula.getImplementation()->getClassName() != "IndependentCopula") ||
      indicesToRemove.size() == 0)
  {
    newColl = coll;
  }
  else
  {
    for (UnsignedInteger i = 0; i < coll.getSize(); ++i)
    {
      if (indicesToRemove.find(i) == indicesToRemove.end())
        newColl.add(coll[i]);
    }
  }

  // update copulas collection
  if (copula.getImplementation()->getClassName() != "IndependentCopula")
  {
    Distribution newCopula(copula);
    newCopula.setDescription(inputNames);
    if (indicesToRemove.size() != 1)
    {
      // add copula
      newColl.add(newCopula);
    }
    else
    {
      // replace copula
      // do not : erase copula_i then add newCopula
      // do like this only for the copulas definition window behavior
      newColl[*indicesToRemove.begin()] = newCopula;
    }
  }

  // update composedCopula_
  if (newColl.getSize())
  {
    composedCopula_.setCopulaCollection(newColl);
  }
  else
  {
    composedCopula_ = ComposedCopula();
    // by default a ComposedCopula contain an IndependentCopula with the description ("X0","X1")
    composedCopula_.setDescription(Description(2, ""));
  }
  updateCopula();

  notify("copulaChanged");
}


bool PhysicalModelImplementation::hasMesh() const
{
  return hasMesh_;
}


MeshModel PhysicalModelImplementation::getMeshModel() const
{
  if (!hasMesh_)
    throw NotYetImplementedException(HERE) << "In PhysicalModelImplementation::getMeshModel()";
  return meshModel_;
}


void PhysicalModelImplementation::setMeshModel(const MeshModel& meshModel)
{
  if (!hasMesh_)
    throw NotYetImplementedException(HERE) << "In PhysicalModelImplementation::setMeshModel()";
  meshModel_ = meshModel;
  notify("meshChanged");
}


Bool PhysicalModelImplementation::isParallel() const
{
  return isParallel_;
}


void PhysicalModelImplementation::setParallel(const Bool flag)
{
  isParallel_ = flag;
  notify("parallelizationStatusChanged");
}


bool PhysicalModelImplementation::isValid() const
{
  return getSelectedOutputsNames().getSize() && getInputDimension();
}


String PhysicalModelImplementation::getHtmlDescription(const bool deterministic) const
{
  OSS oss;

  oss << "<h2><center>Physical model</center></h2>";

  if (hasMesh_)
    oss << meshModel_.getHtmlDescription();

  if (deterministic)
  {
    // Inputs
    oss << "<h3>Inputs</h3><p>";
    oss << "<table style=\"width:100%\" border=\"1\" cellpadding=\"5\">";
    oss << "<tr>";
    oss << "  <th>Name</th>";
    oss << "  <th>Description</th>";
    oss << "  <th>Value</th>";
    oss << "</tr>";
    for (UnsignedInteger i = 0; i < getInputDimension(); ++i)
    {
      oss << "<tr>";
      oss << "  <td>" << getInputNames()[i] << "</td>";
      const String desc(getInputs()[i].getDescription());
      oss << "  <td>" << (desc.empty() ? "-" : desc) << "</td>";
      oss << "  <td>" << getInputs()[i].getValue() << "</td>";
      oss << "</tr>";
    }

    oss << "</table></p>";
  }
  else
  {
    // Deterministic inputs
    if (getInputDimension() > getStochasticInputNames().getSize())
    {
      oss << "<h3>Deterministic inputs</h3><p>";
      oss << "<table style=\"width:100%\" border=\"1\" cellpadding=\"5\">";
      oss << "<tr>";
      oss << "  <th>Name</th>";
      oss << "  <th>Description</th>";
      oss << "  <th>Value</th>";
      oss << "</tr>";
      for (UnsignedInteger i = 0; i < getInputDimension(); ++i)
      {
        if (!getInputs()[i].isStochastic())
        {
          oss << "<tr>";
          oss << "  <td>" << getInputNames()[i] << "</td>";
          const String desc(getInputs()[i].getDescription());
          oss << "  <td>" << (desc.empty() ? "-" : desc) << "</td>";
          oss << "  <td>" << getInputs()[i].getValue() << "</td>";
          oss << "</tr>";
        }
      }

      oss << "</table></p>";
    }

    // Stochastic inputs
    if (hasStochasticInputs())
    {
      oss << "<h3>Stochastic inputs</h3><p>";
      oss << "<table style=\"width:100%\" border=\"1\" cellpadding=\"5\">";
      oss << "<tr>";
      oss << "  <th>Name</th>";
      oss << "  <th>Description</th>";
      oss << "  <th>Distribution</th>";
      oss << "</tr>";
      for (UnsignedInteger i = 0; i < getInputDimension(); ++i)
      {
        if (getInputs()[i].isStochastic())
        {
          oss << "<tr>";
          oss << "  <td>" << getInputNames()[i] << "</td>";
          const String desc(getInputs()[i].getDescription());
          oss << "  <td>" << (desc.empty() ? "-" : desc) << "</td>";
          oss << "  <td>" << getInputs()[i].getDistribution().__str__() << "</td>";
          oss << "</tr>";
        }
      }
      oss << "</table></p>";
    }
    // copula
    if (!getCopula().hasIndependentCopula())
    {
      oss << "<h3>Dependence</h3><p>";
      oss << "<table style=\"width:100%\" border=\"1\" cellpadding=\"5\">";
      oss << "<tr>";
      oss << "  <th>Group of variables</th>";
      oss << "  <th>Copula</th>";
      oss << "</tr>";
      for (UnsignedInteger i = 0; i < getCopulaCollection().getSize(); ++i)
      {
        if (!getCopulaCollection()[i].hasIndependentCopula())
        {
          oss << "<tr>";
          oss << "  <td>" << getCopulaCollection()[i].getDescription() << "</td>";
          oss << "  <td>" << getCopulaCollection()[i].__str__() << "</td>";
          oss << "</tr>";
        }
      }
      oss << "</table></p>";
    }
  }

  return oss;
}


String PhysicalModelImplementation::getPythonScript() const
{
  throw NotYetImplementedException(HERE) << "In PhysicalModelImplementation::getPythonScript()";
}


String PhysicalModelImplementation::getProbaModelPythonScript() const
{
  String result;

  for (UnsignedInteger i = 0; i < getInputDimension(); ++i)
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
        for (UnsignedInteger j = 0; j < parameters.getSize(); ++ j)
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
        for (UnsignedInteger j = 0; j < parameters.getSize(); ++ j)
        {
          oss << parameters[j];
          if (j < parameters.getSize() - 1)
            oss << ", ";
        }
        oss << ")\n";
        oss << "dist_" << inputPythonName << " = ot." << distributionName << "(";
        oss << "dist_" << inputPythonName << ", ";

        const Interval bounds(truncatedDistribution.getBounds());
        if (!(bounds.getFiniteLowerBound()[0] && bounds.getFiniteUpperBound()[0])) // one side truncation ?
        {
          if (bounds.getFiniteLowerBound()[0])    //lower bound truncation
            oss << bounds.getLowerBound()[0] << ")\n";
          else
            oss << bounds.getUpperBound()[0] << ", ot.TruncatedDistribution.UPPER)\n";
        }
        else  // both sides truncation
          oss << "ot.Interval(" << bounds.getLowerBound()[0] << ", " << bounds.getUpperBound()[0] << "))\n";
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
  OSS oss;
  oss.setPrecision(12);

  for (UnsignedInteger i = 0; i < composedCopula_.getCopulaCollection().getSize(); ++i)
  {
    Distribution composedCopula_i = composedCopula_.getCopulaCollection()[i];

    if (composedCopula_i.getImplementation()->getClassName() == "NormalCopula")
    {
      oss << Parameters::GetOTNormalCopulaStr(composedCopula_i);
      oss << getName() << ".setCopula(" << Parameters::GetOTDescriptionStr(composedCopula_i.getDescription()) << ", copula)\n";
    }
    else
    {
      if (composedCopula_i.getImplementation()->getClassName() != "IndependentCopula")
      {
        oss << getName() << ".setCopula(" << Parameters::GetOTDescriptionStr(composedCopula_i.getDescription()) << ", ";
        oss << "ot." << composedCopula_i.getImplementation()->getClassName() << "(" << composedCopula_i.getParameter()[0] << "))\n";
      }
    }
  }

  return oss;
}


/* String converter */
String PhysicalModelImplementation::__repr__() const
{
  OSS oss;
  oss << "class=" << getClassName()
      << " inputs=" << getInputs()
      << " outputs=" << getOutputs()
      << " copula=" << getCopula()
      << " isParallel=" << isParallel();
  if (hasMesh())
    oss << meshModel_.__repr__();
  return oss;
}


/* Method save() stores the object through the StorageManager */
void PhysicalModelImplementation::save(Advocate & adv) const
{
  PersistentObject::save(adv);
  adv.saveAttribute("hasMesh_", hasMesh_);
  adv.saveAttribute("inputs_", inputs_);
  adv.saveAttribute("outputs_", outputs_);
  adv.saveAttribute("composedCopula_", composedCopula_);
  adv.saveAttribute("meshModel_", meshModel_);
  adv.saveAttribute("isParallel_", isParallel_);
}


/* Method load() reloads the object from the StorageManager */
void PhysicalModelImplementation::load(Advocate & adv)
{
  PersistentObject::load(adv);
  adv.loadAttribute("hasMesh_", hasMesh_);
  adv.loadAttribute("inputs_", inputs_);
  adv.loadAttribute("outputs_", outputs_);
  adv.loadAttribute("composedCopula_", composedCopula_);
  adv.loadAttribute("meshModel_", meshModel_);
  adv.loadAttribute("isParallel_", isParallel_);
  updateCopula();
}


/* Nothing to do at this level. Overloaded in child classes if needed.*/
void PhysicalModelImplementation::acceptLaunchParameters(LaunchParametersVisitor* /*visitor*/)
{
}

}
