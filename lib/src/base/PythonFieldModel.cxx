//                                               -*- C++ -*-
/**
 *  @brief Defines physical models thanks analytical formulas and mesh
 *
 *  Copyright 2015-2023 EDF-Phimeca
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
#include "persalys/PythonFieldModel.hxx"

#include "persalys/PythonScriptFieldFunction.hxx"
#include "persalys/BaseTools.hxx"

#include <openturns/PersistentObjectFactory.hxx>

#include <regex>

using namespace OT;

namespace PERSALYS
{

CLASSNAMEINIT(PythonFieldModel)

static Factory<PythonFieldModel> Factory_PythonFieldModel;

/* Default constructor */
PythonFieldModel::PythonFieldModel(const String& name)
  : PythonPhysicalModel(name)
{
  hasMesh_ = true;
  setCode("def _exec(X0):\n    # mesh nodes\n    t = getMesh().getVertices()\n    Y0 = [X0 + t_i[0] for t_i in t]\n    return Y0");
}


/* Constructor with parameters */
PythonFieldModel::PythonFieldModel(const String& name,
    const MeshModel& meshModel,
    const InputCollection& inputs,
    const OutputCollection& outputs,
    const String& code)
  : PythonPhysicalModel(name, inputs, outputs, code)
{
  hasMesh_ = true;
  setMeshModel(meshModel.getImplementation()->clone());
}


/* Virtual constructor */
PythonFieldModel* PythonFieldModel::clone() const
{
  return new PythonFieldModel(*this);
}


PointToFieldFunction PythonFieldModel::generatePointToFieldFunction(const Description & outputNames) const
{
  Indices outIndices;
  for (UnsignedInteger i = 0; i < outputNames.getSize(); ++i)
    for (UnsignedInteger j = 0; j < getOutputNames().getSize(); ++j)
      if (outputNames[i] == getOutputNames()[j])
        outIndices.add(j);

  return PythonScriptFieldFunction(getInputNames(), outputNames, outIndices, getOutputDimension(), getCode(), getMeshModel().getMesh(), isParallel());
}


String PythonFieldModel::getPythonScript() const
{
  OSS oss;

  // mesh
  oss << getMeshModel().getPythonScript();

  // variables
  for (UnsignedInteger i = 0; i < getInputDimension(); ++ i)
    oss << getInputs()[i].getPythonScript();

  for (UnsignedInteger i = 0; i < getOutputDimension(); ++ i)
    oss << getOutputs()[i].getPythonScript();

  oss << "inputs = " << Parameters::GetOTDescriptionStr(getInputNames(), false) << "\n";
  oss << "outputs = " << Parameters::GetOTDescriptionStr(getOutputNames(), false) << "\n";

  // code - escape quotes, eols
  String escaped_code = std::regex_replace(getCode(), std::regex("'"), "\\\\'");
  escaped_code = std::regex_replace(escaped_code, std::regex("\n", std::regex::extended), "\\n");
  oss << "code = '" << escaped_code << "'\n";

  // define model
  oss << getName() << " = persalys.PythonFieldModel('" << getName() << "'," << getMeshModel().getName() << ", inputs, outputs, code)\n";

  if (isParallel())
    oss << getName() << ".setParallel(True)\n";

  oss << PhysicalModelImplementation::getCopulaPythonScript();

  return oss;
}
}
