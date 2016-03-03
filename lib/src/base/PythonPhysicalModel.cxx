//                                               -*- C++ -*-
/**
 *  @brief PythonPhysicalModel implements models defined by a Python script
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
#include "otgui/PythonPhysicalModel.hxx"
#include "otgui/PythonEvaluation.hxx"

#include "PersistentObjectFactory.hxx"

using namespace OT;

namespace OTGUI {

CLASSNAMEINIT(PythonPhysicalModel);

static Factory<PythonPhysicalModel> RegisteredFactory("PythonPhysicalModel");

/* Default constructor */
PythonPhysicalModel::PythonPhysicalModel(const String & name)
  : PhysicalModelImplementation(name)
{
  setCode("def _exec(X0):\n    Y0 = X0\n    return [Y0]");
}


/* Constructor with parameters */
PythonPhysicalModel::PythonPhysicalModel(const String & name,
                                         const InputCollection & inputs,
                                         const OutputCollection & outputs,
                                         const String & code)
  : PhysicalModelImplementation(name, inputs, outputs)
  , code_(code)
{
}


/* Virtual constructor */
PythonPhysicalModel* PythonPhysicalModel::clone() const
{
  return new PythonPhysicalModel(*this);
}


void PythonPhysicalModel::setCode(const String & code)
{
  code_ = code;
  notify("codeChanged");
}


String PythonPhysicalModel::getCode() const
{
  return code_;
}


NumericalMathFunction PythonPhysicalModel::getFunction() const
{
  if (!getInputs().getSize())
    throw PhysicalModelNotValidException(HERE) << "The physical model has no inputs.";
  if (!getOutputs().getSize())
    throw PhysicalModelNotValidException(HERE) << "The physical model has no outputs.";

  NumericalMathFunction function(PythonEvaluation(getInputs().getSize(), getOutputs().getSize(), getCode()));
  function.enableCache();
  return function;
}


String PythonPhysicalModel::getPythonScript() const
{
  String result;

  for (UnsignedInteger i = 0; i < getInputs().getSize(); ++ i)
    result += getInputs()[i].getPythonScript();

  for (UnsignedInteger i = 0; i < getOutputs().getSize(); ++ i)
    result += getOutputs()[i].getPythonScript();

  result += getName() + " = otguibase.PythonPhysicalModel('" + getName() + "')\n";

  for (UnsignedInteger i = 0; i < getInputs().getSize(); ++ i)
    result += getName() + ".addInput(" + getInputs()[i].getName() + ")\n";

  for (UnsignedInteger i = 0; i < getOutputs().getSize(); ++ i)
    result += getName() + ".addOutput(" + getOutputs()[i].getName() + ")\n";

  result += getName() + ".setCode('";

  std::stringstream ss(getCode());
  String to;

  while (std::getline(ss, to, '\n'))
  {
    result += to + "\\n";
  }
  result += "')\n";

  result += PhysicalModelImplementation::getCopulaPythonScript();

  return result;
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
}
