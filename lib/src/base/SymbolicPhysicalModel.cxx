//                                               -*- C++ -*-
/**
 *  @brief Defines physical models thanks analytical formulas
 *
 *  Copyright 2015-2018 EDF-Phimeca
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
#include "otgui/SymbolicPhysicalModel.hxx"

#include "otgui/OTTools.hxx"

#include <openturns/PersistentObjectFactory.hxx>

using namespace OT;

namespace OTGUI
{

CLASSNAMEINIT(SymbolicPhysicalModel)

static Factory<SymbolicPhysicalModel> Factory_SymbolicPhysicalModel;

/* Default constructor */
SymbolicPhysicalModel::SymbolicPhysicalModel(const String& name)
  : PhysicalModelImplementation(name)
  , formulaForEachOutput_()
{
}


/* Constructor with parameters */
SymbolicPhysicalModel::SymbolicPhysicalModel(const String& name,
    const InputCollection& inputs,
    const OutputCollection& outputs,
    const Description& formulas)
  : PhysicalModelImplementation(name, inputs, outputs)
  , formulaForEachOutput_()
{
  setFormulas(formulas);
}


/* Virtual constructor */
SymbolicPhysicalModel* SymbolicPhysicalModel::clone() const
{
  return new SymbolicPhysicalModel(*this);
}


void SymbolicPhysicalModel::addOutput(const Output& output)
{
  if (hasOutputNamed(output.getName()))
    throw InvalidArgumentException(HERE) << "The physical model already contains an output named " << output.getName();

  formulaForEachOutput_[output.getName()] = "";

  PhysicalModelImplementation::addOutput(output);
}


void SymbolicPhysicalModel::removeOutput(const String& outputName)
{
  PhysicalModelImplementation::removeOutput(outputName);
  formulaForEachOutput_.erase(outputName);
}


void SymbolicPhysicalModel::setOutputs(const OutputCollection& outputs)
{
  std::set<String> outputNames;
  for (UnsignedInteger i = 0; i < outputs.getSize(); ++i)
    outputNames.insert(outputs[i].getName());
  if (outputNames.size() != outputs.getSize())
    throw InvalidArgumentException(HERE) << "Two outputs can not have the same name.";

  for (UnsignedInteger i = 0; i < outputs.getSize(); ++i)
    formulaForEachOutput_[outputs[i].getName()] = "";

  PhysicalModelImplementation::setOutputs(outputs);
}


void SymbolicPhysicalModel::setOutputName(const String& outputName, const String& newName)
{
  if (outputName == newName)
    return;

  std::set<String> outputNames;
  outputNames.insert(newName);
  for (UnsignedInteger i = 0; i < getOutputDimension(); ++i)
    if (getOutputNames()[i] != outputName)
      outputNames.insert(getOutputNames()[i]);
  if (outputNames.size() != getOutputDimension())
    throw InvalidArgumentException(HERE) << "The proposed name " << newName << " is not valid. Two outputs can not have the same name.";

  const String outputFormula = formulaForEachOutput_[outputName];
  formulaForEachOutput_.erase(outputName);
  formulaForEachOutput_[newName] = outputFormula;

  PhysicalModelImplementation::setOutputName(outputName, newName);
}


Description SymbolicPhysicalModel::getFormulas() const
{
  Description formulas;
  for (UnsignedInteger i = 0; i < getOutputDimension(); ++i)
  {
    std::map<String, String>::const_iterator it(formulaForEachOutput_.find(getOutputs()[i].getName()));
    if (it != formulaForEachOutput_.end())
      formulas.add(it->second);
  }
  return formulas;
}


void SymbolicPhysicalModel::setFormulas(const Description& formulas)
{
  if (formulas.getSize() != getOutputDimension())
    throw InvalidArgumentException(HERE) << "The list of formulas must have the same dimension as the list of outputs";

  formulaForEachOutput_.clear();
  for (UnsignedInteger i = 0; i < getOutputDimension(); ++i)
    formulaForEachOutput_[getOutputs()[i].getName()] = formulas[i];

  notify("outputFormulaChanged");
}


