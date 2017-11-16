//                                               -*- C++ -*-
/**
 *  @brief PythonPhysicalModel implements models defined by a Python script
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
#include "otgui/PythonPhysicalModel.hxx"
#include "otgui/PythonScriptEvaluation.hxx"

#include <boost/regex.hpp>
#include <boost/algorithm/string.hpp>

#include <openturns/PersistentObjectFactory.hxx>

using namespace OT;

namespace OTGUI
{

CLASSNAMEINIT(PythonPhysicalModel)

static Factory<PythonPhysicalModel> Factory_PythonPhysicalModel;

/* Default constructor */
PythonPhysicalModel::PythonPhysicalModel(const String & name)
  : PhysicalModelImplementation(name)
{
  setCode("def _exec(X0):\n    Y0 = X0\n    return Y0");
}


/* Constructor with parameters */
PythonPhysicalModel::PythonPhysicalModel(const String & name,
    const InputCollection & inputs,
    const OutputCollection & outputs,
    const String & code)
  : PhysicalModelImplementation(name, inputs, outputs)
{
  setCode(code);
}


/* Virtual constructor */
PythonPhysicalModel* PythonPhysicalModel::clone() const
{
  return new PythonPhysicalModel(*this);
}


void PythonPhysicalModel::setCode(const String & code)
{

  std::vector<std::string> lines;
  boost::split(lines, code, boost::is_any_of("\n"));

  Description inputVariables;
  Description outputVariables;

  boost::regex variable("([_a-zA-Z][_a-zA-Z0-9]*)");
  for (unsigned int i = 0; i < lines.size(); ++ i)
  {
    String line = lines[i];
    boost::regex defFunction("def[ ]+\\_exec[ ]*\\(([_a-zA-Z0-9, ]+)\\)[ ]*:", boost::regex::extended);
    boost::smatch what;
    if (boost::regex_match(line, what, defFunction))
    {
      String inputList = what[1];
      std::string::const_iterator start = inputList.begin(), end = inputList.end();
      while (boost::regex_search(start, end, what, variable))
      {
        start = what[0].second;
        inputVariables.add(what[1]);
      }
    }

    boost::regex returnOutput("    return[ ]+([_a-zA-Z0-9, ]+)");
    if (boost::regex_match(line, what, returnOutput))
    {
      String outputList = what[1];
      std::string::const_iterator start = outputList.begin(), end = outputList.end();
      while (boost::regex_search(start, end, what, variable))
      {
        start = what[0].second;
        outputVariables.add(what[1]);
      }
    }
  }

  InputCollection newInputs(inputVariables.getSize());
  for (unsigned int i = 0; i < inputVariables.getSize(); ++ i)
  {
    const String inputName(inputVariables[i]);
    if (hasInputNamed(inputName))
    {
      newInputs[i] = getInputByName(inputName);
    }
    else
    {
      newInputs[i] = Input(inputName);
    }
  }

  PhysicalModelImplementation::setInputs(newInputs);

  OutputCollection newOutputs(outputVariables.getSize());
  for (unsigned int i = 0; i < outputVariables.getSize(); ++ i)
  {
    const String outputName(outputVariables[i]);
    if (hasOutputNamed(outputName))
    {
      newOutputs[i] = getOutputByName(outputName);
    }
    else
    {
      newOutputs[i] = Output(outputName);
    }
  }

  PhysicalModelImplementation::setOutputs(newOutputs);

  code_ = code;
  functionCache_ = Function();
  notify("codeChanged");
}


String PythonPhysicalModel::getCode() const
{
  return code_;
}


Function PythonPhysicalModel::generateFunction(const Description &) const
{
  if (!functionCache_.getEvaluation()->isActualImplementation())
  {
    functionCache_ = Function(PythonScriptEvaluation(getInputDimension(), getOutputDimension(), getCode()));
    functionCache_.enableCache();
  }
  return functionCache_;
}


String PythonPhysicalModel::getPythonScript() const
{
  String result;

  for (UnsignedInteger i = 0; i < getInputDimension(); ++ i)
    result += getInputs()[i].getPythonScript();

  for (UnsignedInteger i = 0; i < getOutputDimension(); ++ i)
    result += getOutputs()[i].getPythonScript();

  result += "inputCollection = []\n";
  for (UnsignedInteger i = 0; i < getInputDimension(); ++ i)
  {
    result += "inputCollection.append(" + getInputs()[i].getName() + ")\n";
  }

  result += "outputCollection = []\n";
  for (UnsignedInteger i = 0; i < getOutputDimension(); ++ i)
  {
    result += "outputCollection.append(" + getOutputs()[i].getName() + ")\n";
  }

  // escape quotes, eols
  String escaped_code = boost::regex_replace(getCode(), boost::regex("'"), "\\\\'");
  escaped_code = boost::regex_replace(escaped_code, boost::regex("\n"), "\\\\n");
  result += "code='" + escaped_code + "'\n";

  result += getName() + " = otguibase.PythonPhysicalModel('" + getName() + "', inputCollection, outputCollection, code)\n";

  result += PhysicalModelImplementation::getCopulaPythonScript();

  return result;
}


/** String converter */
String PythonPhysicalModel::__repr__() const
{
  OSS oss;
  oss << PhysicalModelImplementation::__repr__()
      << " code=" << getCode();
  return oss;
}


/* Method save() stores the object through the StorageManager */
void PythonPhysicalModel::save(Advocate & adv) const
{
  PhysicalModelImplementation::save(adv);
  adv.saveAttribute("code_", code_);
}


/* Method load() reloads the object from the StorageManager */
void PythonPhysicalModel::load(Advocate & adv)
{
  PhysicalModelImplementation::load(adv);
  adv.loadAttribute("code_", code_);
}

void PythonPhysicalModel::setInputs(const InputCollection & inputs)
{
  throw NotYetImplementedException(HERE) << "Use setCode to modify a PythonPhysicalModel";
}


void PythonPhysicalModel::addInput(const Input & input)
{
  throw NotYetImplementedException(HERE) << "Use setCode to modify a PythonPhysicalModel";
}


void PythonPhysicalModel::removeInput(const String & inputName)
{
  throw NotYetImplementedException(HERE) << "Use setCode to modify a PythonPhysicalModel";
}

void PythonPhysicalModel::setOutputs(const OutputCollection & outputs)
{
  throw NotYetImplementedException(HERE) << "Use setCode to modify a PythonPhysicalModel";
}


void PythonPhysicalModel::addOutput(const Output & output)
{
  throw NotYetImplementedException(HERE) << "Use setCode to modify a PythonPhysicalModel";
}


void PythonPhysicalModel::removeOutput(const String & outputName)
{
  throw NotYetImplementedException(HERE) << "Use setCode to modify a PythonPhysicalModel";
}

}
