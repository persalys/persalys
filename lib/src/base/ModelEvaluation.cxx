//                                               -*- C++ -*-
/**
 *  @brief Evaluates the physical models
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
#include "persalys/ModelEvaluation.hxx"

#include "persalys/Study.hxx"

#include <openturns/PersistentObjectFactory.hxx>

using namespace OT;

namespace PERSALYS
{

CLASSNAMEINIT(ModelEvaluation)

static Factory<ModelEvaluation> Factory_ModelEvaluation;

/* Default constructor */
ModelEvaluation::ModelEvaluation()
  : GridDesignOfExperiment()
{
}


/* Constructor with parameters */
ModelEvaluation::ModelEvaluation(const String &name, const PhysicalModel &physicalModel)
  : GridDesignOfExperiment(name, physicalModel)
{
  initializeParameters();
  setInterestVariables(getPhysicalModel().getSelectedOutputsNames());
}


/* Constructor with parameters */
ModelEvaluation::ModelEvaluation(const String &name,
                                 const PhysicalModel &physicalModel,
                                 const Point &inputsValues)
  : GridDesignOfExperiment(name, physicalModel)
{
  inputNames_ = getPhysicalModel().getInputNames();
  Collection<Point> values(inputsValues.getSize(), Point(1));
  for (UnsignedInteger i = 0; i < inputsValues.getSize(); ++i)
    values[i][0] = inputsValues[i];
  setValues(values);
  setInterestVariables(getPhysicalModel().getSelectedOutputsNames());
}


/* Virtual constructor */
ModelEvaluation* ModelEvaluation::clone() const
{
  return new ModelEvaluation(*this);
}


void ModelEvaluation::launch()
{
  if (getOriginalInputSample().getDimension() != getPhysicalModel().getInputDimension())
    throw InvalidArgumentException(HERE) << "Wrong input point dimension";

  // output = f(input) - use a Point on purpose
  const Sample outputP(1, getPhysicalModel().getFunction(getInterestVariables())(getOriginalInputSample()[0]));

  // set design of experiments
  // input sample
  result_.designOfExperiment_.setInputSample(getOriginalInputSample());
  // output sample
  Sample outputSample(outputP);
  outputSample.setDescription(getInterestVariables());
  result_.designOfExperiment_.setOutputSample(outputSample);
}


Parameters ModelEvaluation::getParameters() const
{
  Parameters param;

  param.add("Outputs of interest", getInterestVariables().__str__());
  OSS values;
  if (inputNames_.getSize() == values_.getSize())
  {
    for (UnsignedInteger i = 0; i < inputNames_.getSize(); ++i)
    {
      values << inputNames_[i] << " : " << values_[i][0];
      if (i < inputNames_.getSize() - 1)
        values << "\n";
    }
  }
  param.add("Point", values);

  return param;
}


String ModelEvaluation::getPythonScript() const
{
  String result;

  OSS oss;
  oss << "values = " << Parameters::GetOTPointStr(getOriginalInputSample()[0]) << "\n";
  oss << getName() << " = persalys." << getClassName() << "('" << getName() << "', " << getPhysicalModel().getName();
  oss << ", values)\n";
  if (getInterestVariables().getSize() < getPhysicalModel().getSelectedOutputsNames().getSize())
  {
    oss << "interestVariables = " << Parameters::GetOTDescriptionStr(getInterestVariables()) << "\n";
    oss << getName() << ".setInterestVariables(interestVariables)\n";
  }

  return oss;
}


bool ModelEvaluation::canBeLaunched(String &errorMessage) const
{
  return PhysicalModelAnalysis::canBeLaunched(errorMessage);
}


/* String converter */
String ModelEvaluation::__repr__() const
{
  OSS oss;
  oss << "class=" << GetClassName()
      << " name=" << getName()
      << " physicalModel=" << getPhysicalModel().getName()
      << " inputValues=" << getValues();
  return oss;
}
}
