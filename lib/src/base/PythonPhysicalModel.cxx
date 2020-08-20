//                                               -*- C++ -*-
/**
 *  @brief PythonPhysicalModel implements models defined by a Python script
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
#include "persalys/PythonPhysicalModel.hxx"

#include "persalys/PythonScriptEvaluation.hxx"
#include "persalys/BaseTools.hxx"

#include <boost/regex.hpp>
#include <boost/algorithm/string.hpp>

#include <openturns/PersistentObjectFactory.hxx>
#include <openturns/MemoizeFunction.hxx>

using namespace OT;

namespace PERSALYS
{

CLASSNAMEINIT(PythonPhysicalModel)

static Factory<PythonPhysicalModel> Factory_PythonPhysicalModel;

/* Default constructor */
PythonPhysicalModel::PythonPhysicalModel(const String & name)
  : PhysicalModelImplementation(name)
{
  setCode("def _exec(X0):\n    \"\"\"Main function: *must* be present\"\"\"\n    Y0 = X0\n    return Y0");
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
  bool inExecScope = false;
  for (unsigned int i = 0; i < lines.size(); ++ i)
  {
    String line = lines[i];

    // we know that we are outside the _exec scope when the line begins with a character (different from space)
    if (inExecScope && line[0] != ' ' && line.size() != 0)
      inExecScope = false;

    boost::regex defFunction("def[ ]+\\_exec[ ]*\\(([_a-zA-Z0-9, ]*)\\)[ ]*:", boost::regex::extended);
    boost::smatch what;
    if (boost::regex_match(line, what, defFunction))
    {
      inExecScope = true;
      String inputList = what[1];
      std::string::const_iterator start = inputList.begin(), end = inputList.end();
      inputVariables.clear();
      while (boost::regex_search(start, end, what, variable))
      {
        start = what[0].second;
        inputVariables.add(what[1]);
      }
    }

    boost::regex returnOutput("    return[ ]+([_a-zA-Z0-9, ]+)");
    if (inExecScope && boost::regex_match(line, what, returnOutput))
    {
      String outputList = what[1];
      std::string::const_iterator start = outputList.begin(), end = outputList.end();
      outputVariables.clear();
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
  if (!functionCache_.getEvaluation().getImplementation()->isActualImplementation())
  {
    PythonScriptEvaluation evaluation(getInputNames(), getOutputNames(), getCode());
    evaluation.setParallel(isParallel());
    evaluation.setProcessNumber(getProcessNumber());
    functionCache_ = MemoizeFunction(evaluation);
  }
  return functionCache_;
}


void PythonPhysicalModel::setParallel(const Bool flag)
{
  functionCache_ = Function();
  PhysicalModelImplementation::setParallel(flag);
}


void PythonPhysicalModel::resetCallsNumber()
{
  PythonScriptEvaluation * eval = dynamic_cast<PythonScriptEvaluation*>(functionCache_.getEvaluation().getImplementation().get());
  if (eval)
    eval->resetCallsNumber();
}


String PythonPhysicalModel::getHtmlDescription(const bool deterministic) const
{
  OSS oss;

  oss << PhysicalModelImplementation::getHtmlDescription(deterministic);

  oss << "<h3>Outputs</h3><p>";
  oss << "<table style=\"width:100%\" border=\"1\" cellpadding=\"5\">";
  oss << "<tr>";
  oss << "  <th>Name</th>";
  oss << "  <th>Description</th>";
  oss << "</tr>";
  for (UnsignedInteger i = 0; i < getOutputDimension(); ++i)
  {
    oss << "<tr>";
    oss << "  <td>" << getOutputNames()[i] << "</td>";
    const String desc(getOutputs()[i].getDescription());
    oss << "  <td>" << (desc.empty() ? "-" : desc) << "</td>";
    oss << "</tr>";
  }
  oss << "</table></p>";
  oss << "<h3>Python code</h3>";
  oss << "<pre>";
  String code = getCode();
  // replace all "<" by "&lt;"
  size_t position = code.find("<");
  while (position != std::string::npos)
  {
    code.replace(position, 1, "&lt;" );
    position = code.find("<", position + 4);
  }
  oss << code;
  oss << "</pre>";

  return oss;
}


String PythonPhysicalModel::getPythonScript() const
{
  OSS oss;

  for (UnsignedInteger i = 0; i < getInputDimension(); ++ i)
    oss << getInputs()[i].getPythonScript();

  for (UnsignedInteger i = 0; i < getOutputDimension(); ++ i)
    oss << getOutputs()[i].getPythonScript();

  oss << "inputs = " << Parameters::GetOTDescriptionStr(getInputNames(), false) << "\n";
  oss << "outputs = " << Parameters::GetOTDescriptionStr(getOutputNames(), false) << "\n";

  // escape triple quotes, backslashes
  String escaped_code = boost::regex_replace(getCode(), boost::regex("\\\\"), "\\\\\\\\");
  escaped_code = boost::regex_replace(escaped_code, boost::regex("\"\"\""), "\\\\\"\\\\\"\\\\\"");

  oss << "code = \"\"\"\n" + escaped_code + "\"\"\"\n";

  oss << getName() + " = persalys.PythonPhysicalModel('" + getName() + "', inputs, outputs, code)\n";
  if (isParallel())
    oss << getName() + ".setParallel(True)\n";
  if (getProcessNumber() != 1)
    oss << getName() + ".setProcessNumber(" << getProcessNumber() << ")\n";

  oss << PhysicalModelImplementation::getCopulaPythonScript();

  return oss;
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

void PythonPhysicalModel::setInputs(const InputCollection & /*inputs*/)
{
  throw NotYetImplementedException(HERE) << "Use setCode to modify a PythonPhysicalModel";
}


void PythonPhysicalModel::addInput(const Input & /*input*/)
{
  throw NotYetImplementedException(HERE) << "Use setCode to modify a PythonPhysicalModel";
}


void PythonPhysicalModel::removeInput(const String & /*inputName*/)
{
  throw NotYetImplementedException(HERE) << "Use setCode to modify a PythonPhysicalModel";
}

void PythonPhysicalModel::setOutputs(const OutputCollection & /*outputs*/)
{
  throw NotYetImplementedException(HERE) << "Use setCode to modify a PythonPhysicalModel";
}


void PythonPhysicalModel::addOutput(const Output & /*output*/)
{
  throw NotYetImplementedException(HERE) << "Use setCode to modify a PythonPhysicalModel";
}


void PythonPhysicalModel::removeOutput(const String & /*outputName*/)
{
  throw NotYetImplementedException(HERE) << "Use setCode to modify a PythonPhysicalModel";
}

void PythonPhysicalModel::setEvalTime(const Scalar& evalTime)
{
  PhysicalModelImplementation::setEvalTime(evalTime);
}

Scalar PythonPhysicalModel::getEvalTime() const
{
  return PhysicalModelImplementation::getEvalTime();
}

}
