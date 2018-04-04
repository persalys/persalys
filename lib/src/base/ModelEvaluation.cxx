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
  : PhysicalModelAnalysis()
  , inputNames_()
  , inputValues_()
  , designOfExperiment_()
{
}


/* Constructor with parameters */
ModelEvaluation::ModelEvaluation(const String& name, const PhysicalModel& physicalModel)
  : PhysicalModelAnalysis(name, physicalModel)
  , inputNames_()
  , inputValues_()
  , designOfExperiment_(name, physicalModel)
{
  initializeParameters();
  setInterestVariables(getPhysicalModel().getSelectedOutputsNames());
}


/* Constructor with parameters */
ModelEvaluation::ModelEvaluation(const String& name,
                                 const PhysicalModel& physicalModel,
                                 const Point& inputsValues)
  : PhysicalModelAnalysis(name, physicalModel)
  , inputNames_(getPhysicalModel().getInputNames())
  , inputValues_(inputsValues)
  , designOfExperiment_(name, physicalModel)
{
  setInterestVariables(getPhysicalModel().getSelectedOutputsNames());
}


/* Virtual constructor */
ModelEvaluation* ModelEvaluation::clone() const
{
  ModelEvaluation * newAnalysis = new ModelEvaluation(*this);
  newAnalysis->designOfExperiment_ = designOfExperiment_.getImplementation()->clone();
  return newAnalysis;
}


void ModelEvaluation::initializeParameters()
{
  inputValues_.clear();
  inputNames_ = getPhysicalModel().getInputNames();

  const UnsignedInteger inputSize = inputNames_.getSize();
  inputValues_ = Point(inputSize);

  for (UnsignedInteger i = 0; i < inputSize; ++i)
    inputValues_[i] = getPhysicalModel().getInputs()[i].getValue();
}


void ModelEvaluation::updateParameters()
{
  Description inputNames(inputNames_);
  Point values(inputValues_);

  initializeParameters();

  for (UnsignedInteger i = 0; i < inputNames.getSize(); ++i)
  {
    const Description::const_iterator it = std::find(inputNames.begin(), inputNames.end(), inputNames_[i]);
    if (it != inputNames.end())
      inputValues_[i] = values[it - inputNames.begin()];
  }
}


void ModelEvaluation::initialize()
{
  AnalysisImplementation::initialize();
  designOfExperiment_.getImplementation()->initialize();
}


void ModelEvaluation::launch()
{
  if (getInputValues().getSize() != getPhysicalModel().getInputDimension())
    throw InvalidArgumentException(HERE) << "Wrong input point dimension";

  // output = f(input)
  Sample inputSample(1, getInputValues());
  inputSample.setDescription(inputNames_);

  Sample outputSample = getPhysicalModel().getFunction(getInterestVariables())(inputSample);
  outputSample.setDescription(getInterestVariables());

  // set design of experiments
  designOfExperiment_.setInputSample(inputSample);
  designOfExperiment_.setOutputSample(outputSample);
}


Point ModelEvaluation::getInputValues() const
{
  return inputValues_;
}


void ModelEvaluation::setInputValue(const UnsignedInteger index, const double value)
{
  if (index >= inputValues_.getSize())
    throw InvalidArgumentException(HERE) << "Wrong index value. Must be lesser than " << inputValues_.getSize();

  inputValues_[index] = value;
}


DesignOfExperiment ModelEvaluation::getDesignOfExperiment() const
{
  return designOfExperiment_;
}


Point ModelEvaluation::getOutputValues() const
{
  if (hasValidResult())
    return getDesignOfExperiment().getOutputSample()[0];
  return Point();
}


Parameters ModelEvaluation::getParameters() const
{
  Parameters param;

  param.add("Outputs of interest", getInterestVariables().__str__());
  OSS values;
  for (UnsignedInteger i = 0; i < inputNames_.getSize(); ++i)
  {
    values << inputNames_[i] << " : " << inputValues_[i];
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
  oss << "values = " << inputValues_.__str__() << "\n";
  oss << getName() << " = otguibase.ModelEvaluation('" << getName() << "', " << getPhysicalModel().getName();
  oss << ", values)\n";
  if (getInterestVariables().getSize() < getPhysicalModel().getSelectedOutputsNames().getSize())
  {
    oss << "interestVariables = " << Parameters::GetOTDescriptionStr(getInterestVariables()) << "\n";
    oss << getName() << ".setInterestVariables(interestVariables)\n";
  }

  return oss;
}


bool ModelEvaluation::hasValidResult() const
{
  return getDesignOfExperiment().getOutputSample().getSize() != 0;
}


/* String converter */
String ModelEvaluation::__repr__() const
{
  OSS oss;
  oss << "class=" << GetClassName()
      << " name=" << getName()
      << " physicalModel=" << getPhysicalModel().getName()
      << " inputValues=" << getInputValues();
  return oss;
}


/* Method save() stores the object through the StorageManager */
void ModelEvaluation::save(Advocate & adv) const
{
  PhysicalModelAnalysis::save(adv);
  adv.saveAttribute("inputNames_", inputNames_);
  adv.saveAttribute("inputValues_", inputValues_);
  adv.saveAttribute("designOfExperiment_", designOfExperiment_);
}


/* Method load() reloads the object from the StorageManager */
void ModelEvaluation::load(Advocate & adv)
{
  PhysicalModelAnalysis::load(adv);
  adv.loadAttribute("inputNames_", inputNames_);
  adv.loadAttribute("inputValues_", inputValues_);
  adv.loadAttribute("designOfExperiment_", designOfExperiment_);
}
}
