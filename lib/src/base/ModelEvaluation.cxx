//                                               -*- C++ -*-
/**
 *  @brief Evaluates the physical models
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
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
#include "otgui/ModelEvaluation.hxx"

#include <openturns/PersistentObjectFactory.hxx>

using namespace OT;

namespace OTGUI
{

CLASSNAMEINIT(ModelEvaluation)

static Factory<ModelEvaluation> Factory_ModelEvaluation;

/* Default constructor */
ModelEvaluation::ModelEvaluation()
  : GridDesignOfExperiment()
{
}


/* Constructor with parameters */
ModelEvaluation::ModelEvaluation(const String& name, const PhysicalModel& physicalModel)
  : GridDesignOfExperiment(name, physicalModel)
{
  initializeParameters();
  setInterestVariables(getPhysicalModel().getSelectedOutputsNames());
}


/* Constructor with parameters */
ModelEvaluation::ModelEvaluation(const String& name,
                                 const PhysicalModel& physicalModel,
                                 const Point& inputsValues)
  : GridDesignOfExperiment(name, physicalModel)
{
  inputNames_ = getPhysicalModel().getInputNames();
  setValues(inputsValues);
  setInterestVariables(getPhysicalModel().getSelectedOutputsNames());
}


/* Virtual constructor */
ModelEvaluation* ModelEvaluation::clone() const
{
  return new ModelEvaluation(*this);
}


void ModelEvaluation::launch()
{
  if (getValues().getSize() != getPhysicalModel().getInputDimension())
    throw InvalidArgumentException(HERE) << "Wrong input point dimension";

  // output = f(input)
  const Point outputP(getPhysicalModel().getFunction(getInterestVariables())(getValues()));

  // set design of experiments
  // input sample
  Sample inputSample(1, getValues());
  inputSample.setDescription(inputNames_);
  result_.designOfExperiment_.setInputSample(inputSample);
  // output sample
  Sample outputSample(1, outputP);
  outputSample.setDescription(getInterestVariables());
  result_.designOfExperiment_.setOutputSample(outputSample);
}


Parameters ModelEvaluation::getParameters() const
{
  Parameters param;

  param.add("Outputs of interest", getInterestVariables().__str__());
  OSS values;
  for (UnsignedInteger i = 0; i < inputNames_.getSize(); ++i)
  {
    values << inputNames_[i] << " : " << values_[i];
    if (i < inputNames_.getSize() - 1)
      values << "\n";
  }
  param.add("Point", values);

  return param;
}


String ModelEvaluation::getPythonScript() const
{
  String result;

  OSS oss;
  oss << "values = " << values_.__str__() << "\n";
  oss << getName() << " = otguibase.ModelEvaluation('" << getName() << "', " << getPhysicalModel().getName();
  oss << ", values)\n";
  if (getInterestVariables().getSize() < getPhysicalModel().getSelectedOutputsNames().getSize())
  {
    oss << "interestVariables = " << Parameters::GetOTDescriptionStr(getInterestVariables()) << "\n";
    oss << getName() << ".setInterestVariables(interestVariables)\n";
  }

  return oss;
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


/* Method save() stores the object through the StorageManager */
void ModelEvaluation::save(Advocate & adv) const
{
  GridDesignOfExperiment::save(adv);
}


/* Method load() reloads the object from the StorageManager */
void ModelEvaluation::load(Advocate & adv)
{
  GridDesignOfExperiment::load(adv);
}
}
