//                                               -*- C++ -*-
/**
 *  @brief FMIPhysicalModel implements models defined by an fmu file
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
#include "otgui/FMIPhysicalModel.hxx"
#include "otgui/PythonScriptEvaluation.hxx"
#include "otgui/FMUInfo.hxx"

#include <openturns/PersistentObjectFactory.hxx>


using namespace OT;

namespace OTGUI {

CLASSNAMEINIT(FMIPhysicalModel);

static Factory<FMIPhysicalModel> Factory_FMIPhysicalModel;

/* Default constructor */
FMIPhysicalModel::FMIPhysicalModel(const String & name)
  : PythonPhysicalModel(name)
{
}


/* Constructor with parameters */
FMIPhysicalModel::FMIPhysicalModel(const String & name,
                                   const String & fileName)
  : PythonPhysicalModel(name)
{
  setFMUFileName(fileName);
}


/* Constructor with parameters */
FMIPhysicalModel::FMIPhysicalModel(const String & name,
                                   const InputCollection & inputs,
                                   const OutputCollection & outputs,
                                   const String & fileName)
  : PythonPhysicalModel(name)
{
  PhysicalModelImplementation::setInputs(inputs);
  PhysicalModelImplementation::setOutputs(outputs);

  Description inputNames(getInputNames());
  Description outputNames(getOutputNames());

  setFMUFileName(fileName);

  reassignVariables(inputNames, outputNames);

  // recopy variable data as they're reset by setCode
  PhysicalModelImplementation::setInputs(inputs);
  PhysicalModelImplementation::setOutputs(outputs);
}


/* Virtual constructor */
FMIPhysicalModel* FMIPhysicalModel::clone() const
{
  return new FMIPhysicalModel(*this);
}


String FMIPhysicalModel::getFMUFileName() const
{
  return fmuFileName_;
}


void FMIPhysicalModel::setFMUFileName(const String & fileName)
{
  fmuInfo_ = FMUInfo(fileName);
  Description allVars(fmuInfo_.getVariableNames());
  Indices allCausalities(fmuInfo_.getCausality());

  Description inputVars;
  Description outputVars;
  for (UnsignedInteger i = 0; i < allVars.getSize(); ++ i)
  {
    switch (allCausalities[i])
    {
      case 2:// input
        inputVars.add(allVars[i]);
        break;
      case 3:// output
        outputVars.add(allVars[i]);
        break;
      default:
        break;
    }
  }

  fmuFileName_ = fileName;
  reassignVariables(inputVars, outputVars);

  Description fixedKeys(fmuInfo_.getStartKeys());
  Point fixedValues(fmuInfo_.getStartValues());

  for(UnsignedInteger i = 0; i < fixedKeys.getSize(); ++i)
  {
    if (hasInputNamed(fixedKeys[i]))
      getInputByName(fixedKeys[i]).setValue(fixedValues[i]);
    if (hasOutputNamed(fixedKeys[i]))
      getOutputByName(fixedKeys[i]).setValue(fixedValues[i]);
  }

  notify("fmuChanged");
}


FMUInfo FMIPhysicalModel::getFMUInfo() const
{
  return fmuInfo_;
}


