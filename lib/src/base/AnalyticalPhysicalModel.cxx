//                                               -*- C++ -*-
/**
 *  @brief Defines physical models thanks analytical formulas
 *
 *  Copyright 2015-2016 EDF-Phimeca
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
#include "otgui/AnalyticalPhysicalModel.hxx"

#include "openturns/PersistentObjectFactory.hxx"

using namespace OT;

namespace OTGUI {

CLASSNAMEINIT(AnalyticalPhysicalModel);

static Factory<AnalyticalPhysicalModel> RegisteredFactory;

/* Default constructor */
AnalyticalPhysicalModel::AnalyticalPhysicalModel(const String& name)
  : PhysicalModelImplementation(name)
  , formulaForEachOutput_()
{
}


/* Constructor with parameters */
AnalyticalPhysicalModel::AnalyticalPhysicalModel(const String& name,
                                                 const InputCollection& inputs,
                                                 const OutputCollection& outputs,
                                                 const Description& formulas)
  : PhysicalModelImplementation(name, inputs, outputs)
  , formulaForEachOutput_()
{
  setFormulas(formulas);
}


/* Virtual constructor */
AnalyticalPhysicalModel* AnalyticalPhysicalModel::clone() const
{
  return new AnalyticalPhysicalModel(*this);
}


void AnalyticalPhysicalModel::addOutput(const Output& output)
{
  if (hasOutputNamed(output.getName()))
    throw InvalidArgumentException(HERE) << "The physical model already contains an output named " << output.getName();

  formulaForEachOutput_[output.getName()] = "";

  PhysicalModelImplementation::addOutput(output);
}


void AnalyticalPhysicalModel::removeOutput(const String& outputName)
{
  PhysicalModelImplementation::removeOutput(outputName);
  formulaForEachOutput_.erase(outputName);
}


void AnalyticalPhysicalModel::setOutputs(const OutputCollection& outputs)
{
  std::set<String> outputNames;
  for (UnsignedInteger i=0; i<outputs.getSize(); ++i)
    outputNames.insert(outputs[i].getName());
  if (outputNames.size() != outputs.getSize())
    throw InvalidArgumentException(HERE) << "Two outputs can not have the same name.";

  for (UnsignedInteger i=0; i<outputs.getSize(); ++i)
    formulaForEachOutput_[outputs[i].getName()] = "";

  PhysicalModelImplementation::setOutputs(outputs);
}


void AnalyticalPhysicalModel::setOutputName(const String& outputName, const String& newName)
{
  if (outputName == newName)
    return;

  std::set<String> outputNames;
  outputNames.insert(newName);
  for (UnsignedInteger i=0; i<getOutputNames().getSize(); ++i)
    if (getOutputNames()[i] != outputName)
      outputNames.insert(getOutputNames()[i]);
  if (outputNames.size() != getOutputNames().getSize())
    throw InvalidArgumentException(HERE) << "The proposed name " << newName << " is not valid. Two outputs can not have the same name.";

  const String outputFormula = formulaForEachOutput_[outputName];
  formulaForEachOutput_.erase(outputName);
  formulaForEachOutput_[newName] = outputFormula;

  PhysicalModelImplementation::setOutputName(outputName, newName);
}


Description AnalyticalPhysicalModel::getFormulas() const
{
  Description formulas;
  for (UnsignedInteger i=0; i<getOutputs().getSize(); ++i)
  {
    std::map<String, String>::const_iterator it(formulaForEachOutput_.find(getOutputs()[i].getName()));
    if (it != formulaForEachOutput_.end())
      formulas.add(it->second);
  }
  return formulas;
}


void AnalyticalPhysicalModel::setFormulas(const Description& formulas)
{
  if (formulas.getSize() != getOutputs().getSize())
    throw InvalidArgumentException(HERE) << "The list of formulas must have the same dimension as the list of outputs";

  formulaForEachOutput_.clear();
  for (UnsignedInteger i=0; i<getOutputs().getSize(); ++i)
    formulaForEachOutput_[getOutputs()[i].getName()] = formulas[i];

  notify("outputFormulaChanged");
}


String AnalyticalPhysicalModel::getFormula(const String& outputName) const
{
  if (!getOutputNames().contains(outputName))
    throw InvalidArgumentException(HERE) << "The model does not contain an output named " << outputName;

  std::map<String, String>::const_iterator it(formulaForEachOutput_.find(outputName));
  if (it == formulaForEachOutput_.end())
    throw InvalidArgumentException(HERE) << "Error: no formula set for the output " << outputName;

  return it->second;
}


void AnalyticalPhysicalModel::setFormula(const String& outputName, const String& formula)
{
  if (!getOutputNames().contains(outputName))
    throw InvalidArgumentException(HERE) << "The model does not contain an output named " << outputName;

  formulaForEachOutput_[outputName] = formula;

  notify("outputFormulaChanged");
}


NumericalMathFunction AnalyticalPhysicalModel::getFunction() const
{
  if (!getInputs().getSize())
    throw PhysicalModelNotValidException(HERE) << "The physical model has no inputs.";
  if (!getOutputs().getSize())
    throw PhysicalModelNotValidException(HERE) << "The physical model has no outputs.";

  return NumericalMathFunction(getInputNames(), getOutputNames(), getFormulas());
}


String AnalyticalPhysicalModel::getPythonScript() const
{
  OSS oss;

  for (UnsignedInteger i=0; i<getInputs().getSize(); ++i)
    oss << getInputs()[i].getPythonScript();

  for (UnsignedInteger i=0; i<getOutputs().getSize(); ++i)
    oss << getOutputs()[i].getPythonScript();

  oss << "inputs = [";
  for (UnsignedInteger i=0; i<getInputs().getSize(); ++i)
  {
    oss << getInputs()[i].getName();
    if (i < getInputs().getSize()-1)
      oss << ", ";
  }
  oss << "]\n";

  oss << "outputs = [";
  for (UnsignedInteger i=0; i<getOutputs().getSize(); ++i)
  {
    oss << getOutputs()[i].getName();
    if (i < getOutputs().getSize()-1)
      oss << ", ";
  }
  oss << "]\n";

  oss << "formulas = [";
  for (UnsignedInteger i=0; i<getOutputs().getSize(); ++i)
  {
    oss << "'" << getFormula(getOutputs()[i].getName()) << "'";
    if (i < getOutputs().getSize()-1)
      oss << ", ";
  }
  oss << "]\n";

  oss << getName()+ " = otguibase.AnalyticalPhysicalModel('" + getName() + "', inputs, outputs, formulas)\n";

  oss << PhysicalModelImplementation::getCopulaPythonScript();

  return oss;
}


/* String converter */
String AnalyticalPhysicalModel::__repr__() const
{
  OSS oss;
  oss << PhysicalModelImplementation::__repr__()
      << " formulas=" << getFormulas();
  return oss;
}


/* Method save() stores the object through the StorageManager */
void AnalyticalPhysicalModel::save(Advocate & adv) const
{
  PhysicalModelImplementation::save(adv);
  adv.saveAttribute("formulas_", getFormulas());
}


/* Method load() reloads the object from the StorageManager */
void AnalyticalPhysicalModel::load(Advocate & adv)
{
  PhysicalModelImplementation::load(adv);
  Description formulas;
  adv.loadAttribute("formulas_", formulas);
  setFormulas(formulas);
}
}