String SymbolicPhysicalModel::getFormula(const String& outputName) const
{
  if (!getOutputNames().contains(outputName))
    throw InvalidArgumentException(HERE) << "The model does not contain an output named " << outputName;

  std::map<String, String>::const_iterator it(formulaForEachOutput_.find(outputName));
  if (it == formulaForEachOutput_.end())
    throw InvalidArgumentException(HERE) << "Error: no formula set for the output " << outputName;

  return it->second;
}


void SymbolicPhysicalModel::setFormula(const String& outputName, const String& formula)
{
  if (!getOutputNames().contains(outputName))
    throw InvalidArgumentException(HERE) << "The model does not contain an output named " << outputName;

  formulaForEachOutput_[outputName] = formula;

  notify("outputFormulaChanged");
}

Function SymbolicPhysicalModel::generateFunction(const Description & outputNames) const
{
  Description formulas;
  for (UnsignedInteger i = 0; i < outputNames.getSize(); ++ i)
  {
    formulas.add(getFormula(outputNames[i]));
  }
  return Function(getInputNames(), outputNames, formulas);
}


String SymbolicPhysicalModel::getHtmlDescription(const bool deterministic) const
{
  OSS oss;
  oss << PhysicalModelImplementation::getHtmlDescription(deterministic);
  oss << "<h3>Outputs</h3><p>";
  oss << "<table style=\"width:100%\" border=\"1\" cellpadding=\"5\">";
  oss << "<tr>";
  oss << "  <th>Name</th>";
  oss << "  <th>Description</th>";
  oss << "  <th>Formula</th>";
  oss << "</tr>";
  for (UnsignedInteger i = 0; i < getOutputDimension(); ++i)
  {
    oss << "<tr>";
    oss << "  <td>" << getOutputNames()[i] << "</td>";
    const String desc(getOutputs()[i].getDescription());
    oss << "  <td>" << (desc.empty() ? "-" : desc) << "</td>";
    oss << "  <td>" << getFormulas()[i] << "</td>";
    oss << "</tr>";
  }
  oss << "</table></p>";

  return oss;
}


String SymbolicPhysicalModel::getPythonScript() const
{
  OSS oss;

  for (UnsignedInteger i = 0; i < getInputDimension(); ++i)
    oss << getInputs()[i].getPythonScript();

  for (UnsignedInteger i = 0; i < getOutputDimension(); ++i)
    oss << getOutputs()[i].getPythonScript();

  oss << "inputs = [";
  for (UnsignedInteger i = 0; i < getInputDimension(); ++i)
  {
    oss << getInputs()[i].getName();
    if (i < getInputDimension() - 1)
      oss << ", ";
  }
  oss << "]\n";

  oss << "outputs = [";
  for (UnsignedInteger i = 0; i < getOutputDimension(); ++i)
  {
    oss << getOutputs()[i].getName();
    if (i < getOutputDimension() - 1)
      oss << ", ";
  }
  oss << "]\n";

  oss << "formulas = " << Parameters::GetOTDescriptionStr(getFormulas()) << "\n";

  oss << getName() + " = otguibase.SymbolicPhysicalModel('" + getName() + "', inputs, outputs, formulas)\n";

  oss << PhysicalModelImplementation::getCopulaPythonScript();

  return oss;
}


/* String converter */
String SymbolicPhysicalModel::__repr__() const
{
  OSS oss;
  oss << PhysicalModelImplementation::__repr__()
      << " formulas=" << getFormulas();
  return oss;
}


/* Method save() stores the object through the StorageManager */
void SymbolicPhysicalModel::save(Advocate & adv) const
{
  PhysicalModelImplementation::save(adv);
  adv.saveAttribute("formulas_", getFormulas());
}


/* Method load() reloads the object from the StorageManager */
void SymbolicPhysicalModel::load(Advocate & adv)
{
  PhysicalModelImplementation::load(adv);
  Description formulas;
  adv.loadAttribute("formulas_", formulas);
  setFormulas(formulas);
}
}