void FMIPhysicalModel::reassignVariables(const Description & inputNames,
                                         const Description & outputNames)
{
  // replace dots by underscores in variables names
  Description inputNamesUnderscore;
  for (UnsignedInteger i = 0; i < inputNames.getSize(); ++ i)
  {
    String inputName(inputNames[i]);
    std::replace(inputName.begin(), inputName.end(), '.', '_');
    inputNamesUnderscore.add(inputName);
  }

  Description outputNamesUnderscore;
  for (UnsignedInteger i = 0; i < outputNames.getSize(); ++ i)
  {
    String outputName(outputNames[i]);
    std::replace(outputName.begin(), outputName.end(), '.', '_');
    outputNamesUnderscore.add(outputName);
  }

  OSS code;
  code << "import otfmi\n\n";

  code << "def _exec(";
  for (UnsignedInteger i = 0; i < inputNames.getSize(); ++ i)
  {
    code << inputNamesUnderscore[i];
    if (i < inputNames.getSize() - 1)
      code << ", ";
  }
  code << "):\n";
  code << "    path_fmu = '" << fmuFileName_<< "'\n";
  code << "    inputs = [";
  for (UnsignedInteger i = 0; i < inputNames.getSize(); ++ i)
  {
    code << "'" << inputNames[i] << "'";
    if (i < inputNames.getSize() - 1)
      code << ", ";
  }
  code << "]\n";
  code << "    outputs = [";
  for (UnsignedInteger i = 0; i < outputNames.getSize(); ++ i)
  {
    code << "'" << outputNames[i]<< "'";
    if (i < outputNames.getSize() - 1)
      code << ", ";
  }
  code << "]\n";
  code << "    if not hasattr(_exec, 'model_fmu'):\n";
  code << "        _exec.model_fmu = otfmi.FMUFunction(path_fmu, inputs_fmu=inputs, outputs_fmu=outputs)\n";
  code << "    __X = [";
  for (UnsignedInteger i = 0; i < inputNames.getSize(); ++ i)
  {
    code << inputNamesUnderscore[i];
    if (i < inputNames.getSize() - 1)
      code << ", ";
  }
  code << "]\n";

  code << "    ";
  for (UnsignedInteger i = 0; i < outputNames.getSize(); ++ i)
  {
    code << outputNamesUnderscore[i];
    if (i < outputNames.getSize() - 1)
      code << ", ";
  }
  // Function always returns a sequence
  code << " = _exec.model_fmu(__X)" << (outputNames.getSize() == 1 ? "[0]" : "") << "\n";

  code << "    return ";
  for (UnsignedInteger i = 0; i < outputNames.getSize(); ++ i)
  {
    code << outputNamesUnderscore[i];
    if (i < outputNames.getSize() - 1)
      code << ", ";
  }
  code << "\n";
  setCode(code);

  // Variable names with dots have changed: restore original names
  for(UnsignedInteger i = 0; i < inputNames.getSize(); ++ i)
  {
    String oldName(inputNames[i]);
    String newName(inputNamesUnderscore[i]);
    if (newName != oldName) {
      getInputByName(newName).setName(oldName);
    }
  }
  for(UnsignedInteger i = 0; i < outputNames.getSize(); ++ i)
  {
    String oldName(outputNames[i]);
    String newName(outputNamesUnderscore[i]);
    if (newName != oldName) {
      getOutputByName(newName).setName(oldName);
    }
  }
}


Function FMIPhysicalModel::getFunction() const
{
  if (!getInputs().getSize())
    throw PhysicalModelNotValidException(HERE) << "The physical model has no inputs.";
  if (!getOutputs().getSize())
    throw PhysicalModelNotValidException(HERE) << "The physical model has no outputs.";

  Function function(PythonScriptEvaluation(getInputs().getSize(), getOutputs().getSize(), getCode()));
  function.enableCache();
  return function;
}


String FMIPhysicalModel::getPythonScript() const
{
  OSS oss;

  for (UnsignedInteger i = 0; i<getInputs().getSize(); ++ i)
    oss << getInputs()[i].getPythonScript();

  for (UnsignedInteger i = 0; i<getOutputs().getSize(); ++ i)
    oss << getOutputs()[i].getPythonScript();

  oss << "inputs = [";
  for (UnsignedInteger i = 0; i<getInputs().getSize(); ++ i)
  {
    String inputName(getInputs()[i].getName());
    std::replace(inputName.begin(), inputName.end(), '.', '_');
    oss << inputName;
    if (i < getInputs().getSize() - 1)
      oss << ", ";
  }
  oss << "]\n";

  oss << "outputs = [";
  for (UnsignedInteger i = 0; i < getOutputs().getSize(); ++ i)
  {
    String outputName(getOutputs()[i].getName());
    std::replace(outputName.begin(), outputName.end(), '.', '_');
    oss << outputName;
    if (i < getOutputs().getSize() - 1)
      oss << ", ";
  }
  oss << "]\n";

  oss << "fmuFile = '" + getFMUFileName() + "'\n";
  oss << getName()+ " = otguibase.FMIPhysicalModel('" + getName() + "', inputs, outputs, fmuFile)\n";

  oss << PhysicalModelImplementation::getCopulaPythonScript();

  return oss;
}


/** String converter */
String FMIPhysicalModel::__repr__() const
{
  OSS oss;
  oss << PhysicalModelImplementation::__repr__()
      << " fmuFileName=" << getFMUFileName();
  return oss;
}


/* Method save() stores the object through the StorageManager */
void FMIPhysicalModel::save(Advocate & adv) const
{
  PythonPhysicalModel::save(adv);
  adv.saveAttribute("fmuFileName_", fmuFileName_);
  adv.saveAttribute("fmuInfo_", fmuInfo_);
}


/* Method load() reloads the object from the StorageManager */
void FMIPhysicalModel::load(Advocate & adv)
{
  PythonPhysicalModel::load(adv);
  adv.loadAttribute("fmuFileName_", fmuFileName_);
  adv.loadAttribute("fmuInfo_", fmuInfo_);
}


}
