//                                               -*- C++ -*-
/**
 *  @brief PythonPhysicalModel implements models defined by a Python script
 *
 *  Copyright 2015-2026 EDF-Phimeca
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

#include <openturns/PersistentObjectFactory.hxx>
#include <openturns/MemoizeFunction.hxx>

#if defined(__GNUC__) && (__GNUC__ < 16)
// we cannot use std::regex on long strings yet
// xref https://gcc.gnu.org/bugzilla/show_bug.cgi?id=86164
#include <boost/regex.hpp>
using regex = boost::regex;
using smatch = boost::smatch;
#else
#include <regex>
using regex = std::regex;
using smatch = std::smatch;
#endif

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
  // split lines
  std::vector<std::string> lines;
  {
    std::stringstream stream(code);
    std::string line;
    while (std::getline(stream, line))
      lines.push_back(line);
  }

  Description inputVariables;
  Description outputVariables;

  regex variable(R"(([_a-zA-Z]\w*))");
  bool inExecScope = false;
  for (const String & line : lines)
  {
    // skip empty lines
    if (line.empty())
      continue;
    // we know that we are outside the _exec scope when the line begins with a character (different from space)
    if (inExecScope && line[0] != ' ')
      inExecScope = false;

    regex defFunction(R"(def\s+_exec\s*\(([\w,\s]*)\)\s*:)");
    smatch what;
    if (regex_match(line, what, defFunction))
    {
      inExecScope = true;
      const String inputList = what[1];
      std::string::const_iterator start = inputList.begin();
      std::string::const_iterator end = inputList.end();
      inputVariables.clear();
      while (regex_search(start, end, what, variable))
      {
        start = what[0].second;
        inputVariables.add(what[1]);
      }
      const UnsignedInteger commaNumber = std::count(inputList.begin(), inputList.end(), ',');
      if (commaNumber + 1 != inputVariables.getSize())
        inputVariables.clear();
    }

    // Allow 2-spaces indent as well to match YACS behavior
    regex returnOutput(R"(  (?:  )?return\s+([\w,\s]*)(#.*)?)");
    if (inExecScope && regex_match(line, what, returnOutput))
    {
      const String outputList = what[1];
      std::string::const_iterator start = outputList.begin();
      std::string::const_iterator end = outputList.end();
      outputVariables.clear();
      while (regex_search(start, end, what, variable))
      {
        start = what[0].second;
        outputVariables.add(what[1]);
      }
      const UnsignedInteger commaNumber = std::count(outputList.begin(), outputList.end(), ',');
      if (commaNumber + 1 != outputVariables.getSize())
        outputVariables.clear();
      break; // we found the return statement, no need to continue
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


String PythonPhysicalModel::getHTMLDescription() const
{
  OSS oss;

  oss << PhysicalModelImplementation::getHTMLDescription();

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
  code = Tools::EscapeHTML(code);
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

  oss << "code = r'''\n" + getCode() + "'''\n";

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

}
