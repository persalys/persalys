//                                               -*- C++ -*-
/**
 *  @brief Defines physical models thanks analytical formulas and mesh
 *
 *  Copyright 2015-2019 EDF-Phimeca
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
#include "persalys/SymbolicFieldModel.hxx"

#include "persalys/BaseTools.hxx"

#include <openturns/SymbolicFunction.hxx>
#include <openturns/PersistentObjectFactory.hxx>

using namespace OT;

namespace PERSALYS
{

CLASSNAMEINIT(SymbolicFieldModel)

static Factory<SymbolicFieldModel> Factory_SymbolicFieldModel;

/* Default constructor */
SymbolicFieldModel::SymbolicFieldModel(const String& name)
  : SymbolicPhysicalModel(name)
{
  hasMesh_ = true;
}


/* Constructor with parameters */
SymbolicFieldModel::SymbolicFieldModel(const String& name,
    const MeshModel& meshModel,
    const InputCollection& inputs,
    const OutputCollection& outputs,
    const Description& formulas)
  : SymbolicPhysicalModel(name, inputs, outputs, formulas)
{
  hasMesh_ = true;
  setMeshModel(meshModel);
}


/* Virtual constructor */
SymbolicFieldModel* SymbolicFieldModel::clone() const
{
  return new SymbolicFieldModel(*this);
}


void SymbolicFieldModel::addInput(const Input& input)
{
  for (UnsignedInteger i = 0; i < getMeshModel().getIndexParameters().getSize(); ++i)
    if (getMeshModel().getIndexParameters()[i].getName() == input.getName())
      throw InvalidArgumentException(HERE) << "The physical model already contains an index parameter named " << input.getName();

  PhysicalModelImplementation::addInput(input);
}


void SymbolicFieldModel::setInputName(const String & inputName, const String & newName)
{
  for (UnsignedInteger i = 0; i < getMeshModel().getIndexParameters().getSize(); ++i)
    if (getMeshModel().getIndexParameters()[i].getName() == newName)
      throw InvalidArgumentException(HERE) << "The physical model already contains an index parameter named " << newName;

  PhysicalModelImplementation::setInputName(inputName, newName);
}


PointToFieldFunction SymbolicFieldModel::generatePointToFieldFunction(const Description& outputNames) const
{
  Description formulas;
  for (UnsignedInteger i = 0; i < outputNames.getSize(); ++ i)
  {
    formulas.add(getFormula(outputNames[i]));
  }
  Description inputNames(getInputNames());
  inputNames.add(getMeshModel().getIndexParameters()[0].getName());
  SymbolicFunction f(inputNames, formulas);
  f.getEvaluation().getImplementation()->setOutputDescription(outputNames);

  return VertexValuePointToFieldFunction(getMeshModel().getMesh(), f);
}


String SymbolicFieldModel::getPythonScript() const
{
  OSS oss;

  // mesh
  oss << getMeshModel().getPythonScript();

  // variables
  for (UnsignedInteger i = 0; i < getInputDimension(); ++i)
    oss << getInputs()[i].getPythonScript();

  for (UnsignedInteger i = 0; i < getOutputDimension(); ++i)
    oss << getOutputs()[i].getPythonScript();

  oss << "inputs = " << Parameters::GetOTDescriptionStr(getInputNames(), false) << "\n";
  oss << "outputs = " << Parameters::GetOTDescriptionStr(getOutputNames(), false) << "\n";

  // define model
  oss << "formulas = " << Parameters::GetOTDescriptionStr(getFormulas()) << "\n";
  oss << getName() << " = persalys.SymbolicFieldModel('" << getName() << "', " << getMeshModel().getName() << ", inputs, outputs, formulas)\n";

  oss << PhysicalModelImplementation::getCopulaPythonScript();

  return oss;
}
}
